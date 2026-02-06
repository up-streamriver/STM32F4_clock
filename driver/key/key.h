#ifndef KEY_H
#define KEY_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "led.h"
#include "usart.h"

typedef enum
{
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_MAX,
}key_id_t;

typedef enum
{
    KEY_STATE_IDLE,
    KEY_STATE_PRESS_DEBOUNCE,
    KEY_STATE_PRESS,
    KEY_STATE_RELEASE_DEBOUNCE,
}key_state_t;



#define KEY_DESC_MAP \
{   \
    {GPIOA,GPIO_Pin_0,EXTI_Line0,EXTI_PortSourceGPIOA,EXTI_PinSource0,EXTI0_IRQn,key1_long_callback,key1_short_callback,KEY_STATE_IDLE,0,0},\
    {GPIOC,GPIO_Pin_4,EXTI_Line4,EXTI_PortSourceGPIOC,EXTI_PinSource4,EXTI4_IRQn,key2_long_callback,key2_short_callback,KEY_STATE_IDLE,0,0},\
    {GPIOC,GPIO_Pin_5,EXTI_Line5,EXTI_PortSourceGPIOC,EXTI_PinSource5,EXTI9_5_IRQn,key3_long_callback,key3_short_callback,KEY_STATE_IDLE,0,0} \
}



#define KEY1_PORT GPIOA
#define KEY1_PIN  GPIO_Pin_0

#define KEY2_PORT GPIOC
#define KEY2_PIN  GPIO_Pin_4


void key_init(void);
bool key_is_pressed(void);
void key_timer_init(void);
uint8_t key_get_level(key_id_t id);

#endif
/*KEY_H */