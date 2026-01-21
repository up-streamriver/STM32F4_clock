#include "st7789.h"
#include "FreeRTOS.h"
#include "task.h"
/*
SPI2 SCK PB13
SPI2 MISO PC2
SPI2 MOSI PC3

LCD_CS PE2
LCD_RESET PE3
LCD_DC PE4
LCD_LED PE5
*/

#define CS_PORT GPIOE
#define CS_PIN GPIO_Pin_2

#define RESET_PORT GPIOE
#define RESET_PIN GPIO_Pin_3

#define DC_PORT GPIOE
#define DC_PIN GPIO_Pin_4

#define LED_PORT GPIOE
#define LED_PIN GPIO_Pin_5


static void st7789_init_display(void);

static void st7789_io_init(void)
{
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);
    GPIO_InitTypeDef GPIO_Structure;
    GPIO_StructInit(&GPIO_Structure);
    GPIO_SetBits(GPIOE, CS_PIN | RESET_PIN | DC_PIN | LED_PIN);
    GPIO_ResetBits(LED_PIN, LED_PIN);

    GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Structure.GPIO_OType = GPIO_OType_PP;
    GPIO_Structure.GPIO_Pin = CS_PIN | RESET_PIN | DC_PIN | LED_PIN;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Structure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE,&GPIO_Structure);

    GPIO_Structure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Structure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB,&GPIO_Structure);

    GPIO_Structure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOC,&GPIO_Structure);
}
static void st7789_spi_init(void)
{
    SPI_InitTypeDef SPI_Structure;
    SPI_StructInit(&SPI_Structure);
    SPI_Structure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_Structure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_Structure.SPI_CPOL = SPI_CPOL_Low;
    SPI_Structure.SPI_DataSize = SPI_DataSize_8b;
    SPI_Structure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_Structure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Structure.SPI_Mode = SPI_Mode_Master;
    SPI_Structure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI2,&SPI_Structure);
    SPI_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
    SPI_Cmd(SPI2,ENABLE);
}
static void st7789_interrupt_init(void)
{

}

static void st7789_dma_init(void)
{

    DMA_InitTypeDef DMA_Structure;
    DMA_StructInit(&DMA_Structure);
    DMA_Structure.DMA_Channel = DMA_Channel_0;
    DMA_Structure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_Structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_Structure.DMA_Mode = DMA_Mode_Normal;
    DMA_Structure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
    DMA_Structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_Structure.DMA_Priority = DMA_Priority_High;
    DMA_Structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_Structure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_Structure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_Structure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
    DMA_Structure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream4,&DMA_Structure);   
}

void st7789_init(void)
{
    st7789_spi_init();

    st7789_dma_init();

    st7789_io_init();
    
    st7789_init_display();
}

static void st7789_write_register(uint8_t reg,uint8_t data[],uint16_t length)
{   
    SPI_DataSizeConfig(SPI2,SPI_DataSize_8b);
    GPIO_ResetBits(CS_PORT,CS_PIN);
    GPIO_ResetBits(DC_PORT,DC_PIN);
    SPI_SendData(SPI2,reg);
    while(SPI_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET);
    while(SPI_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY) == SET);

    GPIO_SetBits(DC_PORT,DC_PIN);
    for(uint16_t i=0;i<length;i++)
    {
        SPI_SendData(SPI2,data[i]);
        while(SPI_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET);
    }
    while(SPI_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY) == SET);

    GPIO_SetBits(CS_PORT,CS_PIN);

}

