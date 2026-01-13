#ifndef BL24C512_H
#define BL24C512_H

#include "stm32f4xx.h"
#include <stdbool.h>
#include <stdint.h>

void bl24c512_init(void);
bool bl24c512_write(uint16_t address, uint8_t data[], uint32_t length);
bool bl24c512_read(uint16_t address, uint8_t data[], uint32_t length);



#endif
/*BL24C512_H */