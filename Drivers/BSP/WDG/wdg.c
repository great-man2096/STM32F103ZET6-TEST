#include "./BSP/LED/led.h"
#include "./BSP/WDG/wdg.h"

/****************************************************************************************/
IWDG_HandleTypeDef g_iwdg_handle;   /* �������Ź���� */

/**
 * @brief       ��ʼ���������Ź� 
 * @param       prer: IWDG_PRESCALER_4~IWDG_PRESCALER_256,��Ӧ4~256��Ƶ
 *   @arg       ��Ƶ���� = 4 * 2^prer. �����ֵֻ����256!
 * @param       rlr: �Զ���װ��ֵ,0~0XFFF. 
 * @note        ʱ�����(���):Tout=((4 * 2^prer) * rlr) / 40 (ms). 
 * @retval      ��
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    g_iwdg_handle.Instance = IWDG;
    g_iwdg_handle.Init.Prescaler = prer; /* ����IWDG��Ƶϵ�� */
    g_iwdg_handle.Init.Reload = rlr;     /* ��װ��ֵ */
    HAL_IWDG_Init(&g_iwdg_handle);       /* ��ʼ��IWDG������ */
}

/**
 * @brief       ι�������Ź�
 * @param       ��
 * @retval      ��
 */
void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&g_iwdg_handle); /* ��װ�ؼ����� */
}
/****************************************************************************************/
WWDG_HandleTypeDef g_wwdg_handle;   /* ���ڿ��Ź���� */

void wwdg_init(uint8_t cr,uint8_t wr,uint32_t fprer){
	
	g_wwdg_handle.Instance = WWDG;
	g_wwdg_handle.Init.Counter = cr;
	g_wwdg_handle.Init.EWIMode = WWDG_EWI_ENABLE;
	g_wwdg_handle.Init.Prescaler = fprer;
	g_wwdg_handle.Init.Window = wr;
	HAL_WWDG_Init(&g_wwdg_handle);       /* ��ʼ��IWDG������ */
}


void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg){
	__HAL_RCC_WWDG_CLK_ENABLE(); /* ʹ�ܴ��ڿ��Ź�ʱ�� */
	HAL_NVIC_SetPriority(WWDG_IRQn,2,3);
	HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

/**
* @brief ���ڿ��Ź��жϷ������
* @param ��
* @retval ��
*/
void WWDG_IRQHandler(void)
{
 HAL_WWDG_IRQHandler(&g_wwdg_handle);
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg){
	HAL_WWDG_Refresh(&g_wwdg_handle);
	LED1_TOGGLE(); /* LED1 ��˸ */
}


void wwdg_feed(){
	HAL_WWDG_Refresh(&g_wwdg_handle);
};










