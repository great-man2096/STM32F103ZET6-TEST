/**
 ****************************************************************************************************
 * @file        font.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       �ֿ� ����
 *              �ṩfonts_update_font��fonts_init�����ֿ���ºͳ�ʼ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200428
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "string.h"
#include "./BSP/LCD/lcd.h"
#include "./TEXT/fonts.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/NORFLASH/norflash.h"


/* �ֿ�����ռ�õ�����������С(3���ֿ�+unigbk��+�ֿ���Ϣ=3238700 �ֽ�,Լռ791��25QXX����,һ������4K�ֽ�) */
#define FONTSECSIZE         791


/* �ֿ�����ʼ��ַ
 * �ӵ�12MB��ַ��ʼ����ֿ�
 * ǰ��12MB��С���ļ�ϵͳռ��
 * 12MB�����3���ֿ�+UNIGBK.BIN,�ܴ�С3.09M, 791������,���ֿ�ռ����,���ܶ�!
 * 15.10M�Ժ�, �û���������ʹ��. ����������100K�ֽڱȽϺ�
 */
#define FONTINFOADDR        12 * 1024 * 1024

 
/* ���������ֿ������Ϣ����ַ����С�� */
_font_info ftinfo;

/* �ֿ����ڴ����е�·�� */
char *const FONT_GBK_PATH[4] =
{
    "/SYSTEM/FONT/UNIGBK.BIN",      /* UNIGBK.BIN�Ĵ��λ�� */
    "/SYSTEM/FONT/GBK12.FON",       /* GBK12�Ĵ��λ�� */
    "/SYSTEM/FONT/GBK16.FON",       /* GBK16�Ĵ��λ�� */
    "/SYSTEM/FONT/GBK24.FON",       /* GBK24�Ĵ��λ�� */
};

/* ����ʱ����ʾ��Ϣ */
char *const FONT_UPDATE_REMIND_TBL[4] =
{
    "Updating UNIGBK.BIN",          /* ��ʾ���ڸ���UNIGBK.bin */
    "Updating GBK12.FON ",          /* ��ʾ���ڸ���GBK12 */
    "Updating GBK16.FON ",          /* ��ʾ���ڸ���GBK16 */
    "Updating GBK24.FON ",          /* ��ʾ���ڸ���GBK24 */
};

/**
 * @brief       ��ʾ��ǰ������½���
 * @param       x, y    : ����
 * @param       size    : �����С
 * @param       totsize : �����ļ���С
 * @param       pos     : ��ǰ�ļ�ָ��λ��
 * @param       color   : ������ɫ
 * @retval      ��
 */
static void fonts_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100)t = 100;

        lcd_show_num(x, y, t, 3, size, color);  /* ��ʾ��ֵ */
    }
}

/**
 * @brief       ����ĳһ���ֿ�
 * @param       x, y    : ��ʾ��Ϣ����ʾ��ַ
 * @param       size    : ��ʾ��Ϣ�����С
 * @param       fpath   : ����·��
 * @param       fx      : ���µ�����
 *   @arg                 0, ungbk;
 *   @Arg                 1, gbk12;
 *   @arg                 2, gbk16;
 *   @arg                 3, gbk24;
 * @param       color   : ������ɫ
 * @retval      0, �ɹ�; ����, �������;
 */
