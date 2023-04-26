#include "./BSP/ADC/adc.h"
#include "adc.h"

ADC_HandleTypeDef g_adc_handler;

DMA_HandleTypeDef g_dma_adc_handler;

uint8_t g_adc_dma_sta = 0;


//ADC�¶ȴ�������ʼ��
void adc_temperature_init(void)
{
	
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* �������ݵĶ��뷽ʽ���Ҷ��� */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* ɨ��ģʽ�������� */
	g_adc_handler.Init.ContinuousConvMode = DISABLE;		  /* ��������ת��ģʽ������ǵ���ת��ģʽ������ת��ģʽ */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* ����ת��ͨ����Ŀ */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* �Ƿ�ʹ�ù���ͨ������ģʽ */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* ���ü��ģʽ�Ĺ���ͨ������ */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC �ⲿ����Դѡ�� */
	HAL_ADC_Init(&g_adc_handler);							  // ��ʼ��

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADCУ׼

	adc_ch_conf.Channel = ADC_CHANNEL_TEMPSENSOR;				   // ADC ת��ͨ����ר�Ų��ڲ��¶ȵ�
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC ת��˳��
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // ����ʱ��

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // ����ADCͨ��
}
//��ȡ�¶�ֵ
short adc_get_temperature(void)
{
	uint32_t adcx;
    short result;
    double temperature;

    adcx = adc_get_result();  /* ��ȡ�ڲ��¶ȴ�����ͨ�� */
    temperature = (float)adcx * (3.3 / 4096);               /* ת��Ϊ��ѹֵ */
    temperature = (1.43 - temperature) / 0.0043 + 25;       /* �����¶� */
    result = temperature *= 100;                            /* ����100��. */
    return result;
}

//���ɼ�ʵ���ʼ��
void adc_over_dma_init(uint32_t mar)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	__HAL_RCC_DMA1_CLK_ENABLE();

	g_dma_adc_handler.Instance = DMA1_Channel1;
	g_dma_adc_handler.Init.Direction = DMA_PERIPH_TO_MEMORY;			  // DMA����
	g_dma_adc_handler.Init.PeriphInc = DMA_PINC_DISABLE;				  // �������������
	g_dma_adc_handler.Init.MemInc = DMA_MINC_ENABLE;					  // �ڴ�����д��
	g_dma_adc_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // �������ݿ��16
	g_dma_adc_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  // �ڴ����ݿ��16
	g_dma_adc_handler.Init.Mode = DMA_NORMAL;							  // ������ѭ��ģʽ
	g_dma_adc_handler.Init.Priority = DMA_PRIORITY_MEDIUM;				  // ���ȼ�

	HAL_DMA_Init(&g_dma_adc_handler);

	__HAL_LINKDMA(&g_adc_handler, DMA_Handle, g_dma_adc_handler);

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* �������ݵĶ��뷽ʽ���Ҷ��� */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* ɨ��ģʽ�������� */
	g_adc_handler.Init.ContinuousConvMode = ENABLE;			  /* ��������ת��ģʽ */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* ����ת��ͨ����Ŀ */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* �Ƿ�ʹ�ù���ͨ������ģʽ */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* ���ü��ģʽ�Ĺ���ͨ������ */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC �ⲿ����Դѡ�� */
	HAL_ADC_Init(&g_adc_handler);							  // ��ʼ��

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADCУ׼

	adc_ch_conf.Channel = ADC_CHANNEL_2;				   // ADC ת��ͨ��2
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC ת��˳��
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_1CYCLE_5; // ����ʱ��

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // ����ADCͨ��

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_DMA_Start_IT(&g_dma_adc_handler, (uint32_t)&ADC1->DR, mar, 0);
	HAL_ADC_Start_DMA(&g_adc_handler, &mar, 0);
}


