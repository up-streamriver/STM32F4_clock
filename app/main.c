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
		delay_ms(100);
}

static bool key_press(void)
{
	if(key_is_pressed())
		{
			delay_ms(100);
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
	key_init();
	//timer_init();
	timer_pwm_init();
	if(aht20_init())
		usart_write_string("initial success\r\n");
	usart_printf("initial end\r\n");
		float humi;
		float temp;

	  while(1)
		{
			// led_set(false);
			// delay_ms(500);
			// led_set(true);
			// delay_ms(500);
			if(!aht20_trigger_measure())
			{
				usart_printf("measure failed\r\n");
			}
			if(!aht20_wait_measure())
			{
				usart_printf("measure timeout\r\n");
			}
			if(!aht20_get_measure(&humi,&temp))
			{
				usart_printf("measure get failed\r\n");
			}
			usart_printf("humi: %.2f\r\n",humi);
			usart_printf("temp: %.2f\r\n",temp);
			delay_ms(1000 * 1000);
			// if(key_press())
			// {
			// 	led_toggle();
			// }
			// uint8_t data;
			// if(rb8_gets(serial_rb,&data,1))
			// {
			// 	usart_printf("data:%d\r\n",data);
			// }
		}
}
