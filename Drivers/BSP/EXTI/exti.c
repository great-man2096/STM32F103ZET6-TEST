#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./SYSTEM/DELAY/delay.h"
#include "./BSP/EXTI/exti.h"


/**
* @brief 外部中断初始化程序
* @param 无
* @retval 无
*/
void exti_init(void){
	GPIO_InitTypeDef gpio_init_sttruct;
	
	//key0初始化
	KEY0_INT_GPIO_CLK_ENABLE();
	gpio_init_sttruct.Pin = KEY0_INT_GPIO_PIN;
	gpio_init_sttruct.Mode = GPIO_MODE_IT_FALLING; /* 下升沿触发 */
	gpio_init_sttruct.Pull = GPIO_PULLUP; //上拉恒高电位
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //速度
	HAL_GPIO_Init(GPIOE,&gpio_init_sttruct);
	HAL_NVIC_SetPriority(KEY0_INT_IRQn,0,2); /* 抢占 0，子优先级 2 */
	HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);	/* 使能中断线 0 */
	
	//key1初始化
	KEY1_INT_GPIO_CLK_ENABLE();
	gpio_init_sttruct.Pin = KEY1_INT_GPIO_PIN;
	gpio_init_sttruct.Mode = GPIO_MODE_IT_FALLING; /* 下升沿触发 */
	gpio_init_sttruct.Pull = GPIO_PULLUP; //上拉恒高电位
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //速度
	HAL_GPIO_Init(GPIOC,&gpio_init_sttruct);
	HAL_NVIC_SetPriority(KEY1_INT_IRQn,1,2);
	HAL_NVIC_EnableIRQ(KEY1_INT_IRQn); /* 使能中断线 1 */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
}

///**
//* @brief KEY0 外部中断服务程序
//* @param 无
//* @retval 无
//*/
//void KEY0_INT_IRQHandler(void)
//{
///* 调用中断处理公用函数 清除 KEY0 所在中断线 的中断标志位 */
//HAL_GPIO_EXTI_IRQHandler(KEY0_INT_GPIO_PIN);
///* HAL 库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
//__HAL_GPIO_EXTI_CLEAR_IT(KEY0_INT_GPIO_PIN);
//}
///**
//* @brief KEY1 外部中断服务程序
//* @param 无
//* @retval 无
//*/
//void KEY1_INT_IRQHandler(void)
//{
///* 调用中断处理公用函数 清除 KEY1 所在中断线 的中断标志位 */
//HAL_GPIO_EXTI_IRQHandler(KEY1_INT_GPIO_PIN);
///* HAL 库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
//__HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);
//}


/**
* @brief 中断服务程序中需要做的事情
 在 HAL 库中所有的外部中断服务函数都会调用此函数
* @param GPIO_Pin:中断引脚号
* @retval 无
*/
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	 delay_ms(10); /* 消抖 */
//	 switch(GPIO_Pin)
//	 {
//		 case KEY0_INT_GPIO_PIN:
//			 if (KEY0 == 0) //如果按键0按下
//			 {
//			 LED0_TOGGLE(); /* LED0 状态取反 */
//			 }
//			 break;
//		 case KEY1_INT_GPIO_PIN:
//			 if (KEY1 == 0) //如果按键1按下
//			 {
//			 LED1_TOGGLE(); /* LED1 状态取反 */
//			 }
//			 break;
//	 }
//}


