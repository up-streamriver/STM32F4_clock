#ifndef APP_H
#define APP_H


#include <stdio.h>
#define APP_VERSION "v1.0"
#define wifi_ssid "river"
#define wifi_pwd    "12345678"

void wifi_init(void);
void wifi_wait_connect(void);

void main_loop_init(void);
void main_loop(void);




#endif
/*APP_H */