static uint8_t fonts_update_fontx(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* �����ڴ� */

    if (fftemp == NULL)rval = 1;

    tempbuf = mymalloc(SRAMIN, 4096);               /* ����4096���ֽڿռ� */

    if (tempbuf == NULL)rval = 1;

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);

    if (res)rval = 2;   /* ���ļ�ʧ�� */

    if (rval == 0)
    {
        switch (fx)
        {
            case 0: /* ���� UNIGBK.BIN */
                ftinfo.ugbkaddr = FONTINFOADDR + sizeof(ftinfo);    /* ��Ϣͷ֮�󣬽���UNIGBKת����� */
                ftinfo.ugbksize = fftemp->obj.objsize;              /* UNIGBK��С */
                flashaddr = ftinfo.ugbkaddr;
                break;

            case 1: /* ���� GBK12.BIN */
                ftinfo.f12addr = ftinfo.ugbkaddr + ftinfo.ugbksize; /* UNIGBK֮�󣬽���GBK12�ֿ� */
                ftinfo.gbk12size = fftemp->obj.objsize;             /* GBK12�ֿ��С */
                flashaddr = ftinfo.f12addr;                         /* GBK12����ʼ��ַ */
                break;

            case 2: /* ���� GBK16.BIN */
                ftinfo.f16addr = ftinfo.f12addr + ftinfo.gbk12size; /* GBK12֮�󣬽���GBK16�ֿ� */
                ftinfo.gbk16size = fftemp->obj.objsize;             /* GBK16�ֿ��С */
                flashaddr = ftinfo.f16addr;                         /* GBK16����ʼ��ַ */
                break;

            case 3: /* ���� GBK24.BIN */
                ftinfo.f24addr = ftinfo.f16addr + ftinfo.gbk16size; /* GBK16֮�󣬽���GBK24�ֿ� */
                ftinfo.gbk24size = fftemp->obj.objsize;             /* GBK24�ֿ��С */
                flashaddr = ftinfo.f24addr;                         /* GBK24����ʼ��ַ */
                break;
        }

        while (res == FR_OK)   /* ��ѭ��ִ�� */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* ��ȡ���� */

            if (res != FR_OK)break;     /* ִ�д��� */

            norflash_write(tempbuf, offx + flashaddr, bread);    /* ��0��ʼд��bread������ */
            offx += bread;
            fonts_progress_show(x, y, size, fftemp->obj.objsize, offx, color);    /* ������ʾ */

            if (bread != 4096)break;    /* ������. */
        }

        f_close(fftemp);
    }

    myfree(SRAMIN, fftemp);     /* �ͷ��ڴ� */
    myfree(SRAMIN, tempbuf);    /* �ͷ��ڴ� */
    return res;
}

/**
 * @brief       ���������ļ�
 *   @note      �����ֿ�һ�����(UNIGBK,GBK12,GBK16,GBK24)
 * @param       x, y    : ��ʾ��Ϣ����ʾ��ַ
 * @param       size    : ��ʾ��Ϣ�����С
 * @param       src     : �ֿ���Դ����
 *   @arg                 "0:", SD��;
 *   @Arg                 "1:", FLASH��
 * @param       color   : ������ɫ
 * @retval      0, �ɹ�; ����, �������;
 */
uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0XFF;
    ftinfo.fontok = 0XFF;
    pname = mymalloc(SRAMIN, 100);  /* ����100�ֽ��ڴ� */
    buf = mymalloc(SRAMIN, 4096);   /* ����4K�ֽ��ڴ� */
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* �����ڴ� */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        myfree(SRAMIN, fftemp);
        myfree(SRAMIN, pname);
        myfree(SRAMIN, buf);
        return 5;   /* �ڴ�����ʧ�� */
    }

    for (i = 0; i < 4; i++) /* �Ȳ����ļ�UNIGBK,GBK12,GBK16,GBK24 �Ƿ����� */
    {
        strcpy((char *)pname, (char *)src);                 /* copy src���ݵ�pname */
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);    /* ׷�Ӿ����ļ�·�� */
        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);/* ���Դ� */

        if (res)
        {
            rval |= 1 << 7; /* ��Ǵ��ļ�ʧ�� */
            break;          /* ������,ֱ���˳� */
        }
    }

    myfree(SRAMIN, fftemp); /* �ͷ��ڴ� */

    if (rval == 0)          /* �ֿ��ļ�������. */
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);    /* ��ʾ���ڲ������� */

        for (i = 0; i < FONTSECSIZE; i++)   /* �Ȳ����ֿ�����,���д���ٶ� */
        {
            fonts_progress_show(x + 20 * size / 2, y, size, FONTSECSIZE, i, color);   /* ������ʾ */
            norflash_read((uint8_t *)buf, ((FONTINFOADDR / 4096) + i) * 4096, 4096); /* ������������������ */

            for (j = 0; j < 1024; j++)          /* У������ */
            {
                if (buf[j] != 0XFFFFFFFF)break; /* ��Ҫ���� */
            }

            if (j != 1024)
            {
                norflash_erase_sector((FONTINFOADDR / 4096) + i); /* ��Ҫ���������� */
            }
        }

        for (i = 0; i < 4; i++) /* ���θ���UNIGBK,GBK12,GBK16,GBK24 */
        {
            lcd_show_string(x, y, 240, 320, size, FONT_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);             /* copy src���ݵ�pname */
            strcat((char *)pname, (char *)FONT_GBK_PATH[i]);/* ׷�Ӿ����ļ�·�� */
            res = fonts_update_fontx(x + 20 * size / 2, y, size, pname, i, color);    /* �����ֿ� */

            if (res)
            {
                myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
                return 1 + i;
            }
        }

        /* ȫ�����º��� */
        ftinfo.fontok = 0XAA;
        norflash_write((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));    /* �����ֿ���Ϣ */
    }

    myfree(SRAMIN, pname);  /* �ͷ��ڴ� */
    myfree(SRAMIN, buf);
    return rval;            /* �޴���. */
}

