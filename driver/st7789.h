#ifndef ST7789_H
#define ST7789_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define st7789_WIDTH 240
#define st7789_HEIGHT 320

#define mkcolor(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

void st7789_init(void);
void st7789_fill_color(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);

#endif
/*ST7789_H */