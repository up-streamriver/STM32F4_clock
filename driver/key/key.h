#ifndef KEY_H
#define KEY_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "led.h"
#include "usart.h"

#define KEY1_PORT GPIOA
#define KEY1_PIN  GPIO_Pin_0

#define KEY2_PORT GPIOC
#define KEY2_PIN  GPIO_Pin_4


void key_init(void);
bool key_is_pressed(void);
void key_timer_init(void);

#endif
/*KEY_H */