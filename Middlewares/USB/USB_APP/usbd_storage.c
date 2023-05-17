/**
 ****************************************************************************************************
 * @file        usbd_storage.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-13
 * @brief       usbd_storage ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
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
 * V1.0 20200513
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "usbd_storage.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./BSP/NORFLASH/norflash.h"


/* �ļ�ϵͳ���ⲿ FLASH ����ʼ��ַ
 * ��Ϊ������FATFSʵ������涨, FATFSռ��FLASH��ǰ12M�ֽڿռ�
 * ���, ����ַ��0, ���FATFSʵ��, diskio.c��˵��
 */
#define USB_STORAGE_FLASH_BASE  0


/* �Լ������һ�����USB״̬�ļĴ���, �����ж�USB״̬
 * bit0 : ��ʾ����������SD��д������
 * bit1 : ��ʾ��������SD����������
 * bit2 : SD��д���ݴ����־λ
 * bit3 : SD�������ݴ����־λ
 * bit4 : 1,��ʾ��������ѯ����(�������ӻ�������)
 */
volatile uint8_t g_usb_state_reg = 0;



/* USB Mass storage ��׼��ѯ����(ÿ��lunռ36�ֽ�) */
const int8_t  STORAGE_Inquirydata[] =
{

    /* LUN 0 */
    0x00,
    0x80,
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 4),
    0x00,
    0x00,
    0x00,
    /* Vendor Identification */
    'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',/* 9�ֽ� */
    /* Product Identification */
    'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',/* 15�ֽ� */
    ' ', 'D', 'i', 's', 'k', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' ',                         /* 4�ֽ� */

    /* LUN 1 */
    0x00,
    0x80,
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 4),
    0x00,
    0x00,
    0x00,
    /* Vendor Identification */
    'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',    /* 9�ֽ� */
    /* Product Identification */
    'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',    /* 15�ֽ� */
    'D', 'i', 's', 'k', ' ', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' ',                             /* 4�ֽ� */
};

/* Private function prototypes ----------------------------------------------- */
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops =
{
    STORAGE_Init,                   /* �����ʼ�� */
    STORAGE_GetCapacity,            /* ��ȡ���� */
    STORAGE_IsReady,                /* ����豸����״̬ */
    STORAGE_IsWriteProtected,       /* ��ѯ�豸������״̬ */
    STORAGE_Read,                   /* ������ */
    STORAGE_Write,                  /* д���� */
    STORAGE_GetMaxLun,              /* ��ȡ������ */
    (int8_t *)STORAGE_Inquirydata,  /* �豸��Ϣ��ʶ */

};

/**
 * @brief       ��ʼ���洢�豸
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SD��
 *   @arg                  1, SPI FLASH
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_Init (uint8_t lun)
{
    uint8_t res = 0;

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_init();
            break;

        case 1: /* SD�� */
            res = sd_init();
            break;
    }

    return res;
}

/**
 * @brief       ��ȡ�洢�豸�������Ϳ��С
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SD��
 *   @arg                  1, SPI FLASH
 * @param       block_num  : ������(������)
 * @param       block_size : ���С(������С)
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    switch (lun)
    {
        case 0: /* SPI FLASH */
            *block_size = 512;
            *block_num = (1024 * 1024 * 12) / 512;   /* SPI FLASH�� 12MB �ֽ�, �ļ�ϵͳ�� */
            break;

        case 1: /* SD�� */
            *block_size = 512;
            *block_num = ((long long)g_sdcard_handler.SdCard.BlockNbr *g_sdcard_handler.SdCard.BlockSize)/512;//g_sd_card_info.CardCapacity / 512;
            break;
    }

    return 0;
}

/**
 * @brief       �鿴�洢�豸�Ƿ����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SD��
 *   @arg                  1, SPI FLASH
 * @retval      ����״̬
 *   @arg       0    , ����
 *   @arg       ���� , δ����
 */
int8_t  STORAGE_IsReady (uint8_t lun)
{
    g_usb_state_reg |= 0X10;    /* �����ѯ */
    return 0;
}

/**
 * @brief       �鿴�洢�豸�Ƿ�д����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SD��
 *   @arg                  1, SPI FLASH
 * @retval      ������״̬
 *   @arg       0    , û�ж�����
 *   @arg       ���� , �ж�����
 */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
    return  0;
}

/**
 * @brief       �Ӵ洢�豸��ȡ����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SD��
 *   @arg                  1, SPI FLASH
 * @param       buf        : ���ݴ洢���׵�ַָ��
 * @param       blk_addr   : Ҫ��ȡ�ĵ�ַ(������ַ)
 * @param       blk_len    : Ҫ��ȡ�Ŀ���(������)
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_Read (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    int8_t res = 0;
    g_usb_state_reg |= 0X02;    /* ������ڶ����� */

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_read(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            break;

        case 1: /* SD�� */
            res = sd_read_disk(buf, blk_addr, blk_len);
            break;
    }

    if (res)
    {
        printf("rerr:%d,%d\r\n", lun, res);
        g_usb_state_reg |= 0X08;    /* ������! */
    }

    return res;
}

/**
 * @brief       ��洢�豸д����
 * @param       lun        : �߼���Ԫ���
 *   @arg                  0, SD��
 *   @arg                  1, SPI FLASH
 * @param       buf        : ���ݴ洢���׵�ַָ��
 * @param       blk_addr   : Ҫд��ĵ�ַ(������ַ)
 * @param       blk_len    : Ҫд��Ŀ���(������)
 * @retval      �������
 *   @arg       0    , �ɹ�
 *   @arg       ���� , �������
 */
int8_t STORAGE_Write (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    int8_t res = 0;
    g_usb_state_reg |= 0X01;    /* �������д���� */

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_write(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            break;

        case 1: /* SD�� */
            res = sd_write_disk(buf, blk_addr, blk_len);
            break;
    }

    if (res)
    {
        g_usb_state_reg |= 0X04;    /* д����! */
        printf("werr:%d,%d\r\n", lun, res);
    }

    return res;
}

/**
 * @brief       ��ȡ֧�ֵ�����߼���Ԫ����
 *   @note      ע��, ���ﷵ�ص��߼���Ԫ�����Ǽ�ȥ��1��.
 *              0, �ͱ�ʾ1��; 1, ��ʾ2��; �Դ�����
 * @param       ��
 * @retval      ֧�ֵ��߼���Ԫ���� - 1
 */
int8_t STORAGE_GetMaxLun (void)
{
    /* STORAGE_LUN_NBR ��usbd_conf.h���涨��, Ĭ����2 */
    if (g_sdcard_handler.SdCard.BlockNbr*g_sdcard_handler.SdCard.BlockSize)    /* ���SD������, ��֧��2������ */
    {
        return STORAGE_LUN_NBR - 1;
    }
    else    /* SD��������, ��ֻ֧��1������ */
    {
        return STORAGE_LUN_NBR - 2;
    }
}





















