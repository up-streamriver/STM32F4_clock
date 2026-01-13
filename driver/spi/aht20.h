#ifndef AHT20_H
#define AHT20_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>

bool aht20_init(void);
bool aht20_write(uint8_t data[],uint32_t length);
bool aht20_read(uint8_t data[],uint32_t length);
bool aht20_trigger_measure(void);
bool aht20_wait_measure(void);
bool aht20_get_measure(float *humi,float *temp);
#endif
/*AHT20_H */