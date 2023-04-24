#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/* TIMX ���ָ������ PWM ����
* ��������� PWM ͨ�� PC6(TIM8_CH1)���, �����öŰ������� PC6 �� PE5, Ȼ���ڳ������潫 PE5 ��
* �óɸ�������Ϳ��� ���� TIM8_CH1 ���� LED1(GREEN)������, ����һ�α�ʾһ�� PWM ��
* Ĭ��ʹ�õ��� TIM8_CH1.
* ע��: ͨ���޸��⼸���궨��, ����֧�� TIM1/TIM8 ��ʱ��, ����һ�� IO �����ָ�������� PWM
*/
#define ATIM_TIMX_NPWM_CHY_GPIO_PORT	GPIOC
#define ATIM_TIMX_NPWM_CHY_GPIO_PIN		GPIO_PIN_6
#define ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0) /* PC ��ʱ��ʹ�� */

#define ATIM_TIMX_NPWM				TIM8
#define ATIM_TIMX_NPWM_IRQn			TIM8_UP_IRQn
#define ATIM_TIMX_NPWM_IRQHandler	TIM8_UP_IRQHandler
#define ATIM_TIMX_NPWM_CHY			TIM_CHANNEL_1 /* ͨ�� Y, 1<= Y <=4 */
#define ATIM_TIMX_NPWM_CHY_CCRX		TIM8->CCR1/* ͨ�� Y ������ȽϼĴ��� */
#define ATIM_TIMX_NPWM_CHY_CLK_ENABLE()	do{ __HAL_RCC_TIM8_CLK_ENABLE();}while(0)




/* TIMX ����Ƚ�ģʽ ����
* ����ͨ�� TIM8 ������Ƚ�ģʽ,���� PC6,PC7,PC8,PC9 ��� 4 · PWM,ռ�ձ� 50%,����ÿһ· PWM
* ֮�����λ��Ϊ 25%���޸� CCRx �����޸���λ. Ĭ������� TIM8
* ע��: ͨ���޸���Щ�궨��,����֧�� TIM1/TIM8 ����һ����ʱ��,����һ�� IO ��ʹ������Ƚ�ģʽ,
* ��� PWM
*/
#define ATIM_TIMX_COMP_CH1_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH1_GPIO_PIN		GPIO_PIN_6
#define ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC ��ʱ��ʹ�� */
#define ATIM_TIMX_COMP_CH2_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH2_GPIO_PIN		GPIO_PIN_7
#define ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC ��ʱ��ʹ�� */
#define ATIM_TIMX_COMP_CH3_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH3_GPIO_PIN		GPIO_PIN_8
#define ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC ��ʱ��ʹ�� */
#define ATIM_TIMX_COMP_CH4_GPIO_PORT	GPIOC
#define ATIM_TIMX_COMP_CH4_GPIO_PIN		GPIO_PIN_9
#define ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC ��ʱ��ʹ�� */

#define ATIM_TIMX_COMP				TIM8 
#define ATIM_TIMX_COMP_CH1_CCRX		ATIM_TIMX_COMP->CCR1 /* ͨ�� 1 ������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CH2_CCRX		ATIM_TIMX_COMP->CCR2 /* ͨ�� 2 ������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CH3_CCRX		ATIM_TIMX_COMP->CCR3 /* ͨ�� 3 ������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CH4_CCRX		ATIM_TIMX_COMP->CCR4 /* ͨ�� 4 ������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CLK_ENABLE()	do{ __HAL_RCC_TIM8_CLK_ENABLE();}while(0) /* TIM8 ʱ��ʹ�� */

/*****************************************************************************/
/* TIMX �������ģʽ ����
* �������û���������Ӳ������, CHY ���������, CHYN ���������
* �޸� CCRx �����޸�ռ�ձ�.
* Ĭ������� TIM1
* ע��: ͨ���޸���Щ�궨��,����֧�� TIM1/TIM8 ��ʱ��, ����һ�� IO ��������� PWM(ǰ���Ǳ�����
�����������)
*/
/* ���ͨ������ */
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT GPIOE
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN GPIO_PIN_9
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0) /* PE ��ʱ��ʹ�� */
/* �������ͨ������ */
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT GPIOE
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN GPIO_PIN_8
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0) /* PE ��ʱ��ʹ�� */
/* ɲ���������� */
#define ATIM_TIMX_CPLM_BKIN_GPIO_PORT GPIOE
#define ATIM_TIMX_CPLM_BKIN_GPIO_PIN GPIO_PIN_15
#define ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0) /* PE ��ʱ��ʹ�� */
/* TIMX REMAP ����
* ��Ϊ PE8/PE9/PE15, Ĭ�ϲ����� TIM1 �ĸ��ù��ܽ�, ���뿪����ȫ��ӳ��, �ſ��Խ�: 
TIM1_CH1->PE9; TIM1_CH1N->PE8; TIM1_BKIN->PE15;
* ����, PE8/PE9/PE15, �������� TIM1 �� CH1N/CH1/BKIN ����.
* ���Ա���ʵ�� ATIM_TIMX_CPLM_CHYN_GPIO_REMAP, ͨ�� sys_gpio_remap_set ����������ӳ��
* �������ʹ��Ĭ�ϵĸ��ù������, ����������ӳ��, �ǿ��Բ���Ҫ�ú�����! ���ݾ�����Ҫ��ʵ��.
*/
#define ATIM_TIMX_CPLM_CHYN_GPIO_REMAP() do{__HAL_RCC_AFIO_CLK_ENABLE();__HAL_AFIO_REMAP_TIM1_ENABLE();}while(0) 
/* �������ʹ�õĶ�ʱ�� */
#define ATIM_TIMX_CPLM TIM1 
#define ATIM_TIMX_CPLM_CHY TIM_CHANNEL_1 
#define ATIM_TIMX_CPLM_CHY_CCRY ATIM_TIMX_CPLM->CCR1 
#define ATIM_TIMX_CPLM_CLK_ENABLE() do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0) /* TIM1 ʱ��ʹ�� */
/*****************************************************************************/


/* TIMX PWM ����ģʽ ����
* ��������벶��ʹ�ö�ʱ�� TIM8_CH1
* Ĭ������� TIM1/TIM8 �ȸ߼���ʱ��
* ע��: ͨ���޸��⼸���궨��,����֧�� TIM1~TIM8 ����һ����ʱ����ͨ�� 1/ͨ�� 2
*/
#define ATIM_TIMX_PWMIN_CHY_GPIO_PORT			GPIOC
#define ATIM_TIMX_PWMIN_CHY_GPIO_PIN			GPIO_PIN_6
#define ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE()	do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0) /* PC IO��ʱ��ʹ�� */
#define ATIM_TIMX_PWMIN							TIM8 
#define ATIM_TIMX_PWMIN_IRQn					TIM8_UP_IRQn
#define ATIM_TIMX_PWMIN_IRQHandler				TIM8_UP_IRQHandler
#define ATIM_TIMX_PWMIN_CHY						TIM_CHANNEL_1 /* ͨ�� Y, 1<= Y <=2*/
#define ATIM_TIMX_PWMIN_CHY_CLK_ENABLE()		do{ __HAL_RCC_TIM8_CLK_ENABLE(); }while(0) /* TIM8 ʱ��ʹ�� */
/* TIM1 / TIM8 �ж����Ĳ����жϷ�����,��Ҫ��������,���� TIM2~5 ��,����Ҫ���¶��� */
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
