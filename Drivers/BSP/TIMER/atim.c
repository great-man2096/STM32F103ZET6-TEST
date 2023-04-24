#include "./BSP/TIMER/atim.h"

/**
* @brief 定时器 TIMX 通道 Y PWM 输入模式 初始化函数
* @note
* 高级定时器的时钟来自 APB2, 而 PCLK2 = 72Mhz, 我们设置 PPRE2 不分频, 因此
* 高级定时器时钟 = 72Mhz
* 定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=定时器工作频率,单位:Mhz
*
* 本函数初始化的时候: 使用 psc=0, arr 固定为 65535. 得到采样时钟频率为 72Mhz,
精度约 13.8ns
*
* @param 无
* @retval 无
*/
TIM_HandleTypeDef g_timx_pwmin_chy_handle;	//定时器句柄

uint8_t g_timxchy_pwmin_sta = 0; /* pwm输入状态 */
uint16_t g_timxchy_pwmin_psc = 0; /* pwm输入分频系数 */
uint16_t g_timxchy_pwmin_hval = 0;	/* pwm输入高电平脉宽 */
uint16_t g_timxchy_pwmin_cval = 0;	/* pwm输入周期宽度 */
void atim_timx_pwmin_chy_init(void)
{
	
	TIM_SlaveConfigTypeDef slave_config = {0};	//从模式配置句柄
	TIM_IC_InitTypeDef tim_ic_pwmin_chy = {0};	//IC捕获句柄
	
//	__HAL_RCC_AFIO_CLK_ENABLE();	//复用时钟使能
//	AFIO_REMAP_PARTIAL(AFIO_EVCR_PORT_PC,AFIO_EVCR_PIN_PX6);/*复用 TIM8_CH1/PC6*/
	
	g_timx_pwmin_chy_handle.Instance = ATIM_TIMX_PWMIN; /* 定时器 8 */
	g_timx_pwmin_chy_handle.Init.Prescaler = 0; /* 定时器预分频系数 */
	g_timx_pwmin_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* 递增计数 */
	g_timx_pwmin_chy_handle.Init.Period = 65535; /* 自动重装载值 */
	HAL_TIM_IC_Init(&g_timx_pwmin_chy_handle);	//定时器初始化

	/* 从模式配置，IT1 触发更新 */
	slave_config.SlaveMode = TIM_SLAVEMODE_RESET; /* 从模式：复位模式 */
	slave_config.InputTrigger = TIM_TS_TI1FP1; /* 定时器输入触发源：TI2FP2 */
	slave_config.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;/*上升沿检测*/
	slave_config.TriggerFilter = 0; /* 不滤波 */
	HAL_TIM_SlaveConfigSynchro(&g_timx_pwmin_chy_handle, &slave_config);
	
	/* IC1 捕获：下降沿触发 TI1FP1 */
	tim_ic_pwmin_chy.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;/* 下降沿检测 */
	tim_ic_pwmin_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;/* IC1 映射到 TI1 上 */
	tim_ic_pwmin_chy.ICPrescaler = TIM_ICPSC_DIV1; /* 不分频 */
	tim_ic_pwmin_chy.ICFilter = 0; /* 不滤波 */
	HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &tim_ic_pwmin_chy,TIM_CHANNEL_1);

	/* IC2 捕获：上升沿触发 TI1FP2 */
	tim_ic_pwmin_chy.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;/*上升沿检测*/
	tim_ic_pwmin_chy.ICSelection = TIM_ICSELECTION_INDIRECTTI;/*IC2 映射到 TI1 上*/
	HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &tim_ic_pwmin_chy,TIM_CHANNEL_2);
	
	

	HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle, TIM_CHANNEL_1);	//通道1使能捕获中断，并且使能定时器
	HAL_TIM_IC_Start(&g_timx_pwmin_chy_handle, TIM_CHANNEL_2);	//通道2使能，并且使能定时器
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM8)
	{
		GPIO_InitTypeDef gpio_init_struct = {0};	//GPIO句柄
	ATIM_TIMX_PWMIN_CHY_CLK_ENABLE();	//计时器时钟使能
	ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE();	//IO时钟使能
		
	__HAL_RCC_AFIO_CLK_ENABLE();
    AFIO_REMAP_PARTIAL(AFIO_EVCR_PORT_PC, AFIO_EVCR_PIN_PX6);       /* 复用TIM8_CH1/PC6 */
		
	gpio_init_struct.Pin = ATIM_TIMX_PWMIN_CHY_GPIO_PIN;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;	//复用推挽输出
	gpio_init_struct.Pull = GPIO_PULLDOWN;	//无上下拉
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH ;	//高速
	HAL_GPIO_Init(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, &gpio_init_struct);	//IO初始化
		
		/* 设置中断优先级，抢占优先级 1，子优先级 3 */
	HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_CC_IRQn, 1, 3); 
	HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_CC_IRQn); /* 开启 TIMx 捕获中断 */
	}
}



