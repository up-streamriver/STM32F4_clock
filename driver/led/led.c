#include "led.h"
#include "FreeRTOS.h"
#include "task.h"


static const struct 
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
}led_pin_map[]= LED_DESC_MAP;

#define BREATH_DELAY_MS   200       // 步长延时，保持BREATH_DELAY_MSms，渐变不卡顿
#define DUTY_MIN          0       // 最小占空比（熄灭）
#define DUTY_MAX          999     // 最大占空比（最亮）

// 呼吸灯任务（优先级2，和按键任务同级，无冲突）
void breath_led_task(void *pvParameters)
{
    (void)pvParameters;
    while(1)
    {
        TIM_SetCompare3(TIM3, DUTY_MIN);    // 熄灭
        vTaskDelay(pdMS_TO_TICKS(BREATH_DELAY_MS));
        TIM_SetCompare3(TIM3, 200);  // 微亮
        vTaskDelay(pdMS_TO_TICKS(BREATH_DELAY_MS));
        TIM_SetCompare3(TIM3, 500);  // 半亮
        vTaskDelay(pdMS_TO_TICKS(BREATH_DELAY_MS));
        TIM_SetCompare3(TIM3, 700);  // 亮
        vTaskDelay(pdMS_TO_TICKS(BREATH_DELAY_MS));
        TIM_SetCompare3(TIM3, DUTY_MAX);  // 最亮
        vTaskDelay(pdMS_TO_TICKS(BREATH_DELAY_MS));
    }
}
void led_init(void)
{
    GPIO_InitTypeDef GPIOStructure;
    GPIOStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIOStructure.GPIO_OType = GPIO_OType_PP;
    GPIOStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOStructure.GPIO_Speed = GPIO_Speed_50MHz;
    for(led_id_t i=LED_1; i<LED_MAX ; i++)
    {
        GPIOStructure.GPIO_Pin = led_pin_map[i].GPIO_Pin;
        GPIO_Init(led_pin_map[i].GPIOx,&GPIOStructure);
    }
}

/*PB0 TIM3_CH3*/
void led_pwm_init(void)
{   
    GPIO_PinAFConfig(led_pin_map[0].GPIOx,GPIO_PinSource0,GPIO_AF_TIM3);
    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Structure.GPIO_OType = GPIO_OType_PP;
    GPIO_Structure.GPIO_Pin = led_pin_map[0].GPIO_Pin;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(led_pin_map[0].GPIOx,&GPIO_Structure);    

    TIM_TimeBaseInitTypeDef TIM_Base_Structure;
    TIM_TimeBaseStructInit(&TIM_Base_Structure);
    TIM_Base_Structure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_Base_Structure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_Base_Structure.TIM_Period = 1000 - 1;
    TIM_Base_Structure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseInit(TIM3,&TIM_Base_Structure);

    TIM_OCInitTypeDef TIM_OC_Structure;
    TIM_OCStructInit(&TIM_OC_Structure);
    TIM_OC_Structure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OC_Structure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC_Structure.TIM_Pulse = 500 - 1;
    TIM_OC_Structure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC3Init(TIM3,&TIM_OC_Structure);
    TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3,ENABLE);


    TIM_Cmd(TIM3,ENABLE);

    xTaskCreate(breath_led_task,"breath_led",1024,NULL,5,NULL);
}

void led_set(led_id_t id,bool status)
{   
    if(id >= LED_MAX)
    {
        return;
    }
    GPIO_WriteBit(led_pin_map[id].GPIOx,led_pin_map[id].GPIO_Pin,status? Bit_RESET:Bit_SET);
}

void led_toggle(led_id_t id)
{
    if(id >= LED_MAX)
    {
        return;
    }
    uint8_t status = GPIO_ReadOutputDataBit(led_pin_map[id].GPIOx,led_pin_map[id].GPIO_Pin);
    bool state = (status == Bit_SET) ? true : false;
    led_set(id,state);    
}



static void led_set_all(bool status)
{
    for(led_id_t i = LED_1;i < LED_MAX;i++)
    {
        led_set(i,status);
    }
}

static void led_set_only(led_id_t id,bool status)
{   
    led_set_all(false);
    led_set(id,true);
}

void led_stream(void)
{
	for(int i=0; i<LED_STREAM_CNT; i++)
	{
        led_set_only(LED_1,true);
        vTaskDelay(pdMS_TO_TICKS(LED_STREAM_DELAY_MS));
        led_set_only(LED_2,true);
        vTaskDelay(pdMS_TO_TICKS(LED_STREAM_DELAY_MS));
        led_set_only(LED_3,true);
        vTaskDelay(pdMS_TO_TICKS(LED_STREAM_DELAY_MS));  
	}
	
	led_set_all(false);
}

