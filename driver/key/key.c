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

typedef void (*key_callback_t)(void);

static TaskHandle_t key_taskHandler;

static TimerHandle_t g_key_timer;


static void key1_long_callback(void)
{
    usart_printf("long key1 pressed over 3 seconds\r\n");
    led_stream();
}

static void key1_short_callback(void)
{
    usart_printf("short key1 pressed over 1 second\r\n");
    led_toggle(LED_1);
    
}

static void key2_long_callback(void)
{
    usart_printf("long key2 pressed over 3 seconds\r\n");
    led_stream();
}

static void key2_short_callback(void)
{
    usart_printf("short key2 pressed over 1 second\r\n");
    led_toggle(LED_1);
    
}

static void key3_long_callback(void)
{
    usart_printf("long key3 pressed over 3 seconds\r\n");
    led_stream();
}

static void key3_short_callback(void)
{
    usart_printf("short key3 pressed over 1 second\r\n");
    led_toggle(LED_1);
    
}


static  struct 
{
    const GPIO_TypeDef* GPIOx;
    const uint16_t GPIO_Pin;
    const uint32_t EXTI_Line;
    const uint8_t EXTI_PortSourceGPIOx;
    const uint8_t EXTI_PinSourcex;
    const uint8_t NVIC_IRQChannel;
    const key_callback_t long_callback;
    const key_callback_t short_callback;
    uint8_t state;
    uint8_t finish;
    uint32_t key_Pressms;     
}key_pin_map[] = KEY_DESC_MAP;


void key_init(void)
{
    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    
    EXTI_InitTypeDef EXTI_Structure;
    EXTI_StructInit(&EXTI_Structure);
    EXTI_Structure.EXTI_LineCmd = ENABLE;
    EXTI_Structure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_Structure.EXTI_Trigger = EXTI_Trigger_Falling;
    

    NVIC_InitTypeDef NVIC_Structure;
    memset(&NVIC_Structure,0,sizeof(NVIC_Structure));
    NVIC_Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Structure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_Structure.NVIC_IRQChannelSubPriority = 0;
    for(key_id_t i = KEY_1; i<KEY_MAX; i++)
    {   
        GPIO_Structure.GPIO_Pin = key_pin_map[i].GPIO_Pin;
        GPIO_Init(key_pin_map[i].GPIOx,&GPIO_Structure);
        EXTI_Structure.EXTI_Line = key_pin_map[i].EXTI_Line;
        SYSCFG_EXTILineConfig(key_pin_map[i].EXTI_PortSourceGPIOx,key_pin_map[i].EXTI_PinSourcex);
        EXTI_Init(&EXTI_Structure);
        NVIC_Structure.NVIC_IRQChannel = key_pin_map[i].NVIC_IRQChannel;
        NVIC_Init(&NVIC_Structure);
    }

}


uint8_t key_get_level(key_id_t id)
{
    return GPIO_ReadInputDataBit(key_pin_map[id].GPIOx,key_pin_map[id].GPIO_Pin);
}

// bool key_is_pressed(void)
// {
//    return  GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN) == Bit_RESET;
// }






static void key_finished_check(key_id_t id)
{   
    if(key_pin_map[id].key_Pressms >= KEY_LONG_TIME)
    {
       key_pin_map[id].long_callback();
    }
    else if(key_pin_map[id].key_Pressms >= KEY_SHORT_TIME && key_pin_map[id].key_Pressms < KEY_LONG_TIME)
    {
        key_pin_map[id].short_callback();
    }
    key_pin_map[id].key_Pressms = 0;
    key_pin_map[id].finish = 0;

}

void key_func(void)
{
    while(1)
    {   

        for(key_id_t i = KEY_1 ; i < KEY_MAX ; i++)
        {
            uint8_t key_level = key_get_level(i);
            if(key_level == KEY_PRESSED_LEVEL && key_pin_map[i].state == KEY_STATE_IDLE)
            {   
                vTimerSetTimerID(g_key_timer,i);
                key_pin_map[i].state = KEY_STATE_PRESS_DEBOUNCE;
                xTimerReset(g_key_timer,0);
                xTimerStart(g_key_timer,0);
            }
            if(key_pin_map[i].finish == 1)
            {   
                key_finished_check(i);
            }
            
        }
        uint8_t all_idle = 1;
        for(key_id_t i = KEY_1 ; i < KEY_MAX ; i++)
        {
            if(key_pin_map[i].state != KEY_STATE_IDLE)
            {
                all_idle = 0;
            }
        }
        if(all_idle)
        {
            ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
        
    }
}

static void key_TimerCallback(TimerHandle_t xTimer)
{   
    key_id_t id = (key_id_t)(pvTimerGetTimerID(xTimer));
    uint8_t key_level = key_get_level(id);
    switch(key_pin_map[id].state)
    {
        case KEY_STATE_PRESS_DEBOUNCE:
        {   
            if(key_level == KEY_PRESSED_LEVEL)
            {
                key_pin_map[id].state = KEY_STATE_PRESS;
                key_pin_map[id].key_Pressms = 0;
            }
            else
            {
                key_pin_map[id].state = KEY_STATE_IDLE;
                xTimerStop(g_key_timer,0);
            }
            break;
        }
        case KEY_STATE_PRESS:
        {   
            if(key_level == KEY_PRESSED_LEVEL)
            {
                key_pin_map[id].key_Pressms += 10;
            }
            else
            {
                key_pin_map[id].state = KEY_STATE_RELEASE_DEBOUNCE;
            }
            break;
        }
        case KEY_STATE_RELEASE_DEBOUNCE:
        {   
            if(key_level != KEY_PRESSED_LEVEL)
            {   
                key_pin_map[id].state= KEY_STATE_IDLE;
                key_pin_map[id].finish = 1;
                xTimerStop(g_key_timer,0);
            }
            else
            {
                key_pin_map[id].state= KEY_STATE_PRESS;
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
    g_key_timer = xTimerCreate("key_timer",pdMS_TO_TICKS(10),pdTRUE,1,key_TimerCallback);
    configASSERT(g_key_timer);

    xTaskCreate(key_func,"key_func",1024,NULL,8,&key_taskHandler);
}

static void key_exti_handler(key_id_t id)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    for(key_id_t i = KEY_1; i< KEY_MAX; i++)
    {
        if( i == id)
        {
            if(EXTI_GetITStatus(key_pin_map[i].EXTI_Line) == SET)
            {
                EXTI_ClearITPendingBit(key_pin_map[i].EXTI_Line);
                vTaskNotifyGiveFromISR(key_taskHandler,&pxHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(pxHigherPriorityTaskWoken); 
            }
        }
    }
}



void EXTI0_IRQHandler(void)
{   
    key_exti_handler(KEY_1);  
}

void EXTI4_IRQHandler(void)
{   
    key_exti_handler(KEY_2);
}


void EXTI9_5_IRQHandler(void)
{   
    key_exti_handler(KEY_3);
}

