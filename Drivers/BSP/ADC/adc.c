#include "./BSP/ADC/adc.h"
#include "adc.h"

ADC_HandleTypeDef g_adc_handler;

DMA_HandleTypeDef g_dma_adc_handler;

uint8_t g_adc_dma_sta = 0;


//ADC温度传感器初始化
void adc_temperature_init(void)
{
	
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* 设置数据的对齐方式：右对齐 */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* 扫描模式：不开启 */
	g_adc_handler.Init.ContinuousConvMode = DISABLE;		  /* 开启连续转换模式否则就是单次转换模式：单次转换模式 */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* 设置转换通道数目 */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* 是否使用规则通道组间断模式 */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* 配置间断模式的规则通道个数 */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC 外部触发源选择 */
	HAL_ADC_Init(&g_adc_handler);							  // 初始化

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADC校准

	adc_ch_conf.Channel = ADC_CHANNEL_TEMPSENSOR;				   // ADC 转换通道，专门测内部温度的
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC 转换顺序
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // 采样时间

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // 配置ADC通道
}
//获取温度值
short adc_get_temperature(void)
{
	uint32_t adcx;
    short result;
    double temperature;

    adcx = adc_get_result();  /* 读取内部温度传感器通道 */
    temperature = (float)adcx * (3.3 / 4096);               /* 转化为电压值 */
    temperature = (1.43 - temperature) / 0.0043 + 25;       /* 计算温度 */
    result = temperature *= 100;                            /* 扩大100倍. */
    return result;
}

//过采集实验初始化
void adc_over_dma_init(uint32_t mar)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	__HAL_RCC_DMA1_CLK_ENABLE();

	g_dma_adc_handler.Instance = DMA1_Channel1;
	g_dma_adc_handler.Init.Direction = DMA_PERIPH_TO_MEMORY;			  // DMA方向
	g_dma_adc_handler.Init.PeriphInc = DMA_PINC_DISABLE;				  // 外设非增量读出
	g_dma_adc_handler.Init.MemInc = DMA_MINC_ENABLE;					  // 内存增量写入
	g_dma_adc_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 外设数据宽度16
	g_dma_adc_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  // 内存数据宽度16
	g_dma_adc_handler.Init.Mode = DMA_NORMAL;							  // 不开启循环模式
	g_dma_adc_handler.Init.Priority = DMA_PRIORITY_MEDIUM;				  // 优先级

	HAL_DMA_Init(&g_dma_adc_handler);

	__HAL_LINKDMA(&g_adc_handler, DMA_Handle, g_dma_adc_handler);

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* 设置数据的对齐方式：右对齐 */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* 扫描模式：不开启 */
	g_adc_handler.Init.ContinuousConvMode = ENABLE;			  /* 开启连续转换模式 */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* 设置转换通道数目 */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* 是否使用规则通道组间断模式 */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* 配置间断模式的规则通道个数 */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC 外部触发源选择 */
	HAL_ADC_Init(&g_adc_handler);							  // 初始化

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADC校准

	adc_ch_conf.Channel = ADC_CHANNEL_2;				   // ADC 转换通道2
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC 转换顺序
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_1CYCLE_5; // 采样时间

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // 配置ADC通道

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_DMA_Start_IT(&g_dma_adc_handler, (uint32_t)&ADC1->DR, mar, 0);
	HAL_ADC_Start_DMA(&g_adc_handler, &mar, 0);
}


/*	ADC多通道采集实验	*/
void adc_nch_dma_init(uint32_t mar)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	__HAL_RCC_DMA1_CLK_ENABLE();

	g_dma_adc_handler.Instance = DMA1_Channel1;
	g_dma_adc_handler.Init.Direction = DMA_PERIPH_TO_MEMORY;			  // DMA方向
	g_dma_adc_handler.Init.PeriphInc = DMA_PINC_DISABLE;				  // 外设非增量读出
	g_dma_adc_handler.Init.MemInc = DMA_MINC_ENABLE;					  // 内存增量写入
	g_dma_adc_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 外设数据宽度16
	g_dma_adc_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  // 内存数据宽度16
	g_dma_adc_handler.Init.Mode = DMA_NORMAL;							  // 不开启循环模式
	g_dma_adc_handler.Init.Priority = DMA_PRIORITY_MEDIUM;				  // 优先级

	HAL_DMA_Init(&g_dma_adc_handler);

	__HAL_LINKDMA(&g_adc_handler, DMA_Handle, g_dma_adc_handler);

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* 设置数据的对齐方式：右对齐 */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_ENABLE;		  /* 扫描模式：开启 */
	g_adc_handler.Init.ContinuousConvMode = ENABLE;			  /* 开启连续转换模式 */
	g_adc_handler.Init.NbrOfConversion = 6;					  /* 设置转换通道数目 */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* 是否使用规则通道组间断模式 */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* 配置间断模式的规则通道个数 */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC 外部触发源选择 */
	HAL_ADC_Init(&g_adc_handler);							  // 初始化

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADC校准

	adc_ch_conf.Channel = ADC_CHANNEL_10;				   // ADC 转换通道10
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC 转换顺序
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // 采样时间
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf);   // 配置ADC通道

	adc_ch_conf.Channel = ADC_CHANNEL_11;				 // ADC 转换通道11
	adc_ch_conf.Rank = ADC_REGULAR_RANK_2;				 // ADC 转换顺序
	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // 配置ADC通道

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

