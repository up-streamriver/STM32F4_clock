#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>

typedef enum
{
    LED_1,
    LED_2,
    LED_3,
    LED_MAX,
}led_id_t;


#define LED_DESC_MAP \
{   \
    {GPIOB,GPIO_Pin_0,TIM3},\
    {GPIOB,GPIO_Pin_1,TIM3},\
    {GPIOE,GPIO_Pin_9,TIM8}\
}

#define LED_STREAM_DELAY_MS 300
#define LED_STREAM_CNT  3
#define LED_TASK_PRIOVITY 5
#define LED_TASK_STACK_SIZE 256

void led_init(void);
void led_pwm_init(void);
void led_set(led_id_t id,bool status);
void led_toggle(led_id_t id);
void led_stream(void);
#endif
/*LED_H */