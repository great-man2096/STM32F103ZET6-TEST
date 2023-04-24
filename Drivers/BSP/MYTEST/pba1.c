#include "./BSP/LED/led.h"
#include "./SYSTEM/DELAY/delay.h"
#include "./BSP/MYTEST/pba1.h"


void pba1_init(void){
	GPIO_InitTypeDef gpio_init_sttruct;
	
	//key0��ʼ��
	__HAL_RCC_GPIOA_CLK_ENABLE();
	gpio_init_sttruct.Pin = GPIO_PIN_1;
	gpio_init_sttruct.Mode = GPIO_MODE_IT_FALLING; /* �����ش��� */
	gpio_init_sttruct.Pull = GPIO_PULLUP; //������ߵ�λ
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //�ٶ�
	HAL_GPIO_Init(GPIOA,&gpio_init_sttruct);
	HAL_NVIC_SetPriority(EXTI1_IRQn,0,2); /* ��ռ 0�������ȼ� 2 */
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);	/* ʹ���ж��� 1 */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
}

/**
* @brief KEY3 �ⲿ�жϷ������
* @param ��
* @retval ��
*/
void EXTI1_IRQHandler(void)
{
/* �����жϴ����ú��� ��� KEY0 �����ж��� ���жϱ�־λ */
HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
/* HAL ��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
}


/**
* @brief �жϷ����������Ҫ��������
 �� HAL �������е��ⲿ�жϷ�����������ô˺���
* @param GPIO_Pin:�ж����ź�
* @retval ��
*/
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	 delay_ms(10); /* ���� */
//	 switch(GPIO_Pin)
//	 {
//		 case KEY0_INT_GPIO_PIN:
//			 if (KEY0 == 0) //�������0����
//			 {
//			 LED0_TOGGLE(); /* LED0 ״̬ȡ�� */
//			 }
//			 break;
//		 case KEY1_INT_GPIO_PIN:
//			 if (KEY1 == 0) //�������1����
//			 {
//			 LED1_TOGGLE(); /* LED1 ״̬ȡ�� */
//			 }
//			 break;
//	 }
//}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 delay_ms(10); /* ���� */
	 switch(GPIO_Pin)
	 {
		 case GPIO_PIN_1:
			 if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0) //�������0����
			 {
//			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1,GPIO_PIN_SET);
//			 while(KEY0 == 0){}
//			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1,GPIO_PIN_RESET);
				LED0_TOGGLE(); /* LED0 ״̬ȡ�� */
			 }
			 break;

	 }
}