// ADC DMA读取 初始化
void adc_dma_init(uint32_t mar)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	__HAL_RCC_DMA1_CLK_ENABLE();

	g_dma_adc_handler.Instance = DMA1_Channel1;
	g_dma_adc_handler.Init.Direction = DMA_PERIPH_TO_MEMORY;			  // DMA方向
	g_dma_adc_handler.Init.PeriphInc = DMA_PINC_DISABLE;				  // 外设非增量读出
	g_dma_adc_handler.Init.MemInc = DMA_MINC_ENABLE;					  // 内存增量写入
	g_dma_adc_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 外设数据宽度16
	g_dma_adc_handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	  // 内存数据宽度16
	g_dma_adc_handler.Init.Mode = DMA_NORMAL;							  // 不开启循环模式
	g_dma_adc_handler.Init.Priority = DMA_PRIORITY_MEDIUM;				  // 优先级

	HAL_DMA_Init(&g_dma_adc_handler);

	__HAL_LINKDMA(&g_adc_handler, DMA_Handle, g_dma_adc_handler);

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* 设置数据的对齐方式：右对齐 */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* 扫描模式：不开启 */
	g_adc_handler.Init.ContinuousConvMode = ENABLE;			  /* 开启连续转换模式 */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* 设置转换通道数目 */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* 是否使用规则通道组间断模式 */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* 配置间断模式的规则通道个数 */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC 外部触发源选择 */
	HAL_ADC_Init(&g_adc_handler);							  // 初始化

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADC校准

	adc_ch_conf.Channel = ADC_CHANNEL_2;				   // ADC 转换通道2
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC 转换顺序
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // 采样时间

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // 配置ADC通道

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 3);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_DMA_Start_IT(&g_dma_adc_handler, (uint32_t)&ADC1->DR, mar, 0);
	HAL_ADC_Start_DMA(&g_adc_handler, &mar, 0);
}

// 使能一次ADC MDA传输函数
void adc_dma_enable(uint16_t cndtr)
{
	ADC1->CR2 &= ~(1 << 0);			 // 关闭ADC
	DMA1_Channel1->CCR &= ~(1 << 0); // 关闭数据流
	while (DMA1_Channel1->CCR & (1 << 0))
		; // 等待关闭成功

	DMA1_Channel1->CNDTR = cndtr; // 设置数量控制器

	DMA1_Channel1->CCR |= (1 << 0); // 开启数据流
	ADC1->CR2 |= (1 << 0);			// 开启ADC转换

	ADC1->CR2 |= (1 << 22); // 触发规则组转换
}
// ADC DMA采集中断服务函数
void DMA1_Channel1_IRQHandler(void)
{
	if (DMA1->ISR & (1 << 1)) // 通过完成标志位来检查是否传输完成
	{
		g_adc_dma_sta = 1;
		DMA1->IFCR |= 1 << 1; // 重置完成标志位为0
	}
}

// ADC单通道初始化
void adc_init(void)
{
	ADC_ChannelConfTypeDef adc_ch_conf = {0};

	g_adc_handler.Instance = ADC1;
	g_adc_handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;		  /* 设置数据的对齐方式：右对齐 */
	g_adc_handler.Init.ScanConvMode = ADC_SCAN_DISABLE;		  /* 扫描模式：不开启 */
	g_adc_handler.Init.ContinuousConvMode = DISABLE;		  /* 开启连续转换模式否则就是单次转换模式：单次转换模式 */
	g_adc_handler.Init.NbrOfConversion = 1;					  /* 设置转换通道数目 */
	g_adc_handler.Init.DiscontinuousConvMode = DISABLE;		  /* 是否使用规则通道组间断模式 */
	g_adc_handler.Init.NbrOfDiscConversion = 0;				  /* 配置间断模式的规则通道个数 */
	g_adc_handler.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ADC 外部触发源选择 */
	HAL_ADC_Init(&g_adc_handler);							  // 初始化

	HAL_ADCEx_Calibration_Start(&g_adc_handler); // ADC校准

	adc_ch_conf.Channel = ADC_CHANNEL_2;				   // ADC 转换通道2
	adc_ch_conf.Rank = ADC_REGULAR_RANK_1;				   // ADC 转换顺序
	adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // 采样时间

	HAL_ADC_ConfigChannel(&g_adc_handler, &adc_ch_conf); // 配置ADC通道
}
// ADC·MSP初始化
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

		HAL_RCCEx_PeriphCLKConfig(&adc_clk_init); // 配置ADC时钟的预分频系数
	}
}
// 获得ADC转化后的结果函数
uint32_t adc_get_result(void)
{
	// 启动A/D转换
	HAL_ADC_Start(&g_adc_handler);

	// 等待规则通道转化完成
	HAL_ADC_PollForConversion(&g_adc_handler, 10); // 10ms等待

	// 获取规则通道A/D转换结果
	return (uint16_t)HAL_ADC_GetValue(&g_adc_handler);
}