/*	ADC��ͨ���ɼ�ʵ��	*/
void adc_nch_dma_init(uint32_t mar)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	__HAL_RCC_DMA1_CLK_ENABLE();

	g_dma_adc_handler.Instance = DMA1_Channel1;
	g_dma_adc_handler.Init.Direction = DMA_PERIPH_TO_MEMORY;			  // DMA����
	g_dma_adc_handler.Init.PeriphInc = DMA_PINC_DISABLE;				  // �������������
	g_dma_adc_handler.Init.MemInc = DMA_MINC_ENABLE;					  // �ڴ�����д��
	g_dma_adc_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // �������ݿ��16
	g_dma_adc_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  // �ڴ����ݿ��16
	g_dma_adc_handler.Init.Mode = DMA_NORMAL;							  // ������ѭ��ģʽ
	g_dma_adc_handler.Init.Priority = DMA_PRIORITY_MEDIUM;				  // ���ȼ�

	HAL_DMA_Init(&g_dma_adc_handler);

	__HAL_LINKDMA(&g_adc_handler, DMA_Handle, g_dma_adc_handler);

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* �������ݵĶ��뷽ʽ���Ҷ��� */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_ENABLE;		  /* ɨ��ģʽ������ */
	g_adc_handler.Init.ContinuousConvMode = ENABLE;			  /* ��������ת��ģʽ */
	g_adc_handler.Init.NbrOfConversion = 6;					  /* ����ת��ͨ����Ŀ */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* �Ƿ�ʹ�ù���ͨ������ģʽ */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* ���ü��ģʽ�Ĺ���ͨ������ */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC �ⲿ����Դѡ�� */
	HAL_ADC_Init(&g_adc_handler);							  // ��ʼ��

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADCУ׼

	adc_ch_conf.Channel = ADC_CHANNEL_10;				   // ADC ת��ͨ��10
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC ת��˳��
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // ����ʱ��
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf);   // ����ADCͨ��

	adc_ch_conf.Channel = ADC_CHANNEL_11;				 // ADC ת��ͨ��11
	adc_ch_conf.Rank = ADC_REGULAR_RANK_2;				 // ADC ת��˳��
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // ����ADCͨ��

	adc_ch_conf.Channel = ADC_CHANNEL_12;
	adc_ch_conf.Rank = ADC_REGULAR_RANK_3;
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf);

	adc_ch_conf.Channel = ADC_CHANNEL_13;
	adc_ch_conf.Rank = ADC_REGULAR_RANK_4;
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf);

	adc_ch_conf.Channel = ADC_CHANNEL_14;
	adc_ch_conf.Rank = ADC_REGULAR_RANK_5;
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf);

	adc_ch_conf.Channel = ADC_CHANNEL_15;
	adc_ch_conf.Rank = ADC_REGULAR_RANK_6;
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf);

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_DMA_Start_IT(&g_dma_adc_handler, (uint32_t)&ADC1->DR, mar, 0);
	HAL_ADC_Start_DMA(&g_adc_handler, &mar, 0);
}

// ADC DMA��ȡ ��ʼ��
void adc_dma_init(uint32_t mar)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	__HAL_RCC_DMA1_CLK_ENABLE();

	g_dma_adc_handler.Instance = DMA1_Channel1;
	g_dma_adc_handler.Init.Direction = DMA_PERIPH_TO_MEMORY;			  // DMA����
	g_dma_adc_handler.Init.PeriphInc = DMA_PINC_DISABLE;				  // �������������
	g_dma_adc_handler.Init.MemInc = DMA_MINC_ENABLE;					  // �ڴ�����д��
	g_dma_adc_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // �������ݿ��16
	g_dma_adc_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  // �ڴ����ݿ��16
	g_dma_adc_handler.Init.Mode = DMA_NORMAL;							  // ������ѭ��ģʽ
	g_dma_adc_handler.Init.Priority = DMA_PRIORITY_MEDIUM;				  // ���ȼ�

	HAL_DMA_Init(&g_dma_adc_handler);

	__HAL_LINKDMA(&g_adc_handler, DMA_Handle, g_dma_adc_handler);

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* �������ݵĶ��뷽ʽ���Ҷ��� */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* ɨ��ģʽ�������� */
	g_adc_handler.Init.ContinuousConvMode = ENABLE;			  /* ��������ת��ģʽ */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* ����ת��ͨ����Ŀ */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* �Ƿ�ʹ�ù���ͨ������ģʽ */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* ���ü��ģʽ�Ĺ���ͨ������ */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC �ⲿ����Դѡ�� */
	HAL_ADC_Init(&g_adc_handler);							  // ��ʼ��

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADCУ׼

	adc_ch_conf.Channel = ADC_CHANNEL_2;				   // ADC ת��ͨ��2
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC ת��˳��
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // ����ʱ��

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // ����ADCͨ��

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_DMA_Start_IT(&g_dma_adc_handler, (uint32_t)&ADC1->DR, mar, 0);
	HAL_ADC_Start_DMA(&g_adc_handler, &mar, 0);
}

