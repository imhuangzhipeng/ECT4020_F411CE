#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "adc.h"
#include "user_rtt_printf.h"

/* ADC转换值缓冲区大小 */
#define ADC_BUFFER_SIZE 4

/* 内部参考电压的校准值 */
#define VREFINT_CAL 1
/* 内部参考电压实际值 */
#define VREFINT_DATA 1
/* ADC分辨率
 * 规则通道只有一个数据寄存器ADC_DR, 只有低16位有效
 * 数据采用右对齐方式, 转换的数据范围为0~2^12-1, 即0~4095
 */
#define FULL_SCALE 4095

/* 邮箱数据结构体 */
typedef struct
{
    /* DMA缓冲区指向 */
    uint8_t *buffer;
    /* 消息数 */
    uint8_t msg_num;
    /* 邮箱号 */
    uint8_t mail_id;
} adc_dma_mailbox_msg_t;

/* 邮箱数据域 */
static adc_dma_mailbox_msg_t adc_dma_mailbox_msg;

/* DMA双缓冲区数组 */
static uint8_t **dma_double_buffer;

static QueueHandle_t adc_dma_mailbox;

static SemaphoreHandle_t dmaBufMtx;
static SemaphoreHandle_t adcTransmitSemphr;

static TaskHandle_t sendMailbox;
static TaskHandle_t recvMailbox;

/**
 * @brief 创建DMA双缓冲区
 * @param size 单个缓冲区的大小
 * @retval NULL - 缓冲区内存分配失败
 * @retval pbuffer_arr - 双缓冲区数组指针
 */
uint8_t **create_dma_double_buffer(size_t size)
{
    size_t i, j = 0;
    size_t freeHeapMemSize = 0;
    static uint8_t *pbuffer_arr[2] = {NULL, NULL};

    for (i = 0; i < 2; i++)
    {
        /* 查看剩余内存空间是否足够 */
        freeHeapMemSize = xPortGetFreeHeapSize();
        if (freeHeapMemSize < size)
        {
            LOG("Heap space insufficient.\r\n");
            return NULL;
        }

        /* 创建DMA缓冲区 */
        pbuffer_arr[i] = pvPortMalloc(size);
        if (!pbuffer_arr[i])
        {
            LOG("Create dma buffer fail.\r\n");
            return NULL;
        }

        /* 清空缓冲区 */
        for (j = 0; j < size; j++)
        {
            pbuffer_arr[i][j] = 0xff;
        }
    }

    return pbuffer_arr;
}

/**
 * @description: 任务A: 切换DMA转换缓冲区，通知任务B读取缓冲区数据
 * @param {void} *args
 * @return {*}
 */
void adc_process_dma_transfer(void *args)
{
    BaseType_t ret = pdFAIL;
    adc_dma_mailbox_msg_t *padc_dma_mailbox_msg = NULL;

    while (1)
    {
        // 1. 阻塞等待DMA事件完成的信号量
        xSemaphoreTake(adcTransmitSemphr, portMAX_DELAY);

        // 2. 切换ADC转换缓冲区指向

        /* 邮件计数值+1 */
        padc_dma_mailbox_msg->msg_num++;

        /* DMA指向新的缓冲区 */
        uint8_t index = (adc_dma_mailbox_msg.msg_num) % 2;
        adc_dma_mailbox_msg.buffer = dma_double_buffer[index];

        /* 重新启动DMA */
        HAL_ADC_Stop_DMA(&hadc1);
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_mailbox_msg.buffer, ADC_BUFFER_SIZE);

        // 3. 发送邮箱数据

        /* 获取互斥锁 */
        xSemaphoreTake(dmaBufMtx, portMAX_DELAY);

        /* 覆写邮箱数据 */
        ret = xQueueOverwrite(adc_dma_mailbox, &padc_dma_mailbox_msg);
        if (ret != pdPASS)
        {
            LOG("Overwrite mailbox fail!\r\n");
            continue;
        }

        /* 释放互斥锁 */
        ret = xSemaphoreGive(dmaBufMtx);
        if (ret != pdTRUE)
        {
            LOG("DMA buffer mutex give fail!\r\n");
            break;
        }
    }

    /* 发生严重错误，删除任务，关闭子模块功能 */
    vTaskDelete(NULL);
}

/**
 * @description: 任务B: 读取缓冲区数据，计算ADC电压值并打印
 * @param {void} *args
 * @return {*}
 */
