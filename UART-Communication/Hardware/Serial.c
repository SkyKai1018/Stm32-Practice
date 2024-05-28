#include "stm32f10x.h"  // 設備頭文件
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_RxData;  // 定義串口接收的數據變量
uint8_t Serial_RxFlag;  // 定義串口接收的標誌位變量

/**
 * 函    數：串口初始化
 * 參    數：無
 * 返 回 值：無
 */
void Serial_Init(void) {
    /* 開啟時鐘 */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // 開啟 USART1 的時鐘
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;    // 開啟 GPIOA 的時鐘

    /* GPIO 初始化 */
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1);  // 將 PA9 引腳初始化為復用推挽輸出

    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;  // 將 PA10 引腳初始化為上拉輸入

    /* USART 初始化 */
    USART1->BRR = 0x1D4C;  // 設置波特率為 9600，基於 72MHz 主頻
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;  // 使能發送和接收
    USART1->CR1 |= USART_CR1_UE;  // 使能 USART1

    /* 使能接收中斷 */
    USART1->CR1 |= USART_CR1_RXNEIE;

    /* NVIC 配置 */
    NVIC_SetPriority(USART1_IRQn, 1);  // 設置搶占優先級為 1
    NVIC_EnableIRQ(USART1_IRQn);  // 使能 USART1 中斷
}

/**
 * 函    數：串口發送一個字節
 * 參    數：Byte 要發送的一個字節
 * 返 回 值：無
 */
void Serial_SendByte(uint8_t Byte) {
    USART1->DR = Byte;
    while ((USART1->SR & USART_SR_TXE) == 0);  // 等待發送完成
}

/**
 * 函    數：獲取串口接收標誌位
 * 參    數：無
 * 返 回 值：串口接收標誌位，範圍：0~1，接收到數據後，標誌位置 1，讀取後標誌位自動清零
 */
uint8_t Serial_GetRxFlag(void) {
    if (Serial_RxFlag == 1) {
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}

/**
 * 函    數：獲取串口接收的數據
 * 參    數：無
 * 返 回 值：接收的數據，範圍：0~255
 */
uint8_t Serial_GetRxData(void) {
    return Serial_RxData;
}

/**
 * 函    數：USART1 中斷函數
 * 參    數：無
 * 返 回 值：無
 * 注意事項：此函數為中斷函數，無需調用，中斷觸發後自動執行
 *           函數名為預留的指定名稱，可以從啟動文件複製
 *           請確保函數名正確，不能有任何差異，否則中斷函數將不能進入
 */
void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE) {
        Serial_RxData = USART1->DR;
        Serial_RxFlag = 1;
    }
}