void TIM8_CC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);	//定时器共用处理函数
}

/**
* @brief 定时器 TIMX PWM 输入模式 重新启动捕获
* @param 无
* @retval 无
*/
void atim_timx_pwmin_chy_restart(void)
{
	sys_intx_disable(); /* 关闭中断 */
	g_timxchy_pwmin_sta = 0; /* 清零状态,重新开始检测 */
	g_timxchy_pwmin_hval = 0;	/* pwm输入高电平脉宽 */
	g_timxchy_pwmin_cval = 0;	/* pwm输入周期宽度 */
	sys_intx_enable(); /* 打开中断 */
}

/**
* @brief 定时器 TIMX 通道 Y PWM 输入模式 中断处理函数
* @note
*
* @param 无
* @retval 无
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
	
//	static uint8_t sflag = 0; /* 启动 PWMIN 输入检测标志 */
//	if (g_timxchy_pwmin_sta)
//	{
//		g_timxchy_pwmin_psc = 0;
//		ATIM_TIMX_PWMIN->SR = 0; /* 清除所有中断标志位 */
//		__HAL_TIM_SET_COUNTER(&g_timx_pwmin_chy_handle, 0); /* 计数器清零 */
//		return ;
//	}
//	/* 如果发生了更新中断 */
//	if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_UPDATE))
//	{
//		/* 清除更新中断标记 */
//		__HAL_TIM_CLEAR_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_UPDATE); 
//		/* 没有发生周期捕获中断,且捕获未完成 */
//		if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_CC1) == 0) 
//		{
//			sflag = 0;
//			if (g_timxchy_pwmin_psc == 0) /* 从 0 到 1 */
//			{
//				g_timxchy_pwmin_psc ++;
//			}
//			else
//			{
//				if (g_timxchy_pwmin_psc == 65535) /* 已经最大了,可能是无输入状态 */
//				{
//					g_timxchy_pwmin_psc = 0; /* 重新恢复不分频 */
//				}
//				else if (g_timxchy_pwmin_psc > 32767)/* 不能倍增了 */
//				{
//					g_timxchy_pwmin_psc = 65535; /* 直接等于最大分频系数 */
//				}
//				else
//				{
//					g_timxchy_pwmin_psc += g_timxchy_pwmin_psc; /* 倍增 */
//				}
//			}
//			__HAL_TIM_SET_PRESCALER(&g_timx_pwmin_chy_handle,
//			g_timxchy_pwmin_psc); /* 设置定时器预分频系数 */
//			__HAL_TIM_SET_COUNTER(&g_timx_pwmin_chy_handle, 0); /* 计数器清零 */
//			ATIM_TIMX_PWMIN->SR = 0; /* 清除所有中断标志位 */
//			return ;
//		}
//	}
//	if (sflag == 0) /* 第一次采集到捕获中断 */
//	{
//		/* 检测到了第一次周期捕获中断 */
//		if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_CC1)) 
//		{
//			sflag = 1; /* 标记第一次周期已经捕获, 第二次周期捕获可以开始了 */
//		}
//		ATIM_TIMX_PWMIN->SR = 0; /* 清除所有中断标志位 */
//		return ; /* 完成此次操作 */
//	}
//	if (g_timxchy_pwmin_sta == 0)/* 还没有成功捕获 */
//	{
//		/* 检测到了周期捕获中断 */
//		if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_chy_handle, TIM_FLAG_CC1)) 
//		{
//			g_timxchy_pwmin_hval = HAL_TIM_ReadCapturedValue(
//			&g_timx_pwmin_chy_handle, TIM_CHANNEL_2) + 1; /* 高定平脉宽捕获值 */
//			g_timxchy_pwmin_cval = HAL_TIM_ReadCapturedValue(
//			&g_timx_pwmin_chy_handle, TIM_CHANNEL_1) + 1; /* 周期捕获值 */
//			/* 高电平脉宽必定小于周期长度 */
//			if (g_timxchy_pwmin_hval < g_timxchy_pwmin_cval)
//			{
//				g_timxchy_pwmin_sta = 1; /* 标记捕获成功 */
//				g_timxchy_pwmin_psc = ATIM_TIMX_PWMIN->PSC;/* 获取 PWM 输入分频系数 */

