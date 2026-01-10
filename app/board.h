#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "led.h"
#include "usart.h"
#include "ringbuffer.h"

void board_init(void);
void delay_init(void);
void delay_ms(uint32_t ms);
uint32_t bl_now(void);

#endif
/*BOARD_H */