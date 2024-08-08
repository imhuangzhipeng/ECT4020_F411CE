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

uint8_t *create_dma_buffer(size_t size)
{
    uint8_t *buffer = NULL;

    size_t freeHeapMemSize = 0;

    /* 查询剩余内存 */
    freeHeapMemSize = xPortGetFreeHeapSize();
    if (freeHeapMemSize < size)
    {
        return NULL;
    }

    /* 创建DMA缓冲区 */
    buffer = pvPortMalloc(size);
    if (!buffer)
    {
        return NULL;
    }

    return buffer;
}

void dma_buffer_init(uint8_t *buffer, size_t size)
{
    size_t i = 0;

    /* 清空缓冲区 */
    for (i = 0; i < size; i++)
    {
        buffer[i] = 0xff;
    }

    return;
}

/**
 * @brief 创建2个DMA缓冲区
 * @param size 单个缓冲区的大小
 * @retval uint8_t **pbuffer_arr 记录双缓冲区地址的指针数组的指针
 */
uint8_t **create_dma_double_buffer(size_t size)
{
    /* DMA双缓冲区数组 */
    uint8_t *dma_double_buffer[2] = {NULL, NULL};
    uint8_t **pbuffer_arr = dma_double_buffer;

    /* 创建DMA双缓冲区 */
    for (int i = 0; i < 2; i++)
    {
        dma_double_buffer[i] = create_dma_buffer(size);
        if (!dma_double_buffer[i])
        {
            return NULL;
        }

        dma_buffer_init(dma_double_buffer[i], size);
    }

    return pbuffer_arr;
}

void adc_send_mailbox(void *args)
{
    BaseType_t ret = pdFAIL;
    adc_dma_mailbox_msg_t *padc_dma_mailbox_msg = NULL;

    while (1)
    {
        // 1. 阻塞等待DMA事件完成的信号量
        xSemaphoreTake(adcTransmitSemphr, portMAX_DELAY);

        // 2. 重新启动DMA, DMA指向新的缓冲区
        HAL_ADC_Stop_DMA(&hadc1);
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_mailbox_msg.buffer, ADC_BUFFER_SIZE);

        // 3. 发送邮箱数据

        /* 获取互斥锁 */
        xSemaphoreTake(dmaBufMtx, portMAX_DELAY);

        /* 邮件计数值+1 */
        padc_dma_mailbox_msg->msg_num++;

        /* 覆写邮箱数据 */
        ret = xQueueOverwrite(adc_dma_mailbox, &padc_dma_mailbox_msg);
        if (ret != pdPASS)
        {
            LOG("xQueuePeek mailbox failed!\r\n");
            continue;
        }

        /* 释放互斥锁 */
        ret = xSemaphoreGive(dmaBufMtx);
        if (ret != pdTRUE)
        {
            LOG("Semaphore give failed!\r\n");
            break;
        }
    }

    /* 发生严重错误，删除任务，关闭子模块功能 */
    vTaskDelete(NULL);
}

void adc_recv_mailbox(void *args)
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
        if (ret != pdPASS)
        {
            LOG("xQueueReceive failed! \r\n");
            break;
        }

        /* 释放互斥锁 */
        ret = xSemaphoreGive(dmaBufMtx);
        if (ret != pdTRUE)
        {
            LOG("Semaphore give failed!\r\n");
            break;
        }

        // 2. 计算ADC转换值
        adc_value = (uint32_t)(*(padc_dma_mailbox_msg->buffer));                     // ADC原始值
        vol = (float)((adc_value * 3.3f * VREFINT_CAL) / VREFINT_DATA * FULL_SCALE); // ADC转换值

        // 3. 打印ADC转换值
        LOG("adc value: %d, voltage: %1.3f V\r\n",
              adc_value,
              vol);
    }

    /* 发生严重错误，删除任务，关闭子模块功能 */
    vTaskDelete(NULL);
}

void adc_dma_transmit_init(void)
{
    /* 创建DMA双缓冲区 */
    dma_double_buffer = create_dma_double_buffer(ADC_BUFFER_SIZE);

    /* 初始化邮箱数据 */
    adc_dma_mailbox_msg.buffer = dma_double_buffer[0]; // 默认Buffer1为ADC转换缓冲区
    adc_dma_mailbox_msg.msg_num = 0;                   // 邮件计数, 邮箱数据更新时计数+1
    adc_dma_mailbox_msg.mail_id = 0;                   // 邮箱号, 用于广播订阅主题时使用

    /* 创建二值信号量, 用于ADC外设与任务A同步, 二值信号量创建后的值是0 */
    adcTransmitSemphr = xSemaphoreCreateBinary();
    if (!adcTransmitSemphr)
    {
        LOG("failed to create adcTransmitSemphr");
        return;
    }

    /* 创建互斥信号量, 用于任务A与任务B访问临界资源 */
    dmaBufMtx = xSemaphoreCreateMutex();
    if (!dmaBufMtx)
    {
        LOG("failed to create dmaBufMtx");
        return;
    }

    /* 创建邮箱, 用于任务A与任务B通信 */
    adc_dma_mailbox = xQueueCreate(1, sizeof(adc_dma_mailbox_msg_t *));
    if (!adc_dma_mailbox)
    {
        LOG("xQueueCreate failed! \r\n");
        return;
    }

    /* 创建邮箱任务 */
    if (pdPASS != xTaskCreate(adc_send_mailbox,
                              "adc_taskA",
                              128,
                              NULL,
                              osPriorityNormal2,
                              &sendMailbox) ||
        pdPASS != xTaskCreate(adc_recv_mailbox,
                              "adc_taskB",
                              128,
                              NULL,
                              osPriorityNormal2,
                              &recvMailbox))
    {
        LOG("xTaskCreate adc task failed! \r\n");
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
    if (hadc == &hadc1)
    {
        /* 切换ADC转换缓冲区指向 */
        uint8_t index = (adc_dma_mailbox_msg.msg_num) % 2;
        adc_dma_mailbox_msg.buffer = dma_double_buffer[index];

        /* DMA转换完成, 释放二值信号量通知任务A */
        BaseType_t ret = xSemaphoreGive(adcTransmitSemphr);
        if (ret != pdPASS)
        {
            LOG("Semaphore give failed!\r\n");
            return;
        }
    }
}