// ʹ��һ��ADC MDA���亯��
void adc_dma_enable(uint16_t cndtr)
{
	ADC1->CR2 &= ~(1 << 0);			 // �ر�ADC
	DMA1_Channel1->CCR &= ~(1 << 0); // �ر�������
	while (DMA1_Channel1->CCR & (1 << 0))
		; // �ȴ��رճɹ�

	DMA1_Channel1->CNDTR = cndtr; // ��������������

	DMA1_Channel1->CCR |= (1 << 0); // ����������
	ADC1->CR2 |= (1 << 0);			// ����ADCת��

	ADC1->CR2 |= (1 << 22); // ����������ת��
}
// ADC DMA�ɼ��жϷ�����
void DMA1_Channel1_IRQHandler(void)
{
	if (DMA1->ISR & (1 << 1)) // ͨ����ɱ�־λ������Ƿ������
	{
		g_adc_dma_sta = 1;
		DMA1->IFCR |= 1 << 1; // ������ɱ�־λΪ0
	}
}

// ADC��ͨ����ʼ��
void adc_init(void)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* �������ݵĶ��뷽ʽ���Ҷ��� */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* ɨ��ģʽ�������� */
	g_adc_handler.Init.ContinuousConvMode = DISABLE;		  /* ��������ת��ģʽ������ǵ���ת��ģʽ������ת��ģʽ */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* ����ת��ͨ����Ŀ */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* �Ƿ�ʹ�ù���ͨ������ģʽ */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* ���ü��ģʽ�Ĺ���ͨ������ */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC �ⲿ����Դѡ�� */
	HAL_ADC_Init(&g_adc_handler);							  // ��ʼ��

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADCУ׼

	adc_ch_conf.Channel = ADC_CHANNEL_2;				   // ADC ת��ͨ��2
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC ת��˳��
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // ����ʱ��

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // ����ADCͨ��
}
// ADC��MSP��ʼ��
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1)
	{
		GPIO_InitTypeDef gpio_init_struct;
		RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_ADC1_CLK_ENABLE();

		gpio_init_struct.Pin = GPIO_PIN_2;
		gpio_init_struct.Mode = GPIO_MODE_ANALOG;
		// gpio_init_struct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOA, &gpio_init_struct);

		// gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
		// gpio_init_struct.Mode = GPIO_MODE_ANALOG;
		// HAL_GPIO_Init(GPIOC, &gpio_init_struct);

		adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;
		adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;

		HAL_RCCEx_PeriphCLKConfig(&adc_clk_init); // ����ADCʱ�ӵ�Ԥ��Ƶϵ��
	}
}
// ���ADCת����Ľ������
uint32_t adc_get_result(void)
{
	// ����A/Dת��
	HAL_ADC_Start(&g_adc_handler);

	// �ȴ�����ͨ��ת�����
	HAL_ADC_PollForConversion(&g_adc_handler, 10); // 10ms�ȴ�

	// ��ȡ����ͨ��A/Dת�����
	return (uint16_t)HAL_ADC_GetValue(&g_adc_handler);
}

/***************************************��ͨ��ADC�ɼ�(DMA��ȡ)ʵ�����*****************************************/

// DMA_HandleTypeDef g_dma_adc_handle = {0};                                 /* ����Ҫ����ADC���ݵ�DMA��� */
// ADC_HandleTypeDef g_adc_dma_handle = {0};                                 /* ����ADC��DMA��ȡ����� */
// uint8_t g_adc_dma_sta = 0;                                                /* DMA����״̬��־, 0,δ���; 1, ����� */

///**
// * @brief       ADC DMA��ȡ ��ʼ������
// *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
// * @param       par         : �����ַ
// * @param       mar         : �洢����ַ
// * @retval      ��
// */
// void adc_dma_init(uint32_t mar)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
//    ADC_ChannelConfTypeDef adc_ch_conf = {0};

//    ADC_ADCX_CHY_CLK_ENABLE();                                              /* ʹ��ADCxʱ�� */
//    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                                         /* ����GPIOʱ�� */

