#ifndef TIM_DELAY_H
#define TIM_DELAY_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*tim_periodic_callback_t) (void);
void tim_delay_init(void);
uint64_t tim_now(void);
uint64_t tim_get_us(void);
uint64_t tim_get_ms(void);
void tim_delay_us(uint64_t us);
void tim_delay_ms(uint64_t ms);

void tim_periodic_callback_register(tim_periodic_callback_t callback);

#endif
/*TIM_DELAY_H */

