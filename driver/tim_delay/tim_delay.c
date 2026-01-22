#include "tim_delay.h"
#include <string.h>

static volatile uint32_t tim_tick_count;
static tim_periodic_callback_t tim_periodic_callback;

void tim_delay_init(void)
{   
    RCC_ClocksTypeDef RCC_CLOCK_Structure;
    RCC_GetClocksFreq(&RCC_CLOCK_Structure);
    uint32_t apb1_tim6_mhz =RCC_CLOCK_Structure.PCLK1_Frequency / 1000 / 1000 *2; //定时器频率是时钟线2倍

    TIM_TimeBaseInitTypeDef TimeBase_Structure;
    TIM_TimeBaseStructInit(&TimeBase_Structure);
    TimeBase_Structure.TIM_ClockDivision  = TIM_CKD_DIV1;
    TimeBase_Structure.TIM_CounterMode = TIM_CounterMode_Up;
    TimeBase_Structure.TIM_Period = 1000 - 1;
    TimeBase_Structure.TIM_Prescaler = apb1_tim6_mhz - 1;
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
    TIM_TimeBaseInit(TIM6,&TimeBase_Structure);
    TIM_Cmd(TIM6,ENABLE);

    NVIC_InitTypeDef NVIC_Structure;
    memset(&NVIC_Structure,0,sizeof(NVIC_Structure));
    NVIC_Structure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Structure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_Structure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_Structure);
}

uint64_t tim_now(void)
{
    uint64_t cur_count,cur_time;
    do
    {
        cur_count = tim_tick_count;
        cur_time = tim_tick_count + TIM_GetCounter(TIM6);
    } while (cur_count != cur_time);
    return cur_time;
}

uint64_t tim_get_us(void)
{
    return tim_now();
}

uint64_t tim_get_ms(void)
{
    return tim_now() / 1000;
}

void tim_delay_us(uint64_t us)
{
    uint64_t start = tim_get_us();
    while(tim_get_us() - start < us);
}

void tim_delay_ms(uint64_t ms)
{
    uint64_t start = tim_get_ms();
    while(tim_get_ms() - start < ms);
}

void tim_periodic_callback_register(tim_periodic_callback_t callback)
{
    tim_periodic_callback = callback;
}



void TIM6_DAC_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
        tim_tick_count += 1000;
        if(tim_periodic_callback)
        {
            tim_periodic_callback();
        }
    }
}