# 电控组第二次培训作业
**颜伟锟**  
***20263009310***



## 流水灯实现思路
***若是用HAL_Delay那么会阻塞cpu，那么就不妨去想着调用一个时钟（或者叫闹钟？），到点就发出一个信号使其切换电平，从而实现灯亮灭的切换***
__HAL_RCC_GPIOA_CLK_ENABLE();
GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
***先将GPIO初始化了，然后再去配置时钟***
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
TIM2->PSC = 7199；
***这里是将计时器的速度设置为10000/秒，没招，72MHz不好算时间***
TIM2->ARR = 4999;
***这里是计算了一下每次要记得时长，中断周期=计数频率/（ARR+1），也就是到0.5s的时候，发出信号进行切换***
​

// 4. 开启中断并启动
TIM2->DIER |= TIM_DIER_UIE; // 允许更新中断
TIM2->CR1 |= TIM_CR1_CEN;   // 启动计数器
NVIC_EnableIRQ(TIM2_IRQn);  
***这里是把这个信号发出去***
void TIM2_IRQHandler(void)
{
  if (TIM2->SR & TIM_SR_UIF) // 检查是不是 TIM2 溢出了
  {
    TIM2->SR &= ~TIM_SR_UIF; 
***这里清了一下中断标志***

    // 流水灯状态机逻辑
    static uint8_t led_step = 0; // 静态变量，记住当前亮到哪一步了
    switch (led_step)
    {
      case 0: GPIOA->ODR = GPIO_PIN_0; break; // 只亮第1个
      case 1: GPIOA->ODR = GPIO_PIN_1; break; // 只亮第2个
      case 2: GPIOA->ODR = GPIO_PIN_2; break; // 只亮第3个
    }
    
    led_step++; // 下一步
    if (led_step > 2) led_step = 0; // 
  }
}