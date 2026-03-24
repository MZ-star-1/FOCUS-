# FOCUS-嵌入式
***
## 3.23
### 1）学写markdown标记语言
比如# Chapter ## subchapter **emphasis** `code`
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

1. 配置GPIO引脚（PB0与PB1）
2. 配置系统时钟
   - 遇到提示“No solution found using the current selected sources”,改用内部时钟，设置HCLK为48Hz解决
3. 设置工程并生成代码
   - 遇到提示“The Firmware Package or one of its dependencies required by the Project is not available in your STM32CubeMX Repository”,下载STM32F1固件包解决

***
## 3.24
### 1) Task1续

4. 在Keil中添加控制代码
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
   - 不清楚ST-LINK的接口排布
```
┌────────────────────────────┐
│  (缺口)                     │
│  ┌───┬───┬───┬───┬───┐    │
│  │ 2 │ 4 │ 6 │ 8 │ 10│    │
│  └───┴───┴───┴───┴───┘    │
│  ┌───┬───┬───┬───┬───┐    │
│  │ 1 │ 3 │ 5 │ 7 │ 9 │    │
│  └───┴───┴───┴───┴───┘    │
│                             │
└────────────────────────────┘
```
***
## 3.25
***
## 3.26
***
## 3.27
***
## 3.28
