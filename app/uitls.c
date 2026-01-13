#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdio.h>

static volatile uint64_t ticks;

void delay_init(void)
{
    SysTick_Config( SystemCoreClock / 1000000);
}

void delay_us(uint64_t us)
{
    uint64_t start = ticks;
    while(ticks - start < us);
}

uint64_t bl_now_us(void)
{
    return ticks;
}

uint64_t bl_now_ms(void)
{
    return ticks / 1000;
}

void SysTick_Handler(void)
{
    ticks++;
}