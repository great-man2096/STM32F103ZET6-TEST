#include "./BSP/DMA/dma.h"

DMA_HandleTypeDef g_dma_handler;
uint8_t src_buf[10] = {0x0a, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
uint8_t dest_buf[10] = {0};

//内存到内存DMA传输
void dma_init(void)
{
	//使能DMA1时钟
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	g_dma_handler.Instance = DMA1_Channel1;	//确定DMA通道
	
	g_dma_handler.Init.Direction = DMA_MEMORY_TO_MEMORY;	//确定传输方向
	g_dma_handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;	//传输大小设置为一个字节
	g_dma_handler.Init.MemInc = DMA_MINC_ENABLE;	//写出增量模式
	
	g_dma_handler.Init.Mode = DMA_NORMAL;	//内存到内存不支持循环模式
	g_dma_handler.Init.PeriphInc = DMA_PINC_ENABLE;	//写入增量模式
	g_dma_handler.Init.Priority = DMA_PRIORITY_HIGH;	//优先级
	
	HAL_DMA_Init(&g_dma_handler);
	HAL_DMA_Start(&g_dma_handler, (uint32_t)src_buf, (uint32_t)dest_buf, 0);
	
}
void dma_enable_transmit(uint16_t cndtr)
{
	__HAL_DMA_DISABLE(&g_dma_handler);
	
	g_dma_handler.Instance->CNDTR = cndtr;
	
	__HAL_DMA_ENABLE(&g_dma_handler);

}
