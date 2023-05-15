#include "./BSP/OLED/oled.h"
#include "./BSP/IIC/myiic.h"
#include "./SYSTEM/delay/delay.h"


static void OLED_1in3_WriteReg(uint8_t Reg)
{
	iic_start();
	iic_send_byte(0x78);
	iic_wait_ack();
	iic_send_byte(0x00);
	iic_wait_ack();
	iic_send_byte(Reg);
	iic_wait_ack();
	iic_stop();
}

static void OLED_1in3_WriteData(uint8_t Data)
{
	iic_start();
	iic_send_byte(0x78);
	iic_wait_ack();
	iic_send_byte(0x40);
	iic_wait_ack();
	iic_send_byte(Data);
	iic_wait_ack();
	iic_stop();
}

void OLED_Init()
{
    iic_init();
    //System_Init
    OLED_CS_0;
	OLED_DC_0;//DC = 0,1 >> Address = 0x3c,0x3d 
    //Hardware reset 做不到啊,只有四个引脚
    //Set the initialization register
    OLED_1in3_WriteReg(0xAE);//--turn off oled panel

    OLED_1in3_WriteReg(0x02);//---set low column address
    OLED_1in3_WriteReg(0x10);//---set high column address

    OLED_1in3_WriteReg(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_1in3_WriteReg(0x81);//--set contrast control register
    OLED_1in3_WriteReg(0xA0);//--Set SEG/Column Mapping a0/a1
    OLED_1in3_WriteReg(0xC0);//Set COM/Row Scan Direction
    OLED_1in3_WriteReg(0xA6);//--set normal display a6/a7
    OLED_1in3_WriteReg(0xA8);//--set multiplex ratio(1 to 64)
    OLED_1in3_WriteReg(0x3F);//--1/64 duty
    OLED_1in3_WriteReg(0xD3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    OLED_1in3_WriteReg(0x00);//-not offset
    OLED_1in3_WriteReg(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_1in3_WriteReg(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_1in3_WriteReg(0xD9);//--set pre-charge period
    OLED_1in3_WriteReg(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_1in3_WriteReg(0xDA);//--set com pins hardware configuration
    OLED_1in3_WriteReg(0x12);
    OLED_1in3_WriteReg(0xDB);//--set vcomh
    OLED_1in3_WriteReg(0x40);//Set VCOM Deselect Level
    OLED_1in3_WriteReg(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_1in3_WriteReg(0x02);//
    OLED_1in3_WriteReg(0xA4);// Disable Entire Display On (0xa4/0xa5)
    OLED_1in3_WriteReg(0xA6);// Disable Inverse Display On (0xa6/a7)

    delay_ms(200);
    //Turn on the OLED display
    OLED_1in3_WriteReg(0xaf);

}




void OLED_1IN3_Clear()
{
    // UWORD Width, Height;
    UWORD i, j;
    // Width = (OLED_1IN3_WIDTH % 8 == 0)? (OLED_1IN3_WIDTH / 8 ): (OLED_1IN3_WIDTH / 8 + 1);
    // Height = OLED_1IN3_HEIGHT; 
    for (i=0; i<8; i++) {
        /* set page address */
        OLED_1in3_WriteReg(0xB0 + i);
        /* set low column address */
        OLED_1in3_WriteReg(0x02);
        /* set high column address */
        OLED_1in3_WriteReg(0x10);
        for(j=0; j<128; j++) {
            /* write data */
            OLED_1in3_WriteData(0x00);  
        }
    }
}

/********************************************************************************
function:	Update memory to OLED
********************************************************************************/
void OLED_1IN3_Display(const UBYTE *Image)
{
    UWORD page, column, temp;

    for (page=0; page<8; page++) {
        /* set page address */
        OLED_1in3_WriteReg(0xB0 + page);
        /* set low column address */
        OLED_1in3_WriteReg(0x02);
        /* set high column address */
        OLED_1in3_WriteReg(0x10);

        /* write data */
        for(column=0; column<128; column++) {
            temp = Image[(7-page) + column*8];
            OLED_1in3_WriteData(temp);
        }       
    }
}




