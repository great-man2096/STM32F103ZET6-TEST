#ifndef __DMA_H
#define __DMA_H

#include "./SYSTEM/sys/sys.h"

void dma_init(void);	//��ʼ��DMA
void dma_enable_transmit(uint16_t cndtr);	//���贫�����ֵ

#endif