static void st7789_write_gram(uint8_t data[],uint32_t length,bool single_color)
{   
    SPI_DataSizeConfig(SPI2,SPI_DataSize_16b);
    GPIO_ResetBits(CS_PORT, CS_PIN);
    GPIO_SetBits(DC_PORT, DC_PIN);
    length >>= 1;
    do
    {
    uint32_t chunk_size = length < 65535 ? length : 65535;

    //DMA_Structure.DMA_Memory0BaseAddr = (uint32_t)data; 
    //DMA_Structure.DMA_BufferSize = chunk_size;
    //DMA_Structure.DMA_MemoryInc = single_color? DMA_MemoryInc_Disable : DMA_MemoryInc_Enable;
    DMA1_Stream4->M0AR = (uint32_t)data; 
    DMA1_Stream4->NDTR = chunk_size;
    if(single_color)    DMA1_Stream4->CR  &= ~DMA_SxCR_MINC;
    else                DMA1_Stream4->CR  |= DMA_SxCR_MINC;
 
    DMA_Cmd(DMA1_Stream4,ENABLE);    
    while(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4) == RESET);
    DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4); 
    if(!single_color)    data += chunk_size * 2;  
    length -= chunk_size;  
    }  while(length > 0);
   while(SPI_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY) == SET);   
   GPIO_SetBits(CS_PORT, CS_PIN); 
}

static void st7789_reset(void)
{
    GPIO_ResetBits(RESET_PORT,RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(1)); //20us
    GPIO_SetBits(RESET_PORT, RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(120));
}

static void st7789_set_backlight(bool on)
{
    GPIO_WriteBit(LED_PORT, LED_PIN, on ? Bit_SET : Bit_RESET);
}


static void st7789_init_display(void)
{
    st7789_reset();
   
    st7789_write_register(0x11,NULL,0); //sleep out

    vTaskDelay(pdMS_TO_TICKS(5));

    st7789_write_register(0x36,(uint8_t[]){0x00},1); //Memory Data Access Control 基本显示配置
    st7789_write_register(0x3A,(uint8_t[]){0x55},1); //设置颜色格式rgb565
    st7789_write_register(0xB2,(uint8_t[]){0x0C,0x0C,0x00,0x33,0x33},5); //设置前廊、后廊等时序参数
    st7789_write_register(0xB7,(uint8_t[]){0x46},1); //设置门控扫描方式
    st7789_write_register(0xBB,(uint8_t[]){0x1B},1);
    st7789_write_register(0xC0,(uint8_t[]){0x2C},1);
    st7789_write_register(0xC2,(uint8_t[]){0x01},1);
    st7789_write_register(0xC3,(uint8_t[]){0x0F},1);
    st7789_write_register(0xC4,(uint8_t[]){0x20},1);
    st7789_write_register(0xC6,(uint8_t[]){0x0F},1); // 设置正常模式下的帧率，0x0F表示约60Hz
    st7789_write_register(0xD0,(uint8_t[]){0xA4,0xA1},2);
    st7789_write_register(0xD6,(uint8_t[]){0xA1},1);
    st7789_write_register(0xE0,(uint8_t[]){0xF0,0x00,0x06,0x04,0x05,0x05,0x31,0x44,0x48,0x36,0x12,0x12,0x2B,0x34},14);
    st7789_write_register(0xE1,(uint8_t[]){0xF0,0x0B,0x0F,0x0F,0x0D,0x26,0x31,0x43,0x47,0x38,0x14,0x14,0x2C,0x32},14);
    st7789_write_register(0x21,NULL,0);
    st7789_write_register(0x29,NULL,0);

    st7789_fill_color(0, 0, st7789_WIDTH - 1, st7789_HEIGHT - 1, 0x0000);
    st7789_set_backlight(true);    

}

static bool st7789_is_in_range(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    if(x1 > st7789_WIDTH || x2 > st7789_WIDTH)
    {
        return false;
    }
    if(y1 > st7789_HEIGHT || y2 > st7789_HEIGHT)
    {
        return false;
    }
    if(x1 > x2 || y1 > y2)
    {
        return false;
    }    
    return true;
}

static void st7789_setCursor(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    st7789_write_register(0x2A,(uint8_t []){(x1>>8) & 0xFF,x1 & 0xFF,(x2>>8) & 0xFF,x2 & 0xFF},4);
    st7789_write_register(0x2B,(uint8_t []){(y1>>8) & 0xFF,y1 & 0xFF,(y2>>8) & 0xFF,y2 & 0xFF},4);  
}

