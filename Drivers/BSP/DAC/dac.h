#ifndef __DAC_H
#define __DAC_H

#include "./SYSTEM/sys/sys.h"


void dac_init(uint8_t outx);
void dac_set_voltage(uint8_t outx, uint16_t vol);


#endif 

