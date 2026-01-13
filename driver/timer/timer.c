#include "timer.h"

void timer_init(void)
{   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    TIM_TimeBaseInitTypeDef TimBaseStructure;
    TimBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TimBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TimBaseStructure.TIM_Period = 1000 - 1;
    TimBaseStructure.TIM_Prescaler = 84-1;
    TIM_TimeBaseInit(TIM6,&TimBaseStructure);

    NVIC_InitTypeDef NVICStructure;
    memset(&NVICStructure,0,sizeof(NVICStructure));
    NVICStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVICStructure.NVIC_IRQChannelCmd = ENABLE;
    NVICStructure.NVIC_IRQChannelSubPriority = 0;
    NVICStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVICStructure);

    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM6,ENABLE);

}

//pb3 tim2
/*
PWM频率：	Freq = CK_PSC / (PSC + 1) / (ARR + 1)
PWM占空比：	Duty = CCR / (ARR + 1)
PWM分辨率：	Reso = 1 / (ARR + 1)
*/
void timer_pwm_init(void)
{   
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2);

    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Structure.GPIO_OType = GPIO_OType_PP;
		GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Structure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_Structure);

    TIM_TimeBaseInitTypeDef TimBaseStructure;
    TIM_TimeBaseStructInit(&TimBaseStructure);
    TimBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TimBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TimBaseStructure.TIM_Prescaler = 84 - 1;
    TimBaseStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseInit(TIM2,&TimBaseStructure);

    TIM_OCInitTypeDef TimOcStructure;
    TIM_OCStructInit(&TimOcStructure);
    TimOcStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TimOcStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TimOcStructure.TIM_OutputState = TIM_OutputState_Enable;
    TimOcStructure.TIM_Pulse = 500;    
    TIM_OC2Init(TIM2,&TimOcStructure);

    TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    TIM_Cmd(TIM2,ENABLE);

}

void TIM6_DAC_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
    {   
        TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
        static int cnt = 0;
        if(++cnt >1000)
        {
            cnt = 0;
            led_toggle();
        }       
        
    }
}