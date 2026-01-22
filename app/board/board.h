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
#include "wifi.h"
#include "page.h"
#include "rtc.h"
#include "aht20.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tim_delay.h"


void board_lowlevel_init(void);
void board_init(void);

 

#endif
/*BOARD_H */