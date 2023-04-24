#include "./BSP/TIMER/atim.h"

/**
* @brief ��ʱ�� TIMX ͨ�� Y PWM ����ģʽ ��ʼ������
* @note
* �߼���ʱ����ʱ������ APB2, �� PCLK2 = 72Mhz, �������� PPRE2 ����Ƶ, ���
* �߼���ʱ��ʱ�� = 72Mhz
* ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=��ʱ������Ƶ��,��λ:Mhz
*
* ��������ʼ����ʱ��: ʹ�� psc=0, arr �̶�Ϊ 65535. �õ�����ʱ��Ƶ��Ϊ 72Mhz,
����Լ 13.8ns
*
* @param ��
* @retval ��
*/
TIM_HandleTypeDef g_timx_pwmin_chy_handle;	//��ʱ�����

uint8_t g_timxchy_pwmin_sta = 0; /* pwm����״̬ */
uint16_t g_timxchy_pwmin_psc = 0; /* pwm�����Ƶϵ�� */
uint16_t g_timxchy_pwmin_hval = 0;	/* pwm����ߵ�ƽ���� */
uint16_t g_timxchy_pwmin_cval = 0;	/* pwm�������ڿ�� */
void atim_timx_pwmin_chy_init(void)
{
	
	TIM_SlaveConfigTypeDef slave_config = {0};	//��ģʽ���þ��
	TIM_IC_InitTypeDef tim_ic_pwmin_chy = {0};	//IC������
	
//	__HAL_RCC_AFIO_CLK_ENABLE();	//����ʱ��ʹ��
//	AFIO_REMAP_PARTIAL(AFIO_EVCR_PORT_PC,AFIO_EVCR_PIN_PX6);/*���� TIM8_CH1/PC6*/
	
	g_timx_pwmin_chy_handle.Instance = ATIM_TIMX_PWMIN; /* ��ʱ�� 8 */
	g_timx_pwmin_chy_handle.Init.Prescaler = 0; /* ��ʱ��Ԥ��Ƶϵ�� */
	g_timx_pwmin_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* �������� */
	g_timx_pwmin_chy_handle.Init.Period = 65535; /* �Զ���װ��ֵ */
	HAL_TIM_IC_Init(&g_timx_pwmin_chy_handle);	//��ʱ����ʼ��

	/* ��ģʽ���ã�IT1 �������� */
	slave_config.SlaveMode = TIM_SLAVEMODE_RESET; /* ��ģʽ����λģʽ */
	slave_config.InputTrigger = TIM_TS_TI1FP1; /* ��ʱ�����봥��Դ��TI2FP2 */
	slave_config.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;/*�����ؼ��*/
	slave_config.TriggerFilter = 0; /* ���˲� */
	HAL_TIM_SlaveConfigSynchro(&g_timx_pwmin_chy_handle, &slave_config);
	
	/* IC1 �����½��ش��� TI1FP1 */
	tim_ic_pwmin_chy.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;/* �½��ؼ�� */
	tim_ic_pwmin_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;/* IC1 ӳ�䵽 TI1 �� */
	tim_ic_pwmin_chy.ICPrescaler = TIM_ICPSC_DIV1; /* ����Ƶ */
	tim_ic_pwmin_chy.ICFilter = 0; /* ���˲� */
	HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &tim_ic_pwmin_chy,TIM_CHANNEL_1);

	/* IC2 ���������ش��� TI1FP2 */
	tim_ic_pwmin_chy.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;/*�����ؼ��*/
	tim_ic_pwmin_chy.ICSelection = TIM_ICSELECTION_INDIRECTTI;/*IC2 ӳ�䵽 TI1 ��*/
	HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &tim_ic_pwmin_chy,TIM_CHANNEL_2);
	
	

	HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle, TIM_CHANNEL_1);	//ͨ��1ʹ�ܲ����жϣ�����ʹ�ܶ�ʱ��
	HAL_TIM_IC_Start(&g_timx_pwmin_chy_handle, TIM_CHANNEL_2);	//ͨ��2ʹ�ܣ�����ʹ�ܶ�ʱ��
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM8)
	{
		GPIO_InitTypeDef gpio_init_struct = {0};	//GPIO���
	ATIM_TIMX_PWMIN_CHY_CLK_ENABLE();	//��ʱ��ʱ��ʹ��
	ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE();	//IOʱ��ʹ��
		
	__HAL_RCC_AFIO_CLK_ENABLE();
    AFIO_REMAP_PARTIAL(AFIO_EVCR_PORT_PC, AFIO_EVCR_PIN_PX6);       /* ����TIM8_CH1/PC6 */
		
	gpio_init_struct.Pin = ATIM_TIMX_PWMIN_CHY_GPIO_PIN;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;	//�����������
	gpio_init_struct.Pull = GPIO_PULLDOWN;	//��������
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH ;	//����
	HAL_GPIO_Init(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, &gpio_init_struct);	//IO��ʼ��
		
		/* �����ж����ȼ�����ռ���ȼ� 1�������ȼ� 3 */
	HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_CC_IRQn, 1, 3); 
	HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_CC_IRQn); /* ���� TIMx �����ж� */
	}
}



