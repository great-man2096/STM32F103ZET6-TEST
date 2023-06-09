/**
 ****************************************************************************************************
 * @file        touch.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-25
 * @brief       触摸屏 驱动代码
 *   @note      支持电阻/电容式触摸屏
 *              触摸屏驱动（支持ADS7843/7846/UH7843/7846/XPT2046/TSC2046/GT9147/GT9271/FT5206等）代码
 *
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
 * V1.0 20200425
 * 第一次发布
 * V1.1 20200606
 * 校准函数, case 5里面, 判定差值不合格增加:
 * 判定差值等于0的判断, 差值不能等于0
 * 
 ****************************************************************************************************
 */

#include "stdio.h"
#include "stdlib.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/24CXX/24cxx.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LED/led.h"



_m_tp_dev tp_dev =
{
    tp_init,
    tp_scan,
    tp_adjust,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

/**
 * @brief   MCU向XTP2046发送命令，XTP2046返回数据
 *  @note    手撸代码
 * 
*/
uint16_t tp_write_and_read_ad(uint8_t cmd_data)
{
    uint16_t rd_data = 0;

    /* 开始状态 */
    T_CLK(0);
    T_CS(0);    /*选中触摸IC*/
    T_MOSI(0);
    /*SPI在上升沿采集数据，下降沿需要准备数据*/

    /*发送一字节命令参数*/
    for (uint8_t i = 0; i < 8; i++)
    {
        T_CLK(0);   /*开始准备数据*/
        if (cmd_data & 0x80)
        {
            T_MOSI(1);
        }else
        {
            T_MOSI(0);
        }
        delay_us(1);    /* 准备数据完成 */
        T_CLK(1);   /* 开始读取数据 */
        delay_us(1);    /* 读取数据完成 */
        cmd_data <<= 1;
    }

    /*过滤bush信号*/
    T_CLK(0);
    delay_us(1);
    T_CLK(1);
    delay_us(1);

    /*MCU读取XTP2046返回数据*/
    for (uint8_t i = 0; i < 16; i++)
    {
        T_CLK(0);
        delay_us(1);
        T_CLK(1);
        
        rd_data <<= 1;  /*空出最低位来保存读取到的数据*/
        rd_data |= T_MISO;  /* 读取数据 */
        delay_us(1);
    }

    /*结束*/
    T_CLK(0);
    T_CS(1);    /*取消片选*/

    return (rd_data >>= 4);


}


/**
 * @brief       SPI写数据
 *   @note      向触摸屏IC写入1 byte数据
 * @param       data: 要写入的数据
 * @retval      无
 */
static void tp_write_byte(uint8_t data)
{
    uint8_t count = 0;

    for (count = 0; count < 8; count++)
    {
        if (data & 0x80)    /* 发送1 */
        {
            T_MOSI(1);
        }
        else                /* 发送0 */
        {
            T_MOSI(0);
        }

        data <<= 1;
        T_CLK(0);
        delay_us(1);
        T_CLK(1);           /* 上升沿有效 */
    }
}

/**
 * @brief       SPI读数据
 *   @note      从触摸屏IC读取adc值
 * @param       cmd: 指令
 * @retval      读取到的数据,ADC值(12bit)
 */
static uint16_t tp_read_ad(uint8_t cmd)
{
    uint8_t count = 0;
    uint16_t num = 0;
    T_CLK(0);           /* 先拉低时钟 */
    T_MOSI(0);          /* 拉低数据线 */
    T_CS(0);            /* 选中触摸屏IC */
    tp_write_byte(cmd); /* 发送命令字 */
    delay_us(6);        /* ADS7846的转换时间最长为6us */
    T_CLK(0);
    delay_us(1);
    T_CLK(1);           /* 给1个时钟，清除BUSY */
    delay_us(1);
    T_CLK(0);

    for (count = 0; count < 16; count++)    /* 读出16位数据,只有高12位有效 */
    {
        num <<= 1;
        T_CLK(0);       /* 下降沿有效 */
        delay_us(1);
        T_CLK(1);

        if (T_MISO)num++;
    }

    num >>= 4;          /* 只有高12位有效. */
    T_CS(1);            /* 释放片选 */
    return num;
}

/* 电阻触摸驱动芯片 数据采集 滤波用参数 */
#define TP_READ_TIMES   5       /* 读取次数 */
#define TP_LOST_VAL     1       /* 丢弃值 */

/**
 * @brief       读取一个坐标值(x或者y)
 *   @note      连续读取TP_READ_TIMES次数据,对这些数据升序排列,
 *              然后去掉最低和最高TP_LOST_VAL个数, 取平均值
 *              设置时需满足: TP_READ_TIMES > 2*TP_LOST_VAL 的条件
 *
 * @param       cmd : 指令
 *   @arg       0XD0: 读取X轴坐标(@竖屏状态,横屏状态和Y对调.)
 *   @arg       0X90: 读取Y轴坐标(@竖屏状态,横屏状态和X对调.)
 *
 * @retval      读取到的数据(滤波后的), ADC值(12bit)
 */
static uint16_t tp_read_xoy(uint8_t cmd)
{
    uint16_t i, j;
    uint16_t buf[TP_READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;

    for (i = 0; i < TP_READ_TIMES; i++)   /* 先读取TP_READ_TIMES次数据 */
    {
        buf[i] = tp_read_ad(cmd);
    }

    for (i = 0; i < TP_READ_TIMES - 1; i++)   /* 对数据进行排序 */
    {
        for (j = i + 1; j < TP_READ_TIMES; j++)
        {
            if (buf[i] > buf[j])   /* 升序排列 */
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }

    sum = 0;

    for (i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++)   /* 去掉两端的丢弃值 */
    {
        sum += buf[i];  /* 累加去掉丢弃值以后的数据. */
    }

    temp = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL); /* 取平均值 */
    return temp;
}

/**
 * @brief       读取x, y坐标
 * @param       x,y: 读取到的坐标值
 * @retval      无
 */
static void tp_read_xy(uint16_t *x, uint16_t *y)
{
    uint16_t xval, yval;

    if (tp_dev.touchtype & 0X01)    /* X,Y方向与屏幕相反 */
    {
        xval = tp_read_xoy(0X90);   /* 读取X轴坐标AD值, 并进行方向变换 */
        yval = tp_read_xoy(0XD0);   /* 读取Y轴坐标AD值 */
    }
    else                            /* X,Y方向与屏幕相同 */
    {
        xval = tp_read_xoy(0XD0);   /* 读取X轴坐标AD值 */
        yval = tp_read_xoy(0X90);   /* 读取Y轴坐标AD值 */
    }

    *x = xval;
    *y = yval;
}

/* 连续两次读取X,Y坐标的数据误差最大允许值 */
#define TP_ERR_RANGE    50      /* 误差范围 */

/**
 * @brief       连续读取2次触摸IC数据, 并滤波
 *   @note      连续2次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE,满足
 *              条件,则认为读数正确,否则读数错误.该函数能大大提高准确度.
 *
 * @param       x,y: 读取到的坐标值
 * @retval      0, 失败; 1, 成功;
 */
static uint8_t tp_read_xy2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;

    tp_read_xy(&x1, &y1);   /* 读取第一次数据 */
    tp_read_xy(&x2, &y2);   /* 读取第二次数据 */

    /* 前后两次采样在+-TP_ERR_RANGE内 */
    if (((x2 <= x1 && x1 < x2 + TP_ERR_RANGE) || (x1 <= x2 && x2 < x1 + TP_ERR_RANGE)) &&
            ((y2 <= y1 && y1 < y2 + TP_ERR_RANGE) || (y1 <= y2 && y2 < y1 + TP_ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }

    return 0;
}

/******************************************************************************************/
/* 与LCD部分有关的函数, 用来校准用的 */

/**
 * @brief       画一个校准用的触摸点(十字架)
 * @param       x,y   : 坐标
 * @param       color : 颜色
 * @retval      无
 */
static void tp_draw_touch_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_draw_line(x - 12, y, x + 13, y, color); /* 横线 */
    lcd_draw_line(x, y - 12, x, y + 13, color); /* 竖线 */
    lcd_draw_point(x + 1, y + 1, color);
    lcd_draw_point(x - 1, y + 1, color);
    lcd_draw_point(x + 1, y - 1, color);
    lcd_draw_point(x - 1, y - 1, color);
    lcd_draw_circle(x, y, 6, color);            /* 画中心圈 */
}

/**
 * @brief       画一个大点(2*2的点)
 * @param       x,y   : 坐标
 * @param       color : 颜色
 * @retval      无
 */
void tp_draw_big_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_draw_point(x, y, color);       /* 中心点 */
    lcd_draw_point(x + 1, y, color);
    lcd_draw_point(x, y + 1, color);
    lcd_draw_point(x + 1, y + 1, color);
}

/******************************************************************************************/

/**
 * @brief       触摸按键扫描
 * @param       mode: 坐标模式
 *   @arg       0, 屏幕坐标;
 *   @arg       1, 物理坐标(校准等特殊场合用)
 *
 * @retval      0, 触屏无触摸; 1, 触屏有触摸;
 */
uint8_t tp_scan(uint8_t mode)
{
    if (T_PEN == 0)     /* 有按键按下 */
    {
        if (mode)       /* 读取物理坐标, 无需转换 */
        {
            tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]);
        }
        else if (tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]))     /* 读取屏幕坐标, 需要转换 */
        {
            /* 将X轴 物理坐标转换成逻辑坐标(即对应LCD屏幕上面的X坐标值) */
            tp_dev.x[0] = (signed short)(tp_dev.x[0] - tp_dev.xc) / tp_dev.xfac + lcddev.width / 2;

            /* 将Y轴 物理坐标转换成逻辑坐标(即对应LCD屏幕上面的Y坐标值) */
            tp_dev.y[0] = (signed short)(tp_dev.y[0] - tp_dev.yc) / tp_dev.yfac + lcddev.height / 2;
        }

        if ((tp_dev.sta & TP_PRES_DOWN) == 0)   /* 之前没有被按下 */
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;   /* 按键按下 */
            tp_dev.x[CT_MAX_TOUCH - 1] = tp_dev.x[0];   /* 记录第一次按下时的坐标 */
            tp_dev.y[CT_MAX_TOUCH - 1] = tp_dev.y[0];
        }
    }
    else
    {
        if (tp_dev.sta & TP_PRES_DOWN)      /* 之前是被按下的 */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;    /* 标记按键松开 */
        }
        else     /* 之前就没有被按下 */
        {
            tp_dev.x[CT_MAX_TOUCH - 1] = 0;
            tp_dev.y[CT_MAX_TOUCH - 1] = 0;
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
        }
    }

    return tp_dev.sta & TP_PRES_DOWN; /* 返回当前的触屏状态 */
}

