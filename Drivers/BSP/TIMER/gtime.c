#include "./BSP/TIMER/gtime.h"
#include "./BSP/LED/led.h"

TIM_HandleTypeDef g_timx_pwm_chy_handle;

TIM_HandleTypeDef g_timx_cap_chy_handle;

TIM_HandleTypeDef	g_timx_cnt_chy_handler;

/**
* @brief ͨ�ö�ʱ�� TIMX ͨ�� Y ������� ��ʼ������
* @note
* ������ѡ��ͨ�ö�ʱ����ʱ��ѡ��: �ⲿʱ��Դģʽ 1(SMS[2:0] = 111)
* CNT �ļ���ʱ��Դ������ TIMX_CH1/CH2, ����ʵ���ⲿ�������(������� CH1/CH2)
*
* ʱ�ӷ�Ƶ�� = psc, һ������Ϊ 0, ��ʾÿһ��ʱ�Ӷ������һ��, ����߾���.
* ͨ����ȡ CNT ���������, �����򵥼���, ���Եõ���ǰ�ļ���ֵ, �Ӷ�ʵ���������
* @param arr: �Զ���װֵ
* @retval ��
*/
void gtim_timx_cnt_chy_init(uint16_t psc)
{
 GPIO_InitTypeDef gpio_init_struct;
 TIM_SlaveConfigTypeDef tim_slave_config = {0};
 GTIM_TIMX_CNT_CHY_CLK_ENABLE(); /* ʹ�� TIMx ʱ�� */
 GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE(); /* ���� GPIOA ʱ�� */
 
 g_timx_cnt_chy_handler.Instance = GTIM_TIMX_CNT; /* ��ʱ�� x */
 g_timx_cnt_chy_handler.Init.Prescaler = psc; /* ��ʱ����Ƶ */
 g_timx_cnt_chy_handler.Init.CounterMode = TIM_COUNTERMODE_UP;/* ��������ģʽ */
 g_timx_cnt_chy_handler.Init.Period = 65535; /* �Զ���װ��ֵ */
 HAL_TIM_IC_Init(&g_timx_cnt_chy_handler);
 gpio_init_struct.Pin = GTIM_TIMX_CNT_CHY_GPIO_PIN; /* ���벶��� GPIO �� */
 gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* ����������� */
 gpio_init_struct.Pull = GPIO_PULLDOWN; /* ���� */
 gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
 HAL_GPIO_Init(GTIM_TIMX_CNT_CHY_GPIO_PORT, &gpio_init_struct);
 /* ��ģʽ���ⲿ����ģʽ 1 */
 tim_slave_config.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;/*��ģʽ���ⲿ����ģʽ 1*/
 tim_slave_config.InputTrigger = TIM_TS_TI1FP1; /* TI1FP1 ��Ϊ��������Դ */
 tim_slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;/* ������ */
 tim_slave_config.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;/* ����Ƶ*/
 tim_slave_config.TriggerFilter = 0x0; /* �˲��������в���Ҫ�κ��˲� */
HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_chy_handler, &tim_slave_config);
/* �����ж����ȼ�����ռ���ȼ� 1�������ȼ� 3 */
 HAL_NVIC_SetPriority(GTIM_TIMX_CNT_IRQn, 1, 3); 
 HAL_NVIC_EnableIRQ(GTIM_TIMX_CNT_IRQn);
 __HAL_TIM_ENABLE_IT(&g_timx_cnt_chy_handler, TIM_IT_UPDATE); /* ʹ�ܸ����ж� */
 HAL_TIM_IC_Start(&g_timx_cnt_chy_handler, GTIM_TIMX_CNT_CHY);/* ʹ��ͨ������ */
}

/**
* @brief ͨ�ö�ʱ�� TIMX ������� �����жϷ�����
* @param ��
* @retval ��
*/

uint8_t g_timxchy_cnt_ofcnt;
void GTIM_TIMX_CNT_IRQHandler(void)
{
 /* ���´���û��ʹ�ö�ʱ�� HAL �⹲�ô���������������ֱ��ͨ���ж��жϱ�־λ�ķ�ʽ */
 if(__HAL_TIM_GET_FLAG(&g_timx_cnt_chy_handler, TIM_FLAG_UPDATE) != RESET)
 {
	g_timxchy_cnt_ofcnt++; /* �ۼ�������� */
 }
 __HAL_TIM_CLEAR_IT(&g_timx_cnt_chy_handler, TIM_IT_UPDATE);
}

