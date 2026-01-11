#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "led.h"
#include "usart.h"

void timer_init(void);
void timer_pwm_init(void);

#endif
/*TIMER_H */