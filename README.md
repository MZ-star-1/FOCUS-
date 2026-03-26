# FOCUS-嵌入式
***
## 3.23
### 1）学写markdown标记语言
如# Chapter ## subchapter **emphasis** `code`
```javascript
const name = "lihaoyang"
```
1. list
2. list

- list
- list

**[link](https://github.com/MZ-star-1/FOCUS-/edit/main/README.md)**

**[image name](https://avatars.githubusercontent.com/u/258089284?v=4&size=64)**

![image name](https://avatars.githubusercontent.com/u/258089284?v=4&size=64)

|col1|col2|col3|
|---|---|---|
|data1|data2|data3|

### 2) Task1

1. 硬件准备与软件安装
- 软件包括STM32CubeMX、Keil MDK-ARM
2. 配置GPIO引脚（PA0与PA1）
- 分别左键设置为GPIO_Output
3. 配置系统时钟
- 遇到提示 **“No solution found using the current selected sources”** ,改用内部时钟，最终设置HCLK为8Hz解决
4. 设置工程并生成代码
- 遇到提示 **“The Firmware Package or one of its dependencies required by the Project is not available in your STM32CubeMX Repository”** ,下载STM32F1固件包解决
***
## 3.24
### 1) Task1续

5. 在Keil中添加控制代码，共有两处
```javascript
第一处
/* USER CODE BEGIN 0 */
// 电机状态定义
typedef enum {
    MOTOR_STOP = 0,  // 停止
    MOTOR_CW,        // 正转
    MOTOR_CCW        // 反转
} Motor_State;

// 电机控制函数
void Motor_Control(Motor_State state)
{
    switch(state) {
        case MOTOR_STOP:  // 停止
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
            break;
            
        case MOTOR_CW:    // 正转
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
            break;
            
        case MOTOR_CCW:   // 反转
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
            break;
    }
}
/* USER CODE END 0 */

第二处
/* USER CODE BEGIN WHILE */
uint32_t last_tick = HAL_GetTick();  // 记录上次切换时间
uint8_t motor_mode = 0;              // 当前电机模式

while (1)
{
    // 每2秒切换一次状态
    if(HAL_GetTick() - last_tick >= 2000)
    {
        last_tick = HAL_GetTick();  // 更新时间
        
        switch(motor_mode) {
            case 0:  // 正转2秒
                Motor_Control(MOTOR_CW);
                motor_mode = 1;
                break;
                
            case 1:  // 停止2秒
                Motor_Control(MOTOR_STOP);
                motor_mode = 2;
                break;
                
            case 2:  // 反转2秒
                Motor_Control(MOTOR_CCW);
                motor_mode = 3;
                break;
                
            case 3:  // 停止2秒，然后重新开始
                Motor_Control(MOTOR_STOP);
                motor_mode = 0;
                break;
        }
        
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
    
    /* USER CODE END WHILE */
    
    /* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */
```

6. 连接与烧录
- 杜邦线母头插不进STM32排针，确认母头无正反之分，适当用力插入
- 不清楚ST-LINK的接口排布，参考视频**[ST-Link的接线及使用](https://www.bilibili.com/video/BV1TPbyz8Emg?buvid=Y64814682A5A81F748A99AE3AF898D87BC6A&from_spmid=main.my-history.0.0&is_story_h5=false&mid=T3lXMOTTPxrUA5U4Rq0ehw%3D%3D&plat_id=116&share_from=ugc&share_medium=iphone&share_plat=ios&share_session_id=49F0E274-D3C2-4392-9BF3-755EF44B18D8&share_source=WEIXIN&share_tag=s_i&spmid=united.player-video-detail.0.0&timestamp=1774411950&unique_k=cI93knI&up_id=364108423)**
```
┌────────────────────────────┐
│  (缺口)                     │
│  ┌───┬───┬───┬───┬───┐    │
│  │ 9 │ 7 │ 5 │ 3 │ 1 │    │
│  └───┴───┴───┴───┴───┘    │
│  ┌───┬───┬───┬───┬───┐    │
│  │ 10│ 8 │ 6 │ 4 │ 2 │    │
│  └───┴───┴───┴───┴───┘    │
│                             │
└────────────────────────────┘
```
- ST-LINK与STM32相接并接入电脑后，STM32的指示灯不亮，检查ST-LINK的3.3V接口是否正确
- 烧录过程中显示 **“No ST-LINK detected”** ，安装ST-LINK驱动，并且点击amd64（我的电脑是64位系统）
- 回归检查最开始的硬件连接，重新检查四根线是否连对，回到5.2的位置，找到相关连接视频。
***
反思：之前连接ST-LINK与STM32并接入电脑后，STM32闪烁蓝光，询问AI这是什么情况，AI让我立即断电防止烧坏芯片，导致一直是在用错误的硬件连接进行下面的操作。现在按视频操作连接，发现**STM32闪蓝光**，换个AI问，告诉我这是成功的信号，让人忍俊不禁。反思不能只从AI处获得问题解答，有些操作问题尤其是需要视频演示的，可以上网查阅，同时也要比对多方信息，不要轻信AI！！！**（AI给的连接图也是错的！现已根据视频修正）**
***
## 3.25
### 1) Task1续
6. 连接与烧录
- 进行了漫长的查错，之前应该是烧录成功了，但后来Utility中点击connect总是显示**Can not connect to target!**，也是试了很多方法：下载了STM32CubeProgrammer,用USB转TTL与STM32连接进行“串口ISP解锁”，又执行全篇擦除，结果进度条5分钟还没反应，之后又选择烧录新程序，应该就是用原来Keil中的覆盖掉现在芯片里存的，但我发现好像两个存的是一样的，说明我前面应该是烧录成功了，但不知道为啥后来又connect不上，中间好几次都是ST-LINK红蓝光交替闪烁（应该是正在连接的意思），最后又变成红光常亮不闪，总之最后将L9110和马达都接入进去，（还用上面包板了），也是让马达动起来了，但好像和Keil中编写的main.c中的内容不太符合。
***
反思：给**文件夹命名**尽量避开中文和空格。我一开始命名的文件夹名字含中文，结果就是Programmer报错显示乱码字符。
***
## 3.26
### 1) Task1终
7. 结果不理想，重新做一遍Task1
- 马达直接连电源也是顺着一个方向转！说明我前面做的东西根本没有控制马达的转动
- 
***
## 3.27
***
## 3.28
