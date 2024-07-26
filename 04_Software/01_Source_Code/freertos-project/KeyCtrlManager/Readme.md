# 代码规范：
- 类名/接口名采用帕斯卡命名方式;
- 函数名采用驼峰命名方式;
- 变量名采用下划线命名方式;
- 常量采用常量命名方式;

```cpp
class MyClass { // 类名使用帕斯卡命名法
public:
    void myFunction(); // 函数名使用驼峰式命名法
private:
    int my_variable; // 变量名使用下划线命名法
    const int MY_CONSTANT = 100; // 常量名使用全大写字母，单词间使用下划线分隔
};
```

# 单元测试接口规范
- 单元测试函数名命名规范:功能_条件_期望

```cpp
/**
 * 接口功能:
 * 传入参数:
 * 返回值:
 * 触发条件:
 * 期望结果:
*/
Process_OrderUnshipped_SetShippment(/* args */)
{
    /* code */
}
```

# 文件结构：
```bash
.
├─Example   // 单元测试代码存放目录
├─Model     // 驱动代码存放目录
│  ├─gpio
│  ├─key
│  ├─led
│  ├─linklist
│  └─uart
├─Task      // 业务代码存放目录
│  └─key_ctrl
│      ├─inc
│      └─src
```

# 历史提交:
#2024.7.18 推送按键驱动，LED驱动，基于FreeRTOS消息队列实现功能：单次按下按键，实现LED的翻转；长按按键，实现LED灯闪烁三次。