//				if (g_timxchy_pwmin_psc == 0) /* 分频系数为 0 的时候, 修正读取数据 */
//				{
//					g_timxchy_pwmin_hval++; /* 修正系数为 1, 加 1 */
//					g_timxchy_pwmin_cval++; /* 修正系数为 1, 加 1 */
//				}
//				sflag = 0;
//				/* 每次捕获 PWM 输入成功后, 停止捕获,避免频繁中断影响系统正常代码运行 */
//				ATIM_TIMX_PWMIN->CR1 &= ~(1 << 0); /* 关闭定时器 TIMX */
//				/* 关闭通道 1 捕获中断 */
//				__HAL_TIM_DISABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_CC1);
//				/* 关闭通道 2 捕获中断 */
//				__HAL_TIM_DISABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_CC2);
//				/* 关闭更新中断 */
//				__HAL_TIM_DISABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_UPDATE);
//				ATIM_TIMX_PWMIN->SR = 0; /* 清除所有中断标志位 */
//			}
//			else
//			{
//				atim_timx_pwmin_chy_restart();
//			}
//		}
//	}
//	ATIM_TIMX_PWMIN->SR = 0; /* 清除所有中断标志位 */
}


/**
* @brief 高级定时器 TIMX 互补输出 初始化函数（使用 PWM 模式 1）
* @note
* 配置高级定时器 TIMX 互补输出, 一路 OCy 一路 OCyN, 并且可以设置死区时间
*
* 高级定时器的时钟来自 APB2, 而 PCLK2 = 72Mhz, 我们设置 PPRE2 不分频, 因此
* 高级定时器时钟 = 72Mhz
* 定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=定时器工作频率,单位:Mhz
* @param arr: 自动重装值。
* @param psc: 时钟预分频数
* @retval 无
*/
TIM_HandleTypeDef g_timx_cplm_pwm_handle; //定时器句柄
TIM_BreakDeadTimeConfigTypeDef g_sbreak_dead_time_config; //刹车死区句柄
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef gpio_init_struct = {0}; //GPIO句柄
	TIM_OC_InitTypeDef tim_oc_cplm_pwm = {0}; //计时器PWM通道配置句柄
	ATIM_TIMX_CPLM_CLK_ENABLE(); /* TIMx 时钟使能 */
	ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE(); /* 通道 X 对应 IO 口时钟使能 */
	ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE(); /* 通道 X 互补通道对应 IO 口时钟使能 */
	ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE(); /* 通道 X 刹车输入对应 IO 口时钟使能 */

	//下面是IO口配置和初始化
	gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHY_GPIO_PIN;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Pull = GPIO_PULLUP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH ;
	HAL_GPIO_Init(ATIM_TIMX_CPLM_CHY_GPIO_PORT, &gpio_init_struct);
	gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHYN_GPIO_PIN;
	HAL_GPIO_Init(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, &gpio_init_struct);
	gpio_init_struct.Pin = ATIM_TIMX_CPLM_BKIN_GPIO_PIN;
	HAL_GPIO_Init(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, &gpio_init_struct);

	ATIM_TIMX_CPLM_CHYN_GPIO_REMAP(); /* 重映射定时器 IO */
	g_timx_cplm_pwm_handle.Instance = ATIM_TIMX_CPLM; /* 定时器 x */
	g_timx_cplm_pwm_handle.Init.Prescaler = psc; /* 定时器预分频系数 */
	g_timx_cplm_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* 递增计数 */
	g_timx_cplm_pwm_handle.Init.Period = arr; /* 自动重装载值 */
	/* CKD[1:0] = 10, tDTS = 4 * tCK_INT = Ft / 4 = 18Mhz */
	g_timx_cplm_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
	g_timx_cplm_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* 使能影子寄存器 TIMx_ARR */
	HAL_TIM_PWM_Init(&g_timx_cplm_pwm_handle);
	tim_oc_cplm_pwm.OCMode = TIM_OCMODE_PWM1; /* PWM 模式 1 */
	tim_oc_cplm_pwm.OCPolarity = TIM_OCPOLARITY_HIGH; /* OCy 输出极性高电平有效 */
	tim_oc_cplm_pwm.OCNPolarity = TIM_OCNPOLARITY_HIGH; /* OCyN 输出极性高电平有效 */
	tim_oc_cplm_pwm.OCIdleState = TIM_OCIDLESTATE_RESET; /* 当 MOE=0，OCx=0 */
	tim_oc_cplm_pwm.OCNIdleState = TIM_OCNIDLESTATE_RESET;/* 当 MOE=0，OCxN=0 */
	HAL_TIM_PWM_ConfigChannel(&g_timx_cplm_pwm_handle, &tim_oc_cplm_pwm,ATIM_TIMX_CPLM_CHY);
	
	/* 设置死区参数，开启死区中断 */
	/* 运行模式的关闭输出状态 */
	g_sbreak_dead_time_config.OffStateRunMode = TIM_OSSR_DISABLE; 
	/* 空闲模式的关闭输出状态 */
	g_sbreak_dead_time_config.OffStateIDLEMode = TIM_OSSI_DISABLE; 
	g_sbreak_dead_time_config.LockLevel = TIM_LOCKLEVEL_OFF;/* 不用寄存器锁功能 */
	g_sbreak_dead_time_config.BreakState = TIM_BREAK_ENABLE;/* 使能刹车输入 */
	/* 刹车输入有效信号极性为高 */
	g_sbreak_dead_time_config.BreakPolarity = TIM_BREAKPOLARITY_HIGH; 
	/* 使能 AOE 位，允许刹车结束后自动恢复输出 */
	g_sbreak_dead_time_config.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle,&g_sbreak_dead_time_config);
	/* 使能 OCy 输出 */
	HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY);
	/* 使能 OCyN 输出 */
	HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY);
}

