#include "./BSP/DAC/dac.h"

DAC_HandleTypeDef g_dac_handle;         /* DAC句柄 */

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
    GPIO_InitTypeDef gpio_init_struct;
    DAC_ChannelConfTypeDef dac_ch_conf;
    
    __HAL_RCC_DAC_CLK_ENABLE();                                 /* 使能DAC1的时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                               /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    
    gpio_init_struct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;  /* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    g_dac_handle.Instance = DAC;
    HAL_DAC_Init(&g_dac_handle);                                /* 初始化DAC */


    dac_ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;                 /* 不使用触发功能 */
    dac_ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;    /* DAC1输出缓冲关闭 */
    
    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&g_dac_handle, &dac_ch_conf, DAC_CHANNEL_1);  /* 配置DAC通道1 */
            HAL_DAC_Start(&g_dac_handle,DAC_CHANNEL_1);                         /* 开启DAC通道1 */
            break;
        case 2:
            HAL_DAC_ConfigChannel(&g_dac_handle, &dac_ch_conf, DAC_CHANNEL_2);  /* 配置DAC通道2 */
            HAL_DAC_Start(&g_dac_handle,DAC_CHANNEL_2);                         /* 开启DAC通道2 */
            break;
        default:break;
    }

}

// //dac msp初始化
// void dac_MspInit(DAC_HandleTypeDef *hdac)
// {
//     if (hdac->Instance == DAC)
//     {
//         GPIO_InitTypeDef gpio_init_struct;
        
//         __HAL_RCC_GPIOA_CLK_ENABLE();
//         __HAL_RCC_DAC_CLK_ENABLE();
        
//         gpio_init_struct.Pin = GPIO_PIN_5;
//         gpio_init_struct.Mode = GPIO_MODE_ANALOG;
//         HAL_GPIO_Init(GPIOA ,&gpio_init_struct);
//     }
// }

//设置通道输出电压
// arge0: 1/2
// arge1: 0~3300
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* 如果值大于等于4096, 则取4095 */

    if (outx == 1)   /* 通道1 */
    {
        HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp); /* 12位右对齐数据格式设置DAC值 */
    }
    else            /* 通道2 */
    {
        HAL_DAC_SetValue(&g_dac_handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp); /* 12位右对齐数据格式设置DAC值 */
    }
}
