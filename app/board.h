#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "led.h"
#include "usart.h"
#include "ringbuffer.h"
#include "key.h"
#include "timer.h"
#include "st7789.h"
#include "esp_at.h"
#include "weather.h"


void board_init(void);
void delay_init(void);
void delay_us(uint64_t us);
uint64_t bl_now_us(void);
uint64_t bl_now_ms(void);

#endif
/*BOARD_H */