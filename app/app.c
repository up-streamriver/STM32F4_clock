#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rtc.h"
#include "aht20.h"
#include "esp_at.h"
#include "weather.h"
#include "page.h"
#include "app.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "workqueue.h"

#define MILLISECONDS(x) (x)
#define SECONDS(x)      MILLISECONDS((x) * 1000)
#define MINUTES(x)      SECONDS((x) * 60)
#define HOURS(x)        MINUTES((x) * 60)
#define DAYS(x)          HOURS((x) * 24)

#define TIME_SYNC_INTERVAL          HOURS(1)
#define WIFI_UPDATE_INTERVAL        SECONDS(5)
#define TIME_UPDATE_INTERVAL        SECONDS(1)
#define INNER_UPDATE_INTERVAL       SECONDS(3)
#define OUTDOOR_UPDATE_INTERVAL     MINUTES(1)

#define MLOOP_EVT_TIME_SYNC         (1 << 0)
#define MLOOP_EVT_WIFI_UPDATE       (1 << 1)
#define MLOOP_EVT_TIME_UPDATE       (1 << 2)
#define MLOOP_EVT_INNER_UPDATE      (1 << 3)
#define MLOOP_EVT_OUTDOOR_UPDATE    (1 << 4)
#define MLOOP_EVT_ALL       (\
                            MLOOP_EVT_TIME_SYNC     |\    
                            MLOOP_EVT_WIFI_UPDATE   |\    
                            MLOOP_EVT_TIME_UPDATE   |\    
                            MLOOP_EVT_INNER_UPDATE  |\    
                            MLOOP_EVT_OUTDOOR_UPDATE \
                            )

static TaskHandle_t mloop_task;
static TimerHandle_t time_sycn_timer;
static TimerHandle_t wifi_update_timer;
static TimerHandle_t time_update_timer;
static TimerHandle_t inner_update_timer;
static TimerHandle_t outdoor_update_timer;


static void time_sync(void)
{
    uint32_t restart_sync_delay = HOURS(1);

    esp_sntp_info_t esp_date = { 0 };
    xTimerChangePeriod(time_sycn_timer,pdMS_TO_TICKS(TIME_SYNC_INTERVAL),0);
    if (!esp_at_sntp_get(&esp_date))
    {
        usart_printf("[SNTP] get time failed\n");
        restart_sync_delay = SECONDS(1);
        goto err;
    }
    
    if (esp_date.year < 2000)
    {
        usart_printf("[SNTP] invalid date formate\n");
        restart_sync_delay = SECONDS(1);
        goto err;
    }
    
    usart_printf("[SNTP] sync time: %04u-%02u-%02u %02u:%02u:%02u (%d)\n",
        esp_date.year, esp_date.month, esp_date.day,
        esp_date.hour, esp_date.minute, esp_date.second, esp_date.weekday);
    
    rtc_date_time_t rtc_date = { 0 };
    rtc_date.year = esp_date.year;
    rtc_date.month = esp_date.month;
    rtc_date.day = esp_date.day;
    rtc_date.hour = esp_date.hour;
    rtc_date.minute = esp_date.minute;
    rtc_date.second = esp_date.second;
    rtc_date.weekday = esp_date.weekday;
    rtc_set_time(&rtc_date);
    
err:
    xTimerChangePeriod(time_sycn_timer,pdMS_TO_TICKS(restart_sync_delay),0);
    xTaskNotify(mloop_task,MLOOP_EVT_TIME_SYNC,eSetBits);
}

static void wifi_update(void)
{
    static esp_wifi_info_t last_info = { 0 };
    xTimerChangePeriod(wifi_update_timer,pdMS_TO_TICKS(WIFI_UPDATE_INTERVAL),0);
    
    esp_wifi_info_t info = { 0 };
    if (!esp_at_get_wifi_info(&info))
    {
        usart_printf("[AT] wifi info get failed\n");
        return;
    }
    
    if (memcmp(&info, &last_info, sizeof(esp_wifi_info_t)) == 0)
    {
        return;
    }
    
    if (last_info.connected == info.connected)
    {
        return;
    }
    
    if (info.connected)
    {
        usart_printf("[WIFI] connected to %s\n", info.ssid);
        usart_printf("[WIFI] SSID: %s, BSSID: %s, Channel: %d, RSSI: %d\n",
                info.ssid, info.bssid, info.channel, info.rssi);
        main_page_redraw_wifi_ssid(info.ssid);
    }
    else
    {
        usart_printf("[WIFI] disconnected from %s\n", last_info.ssid);
        main_page_redraw_wifi_ssid("wifi lost");
    }
    
    memcpy(&last_info, &info, sizeof(esp_wifi_info_t));
}

static void time_update(void)
{
    static rtc_date_time_t last_date = { 0 };
    xTimerChangePeriod(time_update_timer,pdMS_TO_TICKS(TIME_UPDATE_INTERVAL),0);
    rtc_date_time_t date;
    rtc_get_time(&date);
    
    if (date.year < 2020)
    {
        return;
    }
    
    if (memcmp(&date, &last_date, sizeof(rtc_date_time_t)) == 0)
    {
        return;
    }
    
    memcpy(&last_date, &date, sizeof(rtc_date_time_t));
    main_page_redraw_time(&date);
    main_page_redraw_date(&date);
}

