#include "stm32f105xc.h"
#include "GPIOconfig.h"
#include <stdint.h>



void Timerconfig(void)
{
    RCC->APB2ENR |= 0x00000800;
    RCC->APB1ENR |= 0x00000001;
    /*
  RCC->APB2ENR |= 0x00000800;
  // timer1 enable 
  GPIOA->CRL = 0x444444A4;
  GPIOA->CRH = 0x8884444A;
  // timer2_ch2 = PA1 output compare channel 2 = alternate function push-pull


  TIM1->CNT = 0;
  TIM1->PSC = 20000-1;
  TIM1->ARR = 30000-1;

  TIM1->DIER = 0x00000001;

  NVIC->ISER[0] |= 0x02000000;

  TIM1->BDTR |= 0x00002000;
  TIM1->SR = 0;  
  TIM1->CR1 |= 0x0001;
     */

    TIM2->PSC = 2000-1;
    TIM2->ARR = 10000-1;
    TIM2->CNT = 0;
    TIM2->DIER = 0x0001;


    NVIC->ISER[0] |= (1 << 28);
    TIM2->SR = 0;
    TIM2->CR1 = 0x0005;


}

void Delay_us_irq(void)
{
}

void Delay_ms_irq(void)
{
}