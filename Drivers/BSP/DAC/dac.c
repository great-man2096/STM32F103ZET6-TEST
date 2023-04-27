#include "./BSP/DAC/dac.h"
#include "./SYSTEM/delay/delay.h"
#include "math.h"

DAC_HandleTypeDef g_dac_handle; /* DAC句柄 */
DMA_HandleTypeDef g_dma_adc_handle;
uint16_t g_dac_sin_buf[4096];

/**
 * @brief       DAC初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=36Mhz=27.8ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 4us (F103数据手册有写)
 *              因此DAC输出的最高速度约为:250Khz, 以10个点为一个周期, 最大能输出25Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void dac_init(uint8_t outx)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    DAC_ChannelConfTypeDef dac_ch_conf = {0};

    __HAL_RCC_DAC_CLK_ENABLE();   /* 使能DAC1的时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE(); /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */

    gpio_init_struct.Pin = (outx == 1) ? GPIO_PIN_4 : GPIO_PIN_5; /* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    g_dac_handle.Instance = DAC;
    HAL_DAC_Init(&g_dac_handle); /* 初始化DAC */

    dac_ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;              /* 不使用触发功能 */
    dac_ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE; /* DAC1输出缓冲关闭 */

    switch (outx)
    {
    case 1:
        HAL_DAC_ConfigChannel(&g_dac_handle, &dac_ch_conf, DAC_CHANNEL_1); /* 配置DAC通道1 */
        HAL_DAC_Start(&g_dac_handle, DAC_CHANNEL_1);                       /* 开启DAC通道1 */
        break;
    case 2:
        HAL_DAC_ConfigChannel(&g_dac_handle, &dac_ch_conf, DAC_CHANNEL_2); /* 配置DAC通道2 */
        HAL_DAC_Start(&g_dac_handle, DAC_CHANNEL_2);                       /* 开启DAC通道2 */
        break;
    default:
        break;
    }
}

// //dac msp初始化
void dac_MspInit(DAC_HandleTypeDef *hdac)
{
    if (hdac->Instance == DAC)
    {
        GPIO_InitTypeDef gpio_init_struct = {0};

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_DAC_CLK_ENABLE();

        gpio_init_struct.Pin = GPIO_PIN_4;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &gpio_init_struct);
    }
}

// 设置通道输出电压
//  arge0: 1/2
//  arge1: 0~3300
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)
        temp = 4095; /* 如果值大于等于4096, 则取4095 */

    if (outx == 1) /* 通道1 */
    {
        HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp); /* 12位右对齐数据格式设置DAC值 */
    }
    else /* 通道2 */
    {
        HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp); /* 12位右对齐数据格式设置DAC值 */
    }
}
/**
 * @brief       设置DAC_OUT1输出三角波
 *   @note      输出频率 ≈ 1000 / (dt * samples) Khz, 不过在dt较小的时候,比如小于5us时, 由于delay_us
 *              本身就不准了(调用函数,计算等都需要时间,延时很小的时候,这些时间会影响到延时), 频率会偏小.
 *
 * @param       maxval : 最大值(0 < maxval < 4096), (maxval + 1)必须大于等于samples/2
 * @param       dt     : 每个采样点的延时时间(单位: us)
 * @param       samples: 采样点的个数, samples必须小于等于(maxval + 1) * 2 , 且maxval不能等于0
 * @param       n      : 输出波形个数,0~65535
 *
 * @retval      无
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval; /* 递增量 */
    float Curval; /* 当前值 */

    if ((maxval + 1) <= samples)
        return; /* 数据不合法 */

    incval = (maxval + 1) / (samples / 2); /* 计算递增量 */

    for (j = 0; j < n; j++)
    {
        Curval = 0;
        HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval); /* 先输出0 */
        for (i = 0; i < (samples / 2); i++)                                      /* 输出上升沿 */
        {
            Curval += incval; /* 新的输出值 */
            HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval);
            delay_us(dt);
        }
        for (i = 0; i < (samples / 2); i++) /* 输出下降沿 */
        {
            Curval -= incval; /* 新的输出值 */
            HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval);
            delay_us(dt);
        }
    }
}

void dac_dma_wave_init(void)
{
    DAC_ChannelConfTypeDef dac_ch_conf = {0};

    __HAL_RCC_DMA2_CLK_DISABLE();

    g_dma_adc_handle.Instance = DMA2_Channel3;
    g_dma_adc_handle.Init.Direction = DMA_MEMORY_TO_PERIPH; // 数据从内存搬运到寄存器
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 字宽
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    g_dma_adc_handle.Init.Mode = DMA_CIRCULAR;
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;

    HAL_DMA_Init(&g_dma_adc_handle);

    __HAL_LINKDMA(&g_dac_handle, DMA_Handle1, g_dma_adc_handle);

    g_dac_handle.Instance = DAC;
    HAL_DAC_Init(&g_dac_handle);

    dac_ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    dac_ch_conf.DAC_Trigger = DAC_TRIGGER_T7_TRGO; // 设置定时器7触发

    HAL_DAC_ConfigChannel(&g_dac_handle, &dac_ch_conf, DAC_CHANNEL_1);

    HAL_DMA_Start(&g_dma_adc_handle, (uint32_t)g_dac_sin_buf, (uint32_t)&DAC->DHR12R1, 0);
}

void dac_dma_wave_enable(uint16_t cndtr, uint16_t arr, uint16_t psc)
{
    TIM_HandleTypeDef g_tim_dac_handle = {0};
    TIM_MasterConfigTypeDef g_tim_mc_handle = {0};

    __HAL_RCC_TIM7_CLK_ENABLE();

    g_tim_dac_handle.Instance = TIM7;
    g_tim_dac_handle.Init.Prescaler = psc;
    g_tim_dac_handle.Init.Period = arr;
    HAL_TIM_Base_Init(&g_tim_dac_handle);

    g_tim_mc_handle.MasterOutputTrigger = TIM_TRGO_UPDATE;
    g_tim_mc_handle.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    HAL_TIMEx_MasterConfigSynchronization(&g_tim_dac_handle, &g_tim_mc_handle);

    HAL_TIM_Base_Start(&g_tim_dac_handle);

    HAL_DAC_Stop_DMA(&g_dac_handle, DAC_CHANNEL_1);
    HAL_DAC_Start_DMA(&g_dac_handle, DAC_CHANNEL_1, (uint32_t *)g_dac_sin_buf, cndtr, DAC_ALIGN_12B_R);
}


/**
 * @brief       产生正弦波函序列
 *   @note      需保证: maxval > samples/2
 *
 * @param       maxval : 最大值(0 < maxval < 2048)
 * @param       samples: 采样点的个数
 *
 * @retval      无
 */
void dac_creat_sin_buf(uint16_t maxval, uint16_t samples)
{
	uint8_t i;
	float inc = (2 * 3.1415962) / samples; /* 计算增量（一个周期DAC_SIN_BUF个点）*/
	float outdata = 0;

	for (i = 0; i < samples; i++)
	{
		outdata = maxval * (1 + sin(inc * i)); /* 计算以dots个点为周期的每个点的值，放大maxval倍，并偏移到正数区域 */
		if (outdata > 4095)
			outdata = 4095; /* 上限限定 */
		// printf("%f\r\n",outdata);
		g_dac_sin_buf[i] = outdata;
	}
}

