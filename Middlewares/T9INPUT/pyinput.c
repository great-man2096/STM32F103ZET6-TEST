/**
 ****************************************************************************************************
 * @file        pyinput.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       ƴ�����뷨 ����
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200405
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "string.h"
#include "./T9INPUT/pymb.h"
#include "./T9INPUT/pyinput.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"
#include "./TEXT/text.h"

/* ���ֱ� */
const char *kbd_tbl[9] = {"��", "2", "3", "4", "5", "6", "7", "8", "9"};

/* �ַ��� */
const char *kbs_tbl[9] = {"DEL", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

/* ƴ�����뷨 */
pyinput t9 =
{
    get_pymb,
    0,
};

/**
 * @brief       �Ƚ������ַ�����ƥ�����
 * @param       str1      : �ַ���1
 * @param       str2      : �ַ���2
 * @retval      �ԱȽ��
 *   @arg       0XFF , ��ʾ��ȫƥ��
 *   @arg       ���� , ƥ����ַ���
 */
uint8_t str_match(uint8_t *str1, uint8_t *str2)
{
    uint8_t i = 0;

    while (1)
    {
        if (*str1 != *str2)break;   /* ����ƥ�� */

        if (*str1 == '\0')
        {
            i = 0XFF;   /* ��ȫƥ�� */
            break;
        }

        i++;
        str1++;
        str2++;
    }

    return i;           /* �����ַ������ */
}

/**
 * @brief       ��ȡƥ���ƴ�����
 * @param       strin     : ������ַ���,����:"726"
 * @param       matchlist : �����ƥ���
 * @retval      ƥ��״̬
 *              [7]  , 0,��ʾ��ȫƥ�䣻1����ʾ����ƥ�䣨����û����ȫƥ���ʱ��Ż���֣�
 *              [6:0], ��ȫƥ���ʱ�򣬱�ʾ��ȫƥ���ƴ������
 *                     ����ƥ���ʱ�򣬱�ʾ��Чƥ���λ��
 */
uint8_t get_matched_pymb(uint8_t *strin, py_index **matchlist)
{
    py_index *bestmatch = 0;    /* ���ƥ�� */
    uint16_t pyindex_len = 0;
    uint16_t i = 0;
    uint8_t temp, mcnt = 0, bmcnt = 0;
    
    bestmatch = (py_index *)&py_index3[0];  /* Ĭ��Ϊa��ƥ�� */
    pyindex_len = sizeof(py_index3) / sizeof(py_index3[0]); /* �õ�py������Ĵ�С */

    for (i = 0; i < pyindex_len; i++)
    {
        temp = str_match(strin, (uint8_t *)py_index3[i].py_input);

        if (temp)
        {
            if (temp == 0XFF)
            {
                matchlist[mcnt++] = (py_index *)&py_index3[i];
            }
            else if (temp > bmcnt)  /* �����ƥ�� */
            {
                bmcnt = temp;
                bestmatch = (py_index *)&py_index3[i];  /* ��õ�ƥ�� */
            }
        }
    }

    if (mcnt == 0 && bmcnt)     /* û����ȫƥ��Ľ��,�����в���ƥ��Ľ�� */
    {
        matchlist[0] = bestmatch;
        mcnt = bmcnt | 0X80;    /* ���ز���ƥ�����Чλ�� */
    }

    return mcnt;    /* ����ƥ��ĸ��� */
}

/**
 * @brief       �õ�ƴ�����
 * @param       str       : �����ַ���
 * @retval      ƥ��״̬
 *              [7]  , 0,��ʾ��ȫƥ�䣻1����ʾ����ƥ�䣨����û����ȫƥ���ʱ��Ż���֣�
 *              [6:0], ��ȫƥ���ʱ�򣬱�ʾ��ȫƥ���ƴ������
 *                     ����ƥ���ʱ�򣬱�ʾ��Чƥ���λ��
 */
uint8_t get_pymb(uint8_t *str)
{
    return get_matched_pymb(str, t9.pymb);
}

/**
 * @brief       �õ�ƴ�����(��USMART�õ�)
 * @param       str       : �����ַ���
 * @retval      ��
 */
void test_py(uint8_t *str)
{
    uint8_t t = 0;
    uint8_t i = 0;
    t = t9.getpymb(str);

    if (t & 0X80)
    {
        printf("\r\n��������Ϊ:%s\r\n", str);
        printf("����ƥ��λ��:%d\r\n", t & 0X7F);
        printf("����ƥ����:%s,%s\r\n", t9.pymb[0]->py, t9.pymb[0]->pymb);
    }
    else if (t)
    {
        printf("\r\n��������Ϊ:%s\r\n", str);
        printf("��ȫƥ�����:%d\r\n", t);
        printf("��ȫƥ��Ľ��:\r\n");

        for (i = 0; i < t; i++)
        {
            printf("%s,%s\r\n", t9.pymb[i]->py, t9.pymb[i]->pymb);
        }
    }
    else
    {
        printf("û���κ�ƥ������\r\n");
    }
}





uint16_t kbdxsize;  /* ������̰������ */
uint16_t kbdysize;  /* ������̰����߶� */

/**
 * @brief       ���ؼ��̽���
 * @param       x, y : ������ʼ����
 * @retval      ��
 */
void py_load_ui(uint16_t x, uint16_t y)
{
    uint16_t i;
    lcd_draw_rectangle(x, y, x + kbdxsize * 3, y + kbdysize * 3, RED);
    lcd_draw_rectangle(x + kbdxsize, y, x + kbdxsize * 2, y + kbdysize * 3, RED);
    lcd_draw_rectangle(x, y + kbdysize, x + kbdxsize * 3, y + kbdysize * 2, RED);

    for (i = 0; i < 9; i++)
    {
        text_show_string_middle(x + (i % 3)*kbdxsize, y + 4 + kbdysize * (i / 3), (char *)kbd_tbl[i], 16, kbdxsize, BLUE);
        text_show_string_middle(x + (i % 3)*kbdxsize, y + kbdysize / 2 + kbdysize * (i / 3), (char *)kbs_tbl[i], 16, kbdxsize, BLUE);
    }
}

/**
 * @brief       ����״̬����
 * @param       x, y : ��������
 * @param       key  : ��ֵ��0~8��
 * @retval      ��Ӧ����״̬
 *   @arg       0, �ɿ�;
 *   @arg       1, ����;
 */
void py_key_staset(uint16_t x, uint16_t y, uint8_t keyx, uint8_t sta)
{
    uint16_t i = keyx / 3, j = keyx % 3;

    if (keyx > 8)return;

    if (sta)lcd_fill(x + j * kbdxsize + 1, y + i * kbdysize + 1, x + j * kbdxsize + kbdxsize - 1, y + i * kbdysize + kbdysize - 1, GREEN);
    else lcd_fill(x + j * kbdxsize + 1, y + i * kbdysize + 1, x + j * kbdxsize + kbdxsize - 1, y + i * kbdysize + kbdysize - 1, WHITE);

    text_show_string_middle(x + j * kbdxsize, y + 4 + kbdysize * i, (char *)kbd_tbl[keyx], 16, kbdxsize, BLUE);
    text_show_string_middle(x + j * kbdxsize, y + kbdysize / 2 + kbdysize * i, (char *)kbs_tbl[keyx], 16, kbdxsize, BLUE);
}

/**
 * @brief       �õ�������������
 * @param       x, y : ��������
 * @retval      ������ֵ(1~9 ��Ч��0, ��Ч;)
 */
uint8_t py_get_keynum(uint16_t x, uint16_t y)
{
    uint16_t i, j;
    static uint8_t key_x = 0;       /* 0,û���κΰ������£�1~9��1~9�Ű������� */
    uint8_t key = 0;
    tp_dev.scan(0);

    if (tp_dev.sta & TP_PRES_DOWN)  /* ������������ */
    {
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                if (tp_dev.x[0] < (x + j * kbdxsize + kbdxsize) && 
                    tp_dev.x[0] > (x + j * kbdxsize) && 
                    tp_dev.y[0] < (y + i * kbdysize + kbdysize) && 
                    tp_dev.y[0] > (y + i * kbdysize))
                {
                    key = i * 3 + j + 1;
                    break;
                }
            }

            if (key)
            {
                if (key_x == key)key = 0;
                else
                {
                    py_key_staset(x, y, key_x - 1, 0);
                    key_x = key;
                    py_key_staset(x, y, key_x - 1, 1);
                }

                break;
            }
        }
    }
    else if (key_x)
    {
        py_key_staset(x, y, key_x - 1, 0);
        key_x = 0;
    }

    return key;
}

