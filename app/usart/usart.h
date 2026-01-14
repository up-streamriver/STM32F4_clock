#ifndef USART_H
#define USART_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

typedef void (*usart_recv_callback_t)(uint8_t data);
void usart_recv_handler_register(usart_recv_callback_t callback);
void usart_init(void);
void usart_write_data(uint8_t *data,uint32_t length);
void usart_write_string(const char *str);
void usart_printf(char * format,...);

#endif
/*USART_H */