/* TP_SAVE_ADDR_BASE定义触摸屏校准参数保存在EEPROM里面的位置(起始地址)
 * 占用空间 : 13字节.
 */
#define TP_SAVE_ADDR_BASE   40

/**
 * @brief       保存校准参数
 *   @note      参数保存在EEPROM芯片里面(24C02),起始地址为TP_SAVE_ADDR_BASE.
 *              占用大小为13字节
 * @param       无
 * @retval      无
 */
void tp_save_adjust_data(void)
{
    uint8_t *p = (uint8_t *)&tp_dev.xfac;   /* 指向首地址 */

    /* p指向tp_dev.xfac的地址, p+4则是tp_dev.yfac的地址
     * p+8则是tp_dev.xoff的地址,p+10,则是tp_dev.yoff的地址
     * 总共占用12个字节(4个参数)
     * p+12用于存放标记电阻触摸屏是否校准的数据(0X0A)
     * 往p[12]写入0X0A. 标记已经校准过.
     */
    at24cxx_write(TP_SAVE_ADDR_BASE, p, 12);                /* 保存12个字节数据(xfac,yfac,xc,yc) */
    at24cxx_write_one_byte(TP_SAVE_ADDR_BASE + 12, 0X0A);   /* 保存校准值 */
}

/**
 * @brief       获取保存在EEPROM里面的校准值
 * @param       无
 * @retval      0，获取失败，要重新校准
 *              1，成功获取数据
 */
