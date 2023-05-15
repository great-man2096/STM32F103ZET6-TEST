/**
 ****************************************************************************************************
 * @file        fonts.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       字库 代码
 *              提供fonts_update_font和fonts_init用于字库更新和初始化
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200428
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __FONTS_H
#define __FONTS_H

#include "./SYSTEM/sys/sys.h"



/* 字体信息保存首地址
 * 占41个字节,第1个字节用于标记字库是否存在.后续每8个字节一组,分别保存起始地址和文件大小
 */
extern uint32_t FONTINFOADDR;

/* 字库信息结构体定义
 * 用来保存字库基本信息，地址，大小等
 */
typedef __PACKED_STRUCT
{
    uint8_t fontok;             /* 字库存在标志，0XAA，字库正常；其他，字库不存在 */
    uint32_t ugbkaddr;          /* unigbk的地址 */
    uint32_t ugbksize;          /* unigbk的大小 */
    uint32_t f12addr;           /* gbk12地址 */
    uint32_t gbk12size;         /* gbk12的大小 */
    uint32_t f16addr;           /* gbk16地址 */
    uint32_t gbk16size;         /* gbk16的大小 */
    uint32_t f24addr;           /* gbk24地址 */
    uint32_t gbk24size;         /* gbk24的大小 */
} _font_info;

/* 字库信息结构体 */
extern _font_info ftinfo;


uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);  /* 更新全部字库 */
uint8_t fonts_init(void);       /* 初始化字库 */




/*最大字体微软雅黑24 (32x41) */
#define MAX_HEIGHT_FONT         41
#define MAX_WIDTH_FONT          32
#define OFFSET_BITMAP           

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

//ASCII
typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
  
} sFONT;


//GB2312
typedef struct                                          // 汉字字模数据结构
{
  unsigned char index[2];                               // 汉字内码索引
  const char matrix[MAX_HEIGHT_FONT*MAX_WIDTH_FONT/8];  // 点阵码数据
}CH_CN;


typedef struct
{    
  const CH_CN *table;
  uint16_t size;
  uint16_t ASCII_Width;
  uint16_t Width;
  uint16_t Height;
  
}cFONT;

extern sFONT Font24;
extern sFONT Font20;
extern sFONT Font16;
extern sFONT Font12;
extern sFONT Font8;

extern cFONT Font12CN;
extern cFONT Font24CN;
#ifdef __cplusplus
}
#endif




extern const unsigned char gImage_1in3[];	//rotate 90





#endif





















