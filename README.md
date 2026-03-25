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
2. 配置GPIO引脚（PB0与PB1）
- 分别命名为IN1与IN2
3. 配置系统时钟
- 遇到提示 **“No solution found using the current selected sources”** ,改用内部时钟，设置HCLK为48Hz解决
4. 设置工程并生成代码
- 遇到提示 **“The Firmware Package or one of its dependencies required by the Project is not available in your STM32CubeMX Repository”** ,下载STM32F1固件包解决

***
## 3.24
### 1) Task1续

5. 在Keil中添加控制代码
```javascript
/* USER CODE BEGIN 3 */
while (1)
{
    // 正转：IN1=1, IN2=0
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(2000);   // 延时2秒

    // 停止
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(1000);   // 延时1秒

    // 反转：IN1=0, IN2=1
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_Delay(2000);

    // 停止
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(1000);
}
/* USER CODE END 3 */
```

5. 编译与下载
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
***
## 3.26
***
## 3.27
***
## 3.28