/**
* @brief ͨ�ö�ʱ�� TIMX ͨ�� Y ��ȡ��ǰ����ֵ
* @param ��
* @retval ��ǰ����ֵ
*/
uint32_t gtim_timx_cnt_chy_get_count(void)
{
 uint32_t count = 0;
 count = g_timxchy_cnt_ofcnt * 65536;/* �������������Ӧ�ļ���ֵ */
 count += __HAL_TIM_GET_COUNTER(&g_timx_cnt_chy_handler);/* ���ϵ�ǰ CNT ��ֵ */
 return count;
}

/**
* @brief ͨ�ö�ʱ�� TIMX ͨ�� Y ����������
* @param ��
* @retval ��ǰ����ֵ
*/
void gtim_timx_cnt_chy_restart(void)
{
 __HAL_TIM_DISABLE(&g_timx_cnt_chy_handler); /* �رն�ʱ�� TIMX */
 g_timxchy_cnt_ofcnt = 0; /* �ۼ������� */
 __HAL_TIM_SET_COUNTER(&g_timx_cnt_chy_handler, 0); /* ���������� */
 __HAL_TIM_ENABLE(&g_timx_cnt_chy_handler); /* ʹ�ܶ�ʱ�� TIMX */
}




//ͨ�ö�ʱ��ͨ��y���벶���ʼ������
void gtim_timx_cap_chy_init(uint16_t arr,uint16_t psc){
	TIM_IC_InitTypeDef timx_ic_cap_chy = {0};
	
	g_timx_cap_chy_handle.Instance = TIM5;
	g_timx_cap_chy_handle.Init.Prescaler = psc;
	g_timx_cap_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	g_timx_cap_chy_handle.Init.Period =arr;
	HAL_TIM_IC_Init(&g_timx_cap_chy_handle);
	
	timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_RISING;
	timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;	 /* ӳ�䵽TI1�� */
	timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1;
	timx_ic_cap_chy.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&g_timx_cap_chy_handle,&timx_ic_cap_chy,TIM_CHANNEL_2);
	
	__HAL_TIM_ENABLE_IT(&g_timx_cap_chy_handle,TIM_IT_UPDATE);
	HAL_TIM_IC_Start_IT(&g_timx_cap_chy_handle,TIM_CHANNEL_2);
	
}
//��ʱ�����벶��MSP��ʼ������
//void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim){
//	if(htim->Instance == TIM5){
//		GPIO_InitTypeDef gpio_init_struct;
//		__HAL_RCC_TIM5_CLK_ENABLE();                                                                                                                                                                                                                                       
//		__HAL_RCC_GPIOA_CLK_ENABLE();
//		
//		gpio_init_struct.Pin = GPIO_PIN_1;
//		gpio_init_struct.Mode = GPIO_MODE_AF_PP;
//		gpio_init_struct.Pull = GPIO_PULLDOWN;
//		gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
//		HAL_GPIO_Init(GPIOA,&gpio_init_struct);
//		
//		HAL_NVIC_SetPriority(TIM5_IRQn,1,3);
//		HAL_NVIC_EnableIRQ(TIM5_IRQn);
//	}
//}
/* ���벶��״̬(g_timxchy_cap_sta)
 * [7]  :0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
 * [6]  :0,��û���񵽸ߵ�ƽ;1,�Ѿ����񵽸ߵ�ƽ��.
 * [5:0]:����ߵ�ƽ������Ĵ���,������63��,���������ֵ = 63*65536 + 65535 = 4194303
 *       ע��:Ϊ��ͨ��,����Ĭ��ARR��CCRy����16λ�Ĵ���,����32λ�Ķ�ʱ��(��:TIM5),Ҳֻ��16λʹ��
 *       ��1us�ļ���Ƶ��,����ʱ��Ϊ:4194303 us, Լ4.19��
 *
 *      (˵��һ�£�����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
 */
uint8_t g_timxchy_cap_sta = 0; //���벶��״̬
uint16_t g_timxchy_cap_val = 0; //���벶��ֵ

//��ʱ��5�жϷ�����
void TIM5_IRQHandler(void){
	HAL_TIM_IRQHandler(&g_timx_cap_chy_handle);
}

