#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "usart.h"
#include "ui.h"
#include "string.h"

typedef enum
{
    UI_ACTION_FILL_COLOR,
    UI_ACTION_WRITE_STRING,
    UI_ACTION_DRAW_IMAGE,
} ui_action_t;

typedef struct
{
    ui_action_t action;
    union 
    {
        struct 
        {
            uint16_t x1;
            uint16_t y1;
            uint16_t x2;
            uint16_t y2;
            uint16_t color;
        }fill_color;
        struct 
        {
            uint16_t x;
            uint16_t y;
            const char *str;
            uint16_t color;
            uint16_t bg_color;
            const font_t *font;
        }write_string;        
        struct 
        {
            uint16_t x;
            uint16_t y;
            const image_t *font_image;
        }draw_image;        
    };
    
} ui_message_t;

static QueueHandle_t ui_queue;

static void ui_func(void *param)
{   
    ui_message_t ui_msg;

    st7789_init();

    while(1)
    {
        xQueueReceive(ui_queue,&ui_msg,portMAX_DELAY);

        switch(ui_msg.action)
        {
            case UI_ACTION_FILL_COLOR:
            {
                st7789_fill_color(ui_msg.fill_color.x1,
                                  ui_msg.fill_color.y1,
                                  ui_msg.fill_color.x2,
                                  ui_msg.fill_color.y2,
                                  ui_msg.fill_color.color);
                break;
            }
            case UI_ACTION_WRITE_STRING:
            {
                st7789_write_string(ui_msg.write_string.x,
                                    ui_msg.write_string.y,
                                    ui_msg.write_string.str,
                                    ui_msg.write_string.color,
                                    ui_msg.write_string.bg_color,
                                    ui_msg.write_string.font);
                vPortFree((void *)ui_msg.write_string.str);
                break;
            }
            case UI_ACTION_DRAW_IMAGE:
            {
                st7789_draw_image(ui_msg.draw_image.x,
                                  ui_msg.draw_image.y,
                                  ui_msg.draw_image.font_image);
                break;
            }
            default:
            {
                usart_printf("Unknown UI action: %d\n", ui_msg.action);
                break;
            }                    
        }
    }
}

void ui_init(void)
{
    ui_queue = xQueueCreate(16,sizeof(ui_message_t));
    configASSERT(ui_queue);
    xTaskCreate(ui_func,"ui",1024,NULL,8,NULL);
}

void ui_fill_color(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
    ui_message_t ui_msg;
    ui_msg.action = UI_ACTION_FILL_COLOR;
    ui_msg.fill_color.x1 = x1;
    ui_msg.fill_color.y1 = y1;
    ui_msg.fill_color.x2 = x2;
    ui_msg.fill_color.y2 = y2;
    ui_msg.fill_color.color = color;

    xQueueSend(ui_queue,&ui_msg,portMAX_DELAY);
}
void ui_write_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg_color, const font_t *font)
{   
    char *pstr = pvPortMalloc(strlen(str) + 1);
    if(pstr == NULL)
    {
        usart_printf("ui write string malloc failed: %s", str);
        return;        
    }
    strcpy(pstr,str);

    ui_message_t ui_msg;
    ui_msg.action = UI_ACTION_WRITE_STRING;
    ui_msg.write_string.x = x; 
    ui_msg.write_string.y = y; 
    ui_msg.write_string.str = pstr; 
    ui_msg.write_string.color = color; 
    ui_msg.write_string.bg_color = bg_color; 
    ui_msg.write_string.font = font; 

    xQueueSend(ui_queue,&ui_msg,portMAX_DELAY);
}
void ui_draw_image(uint16_t x, uint16_t y,const image_t *font_image)
{
    ui_message_t ui_msg;
    ui_msg.action = UI_ACTION_DRAW_IMAGE;
    ui_msg.draw_image.x = x;    
    ui_msg.draw_image.y = y;  
    ui_msg.draw_image.font_image = font_image;    

    xQueueSend(ui_queue,&ui_msg,portMAX_DELAY);
}