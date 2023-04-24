#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/* TIMX 输出指定个数 PWM 定义
* 这里输出的 PWM 通过 PC6(TIM8_CH1)输出, 我们用杜邦线连接 PC6 和 PE5, 然后在程序里面将 PE5 设
* 置成浮空输入就可以 看到 TIM8_CH1 控制 LED1(GREEN)的亮灭, 亮灭一次表示一个 PWM 波
* 默认使用的是 TIM8_CH1.
* 注意: 通过修改这几个宏定义, 可以支持 TIM1/TIM8 定时器, 任意一个 IO 口输出指定个数的 PWM
*/
#define ATIM_TIMX_NPWM_CHY_GPIO_PORT	GPIOC
#define ATIM_TIMX_NPWM_CHY_GPIO_PIN		GPIO_PIN_6
#define ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0) /* PC 口时钟使能 */

#define ATIM_TIMX_NPWM				TIM8
#define ATIM_TIMX_NPWM_IRQn			TIM8_UP_IRQn
#define ATIM_TIMX_NPWM_IRQHandler	TIM8_UP_IRQHandler
#define ATIM_TIMX_NPWM_CHY			TIM_CHANNEL_1 /* 通道 Y, 1<= Y <=4 */
#define ATIM_TIMX_NPWM_CHY_CCRX		TIM8->CCR1/* 通道 Y 的输出比较寄存器 */
#define ATIM_TIMX_NPWM_CHY_CLK_ENABLE()	do{ __HAL_RCC_TIM8_CLK_ENABLE();}while(0)




/* TIMX 输出比较模式 定义
* 这里通过 TIM8 的输出比较模式,控制 PC6,PC7,PC8,PC9 输出 4 路 PWM,占空比 50%,并且每一路 PWM
* 之间的相位差为 25%，修改 CCRx 可以修改相位. 默认是针对 TIM8
* 注意: 通过修改这些宏定义,可以支持 TIM1/TIM8 任意一个定时器,任意一个 IO 口使用输出比较模式,
* 输出 PWM
*/
#define ATIM_TIMX_COMP_CH1_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH1_GPIO_PIN		GPIO_PIN_6
#define ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC 口时钟使能 */
#define ATIM_TIMX_COMP_CH2_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH2_GPIO_PIN		GPIO_PIN_7
#define ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC 口时钟使能 */
#define ATIM_TIMX_COMP_CH3_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH3_GPIO_PIN		GPIO_PIN_8
#define ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC 口时钟使能 */
#define ATIM_TIMX_COMP_CH4_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH4_GPIO_PIN		GPIO_PIN_9
#define ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC 口时钟使能 */

#define ATIM_TIMX_COMP				TIM8 
#define ATIM_TIMX_COMP_CH1_CCRX		ATIM_TIMX_COMP->CCR1 /* 通道 1 的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH2_CCRX		ATIM_TIMX_COMP->CCR2 /* 通道 2 的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH3_CCRX		ATIM_TIMX_COMP->CCR3 /* 通道 3 的输出比较寄存器 */
#define ATIM_TIMX_COMP_CH4_CCRX		ATIM_TIMX_COMP->CCR4 /* 通道 4 的输出比较寄存器 */
#define ATIM_TIMX_COMP_CLK_ENABLE()	do{ __HAL_RCC_TIM8_CLK_ENABLE();}while(0) /* TIM8 时钟使能 */

/*****************************************************************************/
/* TIMX 互补输出模式 定义
* 这里设置互补输出相关硬件配置, CHY 即正常输出, CHYN 即互补输出
* 修改 CCRx 可以修改占空比.
* 默认是针对 TIM1
* 注意: 通过修改这些宏定义,可以支持 TIM1/TIM8 定时器, 任意一个 IO 口输出互补 PWM(前提是必须有
互补输出功能)
*/
/* 输出通道引脚 */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT GPIOE
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN GPIO_PIN_9
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0) /* PE 口时钟使能 */
/* 互补输出通道引脚 */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT GPIOE
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN GPIO_PIN_8
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0) /* PE 口时钟使能 */
/* 刹车输入引脚 */
#define ATIM_TIMX_CPLM_BKIN_GPIO_PORT GPIOE
#define ATIM_TIMX_CPLM_BKIN_GPIO_PIN GPIO_PIN_15
#define ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0) /* PE 口时钟使能 */
/* TIMX REMAP 设置
* 因为 PE8/PE9/PE15, 默认并不是 TIM1 的复用功能脚, 必须开启完全重映射, 才可以将: 
TIM1_CH1->PE9; TIM1_CH1N->PE8; TIM1_BKIN->PE15;
* 这样, PE8/PE9/PE15, 才能用作 TIM1 的 CH1N/CH1/BKIN 功能.
* 所以必须实现 ATIM_TIMX_CPLM_CHYN_GPIO_REMAP, 通过 sys_gpio_remap_set 函数设置重映射
* 如果我们使用默认的复用功能输出, 则不用设置重映射, 是可以不需要该函数的! 根据具体需要来实现.
*/
#define ATIM_TIMX_CPLM_CHYN_GPIO_REMAP() do{__HAL_RCC_AFIO_CLK_ENABLE();__HAL_AFIO_REMAP_TIM1_ENABLE();}while(0) 
/* 互补输出使用的定时器 */
#define ATIM_TIMX_CPLM TIM1 
#define ATIM_TIMX_CPLM_CHY TIM_CHANNEL_1 
#define ATIM_TIMX_CPLM_CHY_CCRY ATIM_TIMX_CPLM->CCR1 
#define ATIM_TIMX_CPLM_CLK_ENABLE() do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0) /* TIM1 时钟使能 */
/*****************************************************************************/


/* TIMX PWM 输入模式 定义
* 这里的输入捕获使用定时器 TIM8_CH1
* 默认是针对 TIM1/TIM8 等高级定时器
* 注意: 通过修改这几个宏定义,可以支持 TIM1~TIM8 任意一个定时器的通道 1/通道 2
*/
#define ATIM_TIMX_PWMIN_CHY_GPIO_PORT			GPIOC
#define ATIM_TIMX_PWMIN_CHY_GPIO_PIN			GPIO_PIN_6
#define ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE()	do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC IO口时钟使能 */
#define ATIM_TIMX_PWMIN							TIM8 
#define ATIM_TIMX_PWMIN_IRQn					TIM8_UP_IRQn
#define ATIM_TIMX_PWMIN_IRQHandler				TIM8_UP_IRQHandler
#define ATIM_TIMX_PWMIN_CHY						TIM_CHANNEL_1 /* 通道 Y, 1<= Y <=2*/
#define ATIM_TIMX_PWMIN_CHY_CLK_ENABLE()		do{ __HAL_RCC_TIM8_CLK_ENABLE(); }while(0) /* TIM8 时钟使能 */
/* TIM1 / TIM8 有独立的捕获中断服务函数,需要单独定义,对于 TIM2~5 等,则不需要以下定义 */
#define ATIM_TIMX_PWMIN_CC_IRQn					TIM8_CC_IRQn
#define ATIM_TIMX_PWMIN_CC_IRQHandler			TIM8_CC_IRQHandler


void atim_timx_npwm_chy_init(uint16_t arr, uint16_t psc);
void atim_timx_npwm_chy_set(uint8_t npwm);

void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc);

void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc);
void atim_timx_cplm_pwm_set(uint16_t ccr, uint16_t dtg);

void atim_timx_pwmin_chy_init(void);
void atim_timx_pwmin_chy_restart(void);

#endif

