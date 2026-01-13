#ifndef IMAGE_H
#define IMAGE_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

typedef struct 
{
    uint16_t height;
    uint16_t width;
    const uint8_t * image_model;
}font_image_t;


extern const font_image_t font_image;


#endif
/*IMAGE_H */

