#ifndef __DAC_H
#define __DAC_H

#include "./SYSTEM/sys/sys.h"


void dac_init(uint8_t outx);
void dac_set_voltage(uint8_t outx, uint16_t vol);
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n);
void dac_dma_wave_init(void);
void dac_dma_wave_enable(uint16_t cndtr, uint16_t arr, uint16_t psc);

#endif 

