#ifndef ESP_AT_H
#define ESP_AT_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"

typedef struct 
{
    char ssid[64];
    char bssid[18];
    int channel;
    int rssi;
    bool connected
}esp_wifi_info_t;



bool esp_at_init(void);
bool esp_usart_write_command(const char *command,uint32_t timeout);
const char *esp_at_get_response(void);
bool esp_at_wifi_init(void);
bool esp_at_connect_wifi(const char *ssid, const char *pwd,const char *mac);
bool esp_at_get_wifi_info(esp_wifi_info_t *info);
const char *esp_at_http_get(const char*url);

#endif
/*ESP_AT_H */