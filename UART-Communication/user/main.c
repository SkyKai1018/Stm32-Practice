#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

#define DATA_LENGTH 3

uint8_t RxData = 0x41;			//定义用于接收串口数据的变量
uint8_t RxBuffer[DATA_LENGTH];  // 定義接收緩衝區
uint8_t RxIndex = 0;  // 接收數據索引

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "RxData:");
	
	/*串口初始化*/
	Serial_Init();		//串口初始化
	
	while (1) {
		if (Serial_GetRxFlag() == 1) {  // 檢查串口接收數據的標誌位
			RxBuffer[RxIndex++] = Serial_GetRxData();  // 獲取串口接收的數據

			if (RxIndex >= DATA_LENGTH) {  // 如果接收到完整數據
				RxIndex = 0;  // 重置索引
				for (uint8_t i = 0; i < DATA_LENGTH; i++) {
					Serial_SendByte(RxBuffer[i]);  // 串口將收到的數據回傳，用於測試
					OLED_ShowHexNum(1, 8 + i * 3, RxBuffer[i], 2);  // 顯示串口接收的數據
				}
			}
		}
    }
}