/**
 * @brief       ��ʾ���
 * @param       index: ������
 *   @arg       0    , ��ʾû��һ��ƥ��Ľ��, ���֮ǰ����ʾ
 *   @arg       ���� , ������
 * @retval      ��
 */
void py_show_result(uint8_t index)
{
    lcd_show_num(30 + 144, 125, index, 1, 16, BLUE);            /* ��ʾ��ǰ������ */
    lcd_fill(30 + 40, 125, 30 + 40 + 48, 130 + 16, WHITE);      /* ���֮ǰ����ʾ */
    lcd_fill(30 + 40, 145, lcddev.width - 1, 145 + 48, WHITE);  /* ���֮ǰ����ʾ */

    if (index)
    {
        text_show_string(30 + 40, 125, 200, 16, (char *)t9.pymb[index - 1]->py, 16, 0, BLUE);   /* ��ʾƴ�� */
        text_show_string(30 + 40, 145, lcddev.width - 70, 48, (char *)t9.pymb[index - 1]->pymb, 16, 0, BLUE);   /* ��ʾ��Ӧ�ĺ��� */
        printf("\r\nƴ��:%s\r\n", t9.pymb[index - 1]->py);    /* �������ƴ�� */
        printf("���:%s\r\n", t9.pymb[index - 1]->pymb);      /* ���������� */
    }
}






















