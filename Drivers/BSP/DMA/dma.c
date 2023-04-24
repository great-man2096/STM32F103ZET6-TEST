#include "./BSP/DMA/dma.h"

DMA_HandleTypeDef g_dma_handler;
uint8_t src_buf[10] = {0x0a, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
uint8_t dest_buf[10] = {0};

//�ڴ浽�ڴ�DMA����
void dma_init(void)
{
	//ʹ��DMA1ʱ��
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	g_dma_handler.Instance = DMA1_Channel1;	//ȷ��DMAͨ��
	
	g_dma_handler.Init.Direction = DMA_MEMORY_TO_MEMORY;	//ȷ�����䷽��
	g_dma_handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;	//�����С����Ϊһ���ֽ�
	g_dma_handler.Init.MemInc = DMA_MINC_ENABLE;	//д������ģʽ
	
	g_dma_handler.Init.Mode = DMA_NORMAL;	//�ڴ浽�ڴ治֧��ѭ��ģʽ
	g_dma_handler.Init.PeriphInc = DMA_PINC_ENABLE;	//д������ģʽ
	g_dma_handler.Init.Priority = DMA_PRIORITY_HIGH;	//���ȼ�
	
	HAL_DMA_Init(&g_dma_handler);
	HAL_DMA_Start(&g_dma_handler, (uint32_t)src_buf, (uint32_t)dest_buf, 0);
	
}
void dma_enable_transmit(uint16_t cndtr)
{
	__HAL_DMA_DISABLE(&g_dma_handler);
	
	g_dma_handler.Instance->CNDTR = cndtr;
	
	__HAL_DMA_ENABLE(&g_dma_handler);

}
