#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"
/******************************************************************************************/
/* 引脚 定义 */

#define KEY0_GPIO_PORT                  GPIOE
#define KEY0_GPIO_PIN                   GPIO_PIN_0
#define KEY0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE口时钟使能 */

#define KEY1_GPIO_PORT                  GPIOC
#define KEY1_GPIO_PIN                   GPIO_PIN_13
#define KEY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PC口时钟使能 */


#define KEY2_GPIO_PORT                  GPIOA
#define KEY2_GPIO_PIN                   GPIO_PIN_1
#define KEY2_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */


#define KEY3_GPIO_PORT                  GPIOA
#define KEY3_GPIO_PIN                   GPIO_PIN_0
#define KEY3_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define KEY0        HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN)     /* 读取KEY0引脚 */
#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)     /* 读取KEY1引脚 */
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN)     /* 读取KEY1引脚 */
#define KEY3        HAL_GPIO_ReadPin(KEY3_GPIO_PORT, KEY3_GPIO_PIN)     /* 读取KEY1引脚 */

void key_init(void);
uint8_t key1_scan(void);
uint8_t key2_scan(void);
uint8_t key3_scan(void);
uint8_t key4_scan(void);
uint8_t key1_long_scan(void);
#endif
