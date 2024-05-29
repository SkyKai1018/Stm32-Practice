#include "stm32f10x.h"                  // 設備頭文件

/* 引腳配置層 */

/**
  * 函    數：SPI寫SS引腳電平
  * 參    數：BitValue 協議層傳入的當前需要寫入SS的電平，範圍0~1
  * 返 回 值：無
  * 注意事項：此函數需要用戶實現內容，當BitValue為0時，需要置SS為低電平，當BitValue為1時，需要置SS為高電平
  */
void MySPI_W_SS(uint8_t BitValue)
{
    if (BitValue)
        GPIOA->BSRR = GPIO_Pin_4;       // 設置SS引腳為高電平
    else
        GPIOA->BRR = GPIO_Pin_4;        // 設置SS引腳為低電平
}

/**
  * 函    數：SPI寫SCK引腳電平
  * 參    數：BitValue 協議層傳入的當前需要寫入SCK的電平，範圍0~1
  * 返 回 值：無
  * 注意事項：此函數需要用戶實現內容，當BitValue為0時，需要置SCK為低電平，當BitValue為1時，需要置SCK為高電平
  */
void MySPI_W_SCK(uint8_t BitValue)
{
    if (BitValue)
        GPIOA->BSRR = GPIO_Pin_5;       // 設置SCK引腳為高電平
    else
        GPIOA->BRR = GPIO_Pin_5;        // 設置SCK引腳為低電平
}

/**
  * 函    數：SPI寫MOSI引腳電平
  * 參    數：BitValue 協議層傳入的當前需要寫入MOSI的電平，範圍0~0xFF
  * 返 回 值：無
  * 注意事項：此函數需要用戶實現內容，當BitValue為0時，需要置MOSI為低電平，當BitValue非0時，需要置MOSI為高電平
  */
void MySPI_W_MOSI(uint8_t BitValue)
{
    if (BitValue)
        GPIOA->BSRR = GPIO_Pin_7;       // 設置MOSI引腳為高電平
    else
        GPIOA->BRR = GPIO_Pin_7;        // 設置MOSI引腳為低電平
}

/**
  * 函    數：SPI讀MISO引腳電平
  * 參    數：無
  * 返 回 值：協議層需要得到的當前MISO的電平，範圍0~1
  * 注意事項：此函數需要用戶實現內容，當前MISO為低電平時，返回0，當前MISO為高電平時，返回1
  */
uint8_t MySPI_R_MISO(void)
{
    return (GPIOA->IDR & GPIO_Pin_6) ? 1 : 0;  // 讀取MISO電平並返回
}

/**
  * 函    數：SPI初始化
  * 參    數：無
  * 返 回 值：無
  * 注意事項：此函數需要用戶實現內容，實現SS、SCK、MOSI和MISO引腳的初始化
  */
void MySPI_Init(void)
{
    /* 開啟時鐘 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // 開啟GPIOA的時鐘

    /* GPIO初始化 */
    GPIOA->CRL &= ~((GPIO_CRL_MODE4 | GPIO_CRL_CNF4) | 
                    (GPIO_CRL_MODE5 | GPIO_CRL_CNF5) | 
                    (GPIO_CRL_MODE7 | GPIO_CRL_CNF7)|
					(GPIO_CRL_MODE6 | GPIO_CRL_CNF6)); // 清除PA4~7的設置

	GPIOA->CRL=0x38330000;		//PA6 上拉輸入 PA4 PA5 PA7 推挽輸出 同時為50MHz

    /* 設置默認電平 */
    MySPI_W_SS(1);  // SS默認高電平
    MySPI_W_SCK(0); // SCK默認低電平
}


/*協議層*/

/**
  * 函    數：SPI起始
  * 參    數：無
  * 返 回 值：無
  */
void MySPI_Start(void)
{
	MySPI_W_SS(0);				//拉低SS，開始時序
}

/**
  * 函    數：SPI終止
  * 參    數：無
  * 返 回 值：無
  */
void MySPI_Stop(void)
{
	MySPI_W_SS(1);				//拉高SS，終止時序
}

/**
  * 函    數：SPI交換傳輸一個字節，使用SPI模式0
  * 參    數：ByteSend 要發送的一個字節
  * 返 回 值：接收的一個字節
  */
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	uint8_t i, ByteReceive = 0x00;					//定義接收的數據，並賦初值0x00，此處必須賦初值0x00，後面會用到
	
	for (i = 0; i < 8; i ++)						//循環8次，依次交換每一位數據
	{
		MySPI_W_MOSI(ByteSend & (0x80 >> i));		//使用掩碼的方式取出ByteSend的指定一位數據並寫入到MOSI線
		MySPI_W_SCK(1);								//拉高SCK，上升緣移出數據
		if (MySPI_R_MISO() == 1){ByteReceive |= (0x80 >> i);}	//讀取MISO數據，並存儲到Byte變量
																//當MISO為1時，置變量指定位為1，當MISO為0時，不做處理，指定位為默認的初值0
		MySPI_W_SCK(0);								//拉低SCK，下降緣移入數據
	}
	
	return ByteReceive;								//返回接收到的一個字節數據
}
