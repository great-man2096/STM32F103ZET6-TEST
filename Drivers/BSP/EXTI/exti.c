#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./SYSTEM/DELAY/delay.h"
#include "./BSP/EXTI/exti.h"


/**
* @brief �ⲿ�жϳ�ʼ������
* @param ��
* @retval ��
*/
void exti_init(void){
	GPIO_InitTypeDef gpio_init_sttruct;
	
	//key0��ʼ��
	KEY0_INT_GPIO_CLK_ENABLE();
	gpio_init_sttruct.Pin = KEY0_INT_GPIO_PIN;
	gpio_init_sttruct.Mode = GPIO_MODE_IT_FALLING; /* �����ش��� */
	gpio_init_sttruct.Pull = GPIO_PULLUP; //������ߵ�λ
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //�ٶ�
	HAL_GPIO_Init(GPIOE,&gpio_init_sttruct);
	HAL_NVIC_SetPriority(KEY0_INT_IRQn,0,2); /* ��ռ 0�������ȼ� 2 */
	HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);	/* ʹ���ж��� 0 */
	
	//key1��ʼ��
	KEY1_INT_GPIO_CLK_ENABLE();
	gpio_init_sttruct.Pin = KEY1_INT_GPIO_PIN;
	gpio_init_sttruct.Mode = GPIO_MODE_IT_FALLING; /* �����ش��� */
	gpio_init_sttruct.Pull = GPIO_PULLUP; //������ߵ�λ
	gpio_init_sttruct.Speed = GPIO_SPEED_FREQ_HIGH; //�ٶ�
	HAL_GPIO_Init(GPIOC,&gpio_init_sttruct);
	HAL_NVIC_SetPriority(KEY1_INT_IRQn,1,2);
	HAL_NVIC_EnableIRQ(KEY1_INT_IRQn); /* ʹ���ж��� 1 */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
}

///**
//* @brief KEY0 �ⲿ�жϷ������
//* @param ��
//* @retval ��
//*/
//void KEY0_INT_IRQHandler(void)
//{
///* �����жϴ����ú��� ��� KEY0 �����ж��� ���жϱ�־λ */
//HAL_GPIO_EXTI_IRQHandler(KEY0_INT_GPIO_PIN);
///* HAL ��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
//__HAL_GPIO_EXTI_CLEAR_IT(KEY0_INT_GPIO_PIN);
//}
///**
//* @brief KEY1 �ⲿ�жϷ������
//* @param ��
//* @retval ��
//*/
//void KEY1_INT_IRQHandler(void)
//{
///* �����жϴ����ú��� ��� KEY1 �����ж��� ���жϱ�־λ */
//HAL_GPIO_EXTI_IRQHandler(KEY1_INT_GPIO_PIN);
///* HAL ��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
//__HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);
//}


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


