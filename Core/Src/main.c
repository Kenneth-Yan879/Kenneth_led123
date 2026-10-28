#include "stm32f1xx_hal.h"

/* ==================== 函数原型声明 ==================== */
void SystemClock_Config(void);
void Error_Handler(void);

/* ==================== 主函数 ==================== */
int main(void)
{
  /* MCU Configuration */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* 开启 GPIOA 时钟 */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* 配置 PA0, PA1, PA2 为推挽输出 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* ==================== 直接配置 TIM2 寄存器 ==================== */
  // 开启 TIM2 时钟
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // 配置预分频器：72MHz / (7199+1) = 10kHz (每0.1ms加1)
  TIM2->PSC = 7199;
  // 配置重装载值：10kHz / (4999+1) = 2Hz = 500ms中断一次
  TIM2->ARR = 4999;
  // 清除更新中断标志
  TIM2->SR &= ~TIM_SR_UIF;
  // 允许更新中断
  TIM2->DIER |= TIM_DIER_UIE;
  // 启动定时器
  TIM2->CR1 |= TIM_CR1_CEN;

  // 在 NVIC 中启用 TIM2 中断
  NVIC_EnableIRQ(TIM2_IRQn);
  NVIC_SetPriority(TIM2_IRQn, 0);

  /* ==================== 主循环（彻底解放） ==================== */
  while (1)
  {
    // 这里可以自由添加其他代码
  }
}

/* ==================== TIM2 中断服务函数 ==================== */
void TIM2_IRQHandler(void)
{
  // 检查是否是更新中断（计数器溢出）
  if (TIM2->SR & TIM_SR_UIF)
  {
    // 清除中断标志
    TIM2->SR &= ~TIM_SR_UIF;

    // 流水灯逻辑
    static uint8_t led_step = 0;
    switch (led_step)
    {
      case 0:
        GPIOA->ODR = GPIO_PIN_0;          // 亮PA0，灭PA1、PA2
        break;
      case 1:
        GPIOA->ODR = GPIO_PIN_1;          // 亮PA1，灭PA0、PA2
        break;
      case 2:
        GPIOA->ODR = GPIO_PIN_2;          // 亮PA2，灭PA0、PA1
        break;
    }
    led_step++;
    if (led_step > 2) led_step = 0;
  }
}

/* ==================== 系统时钟配置 ==================== */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK

                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* ==================== 错误处理函数 ==================== */
void Error_Handler(void)
{
  while(1) {}
}