static void inner_update(void)
{
    static float last_temperature, last_humidity;
    xTimerChangePeriod(inner_update_timer,pdMS_TO_TICKS(INNER_UPDATE_INTERVAL),0);
    
    if (!aht20_start_measurement())
    {
        usart_printf("[AHT20] start measurement failed\n");
        return;
    }
    
    if (!aht20_wait_for_measurement())
    {
        usart_printf("[AHT20] wait for measurement failed\n");
        return;
    }
    
    float temperature = 0.0f, humidity = 0.0f;
    
    if (!aht20_read_measurement(&temperature, &humidity))
    {
        usart_printf("[AHT20] read measurement failed\n");
        return;
    }
    
    if (temperature == last_temperature && humidity == last_humidity)
    {
        return;
    }
    
    last_temperature = temperature;
    last_humidity = humidity;
    
    usart_printf("[AHT20] Temperature: %.1f, Humidity: %.1f\n", temperature, humidity);
    main_page_redraw_inner_temperature(temperature);
    main_page_redraw_inner_humidity(humidity);
}

static void outdoor_update(void)
{
    static weather_info_t last_weather = { 0 };
    xTimerChangePeriod(outdoor_update_timer,pdMS_TO_TICKS(OUTDOOR_UPDATE_INTERVAL),0);
    
    weather_info_t weather = { 0 };
    const char *weather_url = "https://api.seniverse.com/v3/weather/now.json?key=St0LIXsCn7a-r05vm&location=beijing&language=en&unit=c";
    const char *weather_http_response = esp_at_http_get(weather_url);
    //extern const char *url = "https://api.seniverse.com/v3/weather/now.json?key=St0LIXsCn7a-r05vm&location=beijing&language=en&unit=c";
    if (weather_http_response == NULL)
    {
        usart_printf("[WEATHER] http error\n");
        return;
    }
    
    if (!parse_seniverse_response(weather_http_response, &weather))
    {
        usart_printf("[WEATHER] parse failed\n");
        return;
    }
    
    if (memcmp(&last_weather, &weather, sizeof(weather_info_t)) == 0)
    {
        return;
    }
    
    memcpy(&last_weather, &weather, sizeof(weather_info_t));
    usart_printf("[WEATHER] %s, %s, %.1f\n", weather.city, weather.weather, weather.temperature);
    
    main_page_redraw_outdoor_temperature(weather.temperature);
    main_page_redraw_outdoor_weather_icon(weather.weather_code);
}


// static void main_loop_func(void *param)
// {
//     uint32_t event;
    
//     while(1)
//     {   
//         event = ulTaskNotifyTake(pdFALSE,portMAX_DELAY);

//         if(event & MLOOP_EVT_TIME_SYNC)
//             time_sync();
//         if(event & MLOOP_EVT_WIFI_UPDATE)
//             wifi_update();
//         if(event & MLOOP_EVT_TIME_UPDATE)
//             time_update();
//         if(event & MLOOP_EVT_INNER_UPDATE)
//             inner_update();
//         if(event & MLOOP_EVT_OUTDOOR_UPDATE)
//             outdoor_update();
//     }
// }

typedef void (*app_job_t)(void);

static void app_work(void *param)
{
    app_job_t job = (app_job_t)param;
    job();
}

static void work_time_cb(TimerHandle_t timer)
{
   app_job_t job = pvTimerGetTimerID(timer);
   workqueue_run(app_work,job);
}


static void app_time_cb(TimerHandle_t timer)
{
   app_job_t job = pvTimerGetTimerID(timer);
   job();
}

void main_loop_init(void)
{   
    time_sycn_timer = xTimerCreate("time sync",1,pdFALSE,(void *)time_sync,app_time_cb);
    wifi_update_timer = xTimerCreate("wifi update",pdMS_TO_TICKS(WIFI_UPDATE_INTERVAL),pdTRUE,(void *)wifi_update,work_time_cb);
    time_update_timer = xTimerCreate("time update",pdMS_TO_TICKS(TIME_UPDATE_INTERVAL),pdTRUE,(void *)time_update,work_time_cb);
    inner_update_timer = xTimerCreate("inner update",pdMS_TO_TICKS(INNER_UPDATE_INTERVAL),pdTRUE,(void *)inner_update,work_time_cb);
    outdoor_update_timer = xTimerCreate("outdoor update",pdMS_TO_TICKS(OUTDOOR_UPDATE_INTERVAL),pdTRUE,(void *)outdoor_update,work_time_cb);

    workqueue_run(app_work,time_sync);
    workqueue_run(app_work,wifi_update);
    workqueue_run(app_work,inner_update);
    workqueue_run(app_work,outdoor_update);
    
    xTimerStart(time_sycn_timer,0);
    xTimerStart(wifi_update_timer,0);
    xTimerStart(time_update_timer,0);
    xTimerStart(inner_update_timer,0);
    xTimerStart(outdoor_update_timer,0);
}