void TIM8_CC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);	//��ʱ�����ô�����
}

/**
* @brief ��ʱ�� TIMX PWM ����ģʽ ������������
* @param ��
* @retval ��
*/
void atim_timx_pwmin_chy_restart(void)
{
	sys_intx_disable(); /* �ر��ж� */
	g_timxchy_pwmin_sta = 0; /* ����״̬,���¿�ʼ��� */
	g_timxchy_pwmin_hval = 0;	/* pwm����ߵ�ƽ���� */
	g_timxchy_pwmin_cval = 0;	/* pwm�������ڿ�� */
	sys_intx_enable(); /* ���ж� */
}

/**
* @brief ��ʱ�� TIMX ͨ�� Y PWM ����ģʽ �жϴ�����
* @note
*
* @param ��
* @retval ��
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == ATIM_TIMX_PWMIN)
	{
		if(g_timxchy_pwmin_sta == 0)
		{
			if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
				g_timxchy_pwmin_hval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle, TIM_CHANNEL_2) + 1 + 1;
				g_timxchy_pwmin_cval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle, TIM_CHANNEL_1) + 1 + 1;
				g_timxchy_pwmin_sta = 1;
			}
		}
	}
	
//	static uint8_t sflag = 0; /* ���� PWMIN �������־ */
//	if (g_timxchy_pwmin_sta)
//	{
//		g_timxchy_pwmin_psc = 0;
//		ATIM_TIMX_PWMIN->SR = 0; /* ��������жϱ�־λ */
//		__HAL_TIM_SET_COUNTER(&g_timx_pwmin_chy_handle, 0); /* ���������� */
//		return ;
//	}
//	/* ��������˸����ж� */
//	if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_UPDATE))
//	{
//		/* ��������жϱ�� */
//		__HAL_TIM_CLEAR_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_UPDATE); 
//		/* û�з������ڲ����ж�,�Ҳ���δ��� */
//		if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_CC1) == 0) 
//		{
//			sflag = 0;
//			if (g_timxchy_pwmin_psc == 0) /* �� 0 �� 1 */
//			{
//				g_timxchy_pwmin_psc ++;
//			}
//			else
//			{
//				if (g_timxchy_pwmin_psc == 65535) /* �Ѿ������,������������״̬ */
//				{
//					g_timxchy_pwmin_psc = 0; /* ���»ָ�����Ƶ */
//				}
//				else if (g_timxchy_pwmin_psc > 32767)/* ���ܱ����� */
//				{
//					g_timxchy_pwmin_psc = 65535; /* ֱ�ӵ�������Ƶϵ�� */
//				}
//				else
//				{
//					g_timxchy_pwmin_psc += g_timxchy_pwmin_psc; /* ���� */
//				}
//			}
//			__HAL_TIM_SET_PRESCALER(&g_timx_pwmin_chy_handle,
//			g_timxchy_pwmin_psc); /* ���ö�ʱ��Ԥ��Ƶϵ�� */
//			__HAL_TIM_SET_COUNTER(&g_timx_pwmin_chy_handle, 0); /* ���������� */
//			ATIM_TIMX_PWMIN->SR = 0; /* ��������жϱ�־λ */
//			return ;
//		}
//	}
//	if (sflag == 0) /* ��һ�βɼ��������ж� */
//	{
//		/* ��⵽�˵�һ�����ڲ����ж� */
//		if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_CC1)) 
//		{
//			sflag = 1; /* ��ǵ�һ�������Ѿ�����, �ڶ������ڲ�����Կ�ʼ�� */
//		}
//		ATIM_TIMX_PWMIN->SR = 0; /* ��������жϱ�־λ */
//		return ; /* ��ɴ˴β��� */
//	}
//	if (g_timxchy_pwmin_sta == 0)/* ��û�гɹ����� */
//	{
//		/* ��⵽�����ڲ����ж� */
//		if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_CC1)) 
//		{
//			g_timxchy_pwmin_hval = HAL_TIM_ReadCapturedValue(
//			&g_timx_pwmin_chy_handle, TIM_CHANNEL_2) + 1; /* �߶�ƽ������ֵ */
//			g_timxchy_pwmin_cval = HAL_TIM_ReadCapturedValue(
//			&g_timx_pwmin_chy_handle, TIM_CHANNEL_1) + 1; /* ���ڲ���ֵ */
//			/* �ߵ�ƽ����ض�С�����ڳ��� */
//			if (g_timxchy_pwmin_hval < g_timxchy_pwmin_cval)
//			{
//				g_timxchy_pwmin_sta = 1; /* ��ǲ���ɹ� */
//				g_timxchy_pwmin_psc = ATIM_TIMX_PWMIN->PSC;/* ��ȡ PWM �����Ƶϵ�� */

