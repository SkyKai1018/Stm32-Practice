#include "stm32f10x.h"                  // 設備頭檔
#include "Delay.h"
#include "OLED.h"
#include "W25Q64.h"

uint8_t MID;                            // 定義用於存放MID號的變數
uint16_t DID;                           // 定義用於存放DID號的變數

uint8_t ArrayWrite[] = {0xA2, 0xB4, 0xC6, 0xD8};   // 定義要寫入數據的測試陣列
uint8_t ArrayRead[4];                               // 定義要讀取數據的測試陣列

int main(void)
{
    /*模組初始化*/
    OLED_Init();                        // OLED初始化
    W25Q64_Init();                      // W25Q64初始化
    
    /*顯示靜態字串*/
    OLED_ShowString(1, 1, "MID:   DID:");
    OLED_ShowString(2, 1, "W:");
    OLED_ShowString(3, 1, "R:");
    
    /*顯示ID號*/
    W25Q64_ReadID(&MID, &DID);          // 獲取W25Q64的ID號
    OLED_ShowHexNum(1, 5, MID, 2);      // 顯示MID
    OLED_ShowHexNum(1, 12, DID, 4);     // 顯示DID
    
    /*W25Q64功能函數測試*/
    W25Q64_SectorErase(0x000000);                   // 扇區擦除
    W25Q64_PageProgram(0x000000, ArrayWrite, 4);    // 將寫入數據的測試陣列寫入到W25Q64中
    
    W25Q64_ReadData(0x000000, ArrayRead, 4);        // 讀取剛寫入的測試數據到讀取數據的測試陣列中
    
    /*顯示數據*/
    OLED_ShowHexNum(2, 3, ArrayWrite[0], 2);        // 顯示寫入數據的測試陣列
    OLED_ShowHexNum(2, 6, ArrayWrite[1], 2);
    OLED_ShowHexNum(2, 9, ArrayWrite[2], 2);
    OLED_ShowHexNum(2, 12, ArrayWrite[3], 2);
    
    OLED_ShowHexNum(3, 3, ArrayRead[0], 2);         // 顯示讀取數據的測試陣列
    OLED_ShowHexNum(3, 6, ArrayRead[1], 2);
    OLED_ShowHexNum(3, 9, ArrayRead[2], 2);
    OLED_ShowHexNum(3, 12, ArrayRead[3], 2);
    
    while (1)
    {
        
    }
}
