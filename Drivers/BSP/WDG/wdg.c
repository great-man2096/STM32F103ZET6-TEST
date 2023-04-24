#include "./BSP/LED/led.h"
#include "./BSP/WDG/wdg.h"

/****************************************************************************************/
IWDG_HandleTypeDef g_iwdg_handle;   /* 独立看门狗句柄 */

/**
 * @brief       初始化独立看门狗 
 * @param       prer: IWDG_PRESCALER_4~IWDG_PRESCALER_256,对应4~256分频
 *   @arg       分频因子 = 4 * 2^prer. 但最大值只能是256!
 * @param       rlr: 自动重装载值,0~0XFFF. 
 * @note        时间计算(大概):Tout=((4 * 2^prer) * rlr) / 40 (ms). 
 * @retval      无
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    g_iwdg_handle.Instance = IWDG;
    g_iwdg_handle.Init.Prescaler = prer; /* 设置IWDG分频系数 */
    g_iwdg_handle.Init.Reload = rlr;     /* 重装载值 */
    HAL_IWDG_Init(&g_iwdg_handle);       /* 初始化IWDG并启动 */
}

/**
 * @brief       喂独立看门狗
 * @param       无
 * @retval      无
 */
void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&g_iwdg_handle); /* 重装载计数器 */
}
/****************************************************************************************/
WWDG_HandleTypeDef g_wwdg_handle;   /* 窗口看门狗句柄 */

void wwdg_init(uint8_t cr,uint8_t wr,uint32_t fprer){
	
	g_wwdg_handle.Instance = WWDG;
	g_wwdg_handle.Init.Counter = cr;
	g_wwdg_handle.Init.EWIMode = WWDG_EWI_ENABLE;
	g_wwdg_handle.Init.Prescaler = fprer;
	g_wwdg_handle.Init.Window = wr;
	HAL_WWDG_Init(&g_wwdg_handle);       /* 初始化IWDG并启动 */
}


void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg){
	__HAL_RCC_WWDG_CLK_ENABLE(); /* 使能窗口看门狗时钟 */
	HAL_NVIC_SetPriority(WWDG_IRQn,2,3);
	HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

/**
* @brief 窗口看门狗中断服务程序
* @param 无
* @retval 无
*/
void WWDG_IRQHandler(void)
{
 HAL_WWDG_IRQHandler(&g_wwdg_handle);
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg){
	HAL_WWDG_Refresh(&g_wwdg_handle);
	LED1_TOGGLE(); /* LED1 闪烁 */
}


void wwdg_feed(){
	HAL_WWDG_Refresh(&g_wwdg_handle);
};