//				if (g_timxchy_pwmin_psc == 0) /* ��Ƶϵ��Ϊ 0 ��ʱ��, ������ȡ���� */
//				{
//					g_timxchy_pwmin_hval++; /* ����ϵ��Ϊ 1, �� 1 */
//					g_timxchy_pwmin_cval++; /* ����ϵ��Ϊ 1, �� 1 */
//				}
//				sflag = 0;
//				/* ÿ�β��� PWM ����ɹ���, ֹͣ����,����Ƶ���ж�Ӱ��ϵͳ������������ */
//				ATIM_TIMX_PWMIN->CR1 &= ~(1 << 0); /* �رն�ʱ�� TIMX */
//				/* �ر�ͨ�� 1 �����ж� */
//				__HAL_TIM_DISABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_CC1);
//				/* �ر�ͨ�� 2 �����ж� */
//				__HAL_TIM_DISABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_CC2);
//				/* �رո����ж� */
//				__HAL_TIM_DISABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_UPDATE);
//				ATIM_TIMX_PWMIN->SR = 0; /* ��������жϱ�־λ */
//			}
//			else
//			{
//				atim_timx_pwmin_chy_restart();
//			}
//		}
//	}
//	ATIM_TIMX_PWMIN->SR = 0; /* ��������жϱ�־λ */
}


