#include "board.h"

void board_lowlevel_init(void)
{   
    //SCB->VTOR = 0x08010000;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
}

void board_init(void)
{
    //cpu_tick_init();
    usart_init();
    usart_printf("[SYS] Build Date: %s %s\n", __DATE__, __TIME__);
    //led_init();

    key_init();
    rtc_init();
    tim_delay_init();
    aht20_init();
    led_pwm_init();
    usart_printf("led pwm init \r\n");
}

void vAssertCalled(const char *file, int line)
{
    //usart_printf("Assert Called: %s(%d)\n", file, line);
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName)
{
   // usart_printf("Stack Overflowed: %s\n", pcTaskName);
    configASSERT(0);
}

void vApplicationMallocFailedHook( void )
{
   // usart_printf("Malloc Failed\n");
    configASSERT(0);
}

