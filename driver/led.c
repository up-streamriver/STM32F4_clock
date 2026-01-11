#include "led.h"

#define LED3_PORT GPIOE
#define LED3_PIN  GPIO_Pin_9

#define LED2_PORT GPIOB
#define LED2_PIN  GPIO_Pin_1

#define LED1_PORT GPIOB
#define LED1_PIN  GPIO_Pin_0

void led_init(void)
{
    GPIO_InitTypeDef GPIOStructure;
    GPIOStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIOStructure.GPIO_OType = GPIO_OType_PP;
    GPIOStructure.GPIO_Pin = LED3_PIN | LED2_PIN | LED1_PIN;
    GPIOStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED3_PORT,&GPIOStructure);
    GPIO_Init(LED2_PORT,&GPIOStructure);
    GPIO_Init(LED1_PORT,&GPIOStructure);
}

void led_set(bool status)
{
    GPIO_WriteBit(LED3_PORT,LED3_PIN,status? Bit_RESET:Bit_SET);
    GPIO_WriteBit(LED2_PORT,LED2_PIN,status? Bit_RESET:Bit_SET);
    GPIO_WriteBit(LED1_PORT,LED1_PIN,status? Bit_RESET:Bit_SET);
}

void led_toggle(void)
{
    uint8_t status = GPIO_ReadOutputDataBit(LED3_PORT,LED3_PIN);
    bool state = (status == Bit_SET) ? true : false;
    led_set(state);
}