/**
* @brief �߼���ʱ�� TIMX ������� ��ʼ��������ʹ�� PWM ģʽ 1��
* @note
* ���ø߼���ʱ�� TIMX �������, һ· OCy һ· OCyN, ���ҿ�����������ʱ��
*
* �߼���ʱ����ʱ������ APB2, �� PCLK2 = 72Mhz, �������� PPRE2 ����Ƶ, ���
* �߼���ʱ��ʱ�� = 72Mhz
* ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=��ʱ������Ƶ��,��λ:Mhz
* @param arr: �Զ���װֵ��
* @param psc: ʱ��Ԥ��Ƶ��
* @retval ��
*/
TIM_HandleTypeDef g_timx_cplm_pwm_handle; //��ʱ�����
TIM_BreakDeadTimeConfigTypeDef g_sbreak_dead_time_config; //ɲ���������
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef gpio_init_struct = {0}; //GPIO���
	TIM_OC_InitTypeDef tim_oc_cplm_pwm = {0}; //��ʱ��PWMͨ�����þ��
	ATIM_TIMX_CPLM_CLK_ENABLE(); /* TIMx ʱ��ʹ�� */
	ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE(); /* ͨ�� X ��Ӧ IO ��ʱ��ʹ�� */
	ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE(); /* ͨ�� X ����ͨ����Ӧ IO ��ʱ��ʹ�� */
	ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE(); /* ͨ�� X ɲ�������Ӧ IO ��ʱ��ʹ�� */

	//������IO�����úͳ�ʼ��
	gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHY_GPIO_PIN;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Pull = GPIO_PULLUP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH ;
	HAL_GPIO_Init(ATIM_TIMX_CPLM_CHY_GPIO_PORT, &gpio_init_struct);
	gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHYN_GPIO_PIN;
	HAL_GPIO_Init(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, &gpio_init_struct);
	gpio_init_struct.Pin = ATIM_TIMX_CPLM_BKIN_GPIO_PIN;
	HAL_GPIO_Init(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, &gpio_init_struct);

	ATIM_TIMX_CPLM_CHYN_GPIO_REMAP(); /* ��ӳ�䶨ʱ�� IO */
	g_timx_cplm_pwm_handle.Instance = ATIM_TIMX_CPLM; /* ��ʱ�� x */
	g_timx_cplm_pwm_handle.Init.Prescaler = psc; /* ��ʱ��Ԥ��Ƶϵ�� */
	g_timx_cplm_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* �������� */
	g_timx_cplm_pwm_handle.Init.Period = arr; /* �Զ���װ��ֵ */
	/* CKD[1:0] = 10, tDTS = 4 * tCK_INT = Ft / 4 = 18Mhz */
	g_timx_cplm_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
	g_timx_cplm_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* ʹ��Ӱ�ӼĴ��� TIMx_ARR */
	HAL_TIM_PWM_Init(&g_timx_cplm_pwm_handle);
	tim_oc_cplm_pwm.OCMode = TIM_OCMODE_PWM1; /* PWM ģʽ 1 */
	tim_oc_cplm_pwm.OCPolarity = TIM_OCPOLARITY_HIGH; /* OCy ������Ըߵ�ƽ��Ч */
	tim_oc_cplm_pwm.OCNPolarity = TIM_OCNPOLARITY_HIGH; /* OCyN ������Ըߵ�ƽ��Ч */
	tim_oc_cplm_pwm.OCIdleState = TIM_OCIDLESTATE_RESET; /* �� MOE=0��OCx=0 */
	tim_oc_cplm_pwm.OCNIdleState = TIM_OCNIDLESTATE_RESET;/* �� MOE=0��OCxN=0 */
	HAL_TIM_PWM_ConfigChannel(&g_timx_cplm_pwm_handle, &tim_oc_cplm_pwm,ATIM_TIMX_CPLM_CHY);
	
	/* �����������������������ж� */
	/* ����ģʽ�Ĺر����״̬ */
	g_sbreak_dead_time_config.OffStateRunMode = TIM_OSSR_DISABLE; 
	/* ����ģʽ�Ĺر����״̬ */
	g_sbreak_dead_time_config.OffStateIDLEMode = TIM_OSSI_DISABLE; 
	g_sbreak_dead_time_config.LockLevel = TIM_LOCKLEVEL_OFF;/* ���üĴ��������� */
	g_sbreak_dead_time_config.BreakState = TIM_BREAK_ENABLE;/* ʹ��ɲ������ */
	/* ɲ��������Ч�źż���Ϊ�� */
	g_sbreak_dead_time_config.BreakPolarity = TIM_BREAKPOLARITY_HIGH; 
	/* ʹ�� AOE λ������ɲ���������Զ��ָ���� */
	g_sbreak_dead_time_config.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle,&g_sbreak_dead_time_config);
	/* ʹ�� OCy ��� */
	HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY);
	/* ʹ�� OCyN ��� */
	HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY);
}

