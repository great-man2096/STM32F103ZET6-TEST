/**
 ******************************************************************************
 * @file     main.c
 * @author   ����ԭ���Ŷ�(ALIENTEK)
 * @version  V1.0
 * @date     2020-08-20
 * @brief    �½�����ʵ��-HAL��汾 ʵ��
 * @license  Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
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
#include "math.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
// #include "./BSP/EXTI/exti.h"
#include "./BSP/WDG/wdg.h"
// #include "./BSP/TIMER/btim.h"
// #include "./BSP/TIMER/gtime.h"
#include "./BSP/TIMER/atim.h"
#include "./BSP/LCD/lcd.h"
#include "./USMART/usmart.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/PWR/pwr.h"
#include "./BSP/DMA/dma.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/DAC/dac.h"
#include "./BSP/PWMDAC/pwmdac.h"
#include "./BSP/24CXX/24cxx.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/CAN/can.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/NRF24L01/nrf24l01.h"
#include "./BSP/STMFLASH/stmflash.h"
#include "./BSP/SRAM/sram.h"
#include "./MALLOC/malloc.h"
#include "./BSP/SDIO/sdio_sdcard.h"

//	extern uint8_t g_timxchy_cap_sta; //���벶��״̬
//	extern uint16_t g_timxchy_cap_val; //���벶��ֵ
//	extern uint8_t g_timxchy_pwmin_sta; /* pwm����״̬ */
//	extern uint16_t g_timxchy_pwmin_psc; /* pwm�����Ƶϵ�� */
//	extern uint16_t g_timxchy_pwmin_hval;	/* pwm����ߵ�ƽ���� */
//	extern uint16_t g_timxchy_pwmin_cval;	/* pwm�������ڿ�� */

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
}

// ��ͨ��ADC��������12ת16�ֱ��ʣ��õ��Ĳ��������������main����������ʧ��
//  #define ADC_OVERSAMPLE_TIMES    256                         /* ADC����������, �������4bit�ֱ���, ��Ҫ256������ */
//  #define ADC_DMA_BUF_SIZE        ADC_OVERSAMPLE_TIMES * 10   /* ADC DMA�ɼ� BUF��С, Ӧ���ڹ����������������� */
//  uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];                   /* ADC DMA BUF */
//  extern uint8_t g_adc_dma_sta;                               /* DMA����״̬��־, 0,δ���; 1, ����� */
// extern uint16_t g_dac_sin_buf[4096];
/* Ҫд�뵽FLASH���ַ������� */
// const uint8_t g_text_buf[] = {"STM32 SPI TEST"};	//const,��ʾ����һ�����ɱ�ĳ���

// #define TEXT_SIZE sizeof(g_text_buf) /* TEXT�ַ������� */
// /* Ҫд�뵽STM32 FLASH���ַ������� */
// const uint8_t g_text_buf[] = {"STM32 FLASH TEST"};
// #define TEXT_LENTH sizeof(g_text_buf) /* ���鳤�� */
// /*SIZE��ʾ���ֳ�(2�ֽ�), ��С������2��������, ������ǵĻ�, ǿ�ƶ��뵽2�������� */
// #define SIZE TEXT_LENTH / 2 + ((TEXT_LENTH % 2) ? 1 : 0)
// #define FLASH_SAVE_ADDR 0X08010000 /* ����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С��Code+RO-data+RW-data�� + 0X08000000) */

