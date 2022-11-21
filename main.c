
#include <stdio.h>
#include <stm32f105xc.h>

#include "SystemClockConfig.h"
#include "GPIOconfig.h"
#include "CANconfig.h"
#include "TimerConfig.h"
#include "GlobalVariable.h"

void TIM2_IRQHandler();

CAN_DATA globalCANdata[3];

int main()
{
    SCB->VTOR = 0x08000000;
    __enable_irq();

    SysclkConfig();
    CAN_config();
    LEDconfig();

    Timerconfig();
    while(1)
    {
    }
    //return 0;
}

void TIM2_IRQHandler(void)
{
    TIM2->SR = 0;  
}