uint8_t tp_get_adjust_data(void)
{
    uint8_t *p = (uint8_t *)&tp_dev.xfac;
    uint8_t temp = 0;

    /* 由于我们是直接指向tp_dev.xfac地址进行保存的, 读取的时候,将读取出来的数据
     * 写入指向tp_dev.xfac的首地址, 就可以还原写入进去的值, 而不需要理会具体的数
     * 据类型. 此方法适用于各种数据(包括结构体)的保存/读取(包括结构体).
     */
    at24cxx_read(TP_SAVE_ADDR_BASE, p, 12);                 /* 读取12字节数据 */
    temp = at24cxx_read_one_byte(TP_SAVE_ADDR_BASE + 12);   /* 读取校准状态标记 */

    if (temp == 0X0A)
    {
        return 1;
    }

    return 0;
}

/* 提示字符串 */
char *const TP_REMIND_MSG_TBL = "Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

/**
 * @brief       提示校准结果(各个参数)
 * @param       xy[5][2]: 5个物理坐标值
 * @param       px,py   : x,y方向的比例因子(约接近1越好)
 * @retval      无
 */
static void tp_adjust_info_show(uint16_t xy[5][2], double px, double py)
{
    uint8_t i;
    char sbuf[20];

    for (i = 0; i < 5; i++)   /* 显示5个物理坐标值 */
    {
        sprintf(sbuf, "x%d:%d", i + 1, xy[i][0]);
        lcd_show_string(40, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
        sprintf(sbuf, "y%d:%d", i + 1, xy[i][1]);
        lcd_show_string(40 + 80, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
    }

    /* 显示X/Y方向的比例因子 */
    lcd_fill(40, 160 + (i * 20), lcddev.width - 1, 16, WHITE);  /* 清除之前的px,py显示 */
    sprintf(sbuf, "px:%0.2f", px);
    sbuf[7] = 0; /* 添加结束符 */
    lcd_show_string(40, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
    sprintf(sbuf, "py:%0.2f", py);
    sbuf[7] = 0; /* 添加结束符 */
    lcd_show_string(40 + 80, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
}

/**
 * @brief       触摸屏校准代码
 *   @note      使用五点校准法(具体原理请百度)
 *              本函数得到x轴/y轴比例因子xfac/yfac及物理中心坐标值(xc,yc)等4个参数
 *              我们规定: 物理坐标即AD采集到的坐标值,范围是0~4095.
 *                        逻辑坐标即LCD屏幕的坐标, 范围为LCD屏幕的分辨率.
 *
 * @param       无
 * @retval      无
 */
void tp_adjust(void)
{
    uint16_t pxy[5][2];     /* 物理坐标缓存值 */
    uint8_t  cnt = 0;
    short s1, s2, s3, s4;   /* 4个点的坐标差值 */
    double px, py;          /* X,Y轴物理坐标比例,用于判定是否校准成功 */
    uint16_t outtime = 0;
    cnt = 0;

    lcd_clear(WHITE);       /* 清屏 */
    lcd_show_string(40, 40, 160, 100, 16, TP_REMIND_MSG_TBL, RED); /* 显示提示信息 */
    tp_draw_touch_point(20, 20, RED);   /* 画点1 */
    tp_dev.sta = 0;         /* 消除触发信号 */

    while (1)               /* 如果连续10秒钟没有按下,则自动退出 */
    {
        tp_dev.scan(1);     /* 扫描物理坐标 */

        if ((tp_dev.sta & 0xc000) == TP_CATH_PRES)   /* 按键按下了一次(此时按键松开了.) */
        {
            outtime = 0;
            tp_dev.sta &= ~TP_CATH_PRES;    /* 标记按键已经被处理过了. */

            pxy[cnt][0] = tp_dev.x[0];      /* 保存X物理坐标 */
            pxy[cnt][1] = tp_dev.y[0];      /* 保存Y物理坐标 */
            cnt++;

            switch (cnt)
            {
                case 1:
                    tp_draw_touch_point(20, 20, WHITE);                 /* 清除点1 */
                    tp_draw_touch_point(lcddev.width - 20, 20, RED);    /* 画点2 */
                    break;

                case 2:
                    tp_draw_touch_point(lcddev.width - 20, 20, WHITE);  /* 清除点2 */
                    tp_draw_touch_point(20, lcddev.height - 20, RED);   /* 画点3 */
                    break;

                case 3:
                    tp_draw_touch_point(20, lcddev.height - 20, WHITE); /* 清除点3 */
                    tp_draw_touch_point(lcddev.width - 20, lcddev.height - 20, RED);    /* 画点4 */
                    break;

                case 4:
                    lcd_clear(WHITE);   /* 画第五个点了, 直接清屏 */
                    tp_draw_touch_point(lcddev.width / 2, lcddev.height / 2, RED);  /* 画点5 */
                    break;

                case 5:     /* 全部5个点已经得到 */
                    s1 = pxy[1][0] - pxy[0][0]; /* 第2个点和第1个点的X轴物理坐标差值(AD值) */
                    s3 = pxy[3][0] - pxy[2][0]; /* 第4个点和第3个点的X轴物理坐标差值(AD值) */
                    s2 = pxy[3][1] - pxy[1][1]; /* 第4个点和第2个点的Y轴物理坐标差值(AD值) */
                    s4 = pxy[2][1] - pxy[0][1]; /* 第3个点和第1个点的Y轴物理坐标差值(AD值) */

                    px = (double)s1 / s3;       /* X轴比例因子 */
                    py = (double)s2 / s4;       /* Y轴比例因子 */

                    if (px < 0)px = -px;        /* 负数改正数 */
                    if (py < 0)py = -py;        /* 负数改正数 */

                    if (px < 0.95 || px > 1.05 || py < 0.95 || py > 1.05 ||     /* 比例不合格 */
                            abs(s1) > 4095 || abs(s2) > 4095 || abs(s3) > 4095 || abs(s4) > 4095 || /* 差值不合格, 大于坐标范围 */
                            abs(s1) == 0 || abs(s2) == 0 || abs(s3) == 0 || abs(s4) == 0            /* 差值不合格, 等于0 */
                       )
                    {
                        cnt = 0;
                        tp_draw_touch_point(lcddev.width / 2, lcddev.height / 2, WHITE); /* 清除点5 */
                        tp_draw_touch_point(20, 20, RED);   /* 重新画点1 */
                        tp_adjust_info_show(pxy, px, py);   /* 显示当前信息,方便找问题 */
                        continue;
                    }

                    tp_dev.xfac = (float)(s1 + s3) / (2 * (lcddev.width - 40));
                    tp_dev.yfac = (float)(s2 + s4) / (2 * (lcddev.height - 40));

                    tp_dev.xc = pxy[4][0];      /* X轴,物理中心坐标 */
                    tp_dev.yc = pxy[4][1];      /* Y轴,物理中心坐标 */

                    lcd_clear(WHITE);   /* 清屏 */
                    lcd_show_string(35, 110, lcddev.width, lcddev.height, 16, "Touch Screen Adjust OK!", BLUE); /* 校正完成 */
                    delay_ms(1000);
                    tp_save_adjust_data();

                    lcd_clear(WHITE);/* 清屏 */
                    return;/* 校正完成 */
            }
        }

        delay_ms(10);
        outtime++;

        if (outtime > 1000)
        {
            tp_get_adjust_data();
            break;
        }
    }

}

/**
 * @brief       触摸屏初始化
 * @param       无
 * @retval      0,没有进行校准
 *              1,进行过校准
 */
uint8_t tp_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    tp_dev.touchtype = 0;                   /* 默认设置(电阻屏 & 竖屏) */
    tp_dev.touchtype |= lcddev.dir & 0X01;  /* 根据LCD判定是横屏还是竖屏 */

    if (lcddev.id == 0X5510 || lcddev.id == 0X4342 || lcddev.id == 0X4384 || lcddev.id == 0X1018)  /* 电容触摸屏,4.3寸/10.1寸屏 */
    {
        gt9xxx_init();
        tp_dev.scan = gt9xxx_scan;      /* 扫描函数指向GT9147触摸屏扫描 */
        tp_dev.touchtype |= 0X80;       /* 电容屏 */
        return 0;
    }
    else if (lcddev.id == 0X1963 || lcddev.id == 0X7084 || lcddev.id == 0X7016)     /* SSD1963 7寸屏或者 7寸800*480/1024*600 RGB屏 */
    {
        if (!ft5206_init())             /* 触摸IC是FT系列的就执行ft5206_init函数以及使用ft5206_scan扫描函数 */
        {
            tp_dev.scan = ft5206_scan;  /* 扫描函数指向FT5206触摸屏扫描 */
        }
        else                            /* 触摸IC是GT系列的就执行gt9xxx_init函数以及使用gt9xxx_scan扫描函数 */
        {
            gt9xxx_init();
            tp_dev.scan = gt9xxx_scan;  /* 扫描函数指向GT9147触摸屏扫描 */
        }
        tp_dev.touchtype |= 0X80;       /* 电容屏 */
        return 0;
    }
    else
    {
        T_PEN_GPIO_CLK_ENABLE();    /* T_PEN脚时钟使能 */
        T_CS_GPIO_CLK_ENABLE();     /* T_CS脚时钟使能 */
        T_MISO_GPIO_CLK_ENABLE();   /* T_MISO脚时钟使能 */
        T_MOSI_GPIO_CLK_ENABLE();   /* T_MOSI脚时钟使能 */
        T_CLK_GPIO_CLK_ENABLE();    /* T_CLK脚时钟使能 */

        gpio_init_struct.Pin = T_PEN_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_INPUT;                 /* 输入 */
        gpio_init_struct.Pull = GPIO_PULLUP;                     /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;           /* 高速 */
        HAL_GPIO_Init(T_PEN_GPIO_PORT, &gpio_init_struct);       /* 初始化T_PEN引脚 */

        gpio_init_struct.Pin = T_MISO_GPIO_PIN;
        HAL_GPIO_Init(T_MISO_GPIO_PORT, &gpio_init_struct);      /* 初始化T_MISO引脚 */

        gpio_init_struct.Pin = T_MOSI_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;             /* 推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                     /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;           /* 高速 */
        HAL_GPIO_Init(T_MOSI_GPIO_PORT, &gpio_init_struct);      /* 初始化T_MOSI引脚 */

        gpio_init_struct.Pin = T_CLK_GPIO_PIN;
        HAL_GPIO_Init(T_CLK_GPIO_PORT, &gpio_init_struct);       /* 初始化T_CLK引脚 */

        gpio_init_struct.Pin = T_CS_GPIO_PIN;
        HAL_GPIO_Init(T_CS_GPIO_PORT, &gpio_init_struct);        /* 初始化T_CS引脚 */

        tp_read_xy(&tp_dev.x[0], &tp_dev.y[0]); /* 第一次读取初始化 */
        at24cxx_init();         /* 初始化24CXX */

        if (tp_get_adjust_data())
        {
            return 0;           /* 已经校准 */
        }
        else                    /* 未校准? */
        {
            lcd_clear(WHITE);   /* 清屏 */
            tp_adjust();        /* 屏幕校准 */
            tp_save_adjust_data();
        }

        tp_get_adjust_data();
    }

    return 1;
}


/**
 * @brief       清空屏幕并在右上角显示"RST"
 * @param       无
 * @retval      无
 */
void load_draw_dialog(void)
{
    lcd_clear(WHITE);                                                /* 清屏 */
    lcd_show_string(lcddev.width - 24, 0, 200, 16, 16, "RST", BLUE); /* 显示清屏区域 */
}



/**
 * @brief       画粗线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       size : 线条粗细程度
 * @param       color: 线的颜色
 * @retval      无
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size)
        return;

    delta_x = x2 - x1; /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1; /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0; /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0; /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x; /* 选取基本增量坐标轴 */
    else
        distance = delta_y;

    for (t = 0; t <= distance + 1; t++) /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       电阻触摸屏测试函数
 * @param       无
 * @retval      无
 */
void rtp_test(void)
{
    uint8_t i = 0;

    while (1)
    {
        tp_dev.scan(0);

        if (tp_dev.sta & TP_PRES_DOWN)  /* 触摸屏被按下 */
        {
            if (tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height)
            {
                if (tp_dev.x[0] > (lcddev.width - 24) && tp_dev.y[0] < 16)
                {
                    load_draw_dialog(); /* 清除 */
                }
                else 
                {
                    tp_draw_big_point(tp_dev.x[0], tp_dev.y[0], RED);   /* 画点 */
                }
            }
        }
        else 
        {
            delay_ms(10);       /* 没有按键按下的时候 */
        }
        
        if (key1_scan())   /* KEY0按下,则执行校准程序 */
        {
            lcd_clear(WHITE);   /* 清屏 */
            tp_adjust();        /* 屏幕校准 */
            tp_save_adjust_data();
            load_draw_dialog();
        }

        i++;

        if (i % 20 == 0)LED0_TOGGLE();
    }
}

/* 10个触控点的颜色(电容触摸屏用) */
const uint16_t POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, YELLOW, MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY};

/**
 * @brief       电容触摸屏测试函数
 * @param       无
 * @retval      无
 */
void ctp_test(void)
{
    uint8_t t = 0;
    uint8_t i = 0;
    uint16_t lastpos[10][2];        /* 最后一次的数据 */
    uint8_t maxp = 5;

    if (lcddev.id == 0X1018)maxp = 10;

    while (1)
    {
        tp_dev.scan(0);

        for (t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if (tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)  /* 坐标在屏幕范围内 */
                {
                    if (lastpos[t][0] == 0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }

                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]); /* 画线 */
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];

                    if (tp_dev.x[t] > (lcddev.width - 24) && tp_dev.y[t] < 20)
                    {
                        load_draw_dialog();/* 清除 */
                    }
                }
            }
            else 
            {
                lastpos[t][0] = 0XFFFF;
            }
        }

        delay_ms(5);
        i++;

        if (i % 20 == 0)LED0_TOGGLE();
    }
}