//    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)                 /* ����DMA1_Channel7, ��ΪDMA2��ͨ���� */
//    {
//        __HAL_RCC_DMA2_CLK_ENABLE();                                        /* DMA2ʱ��ʹ�� */
//    }
//    else
//    {
//        __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1ʱ��ʹ�� */
//    }

//    /* ����ADCʱ�� */
//    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                  /* ADC����ʱ�� */
//    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                     /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
//    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                               /* ����ADCʱ�� */

//    /* ����AD�ɼ�ͨ����ӦIO���Ź���ģʽ */
//    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;                           /* ADCͨ����Ӧ��IO���� */
//    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                               /* ģ�� */
//    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);

//    /* ��ʼ��DMA */
//    g_dma_adc_handle.Instance = ADC_ADCX_DMACx;                             /* ����DMAͨ�� */
//    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* �����赽�洢��ģʽ */
//    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* ���������ģʽ */
//    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* �洢������ģʽ */
//    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* �������ݳ���:16λ */
//    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* �洢�����ݳ���:16λ */
//    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* ��������ģʽ */
//    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* �е����ȼ� */
//    HAL_DMA_Init(&g_dma_adc_handle);

//    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);         /* ��DMA��adc��ϵ���� */

//    g_adc_dma_handle.Instance = ADC_ADCX;                                   /* ѡ���ĸ�ADC */
//    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* ���ݶ��뷽ʽ���Ҷ��� */
//    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                  /* ��ɨ��ģʽ�����õ�һ��ͨ�� */
//    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;                      /* ʹ������ת��ģʽ */
//    g_adc_dma_handle.Init.NbrOfConversion = 1;                              /* ��ֵ��Χ��1~16����ʵ���õ�1������ͨ������ */
//    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* ��ֹ����ͨ������ģʽ */
//    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                          /* ���ü��ģʽ�Ĺ���ͨ����������ֹ����ͨ������ģʽ�󣬴˲������� */
//    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* ����ת����ʽ��������� */
//    HAL_ADC_Init(&g_adc_dma_handle);                                        /* ��ʼ�� */

//    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle);                         /* У׼ADC */

//    /* ����ADCͨ�� */
//    adc_ch_conf.Channel = ADC_ADCX_CHY;                                     /* ͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                  /* ���� */
//    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* ����ʱ�䣬��������������:239.5��ADC���� */
//    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* ͨ������ */

//    /* ����DMA�����������ж����ȼ� */
//    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 3, 3);
//    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);

//    HAL_DMA_Start_IT(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);       /* ����DMA���������ж� */
//    HAL_ADC_Start_DMA(&g_adc_dma_handle, &mar, 0);                          /* ����ADC��ͨ��DMA������ */
//}

///**
// * @brief       ʹ��һ��ADC DMA����
// *   @note      �ú����üĴ�������������ֹ��HAL������������������޸�,ҲΪ�˼�����
// * @param       ndtr: DMA����Ĵ���
// * @retval      ��
// */
// void adc_dma_enable(uint16_t cndtr)
//{
//    ADC_ADCX->CR2 &= ~(1 << 0);                 /* �ȹر�ADC */

//    ADC_ADCX_DMACx->CCR &= ~(1 << 0);           /* �ر�DMA���� */
//    while (ADC_ADCX_DMACx->CCR & (1 << 0));     /* ȷ��DMA���Ա����� */
//    ADC_ADCX_DMACx->CNDTR = cndtr;              /* DMA���������� */
//    ADC_ADCX_DMACx->CCR |= 1 << 0;              /* ����DMA���� */

//    ADC_ADCX->CR2 |= 1 << 0;                    /* ��������ADC */
//    ADC_ADCX->CR2 |= 1 << 22;                   /* ��������ת��ͨ�� */
//}

///**
// * @brief       ADC DMA�ɼ��жϷ�����
// * @param       ��
// * @retval      ��
// */
// void ADC_ADCX_DMACx_IRQHandler(void)
//{
//    if (ADC_ADCX_DMACx_IS_TC())
//    {
//        g_adc_dma_sta = 1;                      /* ���DMA������� */
//        ADC_ADCX_DMACx_CLR_TC();                /* ���DMA1 ������7 ��������ж� */
//    }
//}
