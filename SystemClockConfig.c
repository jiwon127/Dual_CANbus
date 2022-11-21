#include <stm32f105xc.h>
#include <stdint.h>

void SysclkConfig(void)
{

    RCC->CR |= 0x00010081; // PLL exeption, HSE on, HSITRIM = 10000, HSI on

    RCC->CFGR |= 0x060F0002;

    RCC->CR |= 0x31000000; // PLL on

    RCC->APB2ENR |= 0x0000001D; // Port D, Port C, Port B(CAN)

}

void Delay_us(unsigned int time_us)
{
    register unsigned int i;

    for(i = 0; i <time_us; i++)
    {
        asm volatile("NOP");    asm volatile("NOP");    asm volatile("NOP");
        asm volatile("NOP");    asm volatile("NOP");    asm volatile("NOP");
        asm volatile("NOP");    asm volatile("NOP");    asm volatile("NOP");
        asm volatile("NOP");    asm volatile("NOP");    asm volatile("NOP");
        asm volatile("NOP");    asm volatile("NOP");    asm volatile("NOP");
        asm volatile("NOP");    asm volatile("NOP");   
    }
}

void Delay_ms(unsigned int time_ms)
{
    register unsigned int i;
    for(i = 0; i <time_ms; i++)
    {
        Delay_us(1000);
    }
}