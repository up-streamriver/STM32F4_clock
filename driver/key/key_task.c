#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "led.h"
#include "usart.h"
#include "timers.h"

#define KEY_PRESSED_LEVEL Bit_RESET
#define KEY_SHORT_TIME 1000
#define KEY_LONG_TIME 3000

typedef struct 
{
    uint8_t long_key_pressed;
}key_param_t;

typedef enum
{
    KEY_STATE_IDLE,
    KEY_STATE_PRESS_DEBOUNCE,
    KEY_STATE_PRESS,
    KEY_STATE_RELEASE_DEBOUNCE,
}key_state_t;

static TaskHandle_t key_taskHandler;
static TimerHandle_t g_key_timer;
static key_state_t key_state = KEY_STATE_IDLE;
static uint32_t key_Pressms;
static uint8_t key_press_finished;




static void key_long_callback(void)
{
    usart_printf("long key pressed over 3 seconds\r\n");
    led_stream();
}

static void key_short_callback(void)
{
    usart_printf("short key pressed over 1 second\r\n");
    led_toggle(LED_1);
    
}

static void key_finished_check(void)
{
    if(key_Pressms >= KEY_LONG_TIME) 
    {
         key_long_callback();
    }
     else if(key_Pressms >= KEY_SHORT_TIME && key_Pressms < KEY_LONG_TIME) 
    {
         key_short_callback();
    }
    key_Pressms = 0;
    key_press_finished = 0;
}

void key_func(void)
{
    while(1)
    {
        uint8_t key_level = GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN);
        
        if(key_level == KEY_PRESSED_LEVEL && key_state == KEY_STATE_IDLE)
        {
            key_state = KEY_STATE_PRESS_DEBOUNCE;
            xTimerReset(g_key_timer,0);
            xTimerStart(g_key_timer,0);
        }
        if(key_press_finished == 1)
        {   
            key_finished_check();
        }
        if(key_state == KEY_STATE_IDLE)
        {
            ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
        
    }
}

static void key_TimerCallback(TimerHandle_t xTimer)
{
    uint8_t key_level = GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN);
    switch(key_state)
    {
        case KEY_STATE_PRESS_DEBOUNCE:
        {   
            if(key_level == KEY_PRESSED_LEVEL)
            {
                key_state = KEY_STATE_PRESS;
                key_Pressms = 0;
            }
            else
            {
                key_state = KEY_STATE_IDLE;
                xTimerStop(g_key_timer,0);
            }
            break;
        }
        case KEY_STATE_PRESS:
        {   
            if(key_level == KEY_PRESSED_LEVEL)
            {
                key_Pressms += 10;
            }
            else
            {
                key_state = KEY_STATE_RELEASE_DEBOUNCE;
            }
            break;
        }
        case KEY_STATE_RELEASE_DEBOUNCE:
        {   
            if(key_level != KEY_PRESSED_LEVEL)
            {
                key_state = KEY_STATE_IDLE;
                key_press_finished = 1;
                xTimerStop(g_key_timer,0);
            }
            else
            {
                key_state = KEY_STATE_PRESS;
            }
            break;
        }
        default:
        {
            xTimerStop(g_key_timer,0);
            break;
        }
    }
}

void key_timer_init(void)
{
    g_key_timer = xTimerCreate("key_timer",pdMS_TO_TICKS(10),pdTRUE,NULL,key_TimerCallback);
    configASSERT(g_key_timer);

    xTaskCreate(key_func,"key_func",1024,NULL,8,&key_taskHandler);
}

void EXTI4_IRQHandler(void)
{   
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    if(EXTI_GetITStatus(EXTI_Line4) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line4);       
        vTaskNotifyGiveFromISR(key_taskHandler,&pxHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
    
}