#include "bl24c512.h"

/*
I2C1 SCL PB6
I2C1 SDA PB7
*/

#define BL24C512_PAGE_SIZE 128

extern void delay_us(uint32_t ms);

void bl24c512_init(void)
{
    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    GPIO_Structure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Structure.GPIO_OType = GPIO_OType_OD;
    GPIO_Structure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_Structure);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);

    I2C_InitTypeDef I2C_Structure;
    I2C_StructInit(&I2C_Structure);
    I2C_Structure.I2C_Ack = I2C_Ack_Enable;
    I2C_Structure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Structure.I2C_ClockSpeed = 100ul * 1000ul;
    I2C_Structure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_Structure.I2C_Mode = I2C_Mode_I2C;
    I2C_Structure.I2C_OwnAddress1 = 0x00;
    I2C_Init(I2C1,&I2C_Structure);
}

#define I2C_CHECK_EVENT(EVENT,TIMEOUT) \
    do{ \
        uint32_t timeout = TIMEOUT; \ 
        while(!I2C_CheckEvent(I2C1,EVENT) && timeout > 0){\
            delay_us(10); \
            timeout -= 10; \
        }\
        if(timeout <= 0) \
            return false; \
      }while(0)

static bool bl24c512_ready(void)
{
    I2C_AcknowledgeConfig(I2C1,ENABLE);
    I2C_GenerateSTART(I2C1,ENABLE);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,100);
    I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Transmitter);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,100);
    I2C_GenerateSTOP(I2C1,ENABLE);
    return true;
}

static bool bl24c512_wait_ready(uint32_t timeout)
{
    while(timeout >0)
    {
        if(bl24c512_ready())
            return true;
        delay_us(10);
        timeout -= 10;
    }
    return false;
}

static bool bl24c512_page_write(uint16_t address,uint8_t data[],uint32_t length)
{
    I2C_AcknowledgeConfig(I2C1,ENABLE);
    I2C_GenerateSTART(I2C1,ENABLE);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,1000);
    I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Transmitter);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,1000);
    I2C_SendData(I2C1,(address >> 8) & 0xff);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTING,1000);
    I2C_SendData(I2C1,address & 0xff);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTING,1000);
    for(uint32_t i=0;i<length;i++)
    {
        I2C_SendData(I2C1,data[i]);
        I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTING,1000);
    }
    I2C_GenerateSTOP(I2C1,ENABLE);

    return true;
}

static bool bl24c512_page_read(uint16_t address,uint8_t data[],uint32_t length)
{
    I2C_AcknowledgeConfig(I2C1,ENABLE);
    I2C_GenerateSTART(I2C1,ENABLE);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,1000);
    I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Transmitter);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,1000); 
    I2C_SendData(I2C1,(address >> 8) & 0xff);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTING,1000);
    I2C_SendData(I2C1,address & 0xff);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTING,1000);
    
    I2C_AcknowledgeConfig(I2C1,ENABLE);
    I2C_GenerateSTART(I2C1,ENABLE);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_MODE_SELECT,1000);
    I2C_Send7bitAddress(I2C1,0xA0,I2C_Direction_Receiver);
    I2C_CHECK_EVENT(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,1000);
    for(uint32_t i=0;i<length;i++)
    {
        if(i == length - 1)
        {
            I2C_AcknowledgeConfig(I2C1,DISABLE);
        }
        I2C_CHECK_EVENT(I2C_EVENT_MASTER_BYTE_RECEIVED,1000);
        data[i] = I2C_ReceiveData(I2C1);
    }
    I2C_GenerateSTOP(I2C1,ENABLE);
    
    return true;
}

bool bl24c512_write(uint16_t address,uint8_t data[],uint32_t length)
{
    uint32_t available_size = BL24C512_PAGE_SIZE - (address % BL24C512_PAGE_SIZE);
    uint32_t write_size = length < available_size ? length : available_size;
    while(length > 0)
    {
        if(!bl24c512_wait_ready(5 * 1000))
        {
            return false;
        }
        if(bl24c512_page_write(address,data,write_size) == false)
        {
            return false;
        }
        address += write_size;
        data += write_size;
        length -= write_size;
        write_size = length < BL24C512_PAGE_SIZE ? length : BL24C512_PAGE_SIZE;
    }
    return true;
}

bool bl24c512_read(uint16_t address,uint8_t data[],uint32_t length)
{
    uint32_t available_size = BL24C512_PAGE_SIZE - (address % BL24C512_PAGE_SIZE);
    uint32_t read_size = length < available_size ? length : available_size;
    while(length>0)
    {
        if(!bl24c512_page_read(address,data,read_size))
        {
            return false;
        }
        address += read_size;
        data += read_size;
        length -= read_size;
        read_size = length < BL24C512_PAGE_SIZE ? length : BL24C512_PAGE_SIZE;
    }
    return true;
}
#ifdef test
void test(void)
{
		const char default_sn[] = "mhdev_1234567890";
	char sn[17];
	if(bl24c512_read(EEPROM_SN_OFFSET,(uint8_t *)sn,sizeof(default_sn)))
	{	
		
		if(memcmp(default_sn,sn,10) == 0)
		{
			usart_printf("board sn : %s\r\n",sn);
		}
		else
		{
			if(bl24c512_write(EEPROM_SN_OFFSET,(uint8_t *)default_sn,sizeof(default_sn)))
			{
				usart_printf("write board sn success \r\n");
			}
			else
			{
				usart_printf("write board sn error \r\n");
			}
		}
	}
	else
	{
		usart_printf("read error \r\n");
	}
	union{uint32_t value; uint8_t data[4];}boot_count;
	if(bl24c512_read(EEPROM_BOOT_COUNT,boot_count.data,sizeof(boot_count)))
	{
		if(boot_count.value < 100 * 1000)
		{
			boot_count.value ++ ;
		}
		else
		{
			boot_count.value = 0;
		}
		usart_printf("boot count: %u\r\n", boot_count.value);
		if(!bl24c512_write(EEPROM_BOOT_COUNT,(uint8_t *)boot_count.data,sizeof(boot_count)))
		{
			usart_printf("write board bootcnt failed \r\n");
		}
	}
	else
	{
		usart_printf("cnt error \r\n");
	}
}
#endif