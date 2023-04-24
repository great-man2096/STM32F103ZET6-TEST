#include "./BSP/KEY/key.h"
#include "./SYSTEM/DELAY/delay.h"

void key_init(void){
	GPIO_InitTypeDef gpio_init_sttruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	gpio_init_sttruct.Pin = GPIO_PIN_13;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; //����ģʽ
	gpio_init_sttruct.Pull = GPIO_PULLUP; //������ߵ�λ
	HAL_GPIO_Init(GPIOC,&gpio_init_sttruct);
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	gpio_init_sttruct.Pin = GPIO_PIN_0;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; //����ģʽ
	gpio_init_sttruct.Pull = GPIO_PULLUP; //������ߵ�λ
	HAL_GPIO_Init(GPIOE,&gpio_init_sttruct);
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	gpio_init_sttruct.Pin = GPIO_PIN_1;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; /* ����ģʽ */
	gpio_init_sttruct.Pull = GPIO_PULLDOWN; //������͵�λ
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //�ٶ�
	HAL_GPIO_Init(GPIOA,&gpio_init_sttruct);
	
	
	gpio_init_sttruct.Pin = GPIO_PIN_0;
	gpio_init_sttruct.Mode = GPIO_MODE_INPUT; /* ����ģʽ */
	gpio_init_sttruct.Pull = GPIO_PULLDOWN; //������͵�λ
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //�ٶ�
	HAL_GPIO_Init(GPIOA,&gpio_init_sttruct);
	
}


uint8_t key1_scan(void){
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
		delay_ms(10); //�������10ms,��������
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
			while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0); //�ȴ������ɿ�
			return 1; //���·���1
		}
	}
	return 0; //û�а��·���0
	
}


uint8_t key2_scan(void){
	if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) == 0){
		delay_ms(10); //�������10ms,��������
		if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) == 0){
			while(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0) == 0); //�ȴ������ɿ�
			return 1; //���·���1
		}
	}
	return 0; //û�а��·���0
	
}


uint8_t key3_scan(void){
	if(KEY2 == 1){
		delay_ms(10); //�������10ms,��������
		if(KEY2 == 1){
			while(KEY2 == 1); //�ȴ������ɿ�
			return 1; //���·���1
		}
	}
	return 0; //û�а��·���0
	
}

uint8_t key4_scan(void){
	if(KEY3 == 1){
		delay_ms(10); //�������10ms,��������
		if(KEY3 == 1){
			while(KEY3 == 1); //�ȴ������ɿ�
			return 1; //���·���1
		}
	}
	return 0; //û�а��·���0
	
}



uint8_t key1_long_scan(void){
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
		delay_ms(10); //�������10ms,��������
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
			int coutTime = 0;
			while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 0){
				delay_ms(100);
				coutTime++;
				if(coutTime>=20){	//����2�뼰���Ϸ���1
				return 1;
				}
			}; //�ȴ������ɿ�
			
		}
	}
	return 0; //û�а��·���0
	
}

