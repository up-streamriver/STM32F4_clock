#include "board.h"

#define USART_RX_BUFFER_SIZE 512
static uint8_t usart_rx_buffer[USART_RX_BUFFER_SIZE];
static ringbuffer_t serial_rb;

void usart_recv_handler(uint8_t data)
{
	rb8_puts(serial_rb,&data,1);
}

int main(void)
{	
	board_init();
	led_init();
	delay_init();
	serial_rb = rb8_new(usart_rx_buffer,USART_RX_BUFFER_SIZE);
	usart_recv_handler_register(usart_recv_handler);
	usart_init();
	usart_write_string("initial success\r\n");
	usart_printf("print success\r\n");
	  while(1)
		{
			// led_set(false);
			// delay_ms(500);
			// led_set(true);
			// delay_ms(500);
			uint8_t data;
			if(rb8_gets(serial_rb,&data,1))
			{
				usart_printf("data:%d\r\n",data);
			}
		}
}
