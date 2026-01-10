#include "usart.h"

#define USART1_PORT GPIOA
#define USART1_TX_PIN GPIO_Pin_9
#define USART1_RX_PIN GPIO_Pin_10

#define USART2_PORT GPIOA
#define USART2_TX_PIN GPIO_Pin_2
#define USART2_RX_PIN GPIO_Pin_3
usart_recv_callback_t usart_recv_callback;

void usart_recv_handler_register(usart_recv_callback_t callback)
{
    usart_recv_callback = callback;
}

void usart_init(void)
{
    GPIO_InitTypeDef GPIO_Structure;
    USART_InitTypeDef USART_Structure;
    NVIC_InitTypeDef NVIC_Structure;

    GPIO_PinAFConfig(USART1_PORT,GPIO_PinSource9,GPIO_AF_USART1);
    GPIO_PinAFConfig(USART1_PORT,GPIO_PinSource10,GPIO_AF_USART1);
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Structure.GPIO_OType = GPIO_OType_PP;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Structure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART1_PORT,&GPIO_Structure);

    USART_StructInit(&USART_Structure);
    USART_Structure.USART_BaudRate = 115200;
    USART_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Structure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Structure.USART_Parity = USART_Parity_No;
    USART_Structure.USART_StopBits = USART_StopBits_1;
    USART_Structure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1,&USART_Structure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    memset(&NVIC_Structure,0,sizeof(NVIC_Structure));
    NVIC_Structure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Structure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Structure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_Structure);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART1,ENABLE);
}

void usart_write_data(uint8_t *data,uint32_t length)
{	
    for(uint32_t i=0;i<length;i++)
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
        USART_SendData(USART1,data[i]);
    }
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
}

void usart_write_string(const char *str)
{
    uint32_t length = strlen(str);
    usart_write_data((uint8_t *)str,length);
}

void usart_printf(char * format,...)
{
    char buffer[128];
    va_list arg;
    va_start(arg,format);
    vsnprintf(buffer,sizeof(buffer),format,arg);
    va_end(arg);
    usart_write_string(buffer);
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        if(usart_recv_callback)
        {
            usart_recv_callback(data);
        }
    }
}