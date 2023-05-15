#ifndef __OLED_H
#define __OLED_H
#include "./SYSTEM/sys/sys.h"
#include "stdlib.h"

/* 引脚定义 */
#define OLED_CS_Pin GPIO_PIN_6
#define OLED_CS_GPIO_Port GPIOB
#define OLED_CS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define OLED_DC_Pin GPIO_PIN_7
#define OLED_DC_GPIO_Port GPIOB
#define OLED_DC_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

//OLED GPIO
#define OLED_CS_0		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET)
#define OLED_CS_1		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)

#define OLED_DC_0		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_1		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define OLED_1IN3_WIDTH  64//OLED width
#define OLED_1IN3_HEIGHT 128 //OLED height

void OLED_Init(void);
void OLED_1IN3_Clear(void);
void OLED_1IN3_Display(const UBYTE *Image);

#endif


