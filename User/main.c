/**
 ******************************************************************************
 * @file     main.c
 * @author   正点原子团队(ALIENTEK)
 * @version  V1.0
 * @date     2020-08-20
 * @brief    新建工程实验-HAL库版本 实验
 * @license  Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ******************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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

//	extern uint8_t g_timxchy_cap_sta; //输入捕获状态
//	extern uint16_t g_timxchy_cap_val; //输入捕获值
//	extern uint8_t g_timxchy_pwmin_sta; /* pwm输入状态 */
//	extern uint16_t g_timxchy_pwmin_psc; /* pwm输入分频系数 */
//	extern uint16_t g_timxchy_pwmin_hval;	/* pwm输入高电平脉宽 */
//	extern uint16_t g_timxchy_pwmin_cval;	/* pwm输入周期宽度 */

void led_set(uint8_t sta)
{
	LED1(sta);
}

/* 函数参数调用测试函数 */
void test_fun(void (*ledset)(uint8_t), uint8_t sta)
{
	ledset(sta);
}

// 时钟显示函数
uint8_t tbuf[40];
/* 定义字符数组用于显示周 */
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

// 单通道ADC过采样（12转16分辨率）用到的参数，如果不放在main函数外会编译失败
//  #define ADC_OVERSAMPLE_TIMES    256                         /* ADC过采样次数, 这里提高4bit分辨率, 需要256倍采样 */
//  #define ADC_DMA_BUF_SIZE        ADC_OVERSAMPLE_TIMES * 10   /* ADC DMA采集 BUF大小, 应等于过采样次数的整数倍 */
//  uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];                   /* ADC DMA BUF */
//  extern uint8_t g_adc_dma_sta;                               /* DMA传输状态标志, 0,未完成; 1, 已完成 */
// extern uint16_t g_dac_sin_buf[4096];
/* 要写入到FLASH的字符串数组 */
// const uint8_t g_text_buf[] = {"STM32 SPI TEST"};	//const,表示它是一个不可变的常量

// #define TEXT_SIZE sizeof(g_text_buf) /* TEXT字符串长度 */
// /* 要写入到STM32 FLASH的字符串数组 */
// const uint8_t g_text_buf[] = {"STM32 FLASH TEST"};
// #define TEXT_LENTH sizeof(g_text_buf) /* 数组长度 */
// /*SIZE表示半字长(2字节), 大小必须是2的整数倍, 如果不是的话, 强制对齐到2的整数倍 */
// #define SIZE TEXT_LENTH / 2 + ((TEXT_LENTH % 2) ? 1 : 0)
// #define FLASH_SAVE_ADDR 0X08010000 /* 设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小（Code+RO-data+RW-data） + 0X08000000) */

// const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN ", " SRAMEX "};

