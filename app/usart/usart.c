#include "usart.h"

#define USART1_PORT GPIOA
#define USART1_TX_PIN GPIO_Pin_9
#define USART1_RX_PIN GPIO_Pin_10


usart_recv_callback_t usart_recv_callback;

void usart_recv_handler_register(usart_recv_callback_t callback)
{
    usart_recv_callback = callback;
}

static void usart_io_init(void)
{
    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    
    GPIO_PinAFConfig(USART1_PORT,GPIO_PinSource9,GPIO_AF_USART1);
    GPIO_PinAFConfig(USART1_PORT,GPIO_PinSource10,GPIO_AF_USART1);
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Structure.GPIO_OType = GPIO_OType_PP;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Structure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART1_PORT,&GPIO_Structure);
}
static void usart_usart_init(void)
{
    USART_InitTypeDef USART_Structure;
    USART_StructInit(&USART_Structure);
    USART_Structure.USART_BaudRate = 115200;
    USART_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Structure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Structure.USART_Parity = USART_Parity_No;
    USART_Structure.USART_StopBits = USART_StopBits_1;
    USART_Structure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1,&USART_Structure);
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
    USART_Cmd(USART1,ENABLE);

}
static void usart_interrupt_init(void)
{
       
    NVIC_InitTypeDef NVIC_Structure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    memset(&NVIC_Structure,0,sizeof(NVIC_Structure));
    NVIC_Structure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Structure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Structure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_Structure);
}
static void usart_dma_init(void)
{
    DMA_InitTypeDef DMA_Structure;
    DMA_StructInit(&DMA_Structure);
    DMA_Structure.DMA_Channel = DMA_Channel_4;
    DMA_Structure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_Structure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_Structure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_Structure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
    DMA_Structure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_Structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Structure.DMA_Mode = DMA_Mode_Normal;
    DMA_Structure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_Structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_Structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_Structure.DMA_Priority = DMA_Priority_Medium;
    DMA_Structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Init(DMA2_Stream7,&DMA_Structure);     
}

void usart_init(void)
{
    usart_usart_init();
    usart_dma_init();   
    usart_interrupt_init();
    usart_io_init();
}   


void usart_write_data(uint8_t *data,uint32_t length)
{	
    DMA2_Stream7->M0AR = (uint32_t)data; 
    DMA2_Stream7->NDTR = length;
    DMA_Cmd(DMA2_Stream7,ENABLE);
    while(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7) == RESET);
    DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);   
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);    
    USART_ClearFlag(USART1,USART_FLAG_TC);
}

void usart_write_string(const char *str)
{
    uint32_t length = strlen(str);
    usart_write_data((uint8_t *)str,length);
}

void usart_printf(char * format,...)
{
    static char buffer[512];
    va_list arg;
    memset(buffer,0,sizeof(buffer));
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