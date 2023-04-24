#include "./BSP/KEY/key.h"
#include "./SYSTEM/DELAY/delay.h"

void key_init(void){
	GPIO_InitTypeDef gpio_init_sttruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	gpio_init_sttruct.Pin = GPIO_PIN_13;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; //输入模式
	gpio_init_sttruct.Pull = GPIO_PULLUP; //上拉恒高电位
	HAL_GPIO_Init(GPIOC,&gpio_init_sttruct);
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	gpio_init_sttruct.Pin = GPIO_PIN_0;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; //输入模式
	gpio_init_sttruct.Pull = GPIO_PULLUP; //上拉恒高电位
	HAL_GPIO_Init(GPIOE,&gpio_init_sttruct);
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	gpio_init_sttruct.Pin = GPIO_PIN_1;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; /* 输入模式 */
	gpio_init_sttruct.Pull = GPIO_PULLDOWN; //下拉恒低电位
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //速度
	HAL_GPIO_Init(GPIOA,&gpio_init_sttruct);
	
	
	gpio_init_sttruct.Pin = GPIO_PIN_0;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; /* 输入模式 */
	gpio_init_sttruct.Pull = GPIO_PULLDOWN; //下拉恒低电位
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //速度
	HAL_GPIO_Init(GPIOA,&gpio_init_sttruct);
	
}


uint8_t key1_scan(void){
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
		delay_ms(10); //允许误差10ms,消除抖动
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
			while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0); //等待按键松开
			return 1; //按下返回1
		}
	}
	return 0; //没有按下返回0
	
}


uint8_t key2_scan(void){
	if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) == 0){
		delay_ms(10); //允许误差10ms,消除抖动
		if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) == 0){
			while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) == 0); //等待按键松开
			return 1; //按下返回1
		}
	}
	return 0; //没有按下返回0
	
}


uint8_t key3_scan(void){
	if(KEY2 == 1){
		delay_ms(10); //允许误差10ms,消除抖动
		if(KEY2 == 1){
			while(KEY2 == 1); //等待按键松开
			return 1; //按下返回1
		}
	}
	return 0; //没有按下返回0
	
}

uint8_t key4_scan(void){
	if(KEY3 == 1){
		delay_ms(10); //允许误差10ms,消除抖动
		if(KEY3 == 1){
			while(KEY3 == 1); //等待按键松开
			return 1; //按下返回1
		}
	}
	return 0; //没有按下返回0
	
}



uint8_t key1_long_scan(void){
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
		delay_ms(10); //允许误差10ms,消除抖动
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
			int coutTime = 0;
			while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
				delay_ms(100);
				coutTime++;
				if(coutTime>=20){	//连按2秒及以上返回1
				return 1;
				}
			}; //等待按键松开
			
		}
	}
	return 0; //没有按下返回0
	
}

