#include "./BSP/PWMDAC/pwmdac.h"


TIM_HandleTypeDef g_tim1_handle;

/**
* @brief PWM DAC 初始化, 实际上就是初始化定时器
* @note
* 定时器的时钟来自 APB1 / APB2, 当 APB1 / APB2 分频时, 定时器频率自动翻倍
* 所以, 一般情况下, 我们所有定时器的频率, 都是 72Mhz 等于系统时钟频率
* 定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft = 定时器工作频率, 单位: Mhz
* @param arr: 自动重装值。
* @param psc: 时钟预分频数
* @retval 无
*/
void pwmdac_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_pwmdac = {0};
    PWMDAC_TIMX_CLK_ENABLE(); /* PWM DAC 定时器时钟使能 */
    g_tim1_handle.Instance = TIM1; /* 定时器 1 */
    g_tim1_handle.Init.Prescaler = psc; /* 定时器分频 */
    g_tim1_handle.Init.CounterMode = TIM_COUNTERMODE_UP; /* 递增计数模式 */
    g_tim1_handle.Init.Period = arr; /* 自动重装载值 */
    /* 使能 TIMx_ARR 进行缓冲 */
    g_tim1_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 
    HAL_TIM_PWM_Init(&g_tim1_handle); /* 初始化 PWM */
    timx_oc_pwmdac.OCMode = TIM_OCMODE_PWM1; /* CH1/2 PWM 模式 1 */
    timx_oc_pwmdac.Pulse = 0; /* 设置比较值,此值用来确定占空比 */
    timx_oc_pwmdac.OCPolarity = TIM_OCPOLARITY_HIGH; /* 输出比较极性为高 */
    /* 配置 TIM1 通道 1 */
    HAL_TIM_PWM_ConfigChannel(&g_tim1_handle, &timx_oc_pwmdac, PWMDAC_TIMX_CHY);
    HAL_TIM_PWM_Start(&g_tim1_handle, TIM_CHANNEL_1); /* 开启定时器 1 通道 1 */
}

/**
* @brief 定时器底层驱动，时钟使能，引脚配置
* @note
* 此函数会被 HAL_TIM_PWM_Init()调用
* @param htim:定时器句柄
* @retval 无
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct;
    if (htim->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE(); /* 使能定时器 1 */
        __HAL_AFIO_REMAP_TIM1_PARTIAL(); /* TIM1 通道引脚部分重映射使能 */
        PWMDAC_GPIO_CLK_ENABLE(); /* GPIO 时钟使能 */
        gpio_init_struct.Pin = PWMDAC_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(PWMDAC_GPIO_PORT, &gpio_init_struct);
    }
}

/**
* @brief 设置 PWM DAC 输出电压
* @param vol : 0~3300,代表 0~3.3V
* @retval 无
*/
void pwmdac_set_voltage(uint16_t vol)
{
    float temp = vol;
    temp /= 1000; /* 缩小 1000 倍, 得到实际电压值 */
    temp = temp * 256 / 3.3; /* 将电压转换成 PWM 占空比 */
    __HAL_TIM_SET_COMPARE(&g_tim1_handle, PWMDAC_TIMX_CHY, temp);/*设置新的占空比*/
}



