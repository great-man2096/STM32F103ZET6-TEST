#ifndef __DMA_H
#define __DMA_H

#include "./SYSTEM/sys/sys.h"

void dma_init(void);	//初始化DMA
void dma_enable_transmit(uint16_t cndtr);	//重设传输计数值

#endif