// const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN ", " SRAMEX "};

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
	/******************************************************
	SD��ʵ��
	*******************************************************/
	uint8_t t = 0;

	my_mem_init(SRAMIN);                /* ��ʼ���ڲ�SRAM�ڴ�� */

	lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30,  70, 200, 16, 16, "SD TEST", RED);
    lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Read Sector 0", RED);

    while (sd_init()) /* ��ⲻ��SD�� */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SD Card Error!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE(); /* �����˸ */
    }

    /* ��ӡSD�������Ϣ */
    show_sdcard_info();

    /* ���SD���ɹ� */
    lcd_show_string(30, 130, 200, 16, 16, "SD Card OK    ", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "SD Card Size:     MB", BLUE);
    lcd_show_num(30 + 13 * 8, 150, SD_TOTAL_SIZE_MB(&g_sdcard_handler), 5, 16, BLUE); /* ��ʾSD������ */

    while (1)
    {

        if (key1_scan())   /* KEY0������ */
        {
            sd_test_read(0, 1); /* ��0������ȡ1*512�ֽڵ����� */
        }

        t++;
        delay_ms(10);

        if (t == 20)
        {
            LED0_TOGGLE(); /* �����˸ */
            t = 0;
        }
    }

	/******************************************************
	�ڴ����ʵ��
	*******************************************************/	
	// uint8_t paddr[20];  /* ���P Addr:+p��ַ��ASCIIֵ */
    // uint16_t memused = 0;
    // uint8_t i = 0;
    // uint8_t *p = 0;
    // uint8_t *tp = 0;
    // uint8_t sramx = 0;  /* Ĭ��Ϊ�ڲ�sram */
	// // sram_init();                        /* SRAM��ʼ�� */
    // my_mem_init(SRAMIN);                /* ��ʼ���ڲ�SRAM�ڴ�� */
    // // my_mem_init(SRAMEX);                /* ��ʼ���ⲿSRAM�ڴ�� */

    // lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    // lcd_show_string(30,  70, 200, 16, 16, "MALLOC TEST", RED);
    // lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    // lcd_show_string(30, 110, 200, 16, 16, "KEY0:Malloc & WR & Show", RED);
    // lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:SRAMx KEY1:Free", RED);
    // lcd_show_string(60, 160, 200, 16, 16, " SRAMIN ", BLUE);
    // lcd_show_string(30, 176, 200, 16, 16, "SRAMIN   USED:", BLUE);
    // lcd_show_string(30, 192, 200, 16, 16, "SRAMEX   USED:", BLUE);

	// while (1)
	// {
	// 	if (key2_scan()) /* KEY0���� */
	// 	{
	// 		printf("E0����\r\n");
	// 		p = mymalloc(sramx, 2048); /* ����2K�ֽ�,��д������,��ʾ��lcd��Ļ���� */
	// 		printf("P Addr:0X%08X\r\n", p);
	// 		if (p != NULL)
	// 		{
	// 			printf("p != null\r\n");
	// 			sprintf((char *)p, "Memory Malloc Test%03d", i);		/* ��pд��һЩ���� */
	// 			lcd_show_string(30, 260, 209, 16, 16, (char *)p, BLUE); /* ��ʾP������ */
	// 			printf("��ʾP�����ݽ���\r\n");
	// 		}
	// 	}

	// 	if (key1_scan()) /* KEY1���� */
	// 	{
	// 		printf("C13����\r\n");
	// 		myfree(sramx, p); /* �ͷ��ڴ� */
	// 		p = 0;			  /* ָ��յ�ַ */
	// 	}

	// 	if (key4_scan()) /* KEY UP���� */
	// 	{
	// 		printf("A0����\r\n");
	// 		sramx++;

	// 		if (sramx > 1)
	// 			sramx = 0;

	// 		lcd_show_string(60, 160, 200, 16, 16, (char *)SRAM_NAME_BUF[sramx], BLUE);
	// 	}

	// 	if (tp != p)
	// 	{
	// 		tp = p;
	// 		sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)tp);
	// 		lcd_show_string(30, 240, 209, 16, 16, (char *)paddr, BLUE); /* ��ʾp�ĵ�ַ */

	// 		if (p)
	// 		{
	// 			lcd_show_string(30, 260, 280, 16, 16, (char *)p, BLUE); /* ��ʾP������ */
	// 		}
	// 		else
	// 		{
	// 			lcd_fill(30, 260, 209, 296, WHITE); /* p=0,�����ʾ */
	// 		}
	// 	}

	// 	delay_ms(10);
	// 	i++;

	// 	if ((i % 20) == 0)
	// 	{
	// 		memused = my_mem_perused(SRAMIN);
	// 		sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	// 		lcd_show_string(30 + 112, 176, 200, 16, 16, (char *)paddr, BLUE); /* ��ʾ�ڲ��ڴ�ʹ���� */

	// 		// memused = my_mem_perused(SRAMEX);
	// 		// sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	// 		// lcd_show_string(30 + 112, 192, 200, 16, 16, (char *)paddr, BLUE); /* ��ʾTCM�ڴ�ʹ���� */

	// 		LED0_TOGGLE(); /* LED0��˸ */
	// 	}
	// }

	/******************************************************
	stmflashʵ��
	*******************************************************/
    // uint16_t i = 0;
	// uint8_t datatemp[SIZE];

	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    // lcd_show_string(30,  70, 200, 16, 16, "FLASH EEPROM TEST", RED);
    // lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    // lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);

    // while (1)
    // {

    //     if (key2_scan()) /* KEY1����,д��STM32 FLASH */
    //     {
    //         lcd_fill(0, 150, 239, 319, WHITE); /* ������� */
    //         lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", RED);
    //         stmflash_write(FLASH_SAVE_ADDR, (uint16_t *)g_text_buf, SIZE);
    //         lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", RED); /* ��ʾ������� */
    //     }

    //     if (key1_scan()) /* KEY0����,��ȡ�ַ�������ʾ */
    //     {
    //         lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH.... ", RED);
    //         stmflash_read(FLASH_SAVE_ADDR, (uint16_t *)datatemp, SIZE);
    //         lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", RED); /* ��ʾ������� */
    //         lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);       /* ��ʾ�������ַ��� */
    //     }

    //     i++;
    //     delay_ms(10);

    //     if (i == 20)
    //     {
    //         LED0_TOGGLE(); /* ��ʾϵͳ�������� */
    //         i = 0;
    //     }
    // }

	/******************************************************
	2.4G����ͨѶʵ��
	*******************************************************/
	// uint8_t key, mode;
    // uint16_t t = 0;
    // uint8_t tmp_buf[33];
	// nrf24l01_init();                    /* ��ʼ��NRF24L01 */

    // lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    // lcd_show_string(30, 70, 200, 16, 16, "NRF24L01 TEST", RED);
    // lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    // while (nrf24l01_check()) /* ���NRF24L01�Ƿ����� */
    // {
    //     lcd_show_string(30, 110, 200, 16, 16, "NRF24L01 Error", RED);
    //     delay_ms(200);
    //     lcd_fill(30, 110, 239, 130 + 16, WHITE);
    //     delay_ms(200);
    // }

    // lcd_show_string(30, 110, 200, 16, 16, "NRF24L01 OK", RED);

    // while (1) /* �����û�ѡ��ģʽ */
    // {

    //     if (key2_scan())
    //     {
    //         mode = 0; /* ����ģʽ */
    //         break;
    //     }
    //     else if (key1_scan())
    //     {
    //         mode = 1; /* ����ģʽ */
    //         break;
    //     }

    //     t++;

    //     if (t == 100) /* ��ʾ��ʾ��Ϣ */
    //     {
    //         lcd_show_string(10, 130, 230, 16, 16, "key_E0:RX_Mode  key_C13:TX_Mode", RED);
    //     }

    //     if (t == 200) /* �ر���ʾ��Ϣ */
    //     {
    //         lcd_fill(10, 130, 230, 150 + 16, WHITE);
    //         t = 0;
    //     }

    //     delay_ms(5);
    // }

    // lcd_fill(10, 130, 240, 166, WHITE); /* ����������ʾ */

    // if (mode == 0) /* RXģʽ */
    // {
    //     lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 RX_Mode", BLUE);
    //     lcd_show_string(30, 150, 200, 16, 16, "Received DATA:", BLUE);
    //     nrf24l01_rx_mode(); /* ����RXģʽ */

    //     while (1)
    //     {
    //         if (nrf24l01_rx_packet(tmp_buf) == 0) /* һ�����յ���Ϣ,����ʾ����. */
    //         {
    //             tmp_buf[32] = 0; /* �����ַ��������� */
    //             lcd_show_string(0, 170, lcddev.width - 1, 32, 16, (char *)tmp_buf, BLUE);
    //         }
    //         else
    //             delay_us(100);

    //         t++;

    //         if (t == 10000) /* ��Լ1s�Ӹı�һ��״̬ */
    //         {
    //             t = 0;
    //             LED0_TOGGLE();
    //         }
    //     }
    // }
    // else /* TXģʽ */
    // {
    //     lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 TX_Mode", BLUE);
    //     nrf24l01_tx_mode(); /* ����TXģʽ */
    //     mode = ' ';         /* �ӿո����ʼ���� */

    //     while (1)
    //     {
    //         if (nrf24l01_tx_packet(tmp_buf) == 0) /* ���ͳɹ� */
    //         {
    //             lcd_show_string(30, 150, 239, 32, 16, "Sended DATA:", BLUE);
    //             lcd_show_string(0, 170, lcddev.width - 1, 32, 16, (char *)tmp_buf, BLUE);
    //             key = mode;

    //             for (t = 0; t < 32; t++)
    //             {
    //                 key++;

    //                 if (key > ('~'))
    //                     key = ' ';

    //                 tmp_buf[t] = key;
    //             }

    //             mode++;

    //             if (mode > '~')
    //                 mode = ' ';

    //             tmp_buf[32] = 0; /* ��������� */
    //         }
    //         else
    //         {
    //             lcd_fill(0, 150, lcddev.width, 170 + 16 * 3, WHITE); /* �����ʾ */
    //             lcd_show_string(30, 150, lcddev.width - 1, 32, 16, "Send Failed ", BLUE);
    //         }

    //         LED0_TOGGLE();
    //         delay_ms(200);
    //     }
    // }


	/******************************************************
	���败����ʵ��
	*******************************************************/
	// tp_dev.init();                          /* ��������ʼ�� */
	// lcd_show_string(30, 30, 200, 32, 32, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "YSC@burningCloud", RED);

    // if (tp_dev.touchtype != 0XFF)
    // {
    //     lcd_show_string(30, 110, 200, 16, 16, "Press KEY0 to Adjust", RED); /* ����������ʾ */
    // }

    // delay_ms(1500);
    // load_draw_dialog();

    // if (tp_dev.touchtype & 0X80)
    // {
    //     ctp_test(); /* ���������� */
    // }
    // else
    // {
    //     rtp_test(); /* ���������� */
    // }

	

	/******************************************************
	CANʵ��
	*******************************************************/
	// can_init();
    // uint8_t i = 0, t = 0;
    // uint8_t cnt = 0;
    // uint8_t canbuf[8];
    // uint8_t rec_len = 0;
    // uint8_t rec_buf[8];

	// while(1) {
	// 	if (key2_scan()) /* KEY1����,д�� */
	// 	{
	// 		for(i=0; i< 8;i++)
	// 		{
	// 			canbuf[i] = i+cnt;
	// 		}
	// 		can_send_msg(0x12345678,canbuf, 4);
	// 	}
	// 	rec_len = can_receive_msg(rec_buf);
	// 	if (rec_len)
	// 	{
	// 		for (uint8_t x = 0; x < rec_len; x++)
	// 		{
	// 			printf("%X ", rec_buf[x]);
	// 		}
	// 		printf("\r\n");
	// 	}

	// 	t++;
    //     delay_ms(10);

    //     if (t == 20)
    //     {
    //         LED0_TOGGLE(); /* ��ʾϵͳ�������� */
    //         t = 0;
    //         cnt++;
    //     }
	// }

	/******************************************************
	SPIʵ��
	*******************************************************/
	// uint16_t i = 0;
	// uint8_t datatemp[TEXT_SIZE];
	// uint32_t flashsize;
	// uint16_t id = 0;
	// char idinfo[30];
	// norflash_init(); /* ��ʼ��NORFLASH */

	// lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "SPI TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED); /* ��ʾ��ʾ��Ϣ */

	// id = norflash_read_id(); /* ��ȡFLASH ID */

	// while ((id == 0) || (id == 0XFFFF)) /* ��ⲻ��FLASHоƬ */
	// {
	// 	lcd_show_string(30, 130, 200, 16, 16, "FLASH Check Failed!", RED);
	// 	delay_ms(500);
	// 	lcd_show_string(30, 130, 200, 16, 16, "Please Check!      ", RED);
	// 	delay_ms(500);
	// 	LED0_TOGGLE(); /* LED0��˸ */
	// }

	// sprintf(idinfo,"SPI ID:%x FLASH Ready!", id);
	// lcd_show_string(30, 130, 200, 16, 16, idinfo, BLUE);
	// flashsize = 8 * 1024 * 1024; /* FLASH ��СΪ8M�ֽ� */

	// while (1)
	// {
	// 	if (key2_scan()) /* KEY1����,д�� */
	// 	{
	// 		lcd_fill(0, 150, 239, 319, WHITE); /* ������� */
	// 		lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", BLUE);
	// 		sprintf((char *)datatemp, "%s%d", (char *)g_text_buf, i);
	// 		norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);	  /* �ӵ�����100����ַ����ʼ,д��SIZE���ȵ����� */
	// 		lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", BLUE); /* ��ʾ������� */
	// 	}

	// 	if (key1_scan()) /* KEY0����,��ȡ�ַ�������ʾ */
	// 	{
	// 		lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH... . ", BLUE);
	// 		norflash_read(datatemp, flashsize - 100, TEXT_SIZE);				   /* �ӵ�����100����ַ����ʼ,����SIZE���ֽ� */
	// 		lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:   ", BLUE); /* ��ʾ������� */
	// 		lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);		   /* ��ʾ�������ַ��� */
	// 	}

	// 	i++;

	// 	if (i == 20)
	// 	{
	// 		LED0_TOGGLE(); /* LED0��˸ */
	// 		i = 0;
	// 	}

	// 	delay_ms(10);
	// }

	/******************************************************
	IICʵ��
	*******************************************************/
	// at24cxx_init();

	// /* Ҫд�뵽24c02���ַ������� */
	// const uint8_t g_text_buf[] = {"STM32 IIC TEST"};
	// #define TEXT_SIZE       sizeof(g_text_buf)  /* TEXT�ַ������� */
	// uint16_t i = 0;
	// uint8_t datatemp[TEXT_SIZE];
	// lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "IIC TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);    /* ��ʾ��ʾ��Ϣ */
	// while (at24cx x_check()) /* ��ⲻ��24c02 */
	// {
	//     lcd_show_string(30, 130, 200, 16, 16, "24C02 Check Failed!", RED);
	//     lcd_show_string(30, 146, 200, 16, 16, "Please Check!      ", RED);
	//     delay_ms(500);
	//     LED1_TOGGLE();      /* �����˸ */
	// }

	// lcd_show_string(30, 130, 200, 16, 16, "24C02 Ready!", RED);

	// while (1)
	// {

	// 	if (key2_scan())
	// 	{
	// 		lcd_fill(0, 150, 239, 319, WHITE);  /* ������� */
	//         lcd_show_string(30, 150, 200, 16, 16, "Start Write 24C02....", BLUE);
	//         at24cxx_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
	//         lcd_show_string(30, 150, 200, 16, 16, "24C02 Write Finished!", BLUE);   /* ��ʾ������� */
	// 	}
	// 	else if (key1_scan())
	// 	{
	// 		lcd_show_string(30, 150, 200, 16, 16, "Start Read 24C02.... ", BLUE);
	//         at24cxx_read(0, datatemp, TEXT_SIZE);
	//         lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", BLUE);   /* ��ʾ������� */
	//         lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);          /* ��ʾ�������ַ��� */
	// 	}
	// 	i++;

	//     if (i == 20)
	//     {
	//         LED0_TOGGLE();  /* �����˸ */
	//         i = 0;
	//     }

	//     delay_ms(10);
	// }

	/******************************************************
	DACʵ��,PWMDACʵ��
	*******************************************************/
	// extern TIM_HandleTypeDef g_tim1_handler;
	// uint16_t adcx;
	// float temp;
	// uint8_t t = 0;
	// uint16_t pwmval = 0;
	// adc_init();
	// pwmdac_init(256 - 1, 0); /* PWM DAC ��ʼ��, Fpwm = 72M/256 =281.25Khz */
	// lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "PWM DAC TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY_UP:+ KEY1:-", RED);
	// lcd_show_string(30, 130, 200, 16, 16, "PWM VAL:", BLUE);
	// lcd_show_string(30, 150, 200, 16, 16, "DAC VOL:0.000V", BLUE);
	// lcd_show_string(30, 170, 200, 16, 16, "ADC VOL:0.000V", BLUE);
	// while (1)
	// {
	// 	t++;
	// 	if (key1_scan()) /* PWM ռ�ձȵ��� */
	// 	{
	// 		if (pwmval < 3200) /* ��Χ�޶� */
	// 		{
	// 			pwmval += 100;
	// 		}
	// 		/* ����µ� PWM ռ�ձ� */
	// 		pwmdac_set_voltage(pwmval);
	// 	}
	// 	else if (key2_scan()) /* PWM ռ�ձȵ��� */
	// 	{
	// 		if (pwmval > 100) /* ��Χ�޶� */
	// 		{
	// 			pwmval -= 100;
	// 		}
	// 		else
	// 		{
	// 			pwmval = 0;
	// 		}
	// 		/* ����µ� PWM ռ�ձ� */
	// 		pwmdac_set_voltage(pwmval);
	// 	}
	// 	if (t == 10 || key1_scan() || key2_scan())
	// 	{	/* WKUP / KEY1 ������, ���߶�ʱʱ�䵽�� */
	// 		/* PWM DAC ��ʱ������Ƚ�ֵ */
	// 		// adcx = __HAL_TIM_GET_COMPARE(&g_tim1_handler, PWMDAC_TIMX_CHY);
	// 		adcx = PWMDAC_TIMX_CCRX;
	// 		lcd_show_xnum(94, 130, adcx, 3, 16, 0, BLUE); /* ��ʾ CCRX �Ĵ���ֵ */
	// 		temp = (float)adcx * (3.3 / 256);			  /* �õ� DAC ��ѹֵ */
	// 		adcx = temp;
	// 		lcd_show_xnum(94, 150, temp, 1, 16, 0, BLUE); /* ��ʾ��ѹֵ�������� */
	// 		temp -= adcx;
	// 		temp *= 1000;
	// 		lcd_show_xnum(110, 150, temp, 3, 16, 0X80, BLUE); /* ��ѹֵ��С������ */
	// 		adcx = adc_get_result();	  /* ADC3 ͨ�� 1 ��ת����� */
	// 		temp = (float)adcx * (3.3 / 4096);				  /* �õ� ADC ��ѹֵ(adc �� 12bit ��) */
	// 		adcx = temp;
	// 		lcd_show_xnum(94, 170, temp, 1, 16, 0, BLUE); /* ��ʾ��ѹֵ�������� */
	// 		temp -= adcx;
	// 		temp *= 1000;
	// 		lcd_show_xnum(110, 170, temp, 3, 16, 0X80, BLUE); /* ��ѹֵ��С������ */
	// 		LED0_TOGGLE();									  /* LED0 ��˸ */
	// 		t = 0;
	// 	}
	// 	delay_ms(10);
	// }

	/******************************************************
	DACʵ��,���Ҳ�ʵ��
	*******************************************************/
	// adc_init();

	// lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "DAC DMA Sine WAVE TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY0:3Khz  KEY1:30Khz", RED);

	// lcd_show_string(30, 130, 200, 16, 16, "DAC VAL:", BLUE);
	// lcd_show_string(30, 150, 200, 16, 16, "DAC VOL:0.000V", BLUE);
	// lcd_show_string(30, 170, 200, 16, 16, "ADC VOL:0.000V", BLUE);

	// uint16_t adcx;
	// float temp;
	// uint8_t t = 0;
	// dac_dma_wave_init();
	// dac_creat_sin_buf(2048, 100);
	// dac_dma_wave_enable(100, 10 - 1, 72 - 1);
	// while (1)
	// {
	// 	t++;
	// 	if (key1_scan()) /* �߲����� , Լ1Khz���� */
	// 	{
	// 		dac_creat_sin_buf(2048, 100);
	// 		dac_dma_wave_enable(100, 10 - 1, 24 - 1); /* 300Khz����Ƶ��, 100����, �õ����3KHz�����Ҳ�. */
	// 	}
	// 	else if (key2_scan()) /* �Ͳ����� , Լ1Khz���� */
	// 	{
	// 		dac_creat_sin_buf(2048, 10);
	// 		dac_dma_wave_enable(10, 10 - 1, 24 - 1); /* 300Khz����Ƶ��, 10����, ���Եõ����30KHz�����Ҳ�. */
	// 	}

	// 	adcx = DAC1->DHR12R1;						  /* ��ȡDAC1_OUT1�����״̬ */
	// 	lcd_show_xnum(94, 130, adcx, 4, 16, 0, BLUE); /* ��ʾDAC�Ĵ���ֵ */

	// 	temp = (float)adcx * (3.3 / 4096); /* �õ�DAC��ѹֵ */
	// 	adcx = temp;
	// 	lcd_show_xnum(94, 150, temp, 1, 16, 0, BLUE); /* ��ʾ��ѹֵ�������� */

	// 	temp -= adcx;
	// 	temp *= 1000;
	// 	lcd_show_xnum(110, 150, temp, 3, 16, 0X80, BLUE); /* ��ʾ��ѹֵ��С������ */

	// 	adcx = adc_get_result();		   /* �õ�ADC1ͨ��2��ת����� */
	// 	temp = (float)adcx * (3.3 / 4096); /* �õ�ADC��ѹֵ(adc��12bit��) */
	// 	adcx = temp;
	// 	lcd_show_xnum(94, 170, temp, 1, 16, 0, BLUE); /* ��ʾ��ѹֵ�������� */

	// 	temp -= adcx;
	// 	temp *= 1000;
	// 	lcd_show_xnum(110, 170, temp, 3, 16, 0X80, BLUE); /* ��ʾ��ѹֵ��С������ */

	// 	if (t == 40) /* ��ʱʱ�䵽�� */
	// 	{
	// 		LED0_TOGGLE(); /* LED0��˸ */
	// 		t = 0;
	// 	}

	// 	delay_ms(5);
	// }

	/******************************************************
	DACʵ��,���ǲ�ʵ��
	*******************************************************/
	// dac_init(1);                        /* ��ʼ��DAC1_OUT1ͨ�� */
	// uint8_t t = 0;
	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30,  70, 200, 16, 16, "DAC Triangular WAVE TEST", RED);
	// lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY0:Wave1  KEY1:Wave2", RED);
	// lcd_show_string(30, 130, 200, 16, 16, "DAC None", BLUE); /* ��ʾ����� */

	// while (1)
	// {
	//     t++;

	//     if (key1_scan())                        /* �߲����� , Լ1Khz���� */
	//     {
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC Wave1 ", BLUE);
	//         dac_triangular_wave(4095, 5, 2000, 100); /* ��ֵ4095, ��������5us, 200��������, 100������ */
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC None  ", BLUE);
	//     }
	//     else if (key2_scan())                   /* �Ͳ����� , Լ1Khz���� */
	//     {
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC Wave2 ", BLUE);
	//         dac_triangular_wave(4095, 500, 20, 100); /* ��ֵ4095, ��������500us, 20��������, 100������ */
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC None  ", BLUE);
	//     }

	//     if (t == 10)                                 /* ��ʱʱ�䵽�� */
	//     {
	//         LED0_TOGGLE();                           /* LED0��˸ */
	//         t = 0;
	//     }

	//     delay_ms(10);
	// }

	/******************************************************
	DACʵ��,ͨ���������������ѹ�����Ҳɼ���ѹ����ʾ������ʾ
	*******************************************************/
	// adc_init();                             /* ��ʼ��ADC */
	// dac_init(1);								/* ��ʼ��DAC */
	// uint16_t adcx;
	// float temp;
	// uint16_t dacval = 0;

	// dac_set_voltage(1,3300); //1222

	// lcd_show_string(30, 30, 200, 32, 32, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "YSC@burningCloud", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH0_VAL:", BLUE);
	// lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH0_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

	// while (1)
	// {
	// 	if (key1_scan())
	//     {
	//         if (dacval < 4000)dacval += 200;

	//         dac_set_voltage(2,dacval);/* �������200 */
	// 		printf("PA5��ѹ����Ϊ%d\r\n",dacval);
	//     }
	//     else if (key2_scan())
	//     {
	//         if (dacval > 200)dacval -= 200;
	//         else dacval = 0;

	//         dac_set_voltage(2,dacval); /* �������200 */
	// 		printf("PA5��ѹ����Ϊ%d\r\n",dacval);
	//     }
	// 	adcx = adc_get_result(); /* ��ȡͨ��0��ת��ֵ��10��ȡƽ�� */
	// 	lcd_show_xnum(134, 110, adcx, 5, 16, 0, BLUE);   /* ��ʾADCC�������ԭʼֵ */

	// 	temp = (float)adcx * (3.3 / 4096);               /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
	// 	adcx = temp;                                     /* ��ֵ�������ָ�adcx��������ΪadcxΪu16���� */
	// 	lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);   /* ��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3 */

	// 	temp -= adcx;                                    /* ���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111 */
	// 	temp *= 1000;                                    /* С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС���� */
	// 	lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);/* ��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111. */

	// 	showtime();

	// 	LED0_TOGGLE();
	// 	delay_ms(100);
	// }

	// �ڲ��¶ȴ�����ʵ��
	//  short temp;
	//  adc_temperature_init();                     /* ��ʼ��ADC�ڲ��¶ȴ������ɼ� */

	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30,  70, 200, 16, 16, "Temperature TEST", RED);
	// lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 120, 200, 16, 16, "TEMPERATE: 00.00C", BLUE);

	// while (1)
	// {

	//     temp = adc_get_temperature();   /* �õ��¶�ֵ */

	//     if (temp < 0)
	//     {
	//         temp = -temp;
	//         lcd_show_string(30 + 10 * 8, 120, 16, 16, 16, "-", BLUE);   /* ��ʾ���� */
	//     }
	//     else
	//     {
	//         lcd_show_string(30 + 10 * 8, 120, 16, 16, 16, " ", BLUE);   /* �޷��� */
	//     }
	//     lcd_show_xnum(30 + 11 * 8, 120, temp / 100, 2, 16, 0, BLUE);    /* ��ʾ�������� */
	//     lcd_show_xnum(30 + 14 * 8, 120, temp % 100, 2, 16, 0X80, BLUE); /* ��ʾС������ */

	// 	showtime();	//��ʾʱ��
	//     LED0_TOGGLE();  /* LED0��˸,��ʾ�������� */
	//     delay_ms(250);
	// }

	// ��ͨ��ADC��������12ת16�ֱ��ʣ�
	/* ADC����������, ������ADC��βɼ��ķ�ʽ, �����ADC����, �����ٶ�ÿ���4��
	 * ����������� 1bit, ͬʱ, ADC�����ٶȽ���4��, �����4bit����, ��Ҫ256�βɼ�
	 * ���ܵó�1������, �൱��ADC�ٶ�����256��. ������ֻҪADC�㹻��, ���ǿ�������
	 * ���ADC����, ��ʵ����ADC���������޿��, ��������ADC��������, ������λ������
	 * ��߽����Խ��, ��Ҫ�����Լ���ʵ�������ADC��ʵ��������Ȩ��.
	 */

	// 	uint16_t i;
	// 	uint32_t adcx;
	// 	uint32_t sum;
	// 	float temp;

	// 	adc_over_dma_init((uint32_t)&g_adc_dma_buf);     			/* ��ʼ��ADC DMA�ɼ� */

	// 	lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// 	lcd_show_string(30,  70, 200, 16, 16, "ADC OverSample TEST", RED);
	// 	lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// 	lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
	// 	lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE); /* ���ڹ̶�λ����ʾС���� */

	//    adc_dma_enable(ADC_DMA_BUF_SIZE);           /* ����ADC DMA�ɼ� */

	// 	while (1)
	// 	{
	// 		if (g_adc_dma_sta == 1)
	// 		{
	//            /* ����DMA �ɼ�����ADC���ݵ�ƽ��ֵ */
	// 			sum = 0;

	//            for (i = 0; i < ADC_DMA_BUF_SIZE; i++)   /* �ۼ� */
	// 			{
	// 			sum += g_adc_dma_buf[i];
	// 			}

	//            adcx = sum / (ADC_DMA_BUF_SIZE / ADC_OVERSAMPLE_TIMES); /* ȡƽ��ֵ */
	//            adcx >>= 4;   /* ����2^4��, �õ�12+4λ ADC����ֵ, ע��: ��� N bit����, ��Ҫ >> N */

	//            /* ��ʾ��� */
	//            lcd_show_xnum(134, 110, adcx, 5, 16, 0, BLUE);      /* ��ʾADCC�������ԭʼֵ */

	//            temp = (float)adcx * (3.3 / 65536);                 /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
	//            adcx = temp;                                        /* ��ֵ�������ָ�adcx��������ΪadcxΪu16���� */
	//            lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);      /* ��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3 */

	//            temp -= adcx;                                       /* ���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111 */
	//            temp *= 1000;                                       /* С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС���� */
	//            lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);   /* ��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111. */

	//            g_adc_dma_sta = 0;                                  /* ���DMA�ɼ����״̬��־ */
	//            adc_dma_enable(ADC_DMA_BUF_SIZE);                   /* ������һ��ADC DMA�ɼ� */
	// 		}

	// 		LED0_TOGGLE();
	// 		delay_ms(100);
	// 	}

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
	//				printf("PWM Hight time:%.3fus\r\n", ht); /* ��ӡ�ߵ�ƽ������ */
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

	// �����ʵ��
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
