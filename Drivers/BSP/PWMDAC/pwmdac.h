#ifndef __PWMDAC_H
#define __PWMDAC_H

#include "./SYSTEM/sys/sys.h"

/*
* PWMDAC 默认是使用 PA8, 对应的定时器为 TIM1_CH1, 如果你要修改成其他 IO 输出, 则相应
* 的定时器及通道也要进行修改. 请根据实际情况进行修改.
*/
#define PWMDAC_GPIO_PORT GPIOA
#define PWMDAC_GPIO_PIN GPIO_PIN_8
/* PA 口时钟使能 */
#define PWMDAC_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE();}while(0)
#define PWMDAC_TIMX TIM1
#define PWMDAC_TIMX_CHY TIM_CHANNEL_1 /* 通道 Y, 1<= Y <=4 */
#define PWMDAC_TIMX_CCRX PWMDAC_TIMX->CCR1 /* 通道 Y 的输出比较寄存器 */
/* TIM1 时钟使能 */
#define PWMDAC_TIMX_CLK_ENABLE() do{ __HAL_RCC_TIM1_CLK_ENABLE();}while(0) 

void pwmdac_init(uint16_t arr, uint16_t psc);
void pwmdac_set_voltage(uint16_t vol);

#endif 
