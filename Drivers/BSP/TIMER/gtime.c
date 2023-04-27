#include "./BSP/TIMER/gtime.h"
#include "./BSP/LED/led.h"

TIM_HandleTypeDef g_timx_pwm_chy_handle;

TIM_HandleTypeDef g_timx_cap_chy_handle;

TIM_HandleTypeDef	g_timx_cnt_chy_handler;

/**
* @brief 通用定时器 TIMX 通道 Y 脉冲计数 初始化函数
* @note
* 本函数选择通用定时器的时钟选择: 外部时钟源模式 1(SMS[2:0] = 111)
* CNT 的计数时钟源就来自 TIMX_CH1/CH2, 可以实现外部脉冲计数(脉冲接入 CH1/CH2)
*
* 时钟分频数 = psc, 一般设置为 0, 表示每一个时钟都会计数一次, 以提高精度.
* 通过读取 CNT 和溢出次数, 经过简单计算, 可以得到当前的计数值, 从而实现脉冲计数
* @param arr: 自动重装值
* @retval 无
*/
void gtim_timx_cnt_chy_init(uint16_t psc)
{
 GPIO_InitTypeDef gpio_init_struct;
 TIM_SlaveConfigTypeDef tim_slave_config = {0};
 GTIM_TIMX_CNT_CHY_CLK_ENABLE(); /* 使能 TIMx 时钟 */
 GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE(); /* 开启 GPIOA 时钟 */
 
 g_timx_cnt_chy_handler.Instance = GTIM_TIMX_CNT; /* 定时器 x */
 g_timx_cnt_chy_handler.Init.Prescaler = psc; /* 定时器分频 */
 g_timx_cnt_chy_handler.Init.CounterMode = TIM_COUNTERMODE_UP;/* 递增计数模式 */
 g_timx_cnt_chy_handler.Init.Period = 65535; /* 自动重装载值 */
 HAL_TIM_IC_Init(&g_timx_cnt_chy_handler);
 gpio_init_struct.Pin = GTIM_TIMX_CNT_CHY_GPIO_PIN; /* 输入捕获的 GPIO 口 */
 gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* 复用推挽输出 */
 gpio_init_struct.Pull = GPIO_PULLDOWN; /* 下拉 */
 gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
 HAL_GPIO_Init(GTIM_TIMX_CNT_CHY_GPIO_PORT, &gpio_init_struct);
 /* 从模式：外部触发模式 1 */
 tim_slave_config.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;/*从模式：外部触发模式 1*/
 tim_slave_config.InputTrigger = TIM_TS_TI1FP1; /* TI1FP1 作为触发输入源 */
 tim_slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;/* 上升沿 */
 tim_slave_config.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;/* 不分频*/
 tim_slave_config.TriggerFilter = 0x0; /* 滤波：本例中不需要任何滤波 */
HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_chy_handler, &tim_slave_config);
/* 设置中断优先级，抢占优先级 1，子优先级 3 */
 HAL_NVIC_SetPriority(GTIM_TIMX_CNT_IRQn, 1, 3); 
 HAL_NVIC_EnableIRQ(GTIM_TIMX_CNT_IRQn);
 __HAL_TIM_ENABLE_IT(&g_timx_cnt_chy_handler, TIM_IT_UPDATE); /* 使能更新中断 */
 HAL_TIM_IC_Start(&g_timx_cnt_chy_handler, GTIM_TIMX_CNT_CHY);/* 使能通道输入 */
}

/**
* @brief 通用定时器 TIMX 脉冲计数 更新中断服务函数
* @param 无
* @retval 无
*/

uint8_t g_timxchy_cnt_ofcnt;
void GTIM_TIMX_CNT_IRQHandler(void)
{
 /* 以下代码没有使用定时器 HAL 库共用处理函数来处理，而是直接通过判断中断标志位的方式 */
 if(__HAL_TIM_GET_FLAG(&g_timx_cnt_chy_handler, TIM_FLAG_UPDATE) != RESET)
 {
	g_timxchy_cnt_ofcnt++; /* 累计溢出次数 */
 }
 __HAL_TIM_CLEAR_IT(&g_timx_cnt_chy_handler, TIM_IT_UPDATE);
}

/**
* @brief 通用定时器 TIMX 通道 Y 获取当前计数值
* @param 无
* @retval 当前计数值
*/
uint32_t gtim_timx_cnt_chy_get_count(void)
{
 uint32_t count = 0;
 count = g_timxchy_cnt_ofcnt * 65536;/* 计算溢出次数对应的计数值 */
 count += __HAL_TIM_GET_COUNTER(&g_timx_cnt_chy_handler);/* 加上当前 CNT 的值 */
 return count;
}

/**
* @brief 通用定时器 TIMX 通道 Y 重启计数器
* @param 无
* @retval 当前计数值
*/
void gtim_timx_cnt_chy_restart(void)
{
 __HAL_TIM_DISABLE(&g_timx_cnt_chy_handler); /* 关闭定时器 TIMX */
 g_timxchy_cnt_ofcnt = 0; /* 累加器清零 */
 __HAL_TIM_SET_COUNTER(&g_timx_cnt_chy_handler, 0); /* 计数器清零 */
 __HAL_TIM_ENABLE(&g_timx_cnt_chy_handler); /* 使能定时器 TIMX */
}




