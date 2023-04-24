#ifndef __EXTI_H
#define __EXTI_H

#include "./SYSTEM/sys/sys.h"


/* 引脚 和 中断编号 & 中断服务函数 定义 */
#define KEY0_INT_GPIO_PORT GPIOE
#define KEY0_INT_GPIO_PIN GPIO_PIN_0
/* PE 口时钟使能 */
#define KEY0_INT_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)
#define KEY0_INT_IRQn EXTI0_IRQn
#define KEY0_INT_IRQHandler EXTI0_IRQHandler

#define KEY1_INT_GPIO_PORT GPIOC
#define KEY1_INT_GPIO_PIN GPIO_PIN_13
/* PC 口时钟使能 */
#define KEY1_INT_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)
#define KEY1_INT_IRQn EXTI15_10_IRQn
#define KEY1_INT_IRQHandler EXTI15_10_IRQHandler


void exti_init(void);

#endif

