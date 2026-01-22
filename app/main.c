#include "page.h"
#include "app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ui.h"

extern void board_lowlevel_init(void);
extern void board_init(void);

static void main_init(void *param)
{
    board_init();
    
    ui_init();

    welcome_page_display();
    
    wifi_init();
    wifi_page_display();
    wifi_wait_connect();
    
    
    main_page_display();

    main_loop_init();
    
    vTaskDelete(NULL);
}

int main(void)
{
    board_lowlevel_init();

    xTaskCreate(main_init,"main_init",1024,NULL,10,NULL);

    vTaskStartScheduler();
    while(1)
    {
        ; //code should not run here
    }
    return 0;
}
