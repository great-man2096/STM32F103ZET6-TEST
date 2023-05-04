#include "./BSP/CAN/can.h"

CAN_HandleTypeDef g_can1_handler={0};
CAN_TxHeaderTypeDef g_can1_txheader = {0};
CAN_RxHeaderTypeDef g_can1_rxheader = {0};

void can_init(void)
{
    g_can1_handler.Instance = CAN1;
    g_can1_handler.Init.Mode = CAN_MODE_LOOPBACK;   //loopback mode

    //波特率相关
    g_can1_handler.Init.Prescaler = 4;
    g_can1_handler.Init.TimeSeg1 = CAN_BS1_9TQ;
    g_can1_handler.Init.TimeSeg2 = CAN_BS2_8TQ;
    g_can1_handler.Init.SyncJumpWidth = CAN_SJW_1TQ;

    //CAN功能设置
    g_can1_handler.Init.TimeTriggeredMode = DISABLE;
    g_can1_handler.Init.AutoBusOff = DISABLE;
    g_can1_handler.Init.AutoWakeUp = DISABLE;
    g_can1_handler.Init.AutoRetransmission = DISABLE;
    g_can1_handler.Init.ReceiveFifoLocked = DISABLE;
    g_can1_handler.Init.TransmitFifoPriority = DISABLE;

    HAL_CAN_Init(&g_can1_handler);

    CAN_FilterTypeDef g_can1_filter = {0};

    g_can1_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    g_can1_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    g_can1_filter.FilterIdHigh = 0x0000;
    g_can1_filter.FilterIdLow = 0x0000;
    g_can1_filter.FilterMaskIdHigh = 0x0000;
    g_can1_filter.FilterMaskIdLow = 0x0000;

    g_can1_filter.FilterBank = 0;
    g_can1_filter.FilterFIFOAssignment = CAN_FilterFIFO0;
    g_can1_filter.FilterActivation = CAN_FILTER_ENABLE;
    g_can1_filter.SlaveStartFilterBank = 14;

    HAL_CAN_ConfigFilter(&g_can1_handler, &g_can1_filter);

    HAL_CAN_Start(&g_can1_handler);


}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
    CAN_TX_GPIO_CLK_ENABLE();
    CAN_RX_GPIO_CLK_ENABLE();
    __HAL_RCC_CAN1_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init_struct = {0};

    gpio_init_struct.Pin = CAN_TX_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CAN_TX_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = CAN_RX_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(CAN_RX_GPIO_PORT, &gpio_init_struct);

    // HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);

}

void can_send_msg(uint32_t id, uint8_t *msg, uint8_t len)
{
    uint32_t tx_mail = CAN_TX_MAILBOX0;
    g_can1_txheader.ExtId = id;
    g_can1_txheader.DLC = len;
    g_can1_txheader.IDE = CAN_ID_EXT;
    g_can1_txheader.RTR = CAN_RTR_DATA;

    HAL_CAN_AddTxMessage(&g_can1_handler ,&g_can1_txheader ,msg ,&tx_mail);
    while(HAL_CAN_GetTxMailboxesFreeLevel(&g_can1_handler) != 3);
}

uint8_t can_receive_msg(uint8_t *buf)
{
    if(HAL_CAN_GetRxFifoFillLevel(&g_can1_handler,CAN_RX_FIFO0) == 0)
    {
        return 0;
    }
    HAL_CAN_GetRxMessage(&g_can1_handler, CAN_RX_FIFO0, &g_can1_rxheader, buf);
    return g_can1_rxheader.DLC;
}


