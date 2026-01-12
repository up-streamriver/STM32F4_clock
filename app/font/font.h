#ifndef FONT_H
#define FONT_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

typedef struct 
{
    uint16_t height;
    const uint8_t *model;
}font_t;

extern const font_t font16;
extern const font_t font32;
extern const font_t font48;

#endif
/*FONT_H */