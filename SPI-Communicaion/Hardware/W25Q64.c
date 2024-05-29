#include "stm32f10x.h"                  // 設備頭文件
#include "MySPI.h"
#include "W25Q64_Ins.h"

/**
  * 函    數：W25Q64初始化
  * 參    數：無
  * 返 回 值：無
  */
void W25Q64_Init(void)
{
    MySPI_Init();                    // 先初始化底層的SPI
}

/**
  * 函    數：MPU6050讀取ID號
  * 參    數：MID 工廠ID，使用輸出參數的形式返回
  * 參    數：DID 裝置ID，使用輸出參數的形式返回
  * 返 回 值：無
  */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
    MySPI_Start();                               // SPI起始
    MySPI_SwapByte(W25Q64_JEDEC_ID);             // 交換發送讀取ID的指令
    *MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);    // 交換接收MID，通過輸出參數返回
    *DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);    // 交換接收DID高8位
    *DID <<= 8;                                  // 高8位移到高位
    *DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);   // 或上交換接收DID的低8位，通過輸出參數返回
    MySPI_Stop();                                // SPI終止
}

/**
  * 函    數：W25Q64寫使能
  * 參    數：無
  * 返 回 值：無
  */
void W25Q64_WriteEnable(void)
{
    MySPI_Start();                               // SPI起始
    MySPI_SwapByte(W25Q64_WRITE_ENABLE);         // 交換發送寫使能的指令
    MySPI_Stop();                                // SPI終止
}

/**
  * 函    數：W25Q64等待忙
  * 參    數：無
  * 返 回 值：無
  */
void W25Q64_WaitBusy(void)
{
    uint32_t Timeout;
    MySPI_Start();                                           // SPI起始
    MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);           // 交換發送讀狀態寄存器1的指令
    Timeout = 100000;                                        // 給定超時計數時間
    while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)  // 循環等待忙標誌位
    {
        Timeout --;                                          // 等待時，計數值自減
        if (Timeout == 0)                                    // 自減到0後，等待超時
        {
            /* 超時的錯誤處理代碼，可以添加到此處 */
            break;                                           // 跳出等待，不等了
        }
    }
    MySPI_Stop();                                            // SPI終止
}

/**
  * 函    數：W25Q64頁編程
  * 參    數：Address 頁編程的起始地址，範圍：0x000000~0x7FFFFF
  * 參    數：DataArray 用於寫入數據的數組
  * 參    數：Count 要寫入數據的數量，範圍：0~256
  * 返 回 值：無
  * 注意事項：寫入的地址範圍不能跨頁
  */
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
    uint16_t i;
    
    W25Q64_WriteEnable();                       // 寫使能
    
    MySPI_Start();                              // SPI起始
    MySPI_SwapByte(W25Q64_PAGE_PROGRAM);        // 交換發送頁編程的指令
    MySPI_SwapByte(Address >> 16);              // 交換發送地址23~16位
    MySPI_SwapByte(Address >> 8);               // 交換發送地址15~8位
    MySPI_SwapByte(Address);                    // 交換發送地址7~0位
    for (i = 0; i < Count; i ++)                // 循環Count次
    {
        MySPI_SwapByte(DataArray[i]);           // 依次在起始地址後寫入數據
    }
    MySPI_Stop();                               // SPI終止
    
    W25Q64_WaitBusy();                          // 等待忙
}

/**
  * 函    數：W25Q64扇區擦除（4KB）
  * 參    數：Address 指定扇區的地址，範圍：0x000000~0x7FFFFF
  * 返 回 值：無
  */
void W25Q64_SectorErase(uint32_t Address)
{
    W25Q64_WriteEnable();                       // 寫使能
    
    MySPI_Start();                              // SPI起始
    MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);    // 交換發送扇區擦除的指令
    MySPI_SwapByte(Address >> 16);              // 交換發送地址23~16位
    MySPI_SwapByte(Address >> 8);               // 交換發送地址15~8位
    MySPI_SwapByte(Address);                    // 交換發送地址7~0位
    MySPI_Stop();                               // SPI終止
    
    W25Q64_WaitBusy();                          // 等待忙
}

/**
  * 函    數：W25Q64讀取數據
  * 參    數：Address 讀取數據的起始地址，範圍：0x000000~0x7FFFFF
  * 參    數：DataArray 用於接收讀取數據的數組，通過輸出參數返回
  * 參    數：Count 要讀取數據的數量，範圍：0~0x800000
  * 返 回 值：無
  */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
    uint32_t i;
    MySPI_Start();                              // SPI起始
    MySPI_SwapByte(W25Q64_READ_DATA);           // 交換發送讀取數據的指令
    MySPI_SwapByte(Address >> 16);              // 交換發送地址23~16位
    MySPI_SwapByte(Address >> 8);               // 交換發送地址15~8位
    MySPI_SwapByte(Address);                    // 交換發送地址7~0位
    for (i = 0; i < Count; i ++)                // 循環Count次
    {
        DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);    // 依次在起始地址後讀取數據
    }
    MySPI_Stop();                               // SPI終止
}