/**
* @brief ��ʱ�� TIMX ��������Ƚ�ֵ & ����ʱ��
* @param ccr: ����Ƚ�ֵ
* @param dtg: ����ʱ��
* @arg dtg[7:5]=0xx ʱ, ����ʱ�� = dtg[7:0] * tDTS
* @arg dtg[7:5]=10x ʱ, ����ʱ�� = (64 + dtg[6:0]) * 2 * tDTS
* @arg dtg[7:5]=110 ʱ, ����ʱ�� = (32 + dtg[5:0]) * 8 * tDTS
* @arg dtg[7:5]=111 ʱ, ����ʱ�� = (32 + dtg[5:0]) * 16 * tDTS
* @note tDTS = 1 / (Ft / CKD[1:0]) = 1 / 18M = 55.56ns
* @retval ��
*/
void atim_timx_cplm_pwm_set(uint16_t ccr, uint16_t dtg)
{
 g_sbreak_dead_time_config.DeadTime = dtg; /* ����ʱ������ */
HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle,&g_sbreak_dead_time_config); /*��������ʱ��*/
 __HAL_TIM_MOE_ENABLE(&g_timx_cplm_pwm_handle); /* MOE=1,ʹ������� */
 ATIM_TIMX_CPLM_CHY_CCRY = ccr; /* ���ñȽϼĴ��� */
}





/**
* @brief �߼���ʱ�� TIMX ����Ƚ�ģʽ ��ʼ��������ʹ������Ƚ�ģʽ��
* @note
* ���ø߼���ʱ�� TIMX 4 ·����Ƚ�ģʽ PWM ���,ʵ�� 50%ռ�ձ�,��ͬ��λ����
* ע��,����������Ƚ�ģʽ,ÿ 2 ���������ڲ������һ�� PWM ���,������Ƶ�ʼ���
* ����,���ǻ����Կ����ж����ж������޸� CCRx,�Ӷ�ʵ�ֲ�ͬƵ��/��ͬ��λ�Ŀ���
* �������ǲ��Ƽ���ôʹ��,��Ϊ����ܵ��·ǳ�Ƶ�����ж�,�Ӷ�ռ�ô��� CPU ��Դ
*
* �߼���ʱ����ʱ������ APB2, �� PCLK2 = 72Mhz, �������� PPRE2 ����Ƶ, ���
* �߼���ʱ��ʱ�� = 72Mhz
* ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=��ʱ������Ƶ��,��λ:Mhz
*
* @param arr: �Զ���װֵ��
* @param psc: ʱ��Ԥ��Ƶ��
* @retval ��
*/

TIM_HandleTypeDef g_timx_comp_pwm_handle;

void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
	TIM_OC_InitTypeDef timx_oc_comp_pwm = {0};
	
	g_timx_comp_pwm_handle.Instance = ATIM_TIMX_COMP; /* ��ʱ�� x */
	g_timx_comp_pwm_handle.Init.Prescaler = psc ; /* ��ʱ����Ƶ */
	g_timx_comp_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* �������� */
	g_timx_comp_pwm_handle.Init.Period = arr; /* �Զ���װ��ֵ */
	g_timx_comp_pwm_handle.Init.AutoReloadPreload =TIM_AUTORELOAD_PRELOAD_ENABLE; /* ʹ��Ӱ�ӼĴ��� TIMx_ARR */
	HAL_TIM_OC_Init(&g_timx_comp_pwm_handle); /* ����Ƚ�ģʽ��ʼ�� */
	
	timx_oc_comp_pwm.OCMode = TIM_OCMODE_TOGGLE; /* �Ƚ����ģʽ��ת���� */
	timx_oc_comp_pwm.Pulse = 250 - 1; /* ��������ȽϼĴ�����ֵ */
	timx_oc_comp_pwm.OCPolarity = TIM_OCPOLARITY_HIGH;/* ����Ƚϼ���Ϊ�� */
	
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_1); /* ��ʼ����ʱ��������Ƚ�ͨ�� 1 */
	/* CCR1 �Ĵ���Ԥװ��ʹ�� */
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_1);
	timx_oc_comp_pwm.Pulse = 500 - 1;
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_2); /* ��ʼ����ʱ��������Ƚ�ͨ�� 2 */
	/* CCR2 �Ĵ���Ԥװ��ʹ�� */
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_2);
	timx_oc_comp_pwm.Pulse = 750 - 1;
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_3); /* ��ʼ����ʱ��������Ƚ�ͨ�� 3 */
	/* CCR3 �Ĵ���Ԥװ��ʹ�� */
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);
	timx_oc_comp_pwm.Pulse = 1000 - 1;
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_4); /* ��ʼ����ʱ��������Ƚ�ͨ�� 4 */
	/* CCR4 �Ĵ���Ԥװ��ʹ�� */ 
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_4);
	
	/* ʹ�� TIM1 ͨ�� 1~ͨ�� 4 ��� */
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_2);
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_4);
}