/**
 * @brief       ��ʼ������
 * @param       ��
 * @retval      0, �ֿ����; ����, �ֿⶪʧ;
 */
uint8_t fonts_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* ������ȡ10��,���Ǵ���,˵��ȷʵ��������,�ø����ֿ��� */
    {
        t++;
        norflash_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo)); /* ����ftinfo�ṹ������ */

        if (ftinfo.fontok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }
    printf("ugbkaddr:%c\r\n",ftinfo.ugbkaddr);
    printf("ugbksize:%c\r\n",ftinfo.ugbksize);
    printf("f12addr:%c\r\n",ftinfo.f12addr);
    printf("gbk12size:%c\r\n",ftinfo.gbk12size);
    printf("f16addr:%c\r\n",ftinfo.f16addr);
    printf("gbk16size:%c\r\n",ftinfo.gbk16size);
    printf("f24addr:%c\r\n",ftinfo.f24addr);
    printf("gbk24size:%c\r\n",ftinfo.gbk24size);

    if (ftinfo.fontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}





const unsigned char gImage_1in3[1030] = { /*0X00,0X01,0X40,0X00,0X80,0X00,*/
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X78,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFE,0X00,0X00,0X00,
0X00,0X00,0X00,0X01,0XC7,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X83,0X80,0X70,0X00,
0X00,0X00,0X1F,0X07,0X01,0X83,0XFE,0X00,0X00,0X00,0X3F,0XC6,0X01,0XCF,0X8F,0X00,
0X00,0X00,0X30,0XE6,0X00,0XFC,0X03,0X00,0X00,0X00,0X70,0X7E,0X00,0XF0,0X03,0X00,
0X00,0X00,0X60,0X3C,0X00,0X60,0X03,0X00,0X00,0X00,0X60,0X1C,0X00,0X00,0X03,0X00,
0X00,0X00,0X60,0X0C,0X00,0X00,0X07,0X00,0X00,0X00,0X70,0X0C,0X00,0X00,0X06,0X00,
0X00,0X00,0X30,0X00,0X00,0X00,0X0E,0X00,0X00,0X00,0X30,0X00,0X42,0X00,0X0C,0X00,
0X00,0X00,0X38,0X00,0X42,0X00,0X1C,0X00,0X00,0X00,0X18,0X00,0X66,0X00,0X38,0X00,
0X00,0X00,0X1C,0X00,0X6E,0X00,0X70,0X00,0X00,0X00,0X0F,0X00,0X3C,0X01,0XE0,0X00,
0X00,0X00,0X07,0XF8,0X10,0X7F,0XC0,0X00,0X00,0X00,0X07,0XE0,0X00,0X1F,0X00,0X00,
0X00,0X00,0X0F,0X00,0X00,0X07,0X80,0X00,0X00,0X00,0X1C,0X00,0X20,0X01,0XC0,0X00,
0X00,0X00,0X38,0X00,0X60,0X00,0XE0,0X00,0X00,0X00,0X30,0X00,0XC0,0X00,0XF0,0X00,
0X00,0X00,0X70,0X01,0X80,0X01,0XF0,0X00,0X00,0X00,0X60,0X03,0X00,0X03,0XB8,0X00,
0X00,0X00,0X60,0X0E,0X00,0X07,0X18,0X00,0X00,0X00,0XE0,0X1C,0X00,0X0E,0X1C,0X00,
0X00,0X00,0XE0,0X38,0X00,0X1C,0X0C,0X00,0X00,0X00,0XC0,0X70,0X00,0X38,0X0E,0X00,
0X00,0X00,0XC1,0XC0,0X00,0X70,0X0E,0X00,0X00,0X00,0XC3,0X80,0X00,0XE0,0X06,0X00,
0X00,0X00,0XC7,0X00,0X03,0XC0,0X06,0X00,0X00,0X00,0XDC,0X00,0X07,0X00,0X07,0X00,
0X00,0X00,0XF8,0X00,0X0E,0X00,0X07,0X00,0X00,0X01,0XF0,0X00,0X3C,0X00,0X0F,0X00,
0X00,0X01,0XC0,0X00,0X70,0X00,0X1B,0X00,0X00,0X01,0XC0,0X01,0XE0,0X00,0X33,0X00,
0X00,0X01,0XC0,0X03,0XC0,0X00,0X73,0X80,0X00,0X01,0XC0,0X0F,0X00,0X01,0XC3,0X80,
0X00,0X01,0XC0,0X3C,0X00,0X03,0X83,0X80,0X00,0X01,0XC0,0XF8,0X00,0X07,0X01,0X80,
0X00,0X01,0XC3,0XE0,0X00,0X1E,0X01,0X80,0X00,0X01,0XCF,0X80,0X00,0X3C,0X01,0X80,
0X00,0X01,0XFE,0X00,0X00,0XF0,0X01,0X80,0X00,0X01,0XF8,0X00,0X01,0XE0,0X01,0XC0,
0X00,0X01,0XE0,0X00,0X03,0XC0,0X01,0XC0,0X00,0X01,0XC0,0X00,0X0F,0X00,0X01,0XC0,
0X00,0X00,0XC0,0X00,0X3C,0X00,0X01,0XC0,0X00,0X00,0XC0,0X00,0X78,0X00,0X03,0XC0,
0X00,0X00,0XC0,0X03,0XE0,0X00,0X07,0XC0,0X00,0X00,0XC0,0X0F,0X80,0X00,0X0C,0XC0,
0X00,0X00,0XC0,0X3E,0X00,0X00,0X38,0XC0,0X00,0X00,0XC0,0XF8,0X00,0X00,0X70,0XC0,
0X00,0X00,0XC3,0XE0,0X00,0X01,0XE0,0XC0,0X00,0X00,0XFF,0X00,0X00,0X07,0X80,0XC0,
0X00,0X00,0XFC,0X00,0X00,0X0F,0X00,0XC0,0X00,0X00,0XE0,0X00,0X00,0X3C,0X00,0XC0,
0X00,0X00,0XC0,0X00,0X00,0XF0,0X00,0XC0,0X00,0X00,0XE0,0X00,0X03,0XC0,0X00,0XC0,
0X00,0X00,0XE0,0X00,0X0F,0X00,0X00,0XE0,0X00,0X00,0XE0,0X00,0X7C,0X00,0X00,0XE0,
0X00,0X00,0XE0,0X03,0XF0,0X00,0X01,0XE0,0X00,0X00,0X60,0X1F,0X80,0X00,0X03,0XE0,
0X00,0X00,0X63,0XFC,0X00,0X00,0X0E,0XE0,0X00,0X00,0X7F,0XE0,0X00,0X00,0X1C,0XE0,
0X00,0X00,0X7E,0X00,0X00,0X00,0X78,0XE0,0X00,0X00,0X60,0X00,0X00,0X01,0XE0,0XE0,
0X00,0X00,0X60,0X00,0X00,0X07,0XC0,0XE0,0X00,0X00,0X70,0X00,0X00,0X1F,0X00,0XE0,
0X00,0X00,0X70,0X00,0X00,0X7C,0X00,0XE0,0X00,0X00,0X70,0X00,0X03,0XF0,0X00,0XE0,
0X00,0X00,0X30,0X00,0X1F,0X80,0X00,0XE0,0X00,0X00,0X30,0X01,0XFE,0X00,0X00,0XE0,
0X00,0X00,0X30,0X3F,0XF0,0X00,0X03,0XE0,0X00,0X00,0X3F,0XFE,0X00,0X00,0X06,0XE0,
0X00,0X00,0X3F,0XC0,0X00,0X00,0X1C,0XE0,0X00,0X00,0X38,0X00,0X00,0X00,0X38,0XE0,
0X00,0X00,0X38,0X00,0X00,0X01,0XF0,0XE0,0X00,0X00,0X38,0X00,0X00,0X03,0XC0,0XE0,
0X00,0X00,0X38,0X00,0X00,0X1F,0X00,0XE0,0X00,0X00,0X38,0X00,0X00,0XFC,0X00,0XE0,
0X00,0X00,0X38,0X00,0X03,0XF0,0X00,0XE0,0X00,0X00,0X18,0X00,0X3F,0X80,0X01,0XE0,
0X00,0X00,0X18,0X07,0XFC,0X00,0X03,0XE0,0X00,0X00,0X1F,0XFF,0XE0,0X00,0X0E,0XE0,
0X00,0X00,0X1F,0XFC,0X00,0X00,0X1C,0XC0,0X00,0X00,0X1C,0X00,0X00,0X00,0X70,0XC0,
0X00,0X00,0X1C,0X00,0X00,0X01,0XE0,0XC0,0X00,0X00,0X1C,0X00,0X00,0X07,0X80,0XC0,
0X00,0X00,0X0C,0X00,0X00,0X1F,0X00,0XC0,0X00,0X00,0X0C,0X00,0X00,0XFC,0X01,0XC0,
0X00,0X00,0X0E,0X00,0X07,0XE0,0X01,0XC0,0X00,0X00,0X0E,0X00,0X3F,0X80,0X01,0XC0,
0X00,0X00,0X06,0X03,0XFC,0X00,0X07,0X80,0X00,0X00,0X07,0X7F,0XE0,0X00,0X0F,0X80,
0X00,0X00,0X07,0XFE,0X00,0X00,0X1B,0X80,0X00,0X00,0X03,0X80,0X00,0X00,0X73,0X00,
0X00,0X00,0X03,0X80,0X00,0X01,0XE7,0X00,0X00,0X00,0X01,0X80,0X00,0X03,0X8E,0X00,
0X00,0X00,0X01,0XC0,0X00,0X1E,0X3C,0X00,0X00,0X00,0X00,0XE0,0X00,0X78,0X78,0X00,
0X00,0X00,0X00,0X78,0X01,0XF3,0XF8,0X00,0X00,0X00,0X00,0X3F,0X07,0X9F,0X9E,0X00,
0X00,0X00,0X00,0X3F,0XC3,0X80,0X07,0X00,0X00,0X00,0X00,0X70,0X03,0XE0,0X03,0X80,
0X00,0X00,0X00,0XE0,0X03,0X70,0X01,0XC0,0X00,0X00,0X00,0XE0,0X03,0X18,0X00,0XE0,
0X00,0X00,0X00,0XC0,0X02,0X00,0X00,0X60,0X00,0X00,0X00,0XC0,0X02,0X00,0X00,0X30,
0X00,0X00,0X00,0XC0,0X00,0X00,0X00,0X30,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X38,
0X00,0X00,0X01,0XC0,0X00,0X08,0X00,0X18,0X00,0X00,0X01,0XC0,0X00,0X1C,0X00,0X18,
0X00,0X00,0X01,0XC0,0X40,0X1F,0X00,0X18,0X00,0X00,0X01,0XC0,0XC0,0X1B,0X80,0X18,
0X00,0X00,0X00,0XC1,0XE0,0X19,0XE0,0X18,0X00,0X00,0X00,0XC1,0XF0,0X30,0X78,0X38,
0X00,0X00,0X00,0XE3,0XF8,0X70,0X3F,0XF0,0X00,0X00,0X00,0XE7,0X3F,0XE0,0X07,0XF0,
0X00,0X00,0X00,0X7F,0X0F,0XC0,0X00,0X80,0X00,0X00,0X00,0X3C,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,
};






