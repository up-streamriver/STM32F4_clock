#ifndef KEY_H
#define KEY_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "led.h"
#include "usart.h"

void key_init(void);
bool key_is_pressed(void);


#endif
/*KEY_H */