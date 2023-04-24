#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

#define LED0TC TIM4

void gtim_timx_pwm_chy_init(uint16_t arr,uint16_t psc);

void gtim_timx_cap_chy_init(uint16_t arr,uint16_t psc);


/* TIMX �����������
* ������������ʹ�ö�ʱ�� TIM2_CH1,���� WK_UP ����������
* Ĭ������� TIM2~TIM5, ֻ�� CH1 �� CH2 ͨ�����������������, CH3/CH4 ��֧��!
* ע��: ͨ���޸��⼸���궨��,����֧�� TIM1~TIM8 ����һ����ʱ��,CH1/CH2 ��Ӧ IO �����������
* �ر�Ҫע��:Ĭ���õ� PA0,���õ�����������!��������� IO,��Ӧ����������ʽҲ�ø�!
*/
#define GTIM_TIMX_CNT_CHY_GPIO_PORT GPIOA
#define GTIM_TIMX_CNT_CHY_GPIO_PIN GPIO_PIN_0
#define GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE() \
do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) /* PA ��ʱ��ʹ�� */
#define GTIM_TIMX_CNT TIM2
#define GTIM_TIMX_CNT_IRQn TIM2_IRQn
#define GTIM_TIMX_CNT_IRQHandler TIM2_IRQHandler
#define GTIM_TIMX_CNT_CHY TIM_CHANNEL_1 /* ͨ�� Y, 1<= Y <=2 */
#define GTIM_TIMX_CNT_CHY_CLK_ENABLE() \
do{ __HAL_RCC_TIM2_CLK_ENABLE(); }while(0) /* TIM2 ʱ��ʹ�� */


/*********************************������ͨ�ö�ʱ��PWM���ʵ����غ궨��*************************************/

/* TIMX PWM������� 
 * ���������PWM����LED0(RED)������
 * Ĭ�������TIM2~TIM5
 * ע��: ͨ���޸��⼸���궨��,����֧��TIM1~TIM8����һ����ʱ��,����һ��IO�����PWM
 */
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          GPIO_PIN_5
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

/* TIMX REMAP����
 * ��Ϊ����LED0����PB5��, ����ͨ������TIM3�Ĳ�����ӳ�书��, ���ܽ�TIM3_CH2�����PB5��
 * ���, ����ʵ��GTIM_TIMX_PWM_CHY_GPIO_REMAP
 * ����Щʹ��Ĭ�����õĶ�ʱ��PWM�����, ����������ӳ��, �ǲ���Ҫ�ú�����!
 */
#define GTIM_TIMX_PWM_CHY_GPIO_REMAP()      do{__HAL_RCC_AFIO_CLK_ENABLE(); __HAL_AFIO_REMAP_TIM3_PARTIAL(); }while(0)            /* ͨ��REMAP����, �ú������Ǳ����, ������Ҫʵ�� */

#define GTIM_TIMX_PWM                       TIM3 
#define GTIM_TIMX_PWM_CHY                   TIM_CHANNEL_2                               /* ͨ��Y,  1<= Y <=4 */
#define GTIM_TIMX_PWM_CHY_CCRX              TIM3->CCR2                                  /* ͨ��Y������ȽϼĴ��� */
#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 ʱ��ʹ�� */



void gtim_timx_cnt_chy_init(uint16_t psc);
uint32_t gtim_timx_cnt_chy_get_count(void);
void gtim_timx_cnt_chy_restart(void);

#endif

