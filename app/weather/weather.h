#ifndef WEATHER_H
#define WEATHER_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "esp_at.h"
#include "cpu_tick.h"

typedef struct 
{
    char city[32];
    char location[128];
    char weather[16];
    uint8_t weather_code;
    float temperature;
}weather_info_t;

bool parse_seniverse_response(const char *response,weather_info_t *info);


#endif
/*WEATHER_H */