/**
* @brief 定时器 TIMX 设置输出比较值 & 死区时间
* @param ccr: 输出比较值
* @param dtg: 死区时间
* @arg dtg[7:5]=0xx 时, 死区时间 = dtg[7:0] * tDTS
* @arg dtg[7:5]=10x 时, 死区时间 = (64 + dtg[6:0]) * 2 * tDTS
* @arg dtg[7:5]=110 时, 死区时间 = (32 + dtg[5:0]) * 8 * tDTS
* @arg dtg[7:5]=111 时, 死区时间 = (32 + dtg[5:0]) * 16 * tDTS
* @note tDTS = 1 / (Ft / CKD[1:0]) = 1 / 18M = 55.56ns
* @retval 无
*/
void atim_timx_cplm_pwm_set(uint16_t ccr, uint16_t dtg)
{
 g_sbreak_dead_time_config.DeadTime = dtg; /* 死区时间设置 */
HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle,&g_sbreak_dead_time_config); /*重设死区时间*/
 __HAL_TIM_MOE_ENABLE(&g_timx_cplm_pwm_handle); /* MOE=1,使能主输出 */
 ATIM_TIMX_CPLM_CHY_CCRY = ccr; /* 设置比较寄存器 */
}





/**
* @brief 高级定时器 TIMX 输出比较模式 初始化函数（使用输出比较模式）
* @note
* 配置高级定时器 TIMX 4 路输出比较模式 PWM 输出,实现 50%占空比,不同相位控制
* 注意,本例程输出比较模式,每 2 个计数周期才能完成一个 PWM 输出,因此输出频率减半
* 另外,我们还可以开启中断在中断里面修改 CCRx,从而实现不同频率/不同相位的控制
* 但是我们不推荐这么使用,因为这可能导致非常频繁的中断,从而占用大量 CPU 资源
*
* 高级定时器的时钟来自 APB2, 而 PCLK2 = 72Mhz, 我们设置 PPRE2 不分频, 因此
* 高级定时器时钟 = 72Mhz
* 定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=定时器工作频率,单位:Mhz
*
* @param arr: 自动重装值。
* @param psc: 时钟预分频数
* @retval 无
*/