/***************************************单通道ADC采集(DMA读取)实验代码*****************************************/

// DMA_HandleTypeDef g_dma_adc_handle = {0};                                 /* 定义要搬运ADC数据的DMA句柄 */
// ADC_HandleTypeDef g_adc_dma_handle = {0};                                 /* 定义ADC（DMA读取）句柄 */
// uint8_t g_adc_dma_sta = 0;                                                /* DMA传输状态标志, 0,未完成; 1, 已完成 */

///**
// * @brief       ADC DMA读取 初始化函数
// *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
// * @param       par         : 外设地址
// * @param       mar         : 存储器地址
// * @retval      无
// */
// void adc_dma_init(uint32_t mar)
//{
//    GPIO_InitTypeDef gpio_init_struct;
//    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
//    ADC_ChannelConfTypeDef adc_ch_conf = {0};

//    ADC_ADCX_CHY_CLK_ENABLE();                                              /* 使能ADCx时钟 */
//    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                                         /* 开启GPIO时钟 */

//    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)                 /* 大于DMA1_Channel7, 则为DMA2的通道了 */
//    {
//        __HAL_RCC_DMA2_CLK_ENABLE();                                        /* DMA2时钟使能 */
//    }
//    else
//    {
//        __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1时钟使能 */
//    }

//    /* 设置ADC时钟 */
//    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                  /* ADC外设时钟 */
//    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                     /* 分频因子6时钟为72M/6=12MHz */
//    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                               /* 设置ADC时钟 */

//    /* 设置AD采集通道对应IO引脚工作模式 */
//    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;                           /* ADC通道对应的IO引脚 */
//    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                               /* 模拟 */
//    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);

//    /* 初始化DMA */
//    g_dma_adc_handle.Instance = ADC_ADCX_DMACx;                             /* 设置DMA通道 */
//    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* 从外设到存储器模式 */
//    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
//    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
//    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
//    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
//    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* 外设流控模式 */
//    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
//    HAL_DMA_Init(&g_dma_adc_handle);

//    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);         /* 将DMA与adc联系起来 */

//    g_adc_dma_handle.Instance = ADC_ADCX;                                   /* 选择哪个ADC */
//    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* 数据对齐方式：右对齐 */
//    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                  /* 非扫描模式，仅用到一个通道 */
//    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;                      /* 使能连续转换模式 */
//    g_adc_dma_handle.Init.NbrOfConversion = 1;                              /* 赋值范围是1~16，本实验用到1个规则通道序列 */
//    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* 禁止规则通道组间断模式 */
//    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                          /* 配置间断模式的规则通道个数，禁止规则通道组间断模式后，此参数忽略 */
//    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* 触发转换方式：软件触发 */
//    HAL_ADC_Init(&g_adc_dma_handle);                                        /* 初始化 */

//    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle);                         /* 校准ADC */

//    /* 配置ADC通道 */
//    adc_ch_conf.Channel = ADC_ADCX_CHY;                                     /* 通道 */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                  /* 序列 */
//    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* 采样时间，设置最大采样周期:239.5个ADC周期 */
//    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* 通道配置 */

//    /* 配置DMA数据流请求中断优先级 */
//    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 3, 3);
//    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);

//    HAL_DMA_Start_IT(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);       /* 启动DMA，并开启中断 */
//    HAL_ADC_Start_DMA(&g_adc_dma_handle, &mar, 0);                          /* 开启ADC，通过DMA传输结果 */
//}

///**
// * @brief       使能一次ADC DMA传输
// *   @note      该函数用寄存器来操作，防止用HAL库操作对其他参数有修改,也为了兼容性
// * @param       ndtr: DMA传输的次数
// * @retval      无
// */
// void adc_dma_enable(uint16_t cndtr)
//{
//    ADC_ADCX->CR2 &= ~(1 << 0);                 /* 先关闭ADC */

//    ADC_ADCX_DMACx->CCR &= ~(1 << 0);           /* 关闭DMA传输 */
//    while (ADC_ADCX_DMACx->CCR & (1 << 0));     /* 确保DMA可以被设置 */
//    ADC_ADCX_DMACx->CNDTR = cndtr;              /* DMA传输数据量 */
//    ADC_ADCX_DMACx->CCR |= 1 << 0;              /* 开启DMA传输 */

//    ADC_ADCX->CR2 |= 1 << 0;                    /* 重新启动ADC */
//    ADC_ADCX->CR2 |= 1 << 22;                   /* 启动规则转换通道 */
//}

///**
// * @brief       ADC DMA采集中断服务函数
// * @param       无
// * @retval      无
// */
// void ADC_ADCX_DMACx_IRQHandler(void)
//{
//    if (ADC_ADCX_DMACx_IS_TC())
//    {
//        g_adc_dma_sta = 1;                      /* 标记DMA传输完成 */
//        ADC_ADCX_DMACx_CLR_TC();                /* 清除DMA1 数据流7 传输完成中断 */
//    }
//}
