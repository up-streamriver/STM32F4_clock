#ifndef WIFI_H
#define WIFI_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "usart.h"
#include "esp_at.h"



void wifi_init(void);
void wifi_wait_connect(void);

#endif
/*WIFI_H */