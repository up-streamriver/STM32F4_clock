#ifndef FONT_H
#define FONT_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

typedef struct 
{
    const char *name;
    const uint8_t *model;
}font_chinese_t;

typedef struct 
{
    uint16_t size;
    const uint8_t *ascii_model;
    const font_chinese_t * chinese;
}font_t;

extern const font_t font16;
extern const font_t font32;
extern const font_t font48;

#endif
/*FONT_H */