int main(void)
{
	HAL_Init();							/* 初始化HAL库 */
	sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
	delay_init(72);						/* 延时初始化 */
	usart_init(115200);					/* 串口初始化为 115200 */
	led_init();							/* LED初始化 */
	usmart_dev.init(72);				/* USMART初始化 */
	key_init();							/* 初始化按键 */
	rtc_init();							// RTC初始化
	lcd_init();							/* LCD初始化 */
	/******************************************************
	SD卡实验
	*******************************************************/
	uint8_t t = 0;

	my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */

	lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30,  70, 200, 16, 16, "SD TEST", RED);
    lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Read Sector 0", RED);

    while (sd_init()) /* 检测不到SD卡 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SD Card Error!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE(); /* 红灯闪烁 */
    }

    /* 打印SD卡相关信息 */
    show_sdcard_info();

    /* 检测SD卡成功 */
    lcd_show_string(30, 130, 200, 16, 16, "SD Card OK    ", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "SD Card Size:     MB", BLUE);
    lcd_show_num(30 + 13 * 8, 150, SD_TOTAL_SIZE_MB(&g_sdcard_handler), 5, 16, BLUE); /* 显示SD卡容量 */

    while (1)
    {

        if (key1_scan())   /* KEY0按下了 */
        {
            sd_test_read(0, 1); /* 从0扇区读取1*512字节的内容 */
        }

        t++;
        delay_ms(10);

        if (t == 20)
        {
            LED0_TOGGLE(); /* 红灯闪烁 */
            t = 0;
        }
    }

	/******************************************************
	内存管理实验
	*******************************************************/	
	// uint8_t paddr[20];  /* 存放P Addr:+p地址的ASCII值 */
    // uint16_t memused = 0;
    // uint8_t i = 0;
    // uint8_t *p = 0;
    // uint8_t *tp = 0;
    // uint8_t sramx = 0;  /* 默认为内部sram */
	// // sram_init();                        /* SRAM初始化 */
    // my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
    // // my_mem_init(SRAMEX);                /* 初始化外部SRAM内存池 */

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
	// 	if (key2_scan()) /* KEY0按下 */
	// 	{
	// 		printf("E0按下\r\n");
	// 		p = mymalloc(sramx, 2048); /* 申请2K字节,并写入内容,显示在lcd屏幕上面 */
	// 		printf("P Addr:0X%08X\r\n", p);
	// 		if (p != NULL)
	// 		{
	// 			printf("p != null\r\n");
	// 			sprintf((char *)p, "Memory Malloc Test%03d", i);		/* 向p写入一些内容 */
	// 			lcd_show_string(30, 260, 209, 16, 16, (char *)p, BLUE); /* 显示P的内容 */
	// 			printf("显示P的内容结束\r\n");
	// 		}
	// 	}

	// 	if (key1_scan()) /* KEY1按下 */
	// 	{
	// 		printf("C13按下\r\n");
	// 		myfree(sramx, p); /* 释放内存 */
	// 		p = 0;			  /* 指向空地址 */
	// 	}

	// 	if (key4_scan()) /* KEY UP按下 */
	// 	{
	// 		printf("A0按下\r\n");
	// 		sramx++;

	// 		if (sramx > 1)
	// 			sramx = 0;

	// 		lcd_show_string(60, 160, 200, 16, 16, (char *)SRAM_NAME_BUF[sramx], BLUE);
	// 	}

	// 	if (tp != p)
	// 	{
	// 		tp = p;
	// 		sprintf((char *)paddr, "P Addr:0X%08X", (uint32_t)tp);
	// 		lcd_show_string(30, 240, 209, 16, 16, (char *)paddr, BLUE); /* 显示p的地址 */

	// 		if (p)
	// 		{
	// 			lcd_show_string(30, 260, 280, 16, 16, (char *)p, BLUE); /* 显示P的内容 */
	// 		}
	// 		else
	// 		{
	// 			lcd_fill(30, 260, 209, 296, WHITE); /* p=0,清除显示 */
	// 		}
	// 	}

	// 	delay_ms(10);
	// 	i++;

	// 	if ((i % 20) == 0)
	// 	{
	// 		memused = my_mem_perused(SRAMIN);
	// 		sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	// 		lcd_show_string(30 + 112, 176, 200, 16, 16, (char *)paddr, BLUE); /* 显示内部内存使用率 */

	// 		// memused = my_mem_perused(SRAMEX);
	// 		// sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
	// 		// lcd_show_string(30 + 112, 192, 200, 16, 16, (char *)paddr, BLUE); /* 显示TCM内存使用率 */

	// 		LED0_TOGGLE(); /* LED0闪烁 */
	// 	}
	// }

	/******************************************************
	stmflash实验
	*******************************************************/
    // uint16_t i = 0;
	// uint8_t datatemp[SIZE];

	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    // lcd_show_string(30,  70, 200, 16, 16, "FLASH EEPROM TEST", RED);
    // lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    // lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);

    // while (1)
    // {

    //     if (key2_scan()) /* KEY1按下,写入STM32 FLASH */
    //     {
    //         lcd_fill(0, 150, 239, 319, WHITE); /* 清除半屏 */
    //         lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", RED);
    //         stmflash_write(FLASH_SAVE_ADDR, (uint16_t *)g_text_buf, SIZE);
    //         lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", RED); /* 提示传送完成 */
    //     }

    //     if (key1_scan()) /* KEY0按下,读取字符串并显示 */
    //     {
    //         lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH.... ", RED);
    //         stmflash_read(FLASH_SAVE_ADDR, (uint16_t *)datatemp, SIZE);
    //         lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", RED); /* 提示传送完成 */
    //         lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);       /* 显示读到的字符串 */
    //     }

    //     i++;
    //     delay_ms(10);

    //     if (i == 20)
    //     {
    //         LED0_TOGGLE(); /* 提示系统正在运行 */
    //         i = 0;
    //     }
    // }

	/******************************************************
	2.4G无线通讯实验
	*******************************************************/
	// uint8_t key, mode;
    // uint16_t t = 0;
    // uint8_t tmp_buf[33];
	// nrf24l01_init();                    /* 初始化NRF24L01 */

    // lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    // lcd_show_string(30, 70, 200, 16, 16, "NRF24L01 TEST", RED);
    // lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    // while (nrf24l01_check()) /* 检查NRF24L01是否在线 */
    // {
    //     lcd_show_string(30, 110, 200, 16, 16, "NRF24L01 Error", RED);
    //     delay_ms(200);
    //     lcd_fill(30, 110, 239, 130 + 16, WHITE);
    //     delay_ms(200);
    // }

    // lcd_show_string(30, 110, 200, 16, 16, "NRF24L01 OK", RED);

    // while (1) /* 提醒用户选择模式 */
    // {

    //     if (key2_scan())
    //     {
    //         mode = 0; /* 接收模式 */
    //         break;
    //     }
    //     else if (key1_scan())
    //     {
    //         mode = 1; /* 发送模式 */
    //         break;
    //     }

    //     t++;

    //     if (t == 100) /* 显示提示信息 */
    //     {
    //         lcd_show_string(10, 130, 230, 16, 16, "key_E0:RX_Mode  key_C13:TX_Mode", RED);
    //     }

    //     if (t == 200) /* 关闭提示信息 */
    //     {
    //         lcd_fill(10, 130, 230, 150 + 16, WHITE);
    //         t = 0;
    //     }

    //     delay_ms(5);
    // }

    // lcd_fill(10, 130, 240, 166, WHITE); /* 清空上面的显示 */

    // if (mode == 0) /* RX模式 */
    // {
    //     lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 RX_Mode", BLUE);
    //     lcd_show_string(30, 150, 200, 16, 16, "Received DATA:", BLUE);
    //     nrf24l01_rx_mode(); /* 进入RX模式 */

    //     while (1)
    //     {
    //         if (nrf24l01_rx_packet(tmp_buf) == 0) /* 一旦接收到信息,则显示出来. */
    //         {
    //             tmp_buf[32] = 0; /* 加入字符串结束符 */
    //             lcd_show_string(0, 170, lcddev.width - 1, 32, 16, (char *)tmp_buf, BLUE);
    //         }
    //         else
    //             delay_us(100);

    //         t++;

    //         if (t == 10000) /* 大约1s钟改变一次状态 */
    //         {
    //             t = 0;
    //             LED0_TOGGLE();
    //         }
    //     }
    // }
    // else /* TX模式 */
    // {
    //     lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 TX_Mode", BLUE);
    //     nrf24l01_tx_mode(); /* 进入TX模式 */
    //     mode = ' ';         /* 从空格键开始发送 */

    //     while (1)
    //     {
    //         if (nrf24l01_tx_packet(tmp_buf) == 0) /* 发送成功 */
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

    //             tmp_buf[32] = 0; /* 加入结束符 */
    //         }
    //         else
    //         {
    //             lcd_fill(0, 150, lcddev.width, 170 + 16 * 3, WHITE); /* 清空显示 */
    //             lcd_show_string(30, 150, lcddev.width - 1, 32, 16, "Send Failed ", BLUE);
    //         }

    //         LED0_TOGGLE();
    //         delay_ms(200);
    //     }
    // }


	/******************************************************
	电阻触摸屏实验
	*******************************************************/
	// tp_dev.init();                          /* 触摸屏初始化 */
	// lcd_show_string(30, 30, 200, 32, 32, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "YSC@burningCloud", RED);

    // if (tp_dev.touchtype != 0XFF)
    // {
    //     lcd_show_string(30, 110, 200, 16, 16, "Press KEY0 to Adjust", RED); /* 电阻屏才显示 */
    // }

    // delay_ms(1500);
    // load_draw_dialog();

    // if (tp_dev.touchtype & 0X80)
    // {
    //     ctp_test(); /* 电容屏测试 */
    // }
    // else
    // {
    //     rtp_test(); /* 电阻屏测试 */
    // }

	

	/******************************************************
	CAN实验
	*******************************************************/
	// can_init();
    // uint8_t i = 0, t = 0;
    // uint8_t cnt = 0;
    // uint8_t canbuf[8];
    // uint8_t rec_len = 0;
    // uint8_t rec_buf[8];

	// while(1) {
	// 	if (key2_scan()) /* KEY1按下,写入 */
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
    //         LED0_TOGGLE(); /* 提示系统正在运行 */
    //         t = 0;
    //         cnt++;
    //     }
	// }

	/******************************************************
	SPI实验
	*******************************************************/
	// uint16_t i = 0;
	// uint8_t datatemp[TEXT_SIZE];
	// uint32_t flashsize;
	// uint16_t id = 0;
	// char idinfo[30];
	// norflash_init(); /* 初始化NORFLASH */

	// lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "SPI TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED); /* 显示提示信息 */

	// id = norflash_read_id(); /* 读取FLASH ID */

	// while ((id == 0) || (id == 0XFFFF)) /* 检测不到FLASH芯片 */
	// {
	// 	lcd_show_string(30, 130, 200, 16, 16, "FLASH Check Failed!", RED);
	// 	delay_ms(500);
	// 	lcd_show_string(30, 130, 200, 16, 16, "Please Check!      ", RED);
	// 	delay_ms(500);
	// 	LED0_TOGGLE(); /* LED0闪烁 */
	// }

	// sprintf(idinfo,"SPI ID:%x FLASH Ready!", id);
	// lcd_show_string(30, 130, 200, 16, 16, idinfo, BLUE);
	// flashsize = 8 * 1024 * 1024; /* FLASH 大小为8M字节 */

	// while (1)
	// {
	// 	if (key2_scan()) /* KEY1按下,写入 */
	// 	{
	// 		lcd_fill(0, 150, 239, 319, WHITE); /* 清除半屏 */
	// 		lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", BLUE);
	// 		sprintf((char *)datatemp, "%s%d", (char *)g_text_buf, i);
	// 		norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);	  /* 从倒数第100个地址处开始,写入SIZE长度的数据 */
	// 		lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", BLUE); /* 提示传送完成 */
	// 	}

	// 	if (key1_scan()) /* KEY0按下,读取字符串并显示 */
	// 	{
	// 		lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH... . ", BLUE);
	// 		norflash_read(datatemp, flashsize - 100, TEXT_SIZE);				   /* 从倒数第100个地址处开始,读出SIZE个字节 */
	// 		lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:   ", BLUE); /* 提示传送完成 */
	// 		lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);		   /* 显示读到的字符串 */
	// 	}

	// 	i++;

	// 	if (i == 20)
	// 	{
	// 		LED0_TOGGLE(); /* LED0闪烁 */
	// 		i = 0;
	// 	}

	// 	delay_ms(10);
	// }

	/******************************************************
	IIC实验
	*******************************************************/
	// at24cxx_init();

	// /* 要写入到24c02的字符串数组 */
	// const uint8_t g_text_buf[] = {"STM32 IIC TEST"};
	// #define TEXT_SIZE       sizeof(g_text_buf)  /* TEXT字符串长度 */
	// uint16_t i = 0;
	// uint8_t datatemp[TEXT_SIZE];
	// lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "IIC TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);    /* 显示提示信息 */
	// while (at24cx x_check()) /* 检测不到24c02 */
	// {
	//     lcd_show_string(30, 130, 200, 16, 16, "24C02 Check Failed!", RED);
	//     lcd_show_string(30, 146, 200, 16, 16, "Please Check!      ", RED);
	//     delay_ms(500);
	//     LED1_TOGGLE();      /* 红灯闪烁 */
	// }

	// lcd_show_string(30, 130, 200, 16, 16, "24C02 Ready!", RED);

	// while (1)
	// {

	// 	if (key2_scan())
	// 	{
	// 		lcd_fill(0, 150, 239, 319, WHITE);  /* 清除半屏 */
	//         lcd_show_string(30, 150, 200, 16, 16, "Start Write 24C02....", BLUE);
	//         at24cxx_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
	//         lcd_show_string(30, 150, 200, 16, 16, "24C02 Write Finished!", BLUE);   /* 提示传送完成 */
	// 	}
	// 	else if (key1_scan())
	// 	{
	// 		lcd_show_string(30, 150, 200, 16, 16, "Start Read 24C02.... ", BLUE);
	//         at24cxx_read(0, datatemp, TEXT_SIZE);
	//         lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", BLUE);   /* 提示传送完成 */
	//         lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);          /* 显示读到的字符串 */
	// 	}
	// 	i++;

	//     if (i == 20)
	//     {
	//         LED0_TOGGLE();  /* 红灯闪烁 */
	//         i = 0;
	//     }

	//     delay_ms(10);
	// }

	/******************************************************
	DAC实验,PWMDAC实验
	*******************************************************/
	// extern TIM_HandleTypeDef g_tim1_handler;
	// uint16_t adcx;
	// float temp;
	// uint8_t t = 0;
	// uint16_t pwmval = 0;
	// adc_init();
	// pwmdac_init(256 - 1, 0); /* PWM DAC 初始化, Fpwm = 72M/256 =281.25Khz */
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
	// 	if (key1_scan()) /* PWM 占空比调高 */
	// 	{
	// 		if (pwmval < 3200) /* 范围限定 */
	// 		{
	// 			pwmval += 100;
	// 		}
	// 		/* 输出新的 PWM 占空比 */
	// 		pwmdac_set_voltage(pwmval);
	// 	}
	// 	else if (key2_scan()) /* PWM 占空比调低 */
	// 	{
	// 		if (pwmval > 100) /* 范围限定 */
	// 		{
	// 			pwmval -= 100;
	// 		}
	// 		else
	// 		{
	// 			pwmval = 0;
	// 		}
	// 		/* 输出新的 PWM 占空比 */
	// 		pwmdac_set_voltage(pwmval);
	// 	}
	// 	if (t == 10 || key1_scan() || key2_scan())
	// 	{	/* WKUP / KEY1 按下了, 或者定时时间到了 */
	// 		/* PWM DAC 定时器输出比较值 */
	// 		// adcx = __HAL_TIM_GET_COMPARE(&g_tim1_handler, PWMDAC_TIMX_CHY);
	// 		adcx = PWMDAC_TIMX_CCRX;
	// 		lcd_show_xnum(94, 130, adcx, 3, 16, 0, BLUE); /* 显示 CCRX 寄存器值 */
	// 		temp = (float)adcx * (3.3 / 256);			  /* 得到 DAC 电压值 */
	// 		adcx = temp;
	// 		lcd_show_xnum(94, 150, temp, 1, 16, 0, BLUE); /* 显示电压值整数部分 */
	// 		temp -= adcx;
	// 		temp *= 1000;
	// 		lcd_show_xnum(110, 150, temp, 3, 16, 0X80, BLUE); /* 电压值的小数部分 */
	// 		adcx = adc_get_result();	  /* ADC3 通道 1 的转换结果 */
	// 		temp = (float)adcx * (3.3 / 4096);				  /* 得到 ADC 电压值(adc 是 12bit 的) */
	// 		adcx = temp;
	// 		lcd_show_xnum(94, 170, temp, 1, 16, 0, BLUE); /* 显示电压值整数部分 */
	// 		temp -= adcx;
	// 		temp *= 1000;
	// 		lcd_show_xnum(110, 170, temp, 3, 16, 0X80, BLUE); /* 电压值的小数部分 */
	// 		LED0_TOGGLE();									  /* LED0 闪烁 */
	// 		t = 0;
	// 	}
	// 	delay_ms(10);
	// }

	/******************************************************
	DAC实验,正弦波实验
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
	// 	if (key1_scan()) /* 高采样率 , 约1Khz波形 */
	// 	{
	// 		dac_creat_sin_buf(2048, 100);
	// 		dac_dma_wave_enable(100, 10 - 1, 24 - 1); /* 300Khz触发频率, 100个点, 得到最高3KHz的正弦波. */
	// 	}
	// 	else if (key2_scan()) /* 低采样率 , 约1Khz波形 */
	// 	{
	// 		dac_creat_sin_buf(2048, 10);
	// 		dac_dma_wave_enable(10, 10 - 1, 24 - 1); /* 300Khz触发频率, 10个点, 可以得到最高30KHz的正弦波. */
	// 	}

	// 	adcx = DAC1->DHR12R1;						  /* 获取DAC1_OUT1的输出状态 */
	// 	lcd_show_xnum(94, 130, adcx, 4, 16, 0, BLUE); /* 显示DAC寄存器值 */

	// 	temp = (float)adcx * (3.3 / 4096); /* 得到DAC电压值 */
	// 	adcx = temp;
	// 	lcd_show_xnum(94, 150, temp, 1, 16, 0, BLUE); /* 显示电压值整数部分 */

	// 	temp -= adcx;
	// 	temp *= 1000;
	// 	lcd_show_xnum(110, 150, temp, 3, 16, 0X80, BLUE); /* 显示电压值的小数部分 */

	// 	adcx = adc_get_result();		   /* 得到ADC1通道2的转换结果 */
	// 	temp = (float)adcx * (3.3 / 4096); /* 得到ADC电压值(adc是12bit的) */
	// 	adcx = temp;
	// 	lcd_show_xnum(94, 170, temp, 1, 16, 0, BLUE); /* 显示电压值整数部分 */

	// 	temp -= adcx;
	// 	temp *= 1000;
	// 	lcd_show_xnum(110, 170, temp, 3, 16, 0X80, BLUE); /* 显示电压值的小数部分 */

	// 	if (t == 40) /* 定时时间到了 */
	// 	{
	// 		LED0_TOGGLE(); /* LED0闪烁 */
	// 		t = 0;
	// 	}

	// 	delay_ms(5);
	// }

	/******************************************************
	DAC实验,三角波实验
	*******************************************************/
	// dac_init(1);                        /* 初始化DAC1_OUT1通道 */
	// uint8_t t = 0;
	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30,  70, 200, 16, 16, "DAC Triangular WAVE TEST", RED);
	// lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "KEY0:Wave1  KEY1:Wave2", RED);
	// lcd_show_string(30, 130, 200, 16, 16, "DAC None", BLUE); /* 提示无输出 */

	// while (1)
	// {
	//     t++;

	//     if (key1_scan())                        /* 高采样率 , 约1Khz波形 */
	//     {
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC Wave1 ", BLUE);
	//         dac_triangular_wave(4095, 5, 2000, 100); /* 幅值4095, 采样点间隔5us, 200个采样点, 100个波形 */
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC None  ", BLUE);
	//     }
	//     else if (key2_scan())                   /* 低采样率 , 约1Khz波形 */
	//     {
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC Wave2 ", BLUE);
	//         dac_triangular_wave(4095, 500, 20, 100); /* 幅值4095, 采样点间隔500us, 20个采样点, 100个波形 */
	//         lcd_show_string(30, 130, 200, 16, 16, "DAC None  ", BLUE);
	//     }

	//     if (t == 10)                                 /* 定时时间到了 */
	//     {
	//         LED0_TOGGLE();                           /* LED0闪烁 */
	//         t = 0;
	//     }

	//     delay_ms(10);
	// }

	/******************************************************
	DAC实验,通过按键控制输出电压，并且采集电压在显示器上显示
	*******************************************************/
	// adc_init();                             /* 初始化ADC */
	// dac_init(1);								/* 初始化DAC */
	// uint16_t adcx;
	// float temp;
	// uint16_t dacval = 0;

	// dac_set_voltage(1,3300); //1222

	// lcd_show_string(30, 30, 200, 32, 32, "STM32", RED);
	// lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
	// lcd_show_string(30, 90, 200, 16, 16, "YSC@burningCloud", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH0_VAL:", BLUE);
	// lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH0_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// while (1)
	// {
	// 	if (key1_scan())
	//     {
	//         if (dacval < 4000)dacval += 200;

	//         dac_set_voltage(2,dacval);/* 输出增大200 */
	// 		printf("PA5电压设置为%d\r\n",dacval);
	//     }
	//     else if (key2_scan())
	//     {
	//         if (dacval > 200)dacval -= 200;
	//         else dacval = 0;

	//         dac_set_voltage(2,dacval); /* 输出减少200 */
	// 		printf("PA5电压设置为%d\r\n",dacval);
	//     }
	// 	adcx = adc_get_result(); /* 获取通道0的转换值，10次取平均 */
	// 	lcd_show_xnum(134, 110, adcx, 5, 16, 0, BLUE);   /* 显示ADCC采样后的原始值 */

	// 	temp = (float)adcx * (3.3 / 4096);               /* 获取计算后的带小数的实际电压值，比如3.1111 */
	// 	adcx = temp;                                     /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
	// 	lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);   /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

	// 	temp -= adcx;                                    /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
	// 	temp *= 1000;                                    /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
	// 	lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);/* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */

	// 	showtime();

	// 	LED0_TOGGLE();
	// 	delay_ms(100);
	// }

	// 内部温度传感器实验
	//  short temp;
	//  adc_temperature_init();                     /* 初始化ADC内部温度传感器采集 */

	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30,  70, 200, 16, 16, "Temperature TEST", RED);
	// lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// lcd_show_string(30, 120, 200, 16, 16, "TEMPERATE: 00.00C", BLUE);

	// while (1)
	// {

	//     temp = adc_get_temperature();   /* 得到温度值 */

	//     if (temp < 0)
	//     {
	//         temp = -temp;
	//         lcd_show_string(30 + 10 * 8, 120, 16, 16, 16, "-", BLUE);   /* 显示负号 */
	//     }
	//     else
	//     {
	//         lcd_show_string(30 + 10 * 8, 120, 16, 16, 16, " ", BLUE);   /* 无符号 */
	//     }
	//     lcd_show_xnum(30 + 11 * 8, 120, temp / 100, 2, 16, 0, BLUE);    /* 显示整数部分 */
	//     lcd_show_xnum(30 + 14 * 8, 120, temp % 100, 2, 16, 0X80, BLUE); /* 显示小数部分 */

	// 	showtime();	//显示时间
	//     LED0_TOGGLE();  /* LED0闪烁,提示程序运行 */
	//     delay_ms(250);
	// }

	// 单通道ADC过采样（12转16分辨率）
	/* ADC过采样技术, 是利用ADC多次采集的方式, 来提高ADC精度, 采样速度每提高4倍
	 * 采样精度提高 1bit, 同时, ADC采样速度降低4倍, 如提高4bit精度, 需要256次采集
	 * 才能得出1次数据, 相当于ADC速度慢了256倍. 理论上只要ADC足够快, 我们可以无限
	 * 提高ADC精度, 但实际上ADC并不是无限快的, 而且由于ADC性能限制, 并不是位数无限
	 * 提高结果就越好, 需要根据自己的实际需求和ADC的实际性能来权衡.
	 */

	// 	uint16_t i;
	// 	uint32_t adcx;
	// 	uint32_t sum;
	// 	float temp;

	// 	adc_over_dma_init((uint32_t)&g_adc_dma_buf);     			/* 初始化ADC DMA采集 */

	// 	lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// 	lcd_show_string(30,  70, 200, 16, 16, "ADC OverSample TEST", RED);
	// 	lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	// 	lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
	// 	lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	//    adc_dma_enable(ADC_DMA_BUF_SIZE);           /* 启动ADC DMA采集 */

	// 	while (1)
	// 	{
	// 		if (g_adc_dma_sta == 1)
	// 		{
	//            /* 计算DMA 采集到的ADC数据的平均值 */
	// 			sum = 0;

	//            for (i = 0; i < ADC_DMA_BUF_SIZE; i++)   /* 累加 */
	// 			{
	// 			sum += g_adc_dma_buf[i];
	// 			}

	//            adcx = sum / (ADC_DMA_BUF_SIZE / ADC_OVERSAMPLE_TIMES); /* 取平均值 */
	//            adcx >>= 4;   /* 除以2^4倍, 得到12+4位 ADC精度值, 注意: 提高 N bit精度, 需要 >> N */

	//            /* 显示结果 */
	//            lcd_show_xnum(134, 110, adcx, 5, 16, 0, BLUE);      /* 显示ADCC采样后的原始值 */

	//            temp = (float)adcx * (3.3 / 65536);                 /* 获取计算后的带小数的实际电压值，比如3.1111 */
	//            adcx = temp;                                        /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
	//            lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);      /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

	//            temp -= adcx;                                       /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
	//            temp *= 1000;                                       /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
	//            lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);   /* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */

	//            g_adc_dma_sta = 0;                                  /* 清除DMA采集完成状态标志 */
	//            adc_dma_enable(ADC_DMA_BUF_SIZE);                   /* 启动下一次ADC DMA采集 */
	// 		}

	// 		LED0_TOGGLE();
	// 		delay_ms(100);
	// 	}

	// 多通道ADC用DMA采集实验
	// #define ADC_DMA_BUF_SIZE        50 * 6      /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */
	// uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
	// extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */

	// uint16_t i,j;
	// uint16_t adcx;
	// uint32_t sum;
	// float temp;
	// adc_nch_dma_init((uint32_t)&g_adc_dma_buf); /* 初始化ADC DMA采集 */

	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30,  70, 200, 16, 16, "ADC 6CH DMA TEST", RED);
	// lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);

	// lcd_show_string(30, 110, 200, 12, 12, "ADC1_CH0_VAL:", BLUE);
	// lcd_show_string(30, 122, 200, 12, 12, "ADC1_CH0_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// lcd_show_string(30, 140, 200, 12, 12, "ADC1_CH1_VAL:", BLUE);
	// lcd_show_string(30, 152, 200, 12, 12, "ADC1_CH1_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// lcd_show_string(30, 170, 200, 12, 12, "ADC1_CH2_VAL:", BLUE);
	// lcd_show_string(30, 182, 200, 12, 12, "ADC1_CH2_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// lcd_show_string(30, 200, 200, 12, 12, "ADC1_CH3_VAL:", BLUE);
	// lcd_show_string(30, 212, 200, 12, 12, "ADC1_CH3_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// lcd_show_string(30, 230, 200, 12, 12, "ADC1_CH4_VAL:", BLUE);
	// lcd_show_string(30, 242, 200, 12, 12, "ADC1_CH4_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// lcd_show_string(30, 260, 200, 12, 12, "ADC1_CH5_VAL:", BLUE);
	// lcd_show_string(30, 272, 200, 12, 12, "ADC1_CH5_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动ADC DMA采集 */

	// while (1)
	// {
	//     if (g_adc_dma_sta == 1)
	//     {
	//         /* 循环显示通道0~通道5的结果 */
	//         for(j = 0; j < 6; j++)  /* 遍历6个通道 */
	//         {
	//             sum = 0; /* 清零 */
	//             for (i = 0; i < ADC_DMA_BUF_SIZE / 6; i++)  /* 每个通道采集了10次数据,进行10次累加 */
	//             {
	//                 sum += g_adc_dma_buf[(6 * i) + j];      /* 相同通道的转换数据累加 */
	//             }
	//             adcx = sum / (ADC_DMA_BUF_SIZE / 6);        /* 取平均值 */

	//             /* 显示结果 */
	//             lcd_show_xnum(108, 110 + (j * 30), adcx, 4, 12, 0, BLUE);   /* 显示ADCC采样后的原始值 */

	//             temp = (float)adcx * (3.3 / 4096);  /* 获取计算后的带小数的实际电压值，比如3.1111 */
	//             adcx = temp;                        /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
	//             lcd_show_xnum(108, 122 + (j * 30), adcx, 1, 12, 0, BLUE);   /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

	//             temp -= adcx;                       /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
	//             temp *= 1000;                       /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
	//             lcd_show_xnum(120, 122 + (j * 30), temp, 3, 12, 0X80, BLUE);/* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */
	//         }

	//         g_adc_dma_sta = 0;                      /* 清除DMA采集完成状态标志 */
	//         adc_dma_enable(ADC_DMA_BUF_SIZE);       /* 启动下一次ADC DMA采集 */
	//     }
	// 	// showtime();
	//     LED0_TOGGLE();
	//     delay_ms(100);
	// }

	// 单通道ADC用DMA采集实验
	//  #define ADC_DMA_BUF_SIZE        100         /* ADC DMA采集 BUF大小 */
	//  uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
	//  extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */
	//  adc_dma_init((uint32_t)&g_adc_dma_buf); /* 初始化ADC DMA采集 */
	// uint16_t i;
	// uint16_t adcx;
	// uint32_t sum;
	// float temp;
	// lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
	// lcd_show_string(30,  70, 200, 16, 16, "ADC DMA TEST", RED);
	// lcd_show_string(30,  90, 200, 16, 16, "YSC@burningcloud", RED);
	// lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
	// lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	// adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动ADC DMA采集 */

	// while (1)
	// {
	// 	if (g_adc_dma_sta == 1)
	// 	{
	// 	/* 计算DMA 采集到的ADC数据的平均值 */
	// 	sum = 0;

	//         for (i = 0; i < ADC_DMA_BUF_SIZE; i++)   /* 累加 */
	// 		{
	// 		sum += g_adc_dma_buf[i];
	// 		}

	//         adcx = sum / ADC_DMA_BUF_SIZE;           /* 取平均值 */

	//         /* 显示结果 */
	//         lcd_show_xnum(134, 110, adcx, 4, 16, 0, BLUE);      /* 显示ADCC采样后的原始值 */

	//         temp = (float)adcx * (3.3 / 4096);                  /* 获取计算后的带小数的实际电压值，比如3.1111 */
	//         adcx = temp;                                        /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
	//         lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);      /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

	//         temp -= adcx;                                       /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
	//         temp *= 1000;                                       /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
	//         lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);   /* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */

	//      g_adc_dma_sta = 0;                                  /* 清除DMA采集完成状态标志 */
	// 		adc_dma_enable(ADC_DMA_BUF_SIZE);                   /* 启动下一次ADC DMA采集 */
	// 	}

	// 	showtime();
	// 	LED0_TOGGLE();
	// 	delay_ms(100);
	// }

	// 单通道ADC采集实验
	//	adc_init();                             /* 初始化ADC */
	//	 uint16_t adcx;
	//    float temp;
	//    lcd_show_string(30, 30, 200, 32, 32, "STM32", RED);
	//    lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
	//    lcd_show_string(30, 90, 200, 16, 16, "YSC@burningCloud", RED);
	//    lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
	//    lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

	//    while (1)
	//    {
	//        adcx = adc_get_result(); /* 获取通道5的转换值，10次取平均 */
	//        lcd_show_xnum(134, 110, adcx, 5, 16, 0, BLUE);   /* 显示ADCC采样后的原始值 */
	//
	//        temp = (float)adcx * (3.3 / 4096);               /* 获取计算后的带小数的实际电压值，比如3.1111 */
	//        adcx = temp;                                     /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
	//        lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);   /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

	//        temp -= adcx;                                    /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
	//        temp *= 1000;                                    /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
	//        lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);/* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */
	//
	//		showtime();
	//
	//
	//        LED0_TOGGLE();
	//        delay_ms(100);
	//    }
	//

	// DMA数据传输实验（内存传内存）
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
	//					printf("传输完成 \r\n");
	//					break;
	//				}
	//
	//			}
	//
	//		}
	//		LED0_TOGGLE();
	//		delay_ms(200);
	//	}

	// 低功耗实验
	//	uint8_t t = 0;
	//	pwr_wkup_key_init();	//对唤醒键A0中断初始化
	//	pwr_pvd_init(PWR_PVDLEVEL_7);
	//	printf("Enter to LowPower Test \r\n");
	////
	//	while(1){
	//		if (key1_scan())	//如果C13按下
	//		{
	//			/* 进入睡眠模式 */
	//			printf("Enter SLEEP Mode \r\n");
	//			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	//			printf("Exit SLEEP Mode \r\n");
	//		}
	//		else if (key2_scan())	//如果E0按下
	//		{
	//			LED0(1);
	//			/* 进入停止模式 */
	//			printf("Enter STOP Mode \r\n");
	//			HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
	//			sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
	//			HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);	//设置定时器
	//			HAL_SuspendTick();	//停止中断
	//			printf("Exit STOP Mode \r\n");
	//			LED0(0);
	//		}
	//		else if (key3_scan())	//如果A1按下
	//		{
	//			/* 进入待机模式 */
	//			printf("Enter STANDBY Mode \r\n");
	//			__HAL_RCC_PWR_CLK_ENABLE();	//因为要操作电源，使能电源时钟
	//			HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);	//使能WKUP上升沿的唤醒功能
	//			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	//清除唤醒标记
	//
	//			HAL_PWR_EnterSTANDBYMode();
	//			printf("不会打印这条，因为推出待机会重启 Exit StandBy Mode \r\n");
	//		}
	//
	//			t++;
	//			delay_ms(10);
	//		if (t > 50) /* 控制 LED1 闪烁, 提示程序运行状态 */
	//		{
	//			t = 0;
	//			LED1_TOGGLE();
	//		}
	//
	//	}

	// RTC计时与闹钟实验
	//	rtc_init();		//RTC初始化
	////	uint8_t tbuf[40];
	//    uint8_t t = 0;
	//	/* 定义字符数组用于显示周 */
	// char* weekdays[]={"Sunday","Monday","Tuesday","Wednesday",
	//                  "Thursday","Friday","Saterday"};
	//	rtc_set_alarm(2023, 4, 19, 10, 1, 0);  /* 设置一次闹钟年月日时分秒 */
	//	printf("YSC的实验输出：\r\n");
	////	LCD_ShowString(30, 50, 200, 16, 16, "STM32", RED);
	////    LCD_ShowString(30, 70, 200, 16, 16, "RTC TEST", RED);
	////    LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

	//	while (1)
	//    {
	//        t++;

	//        if ((t % 100) == 0)                  /* 每100ms更新一次显示数据 */
	//        {
	//            rtc_get_time();
	//            printf("Time:%02d:%02d:%02d \r\n", calendar.hour, calendar.min, calendar.sec);
	//            printf("Date:%04d-%02d-%02d \r\n", calendar.year, calendar.month, calendar.date);
	//            printf("Week:%s \r\n", weekdays[calendar.week]);
	//        }

	//        if ((t % 20) == 0)
	//        {
	//            LED0_TOGGLE();              /* 每200ms,翻转一次LED0 */
	//        }

	//        delay_ms(10);
	//    }

	// usmart调试实验
	//	   while(1)
	//     {
	//         LED0(0);

	//		delay_ms(500);
	//		LED0(1);

	//		delay_ms(500);
	//    }

	// LCD亮屏实验
	//	uint8_t x = 0;
	//    uint8_t lcd_id[12];
	//	 g_point_color = RED;
	//    sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  /* 将LCD ID打印到lcd_id数组 */

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
	//        lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); /* 显示LCD ID */
	//        x++;

	//        if (x == 12)
	//            x = 0;

	//        LED0_TOGGLE(); /*红灯闪烁*/
	//        delay_ms(1000);
	//    }

	// 高级定时器PWM输入实验
	//		GPIO_InitTypeDef gpio_init_struct;
	//	/* 将 LED0 引脚设置为输入模式, 避免和 PE5 冲突 */
	//	__HAL_RCC_GPIOD_CLK_ENABLE();
	//	gpio_init_struct.Pin = GPIO_PIN_13; /* LED0 引脚 */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* 设置输入状态 */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* 下拉 */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	//	HAL_GPIO_Init(GPIOD, &gpio_init_struct); /* 初始化 LED0 引脚 */
	//	gtim_timx_pwm_chy_init(10 - 1 , 72 - 1);
	//	TIM3->CCR2 = 5;
	//	atim_timx_pwmin_chy_init();

	//	uint8_t t = 0;
	//	double ht, ct, f, tpsc;

	//	while (1)
	//	{
	//		delay_ms(10);
	//		t++;
	//		if (t >= 20) /* 每 200ms 输出一次结果,并闪烁 LED0,提示程序运行 */
	//		{
	//			if (g_timxchy_pwmin_sta) /* 捕获了一次数据 */
	//			{
	//				printf("\r\n"); /* 输出空,另起一行 */
	//				printf("PWM PSC :%d\r\n", g_timxchy_pwmin_psc); /* 打印分频系数 */
	//				printf("PWM Hight:%d\r\n", g_timxchy_pwmin_hval);/*打印高电平脉宽*/
	//				printf("PWM Cycle:%d\r\n", g_timxchy_pwmin_cval);/* 打印周期 */
	//				/* 得到 PWM 采样时钟周期时间 */
	//				tpsc = ((double)g_timxchy_pwmin_psc + 1)/72;
	//				ht = g_timxchy_pwmin_hval * tpsc; /* 计算高电平时间 */
	//				ct = g_timxchy_pwmin_cval * tpsc; /* 计算周期长度 */
	//				f = (1 / ct) * 1000000; /* 计算频率 */
	//				printf("PWM Hight time:%.3fus\r\n", ht); /* 打印高电平脉宽长度 */
	//				printf("PWM Cycle time:%.3fus\r\n", ct); /* 打印周期时间长度 */
	//				printf("PWM Frequency :%.3fHz\r\n", f); /* 打印频率 */
	//				atim_timx_pwmin_chy_restart(); /* 重启 PWM 输入检测 */
	//			}
	//			LED1_TOGGLE(); /* LED0(RED)闪烁 */
	//			t = 0;
	//		}
	//	}

	// 高级定时器互补输出带死区控制实验
	//	GPIO_InitTypeDef gpio_init_struct;
	//	/* 将 LED0 引脚设置为输入模式, 避免和 PC6 冲突 */
	//	gpio_init_struct.Pin = LED0_GPIO_PIN; /* LED0 引脚 */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* 设置输入状态 */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* 下拉 */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	//	HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* 初始化 LED0 引脚 */
	//	/* 将 LED1 引脚设置为输入模式, 避免和 PC6 冲突 */
	//	gpio_init_struct.Pin = LED1_GPIO_PIN; /* LED0 引脚 */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* 设置输入状态 */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* 下拉 */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	//	HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct); /* 初始化 LED0 引脚 */
	//
	//	atim_timx_cplm_pwm_init(10000 - 1, 7200 - 1); /* 1Mhz 的计数频率 1Khz 的周期. */
	//	atim_timx_cplm_pwm_set(3000 - 1, 1000); /* 占空比:7:3,死区时间 100 * tDTS */
	//	while (1){}

	// pwm输出比较实验
	//	GPIO_InitTypeDef gpio_init_struct;
	//	atim_timx_comp_pwm_init(1000 - 1, 72 - 1); /* 1Mhz 的计数频率 1Khz 的周期. */
	//	/* 将 LED0 引脚设置为输入模式, 避免和 PC6 冲突 */
	//	gpio_init_struct.Pin = LED0_GPIO_PIN; /* LED0 引脚 */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* 设置输入状态 */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* 下拉 */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	//	HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* 初始化 LED0 引脚 */
	//	/* 将 LED1 引脚设置为输入模式, 避免和 PC6 冲突 */
	//	gpio_init_struct.Pin = LED1_GPIO_PIN; /* LED0 引脚 */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* 设置输入状态 */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* 下拉 */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	//	HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct); /* 初始化 LED0 引脚 */
	//	ATIM_TIMX_COMP_CH1_CCRX = 250 - 1; /* 通道 1 相位 25% */
	//	ATIM_TIMX_COMP_CH2_CCRX = 500 - 1; /* 通道 2 相位 50% */
	//	ATIM_TIMX_COMP_CH3_CCRX = 750 - 1; /* 通道 3 相位 75% */
	//	ATIM_TIMX_COMP_CH4_CCRX = 1000 - 1; /* 通道 4 相位 100% */
	//	while (1)
	//	{
	//		printf("程序正常运行...\r\n");
	//		delay_ms(10);
	//	}

	// 高级计时器指定PWM数实验
	//	uint8_t t = 0;
	//	GPIO_InitTypeDef gpio_init_struct;

	//	/* 将 LED1 引脚设置为输入模式, 避免和 PC6 冲突 */
	//	gpio_init_struct.Pin = LED0_GPIO_PIN; /* LED0 引脚 */
	//	gpio_init_struct.Mode = GPIO_MODE_INPUT; /* 设置输入状态 */
	//	gpio_init_struct.Pull = GPIO_PULLDOWN; /* 下拉 */
	//	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	//	HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* 初始化 LED0 引脚 */

	//	atim_timx_npwm_chy_init(5000 - 1, 7200 - 1);/*10Khz 的计数频率,2hz 的 PWM 频率*/
	//
	//	atim_timx_npwm_chy_set(5); /* 输出 5 个 PWM 波(控制 LED0)闪烁 5 次) */

	//	while (1)
	//	{
	//		if (key1_scan()) /* KEY0 按下 */
	//		{
	//			atim_timx_npwm_chy_set(5); /* 输出 5 个 PWM 波(控制 LED0 闪烁 5 次) */
	//		}
	//			t++;
	//			delay_ms(10);
	//		if (t > 50) /* 控制 LED1 闪烁, 提示程序运行状态 */
	//		{
	//			t = 0;
	//			LED1_TOGGLE();
	//		}
	//	}
	//

	// 通用定时器脉冲计数实验
	//	uint32_t curcnt = 0;
	// uint32_t oldcnt = 0;
	// uint8_t t = 0;
	// gtim_timx_cnt_chy_init(0); /* 定时器计数初始化, 不分频 */
	// gtim_timx_cnt_chy_restart(); /* 重启计数 */
	//	while (1)
	// {
	// if (key1_scan() == 1) /* KEY1 按键按下,重启计数 */
	// {
	// gtim_timx_cnt_chy_restart(); /* 重新启动计数 */
	// }
	// curcnt = gtim_timx_cnt_chy_get_count(); /* 获取计数值 */
	// if (oldcnt != curcnt)
	// {
	// oldcnt = curcnt;
	// printf("CNT:%d\r\n", oldcnt); /* 打印脉冲个数 */
	// }
	// t++;
	// if (t > 20) /* 200ms 进入一次 */
	// {
	// t = 0;
	// LED0_TOGGLE(); /* LED0 闪烁 ,提示程序运行 */
	// }
	// delay_ms(10);
	// }

	// 测试PA0接口按键好不好使

	//	LED0(1); //点亮led0
	//	while (1)
	//	{
	//		if (key4_scan()){
	//			LED0_TOGGLE();
	//		}else
	//		{
	//			delay_ms(10);//允许误差10ms,消除抖动
	//		}
	//	 }
	//
	// 通用定时器输入/捕获实验
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
	////			printf("串口正常\r\n");
	//		}
	//		delay_ms(10);
	//	}
	//

	// 通用定时器输出实验
	//	extern TIM_HandleTypeDef g_timx_pwm_chy_handle; /* 定时器句柄 */
	//	gtim_timx_pwm_chy_init(400-1,72-1); //定时器初始化
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
	//		//修改比较值控制占空比
	//		__HAL_TIM_SET_COMPARE(&g_timx_pwm_chy_handle,TIM_CHANNEL_2,ledrpwmval);
	//
	//	}
	//

	// 基本计数器实验
	//	btim_timx_int_init(5000 - 1, 7200 - 1); /* 10Khz 的计数频率，计数 5K 次为 500ms */
	//	 while (1)
	//	 {
	//		 LED0_TOGGLE();
	//		 delay_ms(200);
	//	 }

	//	//窗口看门狗实验
	//	LED0(1);
	//	delay_ms(3000);
	//	wwdg_init(0x7F, 0x5F, WWDG_PRESCALER_8);
	//	while(1){
	//		LED0(0);
	//		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == 1){
	////		delay_ms(10); //允许误差10ms,消除抖动
	//			wwdg_feed();
	//		}
	//	}

	//	独立看门狗实验
	//	delay_ms(200); /* 延时 200ms 再初始化看门狗,LED0 的变化"可见" */
	//
	//	iwdg_init(IWDG_PRESCALER_64,625); /* 预分频数 64,重载值为 625,溢出时间约为 1s */
	//	LED0(1); /* 点亮 LED0(红灯) */
	//	 //printf("led0");
	//	 while (1)
	//	 {
	//		 if (key1_scan() == 1) /* 如果 K1 按下,则喂狗 */
	//		 {
	//			iwdg_feed(); /* 喂狗 */
	//		 }else{
	//			delay_ms(10);
	//		 }

	//	 }

	//	uint8_t len;
	//	uint16_t times = 0;
	//	printf("请输入数据,以回车键结束\r\n");
	//	while (1)
	//	 {
	//	 if (g_usart_rx_sta & 0x8000) /* 接收到了数据? */
	//	 {
	//		 len = g_usart_rx_sta & 0x3fff; /* 得到此次接收到的数据长度 */
	//		 printf("\r\n 您发送的消息为:\r\n");
	//		/*发送接收到的数据	Sends an amount of data in blocking mode.*/
	//		 HAL_UART_Transmit(&g_uart1_handle,(uint8_t*)g_usart_rx_buf,len,1000);
	//		/*等待发送结束*/
	//		while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC)!=SET){};
	//		 printf("\r\n\r\n"); /* 插入换行 */
	//		 g_usart_rx_sta = 0;
	//	 }
	//	 else
	//	 {
	//		 times++;
	//		 if (times % 5000 == 0)
	//		 {
	//			 printf("\r\n STM32 开发板 串口实验：\r\n");
	//			 printf("50秒打印一次\r\n\r\n\r\n");
	//		 }
	//		 if (times % 500 == 0) printf("请输入数据,以回车键结束\r\n");  //两秒打印一次
	//		 if (times % 30 == 0) LED0_TOGGLE(); /* 0.3秒闪烁一次 LED,提示系统正在运行. */
	//		 delay_ms(10);
	//	 }
	//	 }

	//	key_init();							//KEY初始化
	//	exti_init();						//EXTI初始化
	//	//外部中断实验
	//	/*
	//	通过外部中断的方式让开发板上的两个独立按键控制 LED 灯：KEY0 控制 LED0 翻转，KEY1 控制 LED1 翻转。
	//	*/
	//	LED0(1); //点亮led0
	//	while (1)
	//	{
	//		printf("OK\r\n");
	//		delay_ms(1000);
	//	 }

	// 跑马灯实验
	//	   while(1)
	//     {
	//         LED0(0);
	//		LED1(1);
	//		delay_ms(500);
	//		LED0(1);
	//		LED1(0);
	//		delay_ms(500);
	//     }
	// 按键实验
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
	//			delay_ms(10);//允许误差10ms,消除抖动
	//		}
	//	}
}