TIM_HandleTypeDef g_timx_comp_pwm_handle;

void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
	TIM_OC_InitTypeDef timx_oc_comp_pwm = {0};
	
	g_timx_comp_pwm_handle.Instance = ATIM_TIMX_COMP; /* 定时器 x */
	g_timx_comp_pwm_handle.Init.Prescaler = psc ; /* 定时器分频 */
	g_timx_comp_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* 递增计数 */
	g_timx_comp_pwm_handle.Init.Period = arr; /* 自动重装载值 */
	g_timx_comp_pwm_handle.Init.AutoReloadPreload =TIM_AUTORELOAD_PRELOAD_ENABLE; /* 使能影子寄存器 TIMx_ARR */
	HAL_TIM_OC_Init(&g_timx_comp_pwm_handle); /* 输出比较模式初始化 */
	
	timx_oc_comp_pwm.OCMode = TIM_OCMODE_TOGGLE; /* 比较输出模式翻转功能 */
	timx_oc_comp_pwm.Pulse = 250 - 1; /* 设置输出比较寄存器的值 */
	timx_oc_comp_pwm.OCPolarity = TIM_OCPOLARITY_HIGH;/* 输出比较极性为高 */
	
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_1); /* 初始化定时器的输出比较通道 1 */
	/* CCR1 寄存器预装载使能 */
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_1);
	timx_oc_comp_pwm.Pulse = 500 - 1;
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_2); /* 初始化定时器的输出比较通道 2 */
	/* CCR2 寄存器预装载使能 */
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_2);
	timx_oc_comp_pwm.Pulse = 750 - 1;
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_3); /* 初始化定时器的输出比较通道 3 */
	/* CCR3 寄存器预装载使能 */
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);
	timx_oc_comp_pwm.Pulse = 1000 - 1;
	HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &timx_oc_comp_pwm,TIM_CHANNEL_4); /* 初始化定时器的输出比较通道 4 */
	/* CCR4 寄存器预装载使能 */ 
	__HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_4);
	
	/* 使能 TIM1 通道 1~通道 4 输出 */
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_2);
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);
	HAL_TIM_OC_Start(&g_timx_comp_pwm_handle, TIM_CHANNEL_4);
}


/**
* @brief 定时器底层驱动，时钟使能，引脚配置
 此函数会被 HAL_TIM_OC_Init()调用
* @param htim:定时器句柄
* @retval 无
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
* @brief 高级定时器 TIMX 通道 Y 输出指定个数 PWM 初始化函数
* @note
* 高级定时器的时钟来自 APB2, 而 PCLK2 = 72Mhz, 我们设置 PPRE2 不分频, 因此
* 高级定时器时钟 = 72Mhz
* 定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
* Ft=定时器工作频率,单位:Mhz
* @param arr: 自动重装值
* @param psc: 时钟预分频数
* @retval 无
*/
TIM_HandleTypeDef g_timx_npwm_chy_handle;