//通用定时器通道y输入捕获初始化函数
void gtim_timx_cap_chy_init(uint16_t arr,uint16_t psc){
	TIM_IC_InitTypeDef timx_ic_cap_chy = {0};
	
	g_timx_cap_chy_handle.Instance = TIM5;
	g_timx_cap_chy_handle.Init.Prescaler = psc;
	g_timx_cap_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	g_timx_cap_chy_handle.Init.Period =arr;
	HAL_TIM_IC_Init(&g_timx_cap_chy_handle);
	
	timx_ic_cap_chy.ICPolarity = TIM_ICPOLARITY_RISING;
	timx_ic_cap_chy.ICSelection = TIM_ICSELECTION_DIRECTTI;	 /* 映射到TI1上 */
	timx_ic_cap_chy.ICPrescaler = TIM_ICPSC_DIV1;
	timx_ic_cap_chy.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&g_timx_cap_chy_handle,&timx_ic_cap_chy,TIM_CHANNEL_2);
	
	__HAL_TIM_ENABLE_IT(&g_timx_cap_chy_handle,TIM_IT_UPDATE);
	HAL_TIM_IC_Start_IT(&g_timx_cap_chy_handle,TIM_CHANNEL_2);
	
}
//定时器输入捕获MSP初始化函数
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
/* 输入捕获状态(g_timxchy_cap_sta)
 * [7]  :0,没有成功的捕获;1,成功捕获到一次.
 * [6]  :0,还没捕获到高电平;1,已经捕获到高电平了.
 * [5:0]:捕获高电平后溢出的次数,最多溢出63次,所以最长捕获值 = 63*65536 + 65535 = 4194303
 *       注意:为了通用,我们默认ARR和CCRy都是16位寄存器,对于32位的定时器(如:TIM5),也只按16位使用
 *       按1us的计数频率,最长溢出时间为:4194303 us, 约4.19秒
 *
 *      (说明一下：正常32位定时器来说,1us计数器加1,溢出时间:4294秒)
 */
uint8_t g_timxchy_cap_sta = 0; //输入捕获状态
uint16_t g_timxchy_cap_val = 0; //输入捕获值

//定时器5中断服务函数
void TIM5_IRQHandler(void){
	HAL_TIM_IRQHandler(&g_timx_cap_chy_handle);
}

//定时器输入捕获中断处理回调函数
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

//定时器输入捕获更新中断回调函数
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


/*********************************通用通用定时器PWM输出实验程序*************************************/

TIM_HandleTypeDef g_timx_pwm_chy_handle;     /* 定时器x句柄 */

/**
 * @brief       通用定时器TIMX 通道Y PWM输出 初始化函数（使用PWM模式1）
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_pwm_chy  = {0};                          /* 定时器PWM输出配置 */

    g_timx_pwm_chy_handle.Instance = GTIM_TIMX_PWM;                     /* 定时器 */
    g_timx_pwm_chy_handle.Init.Prescaler = psc;                         /* 定时器分频 */
    g_timx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* 递增计数模式 */
    g_timx_pwm_chy_handle.Init.Period = arr;                            /* 自动重装载值 */
    HAL_TIM_PWM_Init(&g_timx_pwm_chy_handle);                           /* 初始化PWM */

    timx_oc_pwm_chy.OCMode = TIM_OCMODE_PWM1;                           /* 模式选择PWM1 */
    timx_oc_pwm_chy.Pulse = arr / 2;                                    /* 设置比较值,此值用来确定占空比 */
                                                                        /* 这里默认设置比较值为自动重装载值的一半,即占空比为50% */
    timx_oc_pwm_chy.OCPolarity = TIM_OCPOLARITY_LOW;                    /* 输出比较极性为低 */
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_chy_handle, &timx_oc_pwm_chy, GTIM_TIMX_PWM_CHY); /* 配置TIMx通道y */
    HAL_TIM_PWM_Start(&g_timx_pwm_chy_handle, GTIM_TIMX_PWM_CHY);       /* 开启对应PWM通道 */
}

/**
 * @brief       定时器底层驱动，时钟使能，引脚配置
                此函数会被HAL_TIM_PWM_Init()调用
 * @param       htim:定时器句柄
 * @retval      无
 */
// void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == GTIM_TIMX_PWM)
//     {
//         GPIO_InitTypeDef gpio_init_struct;
//         GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();               /* 开启通道y的CPIO时钟 */
//         GTIM_TIMX_PWM_CHY_CLK_ENABLE();

//         gpio_init_struct.Pin = GTIM_TIMX_PWM_CHY_GPIO_PIN; /* 通道y的CPIO口 */
//         gpio_init_struct.Mode = GPIO_MODE_AF_PP;           /* 复用推完输出 */
//         gpio_init_struct.Pull = GPIO_PULLUP;               /* 上拉 */
//         gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;     /* 高速 */
//         HAL_GPIO_Init(GTIM_TIMX_PWM_CHY_GPIO_PORT, &gpio_init_struct);
//         GTIM_TIMX_PWM_CHY_GPIO_REMAP();                    /* IO口REMAP设置, 是否必要查看头文件配置的说明! */
//     }
// }




