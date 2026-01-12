#include "spi.h"

/*
SPI2 SCK PB13
SPI2 MISO PC2
SPI2 MOSI PC3
*/

void spi_init(void)
{
    GPIO_InitTypeDef GPIOStructure;
    GPIO_StructInit(&GPIOStructure);
    GPIOStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIOStructure.GPIO_OType = GPIO_OType_PP;
    GPIOStructure.GPIO_Pin = GPIO_Pin_13 ;
    GPIOStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIOStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB,&GPIOStructure);
    GPIOStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2;
    GPIO_Init(GPIOC,&GPIOStructure);
    

    GPIO_PinAFConfig(GPIOC,GPIO_PinSource13,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource2,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI2);

    SPI_InitTypeDef SPIStructure;
    SPI_StructInit(&SPIStructure);
    SPIStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPIStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPIStructure.SPI_CPOL = SPI_CPOL_Low;
    SPIStructure.SPI_DataSize = SPI_DataSize_8b;
    SPIStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPIStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPIStructure.SPI_Mode = SPI_Mode_Master;
    SPIStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI2,&SPIStructure);
    SPI_Cmd(SPI2,ENABLE);
    
}