void atim_timx_npwm_chy_init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef gpio_init_struct;	/*GPIO句柄*/
	TIM_OC_InitTypeDef timx_oc_npwm_chy; /* 定时器输出句柄 */
	ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE(); /* TIMX 通道 IO 口时钟使能 */
	ATIM_TIMX_NPWM_CHY_CLK_ENABLE(); /* TIMX 时钟使能 */
	g_timx_npwm_chy_handle.Instance = ATIM_TIMX_NPWM; /* 定时器 x */
	g_timx_npwm_chy_handle.Init.Prescaler = psc; /* 定时器分频 */
	g_timx_npwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;/* 递增计数 */
	g_timx_npwm_chy_handle.Init.Period = arr; /* 自动重装载值 */

	/*	在本实验中我们把自动重装载预装载允许位要置 1，
	这样就算改变 ARR 寄存器的值，该值也不会马上生效，
	而是等待之前设置的 PWM 完整输出后（发生更新事件）才生效	*/
	g_timx_npwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /*使能 TIMx_ARR 进行缓冲 */

	g_timx_npwm_chy_handle.Init.RepetitionCounter = 0; /* 重复计数器初始值 */
	HAL_TIM_PWM_Init(&g_timx_npwm_chy_handle); /* 初始化 PWM */

	gpio_init_struct.Pin = ATIM_TIMX_NPWM_CHY_GPIO_PIN;/* 通道 y 的 CPIO 口 */
	gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* 复用推挽输出 */
	gpio_init_struct.Pull = GPIO_PULLUP; /* 上拉 */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	HAL_GPIO_Init(ATIM_TIMX_NPWM_CHY_GPIO_PORT, &gpio_init_struct);

	timx_oc_npwm_chy.OCMode = TIM_OCMODE_PWM1; /* 模式选择 PWM 1*/
	timx_oc_npwm_chy.Pulse = arr / 2; /* 设置比较值,此值用来确定占空比 */
	
	/*因为我们的 LED0（连接 PD13）是高电平亮，
	而我们希望输出最后一个 PWM 波的时候，LED0 就灭，
	所以我们设置输出比较极性为低 */
	timx_oc_npwm_chy.OCPolarity = TIM_OCPOLARITY_LOW; /* 输出比较极性为低 */
	
	HAL_TIM_PWM_ConfigChannel(&g_timx_npwm_chy_handle, &timx_oc_npwm_chy,ATIM_TIMX_NPWM_CHY); /* 配置 TIMx 通道 y */
	
	/* 设置中断优先级，抢占优先级 1，子优先级 3 */
	HAL_NVIC_SetPriority(ATIM_TIMX_NPWM_IRQn, 1, 3); 
	HAL_NVIC_EnableIRQ(ATIM_TIMX_NPWM_IRQn); /* 开启 ITMx 中断 */
	__HAL_TIM_ENABLE_IT(&g_timx_npwm_chy_handle, TIM_IT_UPDATE);/* 允许更新中断 */
	HAL_TIM_PWM_Start(&g_timx_npwm_chy_handle, ATIM_TIMX_NPWM_CHY);/* 使能输出 */
	ATIM_TIMX_NPWM->CR1 &= ~(TIM_CR1_CEN); //关闭定时器 TIMX，防止程序刚跑就近更新中断
}

///* g_npwm_remain 表示当前还剩下多少个脉冲要发送
//* 每次最多发送 256 个脉冲
//*/
//static uint8_t g_npwm_remain = 0;
///**
//* @brief 高级定时器 TIMX NPWM 设置 PWM 个数
//* @param rcr: PWM 的个数, 1~2^8 次方个
//* @retval 无
//*/
//void atim_timx_npwm_chy_set(uint8_t npwm)
//{
//	if (npwm == 0) return ;
//	g_npwm_remain = npwm; /* 保存脉冲个数 */
//	/* 产生一次更新事件,在中断里面处理脉冲输出 */
//	HAL_TIM_GenerateEvent(&g_timx_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE);
//	__HAL_TIM_ENABLE(&g_timx_npwm_chy_handle); /* 使能定时器 TIMX */
//}


/**
* @brief 定时器中断服务函数
* @param 无
* @retval 无
*/
//void ATIM_TIMX_NPWM_IRQHandler(void)
//{
////	uint16_t npwm = 0;
//	/* 以下代码没有使用定时器 HAL 库共用处理函数来处理，而是直接通过判断中断标志位的方式 */
//	if(__HAL_TIM_GET_FLAG(&g_timx_npwm_chy_handle, TIM_FLAG_UPDATE) != RESET)
//	{
//		
//		if(g_npwm_remain) 
//		{
//			ATIM_TIMX_NPWM->RCR = g_npwm_remain - 1; /* 设置 RCR 值为 npwm-1, 即 npwm 个脉冲 */
//			HAL_TIM_GenerateEvent(&g_timx_npwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* 产生一次更新事件,把RCR 寄存器的值更新到 RCR 影子寄存器中 */
//			__HAL_TIM_ENABLE(&g_timx_npwm_chy_handle); /* 使能定时器 TIMX */
//			g_npwm_remain = 0;
//		}else
//		{
//			/* 关闭定时器 TIMX,使用__HAL_TIM_DISABLE会失能通道输出，所以这里单独操作寄存器关闭定时器 */
//			ATIM_TIMX_NPWM->CR1 &= ~(TIM_CR1_CEN);
//		}
		
		
		/* 清除定时器更新中断标志位 */
//		__HAL_TIM_CLEAR_IT(&g_timx_npwm_chy_handle, TIM_IT_UPDATE); 
//	}
//}



