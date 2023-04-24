/**
 ******************************************************************************
 * @file     main.c
 * @author   ����ԭ���Ŷ�(ALIENTEK)
 * @version  V1.0
 * @date     2020-08-20
 * @brief    �½�����ʵ��-HAL��汾 ʵ��
 * @license  Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ******************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103 ������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 ******************************************************************************
 */
#include "string.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
// #include "./BSP/EXTI/exti.h"
#include "./BSP/WDG/wdg.h"
// #include "./BSP/TIMER/btim.h"
#include "./BSP/TIMER/gtime.h"
#include "./BSP/TIMER/atim.h"
#include "./BSP/LCD/lcd.h"
#include "./USMART/usmart.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/PWR/pwr.h"
#include "./BSP/DMA/dma.h"
#include "./BSP/ADC/adc.h"

//	extern uint8_t g_timxchy_cap_sta; //���벶��״̬
//	extern uint16_t g_timxchy_cap_val; //���벶��ֵ
//	extern uint8_t g_timxchy_pwmin_sta; /* pwm����״̬ */
//	extern uint16_t g_timxchy_pwmin_psc; /* pwm�����Ƶϵ�� */
//	extern uint16_t g_timxchy_pwmin_hval;	/* pwm����ߵ�ƽ���� */
//	extern uint16_t g_timxchy_pwmin_cval;	/* pwm�������ڿ��� */

void led_set(uint8_t sta)
{
	LED1(sta);
}

/* �����������ò��Ժ��� */
void test_fun(void (*ledset)(uint8_t), uint8_t sta)
{
	ledset(sta);
}

