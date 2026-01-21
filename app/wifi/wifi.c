#include "wifi.h"
#include "app.h"
#include "FreeRTOS.h"
#include "task.h"

static	esp_wifi_info_t wifi = { 0 };
static	esp_sntp_info_t date = { 0 };




void wifi_init(void)
{
    if(!esp_at_init())
	{
        usart_printf("[AT] init failed\n");
        goto err;		
	}
	usart_printf("[AT] init success\n");
	if(!esp_at_wifi_init())
	{
        usart_printf("[WIFI] init failed\n");
        goto err;		
	}
	usart_printf("[WIFI] init success\n");

	if (!esp_at_sntp_init())
    {
        usart_printf("[SNTP] init failed\n");
        goto err;
    }
    usart_printf("[SNTP] inited success\n");
    return;
err:
    while (1)
    {
        usart_printf("AT Error\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }		
}

void wifi_wait_connect(void)
{   
    
    for(uint16_t i=0 ; i<500 ; i++)
    {   
        vTaskDelay(pdMS_TO_TICKS(10));
        if(!esp_at_connect_wifi(wifi_ssid, wifi_pwd, NULL))
	    {
            usart_printf("[WIFI] connect failed\n");
            continue;	
	    }
	    usart_printf("[WIFI] connect success\n");
	    if(!wifi_is_connected())
	    {
            usart_printf("[WIFI] disconnect \n");
            continue;			
	    }
	    if(!esp_at_get_wifi_info(&wifi))
	    {
            usart_printf("[WIFI] info failed\n");
            continue;		
	    }
	    usart_printf("[WIFI] ssid:%s  bssid:%s  channel:%d  rssi:%d\n",wifi.ssid,wifi.bssid,wifi.channel,wifi.rssi);	
	    if(!esp_at_sntp_get(&date))
	    {
            usart_printf("[SNTP] info failed\n");
            continue;			
	    }
	    usart_printf("[SNTP] %d,%d,%d,%d,%d,%d,%s\n",date.year,date.month,date.day,date.hour,date.minute,date.second,
	    date.weekday == 1 ? "Monday":
	    date.weekday == 2 ? "Tuesday":
	    date.weekday == 3 ? "Wednesday":
	    date.weekday == 4 ? "Thursday":
	    date.weekday == 5 ? "Friday":
	    date.weekday == 6 ? "Saturday":
	    date.weekday == 7 ? "Sunday": "Unknown");
        return ;
    }
    return ;
}