/**
* @brief ��ʱ���ײ�������ʱ��ʹ�ܣ���������
 �˺����ᱻ HAL_TIM_OC_Init()����
* @param htim:��ʱ�����
* @retval ��
*/
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
 if (htim->Instance == ATIM_TIMX_COMP)
 {
	GPIO_InitTypeDef gpio_init_struct;
	ATIM_TIMX_COMP_CLK_ENABLE();
	ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE();
	ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE();
	ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE();
	ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE();
	gpio_init_struct.Pin = ATIM_TIMX_COMP_CH1_GPIO_PIN;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Pull = GPIO_NOPULL;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ATIM_TIMX_COMP_CH1_GPIO_PORT, &gpio_init_struct);
	gpio_init_struct.Pin = ATIM_TIMX_COMP_CH2_GPIO_PIN;
	HAL_GPIO_Init(ATIM_TIMX_COMP_CH2_GPIO_PORT, &gpio_init_struct);
	gpio_init_struct.Pin = ATIM_TIMX_COMP_CH3_GPIO_PIN;
	HAL_GPIO_Init(ATIM_TIMX_COMP_CH3_GPIO_PORT, &gpio_init_struct);
	gpio_init_struct.Pin = ATIM_TIMX_COMP_CH4_GPIO_PIN;
	HAL_GPIO_Init(ATIM_TIMX_COMP_CH4_GPIO_PORT, &gpio_init_struct);
 }
}





/**
* @brief �߼���ʱ�� TIMX ͨ�� Y ���ָ������ PWM ��ʼ������
* @note
* �߼���ʱ����ʱ������ APB2, �� PCLK2 = 72Mhz, �������� PPRE2 ����Ƶ, ���
* �߼���ʱ��ʱ�� = 72Mhz
* ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=��ʱ������Ƶ��,��λ:Mhz
* @param arr: �Զ���װֵ
* @param psc: ʱ��Ԥ��Ƶ��
* @retval ��
*/
TIM_HandleTypeDef g_timx_npwm_chy_handle;

void atim_timx_npwm_chy_init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef gpio_init_struct;	/*GPIO���*/
	TIM_OC_InitTypeDef timx_oc_npwm_chy; /* ��ʱ�������� */
	ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE(); /* TIMX ͨ�� IO ��ʱ��ʹ�� */
	ATIM_TIMX_NPWM_CHY_CLK_ENABLE(); /* TIMX ʱ��ʹ�� */
	g_timx_npwm_chy_handle.Instance = ATIM_TIMX_NPWM; /* ��ʱ�� x */
	g_timx_npwm_chy_handle.Init.Prescaler = psc; /* ��ʱ����Ƶ */
	g_timx_npwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* �������� */
	g_timx_npwm_chy_handle.Init.Period = arr; /* �Զ���װ��ֵ */

	/*	�ڱ�ʵ�������ǰ��Զ���װ��Ԥװ������λҪ�� 1��
	��������ı� ARR �Ĵ�����ֵ����ֵҲ����������Ч��
	���ǵȴ�֮ǰ���õ� PWM ��������󣨷��������¼�������Ч	*/
	g_timx_npwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /*ʹ�� TIMx_ARR ���л��� */

	g_timx_npwm_chy_handle.Init.RepetitionCounter = 0; /* �ظ���������ʼֵ */
	HAL_TIM_PWM_Init(&g_timx_npwm_chy_handle); /* ��ʼ�� PWM */

	gpio_init_struct.Pin = ATIM_TIMX_NPWM_CHY_GPIO_PIN;/* ͨ�� y �� CPIO �� */
	gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* ����������� */
	gpio_init_struct.Pull = GPIO_PULLUP; /* ���� */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
	HAL_GPIO_Init(ATIM_TIMX_NPWM_CHY_GPIO_PORT, &gpio_init_struct);

	timx_oc_npwm_chy.OCMode = TIM_OCMODE_PWM1; /* ģʽѡ�� PWM 1*/
	timx_oc_npwm_chy.Pulse = arr / 2; /* ���ñȽ�ֵ,��ֵ����ȷ��ռ�ձ� */
	
	/*��Ϊ���ǵ� LED0������ PD13���Ǹߵ�ƽ����
	������ϣ��������һ�� PWM ����ʱ��LED0 ����
	����������������Ƚϼ���Ϊ�� */
	timx_oc_npwm_chy.OCPolarity = TIM_OCPOLARITY_LOW; /* ����Ƚϼ���Ϊ�� */
	
	HAL_TIM_PWM_ConfigChannel(&g_timx_npwm_chy_handle, &timx_oc_npwm_chy,ATIM_TIMX_NPWM_CHY); /* ���� TIMx ͨ�� y */
	
	/* �����ж����ȼ�����ռ���ȼ� 1�������ȼ� 3 */
	HAL_NVIC_SetPriority(ATIM_TIMX_NPWM_IRQn, 1, 3); 
	HAL_NVIC_EnableIRQ(ATIM_TIMX_NPWM_IRQn); /* ���� ITMx �ж� */
	__HAL_TIM_ENABLE_IT(&g_timx_npwm_chy_handle, TIM_IT_UPDATE);/* ��������ж� */
	HAL_TIM_PWM_Start(&g_timx_npwm_chy_handle, ATIM_TIMX_NPWM_CHY);/* ʹ����� */
	ATIM_TIMX_NPWM->CR1 &= ~(TIM_CR1_CEN); //�رն�ʱ�� TIMX����ֹ������ܾͽ������ж�
}