// ʱ����ʾ����
uint8_t tbuf[40];
/* �����ַ�����������ʾ�� */
char *weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saterday"};
void showtime(void)
{
	rtc_get_time();
	sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
	lcd_show_string(30, 160, 210, 16, 16, (char *)tbuf, RED);
	sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
	lcd_show_string(30, 180, 210, 16, 16, (char *)tbuf, RED);
	sprintf((char *)tbuf, "Week:%s", weekdays[calendar.week]);
	lcd_show_string(30, 200, 210, 16, 16, (char *)tbuf, RED);
	lcd_show_string(30, 220, 210, 16, 16, "vscode downlod test", RED);
}
int main(void)
{
	HAL_Init();							/* ��ʼ��HAL�� */
	sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
	delay_init(72);						/* ��ʱ��ʼ�� */
	usart_init(115200);					/* ���ڳ�ʼ��Ϊ 115200 */
	led_init();							/* LED��ʼ�� */
	usmart_dev.init(72);				/* USMART��ʼ�� */
	key_init();							/* ��ʼ������ */
	rtc_init();							// RTC��ʼ��
	lcd_init();							/* LCD��ʼ�� */

	//��ͨ��ADC��������12ת16�ֱ��ʣ�
	


	// ��ͨ��ADC��DMA�ɼ�ʵ��
	// #define ADC_DMA_BUF_SIZE        50 * 6      /* ADC DMA�ɼ� BUF��С, Ӧ����ADCͨ������������ */
	// uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
	// extern uint8_t g_adc_dma_sta;               /* DMA����״̬��־, 0,δ���; 1, ����� */

	// uint16_t i,j;
    // uint16_t adcx;
    // uint32_t sum;
    // float temp;
	// adc_nch_dma_init((uint32_t)&g_adc_dma_buf); /* ��ʼ��ADC DMA�ɼ� */

    // lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    // lcd_show_string(30,  70, 200, 16, 16, "ADC 6CH DMA TEST", RED);
    // lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    // lcd_show_string(30, 110, 200, 12, 12, "ADC1_CH0_VAL:", BLUE);
    // lcd_show_string(30, 122, 200, 12, 12, "ADC1_CH0_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */
    
    // lcd_show_string(30, 140, 200, 12, 12, "ADC1_CH1_VAL:", BLUE);
    // lcd_show_string(30, 152, 200, 12, 12, "ADC1_CH1_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

    // lcd_show_string(30, 170, 200, 12, 12, "ADC1_CH2_VAL:", BLUE);
    // lcd_show_string(30, 182, 200, 12, 12, "ADC1_CH2_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

    // lcd_show_string(30, 200, 200, 12, 12, "ADC1_CH3_VAL:", BLUE);
    // lcd_show_string(30, 212, 200, 12, 12, "ADC1_CH3_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

    // lcd_show_string(30, 230, 200, 12, 12, "ADC1_CH4_VAL:", BLUE);
    // lcd_show_string(30, 242, 200, 12, 12, "ADC1_CH4_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

    // lcd_show_string(30, 260, 200, 12, 12, "ADC1_CH5_VAL:", BLUE);
    // lcd_show_string(30, 272, 200, 12, 12, "ADC1_CH5_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

    // adc_dma_enable(ADC_DMA_BUF_SIZE);   /* ����ADC DMA�ɼ� */

	
    // while (1)
    // {
    //     if (g_adc_dma_sta == 1)
    //     {
    //         /* ѭ����ʾͨ��0~ͨ��5�Ľ�� */
    //         for(j = 0; j < 6; j++)  /* ����6��ͨ�� */
    //         {
    //             sum = 0; /* ���� */
    //             for (i = 0; i < ADC_DMA_BUF_SIZE / 6; i++)  /* ÿ��ͨ���ɼ���10������,����10���ۼ� */
    //             {
    //                 sum += g_adc_dma_buf[(6 * i) + j];      /* ��ͬͨ����ת�������ۼ� */
    //             }
    //             adcx = sum / (ADC_DMA_BUF_SIZE / 6);        /* ȡƽ��ֵ */
                
    //             /* ��ʾ��� */
    //             lcd_show_xnum(108, 110 + (j * 30), adcx, 4, 12, 0, BLUE);   /* ��ʾADCC�������ԭʼֵ */

    //             temp = (float)adcx * (3.3 / 4096);  /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
    //             adcx = temp;                        /* ��ֵ�������ָ�adcx��������ΪadcxΪu16���� */
    //             lcd_show_xnum(108, 122 + (j * 30), adcx, 1, 12, 0, BLUE);   /* ��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3 */

    //             temp -= adcx;                       /* ���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111 */
    //             temp *= 1000;                       /* С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС���� */
    //             lcd_show_xnum(120, 122 + (j * 30), temp, 3, 12, 0X80, BLUE);/* ��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111. */
    //         }

    //         g_adc_dma_sta = 0;                      /* ���DMA�ɼ����״̬��־ */
    //         adc_dma_enable(ADC_DMA_BUF_SIZE);       /* ������һ��ADC DMA�ɼ� */
    //     }
	// 	// showtime();
    //     LED0_TOGGLE();
    //     delay_ms(100);
    // }
	

	// ��ͨ��ADC��DMA�ɼ�ʵ��
	//  #define ADC_DMA_BUF_SIZE        100         /* ADC DMA�ɼ� BUF��С */
	//  uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
	//  extern uint8_t g_adc_dma_sta;               /* DMA����״̬��־, 0,δ���; 1, ����� */
	//  adc_dma_init((uint32_t)&g_adc_dma_buf); /* ��ʼ��ADC DMA�ɼ� */
	// uint16_t i;
	// uint16_t adcx;
	// uint32_t sum;
	// float temp;
	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30,  70, 200, 16, 16, "ADC DMA TEST", RED);
	// lcd_show_string(30,  90, 200, 16, 16, "YSC@burningcloud", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
	// lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

	// adc_dma_enable(ADC_DMA_BUF_SIZE);   /* ����ADC DMA�ɼ� */

	// while (1)
	// {
	// 	if (g_adc_dma_sta == 1)
	// 	{
	// 	/* ����DMA �ɼ�����ADC���ݵ�ƽ��ֵ */
	// 	sum = 0;

	//         for (i = 0; i < ADC_DMA_BUF_SIZE; i++)   /* �ۼ� */
	// 		{
	// 		sum += g_adc_dma_buf[i];
	// 		}

	//         adcx = sum / ADC_DMA_BUF_SIZE;           /* ȡƽ��ֵ */

	//         /* ��ʾ��� */
	//         lcd_show_xnum(134, 110, adcx, 4, 16, 0, BLUE);      /* ��ʾADCC�������ԭʼֵ */

	//         temp = (float)adcx * (3.3 / 4096);                  /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
	//         adcx = temp;                                        /* ��ֵ�������ָ�adcx��������ΪadcxΪu16���� */
	//         lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);      /* ��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3 */

	//         temp -= adcx;                                       /* ���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111 */
	//         temp *= 1000;                                       /* С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС���� */
	//         lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);   /* ��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111. */

	//      g_adc_dma_sta = 0;                                  /* ���DMA�ɼ����״̬��־ */
	// 		adc_dma_enable(ADC_DMA_BUF_SIZE);                   /* ������һ��ADC DMA�ɼ� */
	// 	}

	// 	showtime();
	// 	LED0_TOGGLE();
	// 	delay_ms(100);
	// }

	// ��ͨ��ADC�ɼ�ʵ��
	//	adc_init();                             /* ��ʼ��ADC */
	//	 uint16_t adcx;
	//    float temp;
	//    lcd_show_string(30, 30, 200, 32, 32, "STM32", RED);
	//    lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
	//    lcd_show_string(30, 90, 200, 16, 16, "YSC@burningCloud", RED);
	//    lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
	//    lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

	//    while (1)
	//    {
	//        adcx = adc_get_result(); /* ��ȡͨ��5��ת��ֵ��10��ȡƽ�� */
	//        lcd_show_xnum(134, 110, adcx, 5, 16, 0, BLUE);   /* ��ʾADCC�������ԭʼֵ */
	//
	//        temp = (float)adcx * (3.3 / 4096);               /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
	//        adcx = temp;                                     /* ��ֵ�������ָ�adcx��������ΪadcxΪu16���� */
	//        lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);   /* ��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3 */

	//        temp -= adcx;                                    /* ���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111 */
	//        temp *= 1000;                                    /* С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС���� */
	//        lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);/* ��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111. */
	//
	//		showtime();
	//
	//
	//        LED0_TOGGLE();
	//        delay_ms(100);
	//    }
	//

	// DMA���ݴ���ʵ�飨�ڴ洫�ڴ棩
	//	extern uint8_t src_buf[10];
	//	extern uint8_t dest_buf[10];
	//	extern DMA_HandleTypeDef g_dma_handler;
	//	dma_init();
	//	while(1)
	//	{
	//		if(key1_scan())
	//		{
	//			memset(dest_buf,0,10);
	//			dma_enable_transmit(10);
	//
	//			while(1)
	//			{
	//				if(__HAL_DMA_GET_FLAG(&g_dma_handler,DMA_FLAG_TC1))
	//				{
	//					__HAL_DMA_CLEAR_FLAG(&g_dma_handler, DMA_FLAG_TC1);
	//					printf("������� \r\n");
	//					break;
	//				}
	//
	//			}
	//
	//		}
	//		LED0_TOGGLE();
	//		delay_ms(200);
	//	}

	// �͹���ʵ��
	//	uint8_t t = 0;
	//	pwr_wkup_key_init();	//�Ի��Ѽ�A0�жϳ�ʼ��
	//	pwr_pvd_init(PWR_PVDLEVEL_7);
	//	printf("Enter to LowPower Test \r\n");
	////
	//	while(1){
	//		if (key1_scan())	//���C13����
	//		{
	//			/* ����˯��ģʽ */
	//			printf("Enter SLEEP Mode \r\n");
	//			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	//			printf("Exit SLEEP Mode \r\n");
	//		}
	//		else if (key2_scan())	//���E0����
	//		{
	//			LED0(1);
	//			/* ����ֹͣģʽ */
	//			printf("Enter STOP Mode \r\n");
	//			HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	//			sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
	//			HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);	//���ö�ʱ��
	//			HAL_SuspendTick();	//ֹͣ�ж�
	//			printf("Exit STOP Mode \r\n");
	//			LED0(0);
	//		}
	//		else if (key3_scan())	//���A1����
	//		{
	//			/* �������ģʽ */
	//			printf("Enter STANDBY Mode \r\n");
	//			__HAL_RCC_PWR_CLK_ENABLE();	//��ΪҪ������Դ��ʹ�ܵ�Դʱ��
	//			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);	//ʹ��WKUP�����صĻ��ѹ���
	//			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	//������ѱ��
	//
	//			HAL_PWR_EnterSTANDBYMode();
	//			printf("�����ӡ��������Ϊ�Ƴ����������� Exit StandBy Mode \r\n");
	//		}
	//
	//			t++;
	//			delay_ms(10);
	//		if (t > 50) /* ���� LED1 ��˸, ��ʾ��������״̬ */
	//		{
	//			t = 0;
	//			LED1_TOGGLE();
	//		}
	//
	//	}

	// RTC��ʱ������ʵ��
	//	rtc_init();		//RTC��ʼ��
	////	uint8_t tbuf[40];
	//    uint8_t t = 0;
	//	/* �����ַ�����������ʾ�� */
	// char* weekdays[]={"Sunday","Monday","Tuesday","Wednesday",
	//                  "Thursday","Friday","Saterday"};
	//	rtc_set_alarm(2023, 4, 19, 10, 1, 0);  /* ����һ������������ʱ���� */
	//	printf("YSC��ʵ�������\r\n");
	////	LCD_ShowString(30, 50, 200, 16, 16, "STM32", RED);
	////    LCD_ShowString(30, 70, 200, 16, 16, "RTC TEST", RED);
	////    LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

	//	while (1)
	//    {
	//        t++;

	//        if ((t % 100) == 0)                  /* ÿ100ms����һ����ʾ���� */
	//        {
	//            rtc_get_time();
	//            printf("Time:%02d:%02d:%02d \r\n", calendar.hour, calendar.min, calendar.sec);
	//            printf("Date:%04d-%02d-%02d \r\n", calendar.year, calendar.month, calendar.date);
	//            printf("Week:%s \r\n", weekdays[calendar.week]);
	//        }

	//        if ((t % 20) == 0)
	//        {
	//            LED0_TOGGLE();              /* ÿ200ms,��תһ��LED0 */
	//        }

	//        delay_ms(10);
	//    }

	// usmart����ʵ��
	//	   while(1)
	//     {
	//         LED0(0);

	//		delay_ms(500);
	//		LED0(1);

	//		delay_ms(500);
	//    }

	// LCD����ʵ��
	//	uint8_t x = 0;
	//    uint8_t lcd_id[12];
	//	 g_point_color = RED;
	//    sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  /* ��LCD ID��ӡ��lcd_id���� */

	//    while (1)
	//    {
	//        switch (x)
	//        {
	//        case 0:
	//            lcd_clear(WHITE);
	//            break;

	//        case 1:
	//            lcd_clear(BLACK);
	//            break;

	//        case 2:
	//            lcd_clear(BLUE);
	//            break;

	//        case 3:
	//            lcd_clear(RED);
	//            break;

	//        case 4:
	//            lcd_clear(MAGENTA);
	//            break;

	//        case 5:
	//            lcd_clear(GREEN);
	//            break;

	//        case 6:
	//            lcd_clear(CYAN);
	//            break;

	//        case 7:
	//            lcd_clear(YELLOW);
	//            break;

	//        case 8:
	//            lcd_clear(BRRED);
	//            break;

	//        case 9:
	//            lcd_clear(GRAY);
	//            break;

	//        case 10:
	//            lcd_clear(LGRAY);
	//            break;

	//        case 11:
	//            lcd_clear(BROWN);
	//            break;
	//        }

	//        lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
	//        lcd_show_string(10, 80, 240, 24, 24, "TFTLCD TEST", RED);
	//        lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
	//        lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); /* ��ʾLCD ID */
	//        x++;

	//        if (x == 12)
	//            x = 0;

	//        LED0_TOGGLE(); /*�����˸*/
	//        delay_ms(1000);
	//    }

	// �߼���ʱ��PWM����ʵ��
	//		GPIO_InitTypeDef gpio_init_struct;
	//	/* �� LED0 ��������Ϊ����ģʽ, ����� PE5 ��ͻ */
	//	__HAL_RCC_GPIOD_CLK_ENABLE();
	//	gpio_init_struct.Pin = GPIO_PIN_13; /* LED0 ���� */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* ��������״̬ */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* ���� */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
	//	HAL_GPIO_Init(GPIOD, &gpio_init_struct); /* ��ʼ�� LED0 ���� */
	//	gtim_timx_pwm_chy_init(10 - 1 , 72 - 1);
	//	TIM3->CCR2 = 5;
	//	atim_timx_pwmin_chy_init();

	//	uint8_t t = 0;
	//	double ht, ct, f, tpsc;

	//	while (1)
	//	{
	//		delay_ms(10);
	//		t++;
	//		if (t >= 20) /* ÿ 200ms ���һ�ν��,����˸ LED0,��ʾ�������� */
	//		{
	//			if (g_timxchy_pwmin_sta) /* ������һ������ */
	//			{
	//				printf("\r\n"); /* �����,����һ�� */
	//				printf("PWM PSC :%d\r\n", g_timxchy_pwmin_psc); /* ��ӡ��Ƶϵ�� */
	//				printf("PWM Hight:%d\r\n", g_timxchy_pwmin_hval);/*��ӡ�ߵ�ƽ����*/
	//				printf("PWM Cycle:%d\r\n", g_timxchy_pwmin_cval);/* ��ӡ���� */
	//				/* �õ� PWM ����ʱ������ʱ�� */
	//				tpsc = ((double)g_timxchy_pwmin_psc + 1)/72;
	//				ht = g_timxchy_pwmin_hval * tpsc; /* ����ߵ�ƽʱ�� */
	//				ct = g_timxchy_pwmin_cval * tpsc; /* �������ڳ��� */
	//				f = (1 / ct) * 1000000; /* ����Ƶ�� */
	//				printf("PWM Hight time:%.3fus\r\n", ht); /* ��ӡ�ߵ�ƽ�������� */
	//				printf("PWM Cycle time:%.3fus\r\n", ct); /* ��ӡ����ʱ�䳤�� */
	//				printf("PWM Frequency :%.3fHz\r\n", f); /* ��ӡƵ�� */
	//				atim_timx_pwmin_chy_restart(); /* ���� PWM ������ */
	//			}
	//			LED1_TOGGLE(); /* LED0(RED)��˸ */
	//			t = 0;
	//		}
	//	}

	// �߼���ʱ�������������������ʵ��
	//	GPIO_InitTypeDef gpio_init_struct;
	//	/* �� LED0 ��������Ϊ����ģʽ, ����� PC6 ��ͻ */
	//	gpio_init_struct.Pin = LED0_GPIO_PIN; /* LED0 ���� */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* ��������״̬ */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* ���� */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
	//	HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* ��ʼ�� LED0 ���� */
	//	/* �� LED1 ��������Ϊ����ģʽ, ����� PC6 ��ͻ */
	//	gpio_init_struct.Pin = LED1_GPIO_PIN; /* LED0 ���� */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* ��������״̬ */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* ���� */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
	//	HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct); /* ��ʼ�� LED0 ���� */
	//
	//	atim_timx_cplm_pwm_init(10000 - 1, 7200 - 1); /* 1Mhz �ļ���Ƶ�� 1Khz ������. */
	//	atim_timx_cplm_pwm_set(3000 - 1, 1000); /* ռ�ձ�:7:3,����ʱ�� 100 * tDTS */
	//	while (1){}

	// pwm����Ƚ�ʵ��
	//	GPIO_InitTypeDef gpio_init_struct;
	//	atim_timx_comp_pwm_init(1000 - 1, 72 - 1); /* 1Mhz �ļ���Ƶ�� 1Khz ������. */
	//	/* �� LED0 ��������Ϊ����ģʽ, ����� PC6 ��ͻ */
	//	gpio_init_struct.Pin = LED0_GPIO_PIN; /* LED0 ���� */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* ��������״̬ */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* ���� */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
	//	HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* ��ʼ�� LED0 ���� */
	//	/* �� LED1 ��������Ϊ����ģʽ, ����� PC6 ��ͻ */
	//	gpio_init_struct.Pin = LED1_GPIO_PIN; /* LED0 ���� */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* ��������״̬ */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* ���� */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
	//	HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct); /* ��ʼ�� LED0 ���� */
	//	ATIM_TIMX_COMP_CH1_CCRX = 250 - 1; /* ͨ�� 1 ��λ 25% */
	//	ATIM_TIMX_COMP_CH2_CCRX = 500 - 1; /* ͨ�� 2 ��λ 50% */
	//	ATIM_TIMX_COMP_CH3_CCRX = 750 - 1; /* ͨ�� 3 ��λ 75% */
	//	ATIM_TIMX_COMP_CH4_CCRX = 1000 - 1; /* ͨ�� 4 ��λ 100% */
	//	while (1)
	//	{
	//		printf("������������...\r\n");
	//		delay_ms(10);
	//	}

	// �߼���ʱ��ָ��PWM��ʵ��
	//	uint8_t t = 0;
	//	GPIO_InitTypeDef gpio_init_struct;

	//	/* �� LED1 ��������Ϊ����ģʽ, ����� PC6 ��ͻ */
	//	gpio_init_struct.Pin = LED0_GPIO_PIN; /* LED0 ���� */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* ��������״̬ */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* ���� */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* ���� */
	//	HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* ��ʼ�� LED0 ���� */

	//	atim_timx_npwm_chy_init(5000 - 1, 7200 - 1);/*10Khz �ļ���Ƶ��,2hz �� PWM Ƶ��*/
	//
	//	atim_timx_npwm_chy_set(5); /* ��� 5 �� PWM ��(���� LED0)��˸ 5 ��) */

	//	while (1)
	//	{
	//		if (key1_scan()) /* KEY0 ���� */
	//		{
	//			atim_timx_npwm_chy_set(5); /* ��� 5 �� PWM ��(���� LED0 ��˸ 5 ��) */
	//		}
	//			t++;
	//			delay_ms(10);
	//		if (t > 50) /* ���� LED1 ��˸, ��ʾ��������״̬ */
	//		{
	//			t = 0;
	//			LED1_TOGGLE();
	//		}
	//	}
	//

	// ͨ�ö�ʱ���������ʵ��
	//	uint32_t curcnt = 0;
	// uint32_t oldcnt = 0;
	// uint8_t t = 0;
	// gtim_timx_cnt_chy_init(0); /* ��ʱ��������ʼ��, ����Ƶ */
	// gtim_timx_cnt_chy_restart(); /* �������� */
	//	while (1)
	// {
	// if (key1_scan() == 1) /* KEY1 ��������,�������� */
	// {
	// gtim_timx_cnt_chy_restart(); /* ������������ */
	// }
	// curcnt = gtim_timx_cnt_chy_get_count(); /* ��ȡ����ֵ */
	// if (oldcnt != curcnt)
	// {
	// oldcnt = curcnt;
	// printf("CNT:%d\r\n", oldcnt); /* ��ӡ������� */
	// }
	// t++;
	// if (t > 20) /* 200ms ����һ�� */
	// {
	// t = 0;
	// LED0_TOGGLE(); /* LED0 ��˸ ,��ʾ�������� */
	// }
	// delay_ms(10);
	// }

	// ����PA0�ӿڰ����ò���ʹ

	//	LED0(1); //����led0
	//	while (1)
	//	{
	//		if (key4_scan()){
	//			LED0_TOGGLE();
	//		}else
	//		{
	//			delay_ms(10);//�������10ms,��������
	//		}
	//	 }
	//
	// ͨ�ö�ʱ������/����ʵ��
	//	uint32_t temp = 0;
	//	uint8_t t = 0;
	//	gtim_timx_cap_chy_init(0XFFFF, 72-1);
	//
	//	while(1){
	//		if (g_timxchy_cap_sta & 0X80){
	//			temp = g_timxchy_cap_sta & 0X3F;
	//			temp *= 65536;
	//			temp += g_timxchy_cap_val;
	//			printf("PushTime:%d us\r\n",temp);
	//			g_timxchy_cap_sta = 0;
	//		}
	//
	//		t++;
	//		if (t > 20){
	//			t = 0;
	//			LED0_TOGGLE();
	////			printf("��������\r\n");
	//		}
	//		delay_ms(10);
	//	}
	//

	// ͨ�ö�ʱ�����ʵ��
	//	extern TIM_HandleTypeDef g_timx_pwm_chy_handle; /* ��ʱ����� */
	//	gtim_timx_pwm_chy_init(400-1,72-1); //��ʱ����ʼ��
	//
	//	uint16_t ledrpwmval = 0;
	//	uint8_t dir = 1;
	//
	//	while(1){
	//		delay_ms(7);
	//		if(dir)ledrpwmval++;
	//		else ledrpwmval--;
	//		if(ledrpwmval > 300)dir = 0;
	//		if(ledrpwmval == 10)dir = 1;
	//
	//		//�޸ıȽ�ֵ����ռ�ձ�
	//		__HAL_TIM_SET_COMPARE(&g_timx_pwm_chy_handle,TIM_CHANNEL_2,ledrpwmval);
	//
	//	}
	//

	// ����������ʵ��
	//	btim_timx_int_init(5000 - 1, 7200 - 1); /* 10Khz �ļ���Ƶ�ʣ����� 5K ��Ϊ 500ms */
	//	 while (1)
	//	 {
	//		 LED0_TOGGLE();
	//		 delay_ms(200);
	//	 }

	//	//���ڿ��Ź�ʵ��
	//	LED0(1);
	//	delay_ms(3000);
	//	wwdg_init(0x7F, 0x5F, WWDG_PRESCALER_8);
	//	while(1){
	//		LED0(0);
	//		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 1){
	////		delay_ms(10); //�������10ms,��������
	//			wwdg_feed();
	//		}
	//	}

	//	�������Ź�ʵ��
	//	delay_ms(200); /* ��ʱ 200ms �ٳ�ʼ�����Ź�,LED0 �ı仯"�ɼ�" */
	//
	//	iwdg_init(IWDG_PRESCALER_64,625); /* Ԥ��Ƶ�� 64,����ֵΪ 625,���ʱ��ԼΪ 1s */
	//	LED0(1); /* ���� LED0(���) */
	//	 //printf("led0");
	//	 while (1)
	//	 {
	//		 if (key1_scan() == 1) /* ��� K1 ����,��ι�� */
	//		 {
	//			iwdg_feed(); /* ι�� */
	//		 }else{
	//			delay_ms(10);
	//		 }

	//	 }

	//	uint8_t len;
	//	uint16_t times = 0;
	//	printf("����������,�Իس�������\r\n");
	//	while (1)
	//	 {
	//	 if (g_usart_rx_sta & 0x8000) /* ���յ�������? */
	//	 {
	//		 len = g_usart_rx_sta & 0x3fff; /* �õ��˴ν��յ������ݳ��� */
	//		 printf("\r\n �����͵���ϢΪ:\r\n");
	//		/*���ͽ��յ�������	Sends an amount of data in blocking mode.*/
	//		 HAL_UART_Transmit(&g_uart1_handle,(uint8_t*)g_usart_rx_buf,len,1000);
	//		/*�ȴ����ͽ���*/
	//		while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC)!=SET){};
	//		 printf("\r\n\r\n"); /* ���뻻�� */
	//		 g_usart_rx_sta = 0;
	//	 }
	//	 else
	//	 {
	//		 times++;
	//		 if (times % 5000 == 0)
	//		 {
	//			 printf("\r\n STM32 ������ ����ʵ�飺\r\n");
	//			 printf("50���ӡһ��\r\n\r\n\r\n");
	//		 }
	//		 if (times % 500 == 0) printf("����������,�Իس�������\r\n");  //�����ӡһ��
	//		 if (times % 30 == 0) LED0_TOGGLE(); /* 0.3����˸һ�� LED,��ʾϵͳ��������. */
	//		 delay_ms(10);
	//	 }
	//	 }

	//	key_init();							//KEY��ʼ��
	//	exti_init();						//EXTI��ʼ��
	//	//�ⲿ�ж�ʵ��
	//	/*
	//	ͨ���ⲿ�жϵķ�ʽ�ÿ������ϵ����������������� LED �ƣ�KEY0 ���� LED0 ��ת��KEY1 ���� LED1 ��ת��
	//	*/
	//	LED0(1); //����led0
	//	while (1)
	//	{
	//		printf("OK\r\n");
	//		delay_ms(1000);
	//	 }

	// ������ʵ��
	//	   while(1)
	//     {
	//         LED0(0);
	//		LED1(1);
	//		delay_ms(500);
	//		LED0(1);
	//		LED1(0);
	//		delay_ms(500);
	//     }
	// ����ʵ��
	//	while(1){
	//		if(key1_long_scan()){
	//			LED0_TOGGLE();
	//		}
	//		else if(key2_scan())
	//			{
	//				LED1_TOGGLE();
	//			}
	//		else
	//		{
	//			delay_ms(10);//�������10ms,��������
	//		}
	//	}
}