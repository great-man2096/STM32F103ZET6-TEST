/**
 ****************************************************************************************************
 * @file        pyinput.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-05
 * @brief       拼音输入法 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200405
 * 第一次发布
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

/* 数字表 */
const char *kbd_tbl[9] = {"←", "2", "3", "4", "5", "6", "7", "8", "9"};

/* 字符表 */
const char *kbs_tbl[9] = {"DEL", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

/* 拼音输入法 */
pyinput t9 =
{
    get_pymb,
    0,
};

/**
 * @brief       比较两个字符串的匹配情况
 * @param       str1      : 字符串1
 * @param       str2      : 字符串2
 * @retval      对比结果
 *   @arg       0XFF , 表示完全匹配
 *   @arg       其他 , 匹配的字符数
 */
uint8_t str_match(uint8_t *str1, uint8_t *str2)
{
    uint8_t i = 0;

    while (1)
    {
        if (*str1 != *str2)break;   /* 部分匹配 */

        if (*str1 == '\0')
        {
            i = 0XFF;   /* 完全匹配 */
            break;
        }

        i++;
        str1++;
        str2++;
    }

    return i;           /* 两个字符串相等 */
}

/**
 * @brief       获取匹配的拼音码表
 * @param       strin     : 输入的字符串,形如:"726"
 * @param       matchlist : 输出的匹配表
 * @retval      匹配状态
 *              [7]  , 0,表示完全匹配；1，表示部分匹配（仅在没有完全匹配的时候才会出现）
 *              [6:0], 完全匹配的时候，表示完全匹配的拼音个数
 *                     部分匹配的时候，表示有效匹配的位数
 */
uint8_t get_matched_pymb(uint8_t *strin, py_index **matchlist)
{
    py_index *bestmatch = 0;    /* 最佳匹配 */
    uint16_t pyindex_len = 0;
    uint16_t i = 0;
    uint8_t temp, mcnt = 0, bmcnt = 0;
    
    bestmatch = (py_index *)&py_index3[0];  /* 默认为a的匹配 */
    pyindex_len = sizeof(py_index3) / sizeof(py_index3[0]); /* 得到py索引表的大小 */

    for (i = 0; i < pyindex_len; i++)
    {
        temp = str_match(strin, (uint8_t *)py_index3[i].py_input);

        if (temp)
        {
            if (temp == 0XFF)
            {
                matchlist[mcnt++] = (py_index *)&py_index3[i];
            }
            else if (temp > bmcnt)  /* 找最佳匹配 */
            {
                bmcnt = temp;
                bestmatch = (py_index *)&py_index3[i];  /* 最好的匹配 */
            }
        }
    }

    if (mcnt == 0 && bmcnt)     /* 没有完全匹配的结果,但是有部分匹配的结果 */
    {
        matchlist[0] = bestmatch;
        mcnt = bmcnt | 0X80;    /* 返回部分匹配的有效位数 */
    }

    return mcnt;    /* 返回匹配的个数 */
}

/**
 * @brief       得到拼音码表
 * @param       str       : 输入字符串
 * @retval      匹配状态
 *              [7]  , 0,表示完全匹配；1，表示部分匹配（仅在没有完全匹配的时候才会出现）
 *              [6:0], 完全匹配的时候，表示完全匹配的拼音个数
 *                     部分匹配的时候，表示有效匹配的位数
 */
uint8_t get_pymb(uint8_t *str)
{
    return get_matched_pymb(str, t9.pymb);
}

/**
 * @brief       得到拼音码表(给USMART用的)
 * @param       str       : 输入字符串
 * @retval      无
 */
void test_py(uint8_t *str)
{
    uint8_t t = 0;
    uint8_t i = 0;
    t = t9.getpymb(str);

    if (t & 0X80)
    {
        printf("\r\n输入数字为:%s\r\n", str);
        printf("部分匹配位数:%d\r\n", t & 0X7F);
        printf("部分匹配结果:%s,%s\r\n", t9.pymb[0]->py, t9.pymb[0]->pymb);
    }
    else if (t)
    {
        printf("\r\n输入数字为:%s\r\n", str);
        printf("完全匹配个数:%d\r\n", t);
        printf("完全匹配的结果:\r\n");

        for (i = 0; i < t; i++)
        {
            printf("%s,%s\r\n", t9.pymb[i]->py, t9.pymb[i]->pymb);
        }
    }
    else
    {
        printf("没有任何匹配结果！\r\n");
    }
}





uint16_t kbdxsize;  /* 虚拟键盘按键宽度 */
uint16_t kbdysize;  /* 虚拟键盘按键高度 */

/**
 * @brief       加载键盘界面
 * @param       x, y : 界面起始坐标
 * @retval      无
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
 * @brief       按键状态设置
 * @param       x, y : 键盘坐标
 * @param       key  : 键值（0~8）
 * @retval      对应按键状态
 *   @arg       0, 松开;
 *   @arg       1, 按下;
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
 * @brief       得到触摸屏的输入
 * @param       x, y : 键盘坐标
 * @retval      按键键值(1~9 有效；0, 无效;)
 */
uint8_t py_get_keynum(uint16_t x, uint16_t y)
{
    uint16_t i, j;
    static uint8_t key_x = 0;       /* 0,没有任何按键按下；1~9，1~9号按键按下 */
    uint8_t key = 0;
    tp_dev.scan(0);

    if (tp_dev.sta & TP_PRES_DOWN)  /* 触摸屏被按下 */
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
 * @brief       显示结果
 * @param       index: 索引号
 *   @arg       0    , 表示没有一个匹配的结果, 清空之前的显示
 *   @arg       其他 , 索引号
 * @retval      无
 */
void py_show_result(uint8_t index)
{
    lcd_show_num(30 + 144, 125, index, 1, 16, BLUE);            /* 显示当前的索引 */
    lcd_fill(30 + 40, 125, 30 + 40 + 48, 130 + 16, WHITE);      /* 清除之前的显示 */
    lcd_fill(30 + 40, 145, lcddev.width - 1, 145 + 48, WHITE);  /* 清除之前的显示 */

    if (index)
    {
        text_show_string(30 + 40, 125, 200, 16, (char *)t9.pymb[index - 1]->py, 16, 0, BLUE);   /* 显示拼音 */
        text_show_string(30 + 40, 145, lcddev.width - 70, 48, (char *)t9.pymb[index - 1]->pymb, 16, 0, BLUE);   /* 显示对应的汉字 */
        printf("\r\n拼音:%s\r\n", t9.pymb[index - 1]->py);    /* 串口输出拼音 */
        printf("结果:%s\r\n", t9.pymb[index - 1]->pymb);      /* 串口输出结果 */
    }
}






















