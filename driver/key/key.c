#include "key.h"

#define KEY1_PORT GPIOA
#define KEY1_PIN  GPIO_Pin_0

#define KEY2_PORT GPIOC
#define KEY2_PIN  GPIO_Pin_4

void key_init(void)
{
    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Structure.GPIO_Pin = KEY1_PIN;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY1_PORT,&GPIO_Structure);
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Structure.GPIO_Pin = KEY2_PIN;
	GPIO_Init(KEY2_PORT,&GPIO_Structure);

    EXTI_InitTypeDef EXTI_Structure;
    EXTI_StructInit(&EXTI_Structure);
    EXTI_Structure.EXTI_Line = EXTI_Line4;
    EXTI_Structure.EXTI_LineCmd = ENABLE;
    EXTI_Structure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_Structure.EXTI_Trigger = EXTI_Trigger_Rising;
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource4);
    EXTI_Init(&EXTI_Structure);

    NVIC_InitTypeDef NVIC_Structure;
    memset(&NVIC_Structure,0,sizeof(NVIC_Structure));
    NVIC_Structure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Structure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Structure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_Structure);


}

bool key_is_pressed(void)
{
   return  GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN) == Bit_RESET;
}

void EXTI4_IRQHandler(void)
{
    led_set(false);
    EXTI_ClearITPendingBit(EXTI_Line4);
}
