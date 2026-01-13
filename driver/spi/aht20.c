#include "aht20.h"

/*
I2C2 SCL PB10
I2C2 SDA PB11
*/


extern void delay_ms(uint32_t ms);

bool aht20_get_status(uint8_t *status);
bool aht20_is_ready(void);


bool aht20_init(void)
{
    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Structure.GPIO_OType = GPIO_OType_OD;
    GPIO_Structure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_Structure);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_I2C2);

    I2C_InitTypeDef I2C_Structure;
    I2C_StructInit(&I2C_Structure);
    I2C_Structure.I2C_Ack = I2C_Ack_Enable;
    I2C_Structure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Structure.I2C_ClockSpeed = 100ul * 1000ul;
    I2C_Structure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_Structure.I2C_Mode = I2C_Mode_I2C;
    I2C_Structure.I2C_OwnAddress1 = 0x00;
    I2C_Init(I2C2,&I2C_Structure);

    delay_ms(40 * 1000);

    if(aht20_is_ready())  return true;
    uint8_t data[3]={0xBE,0x08,0x00};
    if(!aht20_write(data,3)) return false;

    for(uint8_t i=0;i<100;i++)
    {
        delay_ms(1000);
        if(aht20_is_ready()) return true;
    }

    return false;

}

#define I2C_CHECK_EVENT(EVENT,TIMEOUT) \
    do{ \
        uint32_t timeout = TIMEOUT; \ 
        while(!I2C_CheckEvent(I2C2,EVENT) && timeout > 0){\
            delay_ms(10); \
            timeout -= 10; \
        }\
        if(timeout <= 0) \
            return false; \
      }while(0)




bool aht20_write(uint8_t data[],uint32_t length)
{
    I2C_AcknowledgeConfig(I2C2,ENABLE);
    I2C_GenerateSTART(I2C2,ENABLE);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,1000);
    I2C_Send7bitAddress(I2C2,0x70,I2C_Direction_Transmitter);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,1000);
    for(uint32_t i=0;i<length;i++)
    {
        I2C_SendData(I2C2,data[i]);
        I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTING,1000);
    }
    I2C_GenerateSTOP(I2C2,ENABLE);

    return true;
}

bool aht20_read(uint8_t data[],uint32_t length)
{

    I2C_AcknowledgeConfig(I2C2,ENABLE);
    I2C_GenerateSTART(I2C2,ENABLE);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,1000);
    I2C_Send7bitAddress(I2C2,0x70,I2C_Direction_Receiver);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,1000);
    for(uint32_t i=0;i<length;i++)
    {
        if(i == length - 1)
        {
            I2C_AcknowledgeConfig(I2C2,DISABLE);
        }
        I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_RECEIVED,1000);
        data[i] = I2C_ReceiveData(I2C2);
    }
    I2C_GenerateSTOP(I2C2,ENABLE);
    
    return true;
}

bool aht20_get_status(uint8_t *status)
{
    uint8_t cmd = 0x71;
    if(!aht20_write(&cmd,1)) return false;
    if(!aht20_read(*status,1)) return false;
    return true;
}

bool aht20_is_ready(void)
{
    uint8_t data;
    if(!aht20_get_status(&data)) return false;
    if(data & 0x08 !=0)        return true;
    return false;
}

bool aht20_is_busy(void)
{
    uint8_t data;
    if(!aht20_get_status(&data)) return true;
    if(data & 0x80 !=0)        return true;
    return false;
}

bool aht20_trigger_measure(void)
{   
    uint8_t data[3] = {0xAC,0x33,0x00};
    return aht20_write(data,3);
}

bool aht20_wait_measure(void)
{
    for(uint8_t i=0;i<200;i++)
    {
        delay_ms(1000);
        if(!aht20_is_busy())    return true;
    }
    return false;
}

bool aht20_get_measure(float *humi,float *temp)
{   
    uint8_t cmd = 0x71;
    uint8_t data[6] = {0};
    if(!aht20_write(&cmd,1)) return false;
    if(!aht20_read(data,6))  return false;

    uint32_t raw_humi = ((uint32_t)data[1] << 12)  |
                        ((uint32_t)data[2] << 4)   |
                        ((uint32_t)(data[3] & 0xF0) >>4);
    uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) <<16) |
                        ((uint32_t)data[4] << 8)  |
                        ((uint32_t)data[5]) ;
                        
    *humi = raw_humi * 100.0f / (float)0x100000;
    *temp = raw_temp * 200.0f / (float)0x100000  - 50.0f;

    return true;
}