static void st7789_set_gram_mode(void)
{
    st7789_write_register(0x2C,NULL,0);
}

void st7789_fill_color(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
    if(!st7789_is_in_range(x1,y1,x2,y2))
    {
        return;
    }
    st7789_setCursor(x1,y1,x2,y2);
    st7789_set_gram_mode();
    uint32_t pixels = (x2 - x1 + 1) * (y2 - y1 + 1);
    st7789_write_gram((uint8_t *)&color,pixels * 2,true);
}

static void st7789_draw_font(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t *model,uint16_t color,uint16_t bg_color)
{
    uint16_t bytes_per_row = (width+7) / 8;
    static uint8_t buff[72 * 72 * 2]; 
    uint8_t *pbuf = buff;
    for(uint16_t row=0;row < height;row++)
    {
        const uint8_t * row_data = model +  row * bytes_per_row;
        for(uint16_t column=0; column < width;column++)
        {
            uint8_t pixel = row_data[column / 8] & (1<<(7 - column % 8));
            uint16_t color_pixel = pixel ? color : bg_color;
            *pbuf++ = color_pixel & 0xff;
            *pbuf++ = (color_pixel >> 8) & 0xff; 
        }       
    }
    st7789_setCursor(x,y,x + width -1,y + height - 1);
    st7789_set_gram_mode();  
    st7789_write_gram(buff,pbuf - buff,false);
}

static void st7789_write_ascii(uint16_t x,uint16_t y,char ch,uint16_t color,uint16_t bg_color,const font_t *font)
{
    if(font == NULL)
    {
        return;
    }
    uint16_t fheight = font->size, fwidth = font->size / 2;
    if(!st7789_is_in_range(x,y,x + fwidth -1,y + fheight - 1))
    {
        return;
    }
    if(ch < 0x20 || ch > 0x7E)
    {
        return;
    }
    uint16_t bytes_per_row = (fwidth + 7) / 8;
	const uint8_t *model = font->ascii_model + (ch - ' ') * fheight * bytes_per_row;
    st7789_draw_font(x, y, fwidth, fheight, model, color, bg_color);       
}

static void st7789_write_chinese(uint16_t x, uint16_t y, char *ch, uint16_t color, uint16_t bg_color, const font_t *font)
{
    if(font == NULL || ch == NULL)
    {
        return;
    }
    uint16_t fheight = font->size, fwidth = font->size;
    if(!st7789_is_in_range(x,y,x + fwidth -1,y + fheight - 1))
    {
        return;
    }
    const font_chinese_t *c = font->chinese;
    for(;c->name != NULL;c++)
    {
        if(strcmp(c->name,ch) ==0)
            break;
    }
    if(c->name == NULL)
        return;
    st7789_draw_font(x, y, fwidth, fheight, c->model, color, bg_color); 
}


static bool is_gb2312(char ch)
{
    return ((unsigned char)ch >= 0xA1 && (unsigned char)ch <= 0xF7);
}


void st7789_write_string(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg_color, const font_t *font)
{
    while (*str)
    {
        // int len = utf8_char_length(*str);
        int len = is_gb2312(*str) ? 2 : 1;
        if (len <= 0)
        {
            str++;
            continue;
        }
        else if (len == 1)
        {
            st7789_write_ascii(x, y, *str, color, bg_color, font);
            str++;
            x += font->size / 2;
        }
        else
        {
            char ch[5];
            strncpy(ch, str, len);
            st7789_write_chinese(x, y, ch, color, bg_color, font);
            str += len;
            x += font->size; 
        }
    }
}

void st7789_draw_image(uint16_t x, uint16_t y,const image_t *font_image)
{
    uint16_t height = font_image ->height;
    uint16_t width = font_image ->width;
    if(!st7789_is_in_range(x,y,x + width - 1,y + height - 1))
    {
        return;
    }
    uint32_t length = height * width * 2;
    st7789_setCursor(x,y,x + width -1,y + height - 1);
    st7789_set_gram_mode();  
    st7789_write_gram((uint8_t *)font_image ->data,length,false);
}