//��ʱ�����벶���жϴ���ص�����
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
//	if(htim->Instance == TIM5){
//		if((g_timxchy_cap_sta & 0X80)==0){
//			if(g_timxchy_cap_sta & 0X40){
//				g_timxchy_cap_sta |= 0X80;
//				g_timxchy_cap_val = HAL_TIM_ReadCapturedValue(&g_timx_cap_chy_handle,TIM_CHANNEL_2);
//				TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle,TIM_CHANNEL_2);
//				TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);
//				
//			}else{
//				g_timxchy_cap_sta = 0;
//				g_timxchy_cap_val = 0;
//				g_timxchy_cap_sta |= 0X40;
//				__HAL_TIM_DISABLE(&g_timx_cap_chy_handle);
//				__HAL_TIM_SET_COUNTER(&g_timx_cap_chy_handle,0);
//				TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle,TIM_CHANNEL_2);
//				TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);
//				__HAL_TIM_ENABLE(&g_timx_cap_chy_handle);
//				
//			}
//		}
//	}
//}

//��ʱ�����벶������жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM5){
		if((g_timxchy_cap_sta & 0X80) == 0){
			if(g_timxchy_cap_sta & 0X40){
				if((g_timxchy_cap_sta & 0X3F) == 0X3F){
					TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handle,TIM_CHANNEL_2);
					TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handle,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);
					g_timxchy_cap_sta |= 0X80;
					g_timxchy_cap_val = 0XFFFF;
				}else{
					g_timxchy_cap_sta++;
				}
			}
		}
	}
}


/*********************************ͨ��ͨ�ö�ʱ��PWM���ʵ�����*************************************/

TIM_HandleTypeDef g_timx_pwm_chy_handle;     /* ��ʱ��x��� */

/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y PWM��� ��ʼ��������ʹ��PWMģʽ1��
 * @note
 *              ͨ�ö�ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ36M, ���Զ�ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_pwm_chy  = {0};                          /* ��ʱ��PWM������� */

    g_timx_pwm_chy_handle.Instance = GTIM_TIMX_PWM;                     /* ��ʱ�� */
    g_timx_pwm_chy_handle.Init.Prescaler = psc;                         /* ��ʱ����Ƶ */
    g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* ��������ģʽ */
    g_timx_pwm_chy_handle.Init.Period = arr;                            /* �Զ���װ��ֵ */
    HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle);                           /* ��ʼ��PWM */

    timx_oc_pwm_chy.OCMode = TIM_OCMODE_PWM1;                           /* ģʽѡ��PWM1 */
    timx_oc_pwm_chy.Pulse = arr / 2;                                    /* ���ñȽ�ֵ,��ֵ����ȷ��ռ�ձ� */
                                                                        /* ����Ĭ�����ñȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50% */
    timx_oc_pwm_chy.OCPolarity = TIM_OCPOLARITY_LOW;                    /* ����Ƚϼ���Ϊ�� */
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, GTIM_TIMX_PWM_CHY); /* ����TIMxͨ��y */
    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY);       /* ������ӦPWMͨ�� */
}

/**
 * @brief       ��ʱ���ײ�������ʱ��ʹ�ܣ���������
                �˺����ᱻHAL_TIM_PWM_Init()����
 * @param       htim:��ʱ�����
 * @retval      ��
 */
// void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == GTIM_TIMX_PWM)
//     {
//         GPIO_InitTypeDef gpio_init_struct;
//         GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();               /* ����ͨ��y��CPIOʱ�� */
//         GTIM_TIMX_PWM_CHY_CLK_ENABLE();

//         gpio_init_struct.Pin = GTIM_TIMX_PWM_CHY_GPIO_PIN; /* ͨ��y��CPIO�� */
//         gpio_init_struct.Mode = GPIO_MODE_AF_PP;           /* ����������� */
//         gpio_init_struct.Pull = GPIO_PULLUP;               /* ���� */
//         gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;     /* ���� */
//         HAL_GPIO_Init(GTIM_TIMX_PWM_CHY_GPIO_PORT, &gpio_init_struct);
//         GTIM_TIMX_PWM_CHY_GPIO_REMAP();                    /* IO��REMAP����, �Ƿ��Ҫ�鿴ͷ�ļ����õ�˵��! */
//     }
// }




