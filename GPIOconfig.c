#include "stm32f105xc.h"


void LEDconfig(void)
{
    GPIOC->CRL |= 0x00000011;
    // MODE = output mode, max speed 10MHz
    // general purpose output push-pull
    // gpio c 0, 1 == LED
    // GPIOA->CRH = (GPIOA->CRH & (unsigned long)0xFFFFFFF0) | 0xB; clock check B = MCO
}

void LED_OnOff(int onoff)
{
    if(onoff == 1) GPIOC->ODR = 0x00000000;
    else           GPIOC->ODR = 0x000000FF;
}


