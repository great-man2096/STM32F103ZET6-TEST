#ifndef __WDG_H
#define __WDG_H

#include "./SYSTEM/sys/sys.h"


void iwdg_init(uint8_t prer, uint16_t rlr);
void iwdg_feed(void);

void wwdg_init(uint8_t cr,uint8_t wr,uint32_t fprer);

void wwdg_feed(void);

#endif