void adc_convert(void *args)
{
    BaseType_t ret = pdFAIL;
    adc_dma_mailbox_msg_t *padc_dma_mailbox_msg = NULL;
    uint32_t adc_value = 0;
    float vol = 0;

    while (1)
    {
        // 1. 接收邮箱数据

        /* 查看邮箱, 没有数据就阻塞 */
        xQueuePeek(adc_dma_mailbox, NULL, portMAX_DELAY);

        /* 获取互斥锁 */
        xSemaphoreTake(dmaBufMtx, portMAX_DELAY);

        /* 取走邮箱数据 */
        ret = xQueueReceive(adc_dma_mailbox,
                            (void *)&padc_dma_mailbox_msg,
                            0);
        if (ret != pdTRUE)
        {
            LOG("Receive mailbox fail! \r\n");
            break;
        }

        /* 释放互斥锁 */
        ret = xSemaphoreGive(dmaBufMtx);
        if (ret != pdTRUE)
        {
            LOG("DMA buffer mutex give fail!\r\n");
            break;
        }

        // 2. 计算ADC转换值
        adc_value = (uint32_t)(*(padc_dma_mailbox_msg->buffer));                     // ADC原始值
        vol = (float)((adc_value * 3.3f * VREFINT_CAL) / VREFINT_DATA * FULL_SCALE); // ADC转换值

        // 3. 打印ADC转换值
        LOG("ADC value: %d, voltage: %1.3f V\r\n",
            adc_value,
            vol);
    }

    /* 发生严重错误，删除任务，关闭子模块功能 */
    vTaskDelete(NULL);
}

/**
 * @description:
 * @return {*}
 */
void adc_dma_transmit_init(void)
{
    /* 创建DMA双缓冲区 */
    dma_double_buffer = create_dma_double_buffer(ADC_BUFFER_SIZE);
    if (!dma_double_buffer)
    {
        LOG("Create dma double buffer fail.\r\n");
        return;
    }

    /* 初始化邮箱数据 */
    adc_dma_mailbox_msg.buffer = dma_double_buffer[0]; // 默认Buffer1为ADC转换缓冲区
    adc_dma_mailbox_msg.msg_num = 0;                   // 邮件计数, 邮箱数据更新时计数+1
    adc_dma_mailbox_msg.mail_id = 0;                   // 邮箱号, 用于广播订阅主题时使用

    /* 创建二值信号量, 用于ADC外设与任务A同步, 二值信号量创建后的值是0 */
    adcTransmitSemphr = xSemaphoreCreateBinary();
    if (!adcTransmitSemphr)
    {
        LOG("Fail to create ADC semaphore.\r\n");
        return;
    }

    /* 创建互斥信号量, 用于任务A与任务B访问临界资源 */
    dmaBufMtx = xSemaphoreCreateMutex();
    if (!dmaBufMtx)
    {
        LOG("Fail to create DMA buffer mutex.\r\n");
        return;
    }

    /* 创建邮箱, 用于任务A与任务B通信 */
    adc_dma_mailbox = xQueueCreate(1, sizeof(adc_dma_mailbox_msg_t *));
    if (!adc_dma_mailbox)
    {
        LOG("Create mailbox fail! \r\n");
        return;
    }

    /* 创建任务 */
    if (pdPASS != xTaskCreate(adc_process_dma_transfer,
                              "adc_process_dma_transfer",
                              128,
                              NULL,
                              osPriorityNormal2,
                              &sendMailbox) ||
        pdPASS != xTaskCreate(adc_convert,
                              "adc_convert",
                              128,
                              NULL,
                              osPriorityNormal2,
                              &recvMailbox))
    {
        LOG("Create ADC task fail! \r\n");
        return;
    }

    /* 开启ADC转换 */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_mailbox_msg.buffer, ADC_BUFFER_SIZE);

    return;
}

/**
 * @brief  重新实现HAL库ADC中断回调
 * @note   ADC 中断回调和 ADC DMA事件完成回调都是HAL_ADC_ConvCpltCallback()
 * @param  hadc : ADC handle
 * @retval None
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (hadc == &hadc1)
    {
        /* DMA转换完成, 释放二值信号量通知任务A */
        xSemaphoreGiveFromISR(adcTransmitSemphr, &xHigherPriorityTaskWoken);

        // 如果有更高优先级的任务被唤醒，则调用 portYIELD() 或 portEND_SWITCHING_ISR()
        if (xHigherPriorityTaskWoken == pdTRUE)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