///* g_npwm_remain ��ʾ��ǰ��ʣ�¶��ٸ�����Ҫ����
//* ÿ����෢�� 256 ������
//*/
//static uint8_t g_npwm_remain = 0;
///**
//* @brief �߼���ʱ�� TIMX NPWM ���� PWM ����
//* @param rcr: PWM �ĸ���, 1~2^8 �η���
//* @retval ��
//*/
//void atim_timx_npwm_chy_set(uint8_t npwm)
//{
//	if (npwm == 0) return ;
//	g_npwm_remain = npwm; /* ����������� */
//	/* ����һ�θ����¼�,���ж����洦��������� */
//	HAL_TIM_GenerateEvent(&g_timx_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE);
//	__HAL_TIM_ENABLE(&g_timx_npwm_chy_handle); /* ʹ�ܶ�ʱ�� TIMX */
//}


/**
* @brief ��ʱ���жϷ�����
* @param ��
* @retval ��
*/
//void ATIM_TIMX_NPWM_IRQHandler(void)
//{
////	uint16_t npwm = 0;
//	/* ���´���û��ʹ�ö�ʱ�� HAL �⹲�ô���������������ֱ��ͨ���ж��жϱ�־λ�ķ�ʽ */
//	if(__HAL_TIM_GET_FLAG(&g_timx_npwm_chy_handle, TIM_FLAG_UPDATE) != RESET)
//	{
//		
//		if(g_npwm_remain) 
//		{
//			ATIM_TIMX_NPWM->RCR = g_npwm_remain - 1; /* ���� RCR ֵΪ npwm-1, �� npwm ������ */
//			HAL_TIM_GenerateEvent(&g_timx_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* ����һ�θ����¼�,��RCR �Ĵ�����ֵ���µ� RCR Ӱ�ӼĴ����� */
//			__HAL_TIM_ENABLE(&g_timx_npwm_chy_handle); /* ʹ�ܶ�ʱ�� TIMX */
//			g_npwm_remain = 0;
//		}else
//		{
//			/* �رն�ʱ�� TIMX,ʹ��__HAL_TIM_DISABLE��ʧ��ͨ��������������ﵥ�������Ĵ����رն�ʱ�� */
//			ATIM_TIMX_NPWM->CR1 &= ~(TIM_CR1_CEN);
//		}
		
		
		/* �����ʱ�������жϱ�־λ */
//		__HAL_TIM_CLEAR_IT(&g_timx_npwm_chy_handle, TIM_IT_UPDATE); 
//	}
//}



