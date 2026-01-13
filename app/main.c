#include "board.h"

#define USART_RX_BUFFER_SIZE 512
#define EEPROM_SN_OFFSET    120
#define EEPROM_BOOT_COUNT   30

static uint8_t usart_rx_buffer[USART_RX_BUFFER_SIZE];
static ringbuffer_t serial_rb;


static const char *url = "https://api.seniverse.com/v3/weather/now.json?key=St0LIXsCn7a-r05vm&location=beijing&language=en&unit=c";

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
	esp_wifi_info_t wifi = { 0 };
	if(!esp_at_init())
	{
        usart_printf("[AT] init failed\n");
        goto err;		
	}
	usart_printf("[AT] init success\n");
	if(!esp_at_wifi_init())
	{
        usart_printf("[WIFI] init failed\n");
        goto err;		
	}
	usart_printf("[WIFI] init success\n");
	if(!esp_at_connect_wifi("river", "12345678", NULL))
	{
        usart_printf("[WIFI] connect failed\n");
        goto err;		
	}
	usart_printf("[WIFI] connect success\n");
	if(!esp_at_get_wifi_info(&wifi))
	{
        usart_printf("[WIFI] info failed\n");
        goto err;		
	}
	usart_printf("[WIFI] info success\n");	

	if (!esp_at_sntp_init())
    {
        usart_printf("[SNTP] init failed\n");
        goto err;
    }
    usart_printf("[SNTP] inited success\n");
	const char *info = 	esp_at_http_get(url);
	weather_info_t weather = { 0 };
	if(parse_seniverse_response(info,&weather))
	{
		usart_printf("[PARSE] inited success\n");
		usart_printf("[WEATHER] %s,%s,%s,%1.f\n",weather.city,weather.location,weather.weather,weather.temperature);
	}
	//usart_printf("info:%s\n",info);
	//key_init();
	//st7789_init();
	//timer_init();
	//timer_pwm_init();
	// usart_printf("measure failed\r\n");
	while(1)
	{
		uint64_t start = bl_now_ms();
		while(bl_now_ms() - start < 1000)
		{
			;
		}
		led_toggle();

	}
err:
    while (1)
    {
        usart_printf("AT Error\r\n");
        delay_us(1000 * 1000);
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
	// st7789_write_string(0,48,"Ã·»¨Ç¶ÈëÊ½",white,black,&font32);
	// st7789_draw_image(0,80,&font_image);