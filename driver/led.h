#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>

void led_init(void);
void led_set(bool status);
void led_toggle(void);
#endif
/*LED_H */