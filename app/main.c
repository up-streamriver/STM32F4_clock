#include "board.h"

#define USART_RX_BUFFER_SIZE 512
#define EEPROM_SN_OFFSET    120
#define EEPROM_BOOT_COUNT   30

static uint8_t usart_rx_buffer[USART_RX_BUFFER_SIZE];
static ringbuffer_t serial_rb;




void usart_recv_handler(uint8_t data)
{
	rb8_puts(serial_rb,&data,1);
}

static void wait_release(void)
{
	while(key_is_pressed())
		delay_us(100);
}

static bool key_press(void)
{
	if(key_is_pressed())
		{
			delay_us(100);
			return key_is_pressed();
		}
}

int main(void)
{	
	board_init();
	led_init();
	delay_init();
	serial_rb = rb8_new(usart_rx_buffer,USART_RX_BUFFER_SIZE);
	usart_recv_handler_register(usart_recv_handler);
	usart_init();

	

	
	wifi_init();
	wifi_wait_connect();
	weather_wait_get();
	//usart_printf("info:%s\n",info);
	//key_init();
	//st7789_init();
	//timer_init();
	//timer_pwm_init();
	// usart_printf("measure failed\r\n");
	while(1)
	{
		
	}

    


	//   while(1)
	// 	{
	// 		uint8_t data;
	// 		if(rb8_gets(serial_rb,&data,1))
	// 		{
	// 			usart_printf("data:%d\r\n",data);
	// 		}
	// 	}
}

			// led_set(false);
			// delay_us(500);
			// led_set(true);
			// delay_us(500);
			// if(!aht20_trigger_measure())
			// {
			// 	usart_printf("measure failed\r\n");
			// }
			// if(!aht20_wait_measure())
			// {
			// 	usart_printf("measure timeout\r\n");
			// }
			// if(!aht20_get_measure(&humi,&temp))
			// {
			// 	usart_printf("measure get failed\r\n");
			// }
			// usart_printf("humi: %.2f\r\n",humi);
			// usart_printf("temp: %.2f\r\n",temp);
			// delay_us(1000 * 1000);
			// if(key_press())
			// {
			// 	led_toggle();
			// }
			// uint8_t data;
			// if(rb8_gets(serial_rb,&data,1))
			// {
			// 	usart_printf("data:%d\r\n",data);
			// }
	//    st7789_fill_color(0, 0, 79, 319, mkcolor(255, 0, 0));
	//    st7789_fill_color(80, 0, 159, 319, mkcolor(0, 255, 0));
	//    st7789_fill_color(160, 0, 239, 319, mkcolor(0, 0, 255));
	//	delay_us(1000 * 1000);				
	// const uint16_t white = mkcolor(255, 255, 255);
	// const uint16_t black = mkcolor(0, 0, 0);
	// st7789_write_string(0,16,"hello world",white,black,&font16);
	// st7789_write_string(0,48,"√∑ª®«∂»Î Ω",white,black,&font32);
	// st7789_draw_image(0,80,&font_image);