#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/*引脚配置层*/

/**
  * 函數：I2C寫SCL引脚電平
  * 參數：BitValue 協議層傳入的當前需要寫入SCL的電平，範圍0~1
  * 返回值：無
  * 注意事項：此函數需要用戶實現內容，當BitValue為0時，需要置SCL為低電平，當BitValue為1時，需要置SCL為高電平
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
    if(BitValue == 0) {
        GPIOB->BRR = GPIO_Pin_10;         // 寫0至位重置寄存器，將SCL設為低電平
    } else {
        GPIOB->BSRR = GPIO_Pin_10;        // 寫1至位設置寄存器，將SCL設為高電平
    }
    Delay_us(10);                        // 延時10us，防止時序頻率超過要求
}


/**
  * 函數：I2C寫SDA引脚電平
  * 參數：BitValue 協議層傳入的當前需要寫入SDA的電平，範圍0~0xFF
  * 返回值：無
  * 注意事項：此函數需要用戶實現內容，當BitValue為0時，需要置SDA為低電平，當BitValue非0時，需要置SDA為高電平
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
    if(BitValue == 0) {
        GPIOB->BRR = GPIO_Pin_11;         // 寫0至位重置寄存器，將SDA設為低電平
    } else {
        GPIOB->BSRR = GPIO_Pin_11;        // 寫1至位設置寄存器，將SDA設為高電平
    }
    Delay_us(10);                        // 延時10us，防止時序頻率超過要求
}


/**
  * 函數：I2C讀SDA引脚電平
  * 參數：無
  * 返回值：協議層需要得到的當前SDA的電平，範圍0~1
  * 注意事項：此函數需要用戶實現內容，當前SDA為低電平時，返回0，當前SDA為高電平時，返回1
  */
uint8_t MyI2C_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = (GPIOB->IDR & GPIO_Pin_11) ? 1 : 0;   // 讀取SDA電平，如果PB11的IDR是1，返回1；否則返回0
    Delay_us(10);                                    // 延時10us，防止時序頻率超過要求
    return BitValue;                                 // 返回SDA電平
}

/**
  * 函數：I2C初始化
  * 參數：無
  * 返回值：無
  * 注意事項：此函數需要用戶實現內容，實現SCL和SDA引脚的初始化
  */
void MyI2C_Init(void)
{
    //開啟時鐘
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;              // 開啟GPIOB的時鐘

    //GPIO初始化 
    GPIOB->CRH &= ~((GPIO_CRH_MODE10 | GPIO_CRH_CNF10) | (GPIO_CRH_MODE11 | GPIO_CRH_CNF11));
	// 將PB10(SCL)和PB11(SDA)設為開漏輸出模式且速度為50MHz速度
	GPIOB->CRH |= (0x07 << (4 * (10 - 8))) | (0x07 << (4 * (11 - 8)));

    //設置默認電平
    GPIOB->BSRR = GPIO_Pin_10 | GPIO_Pin_11;         // 設置PB10和PB11引脚初始化後默認為高電平（釋放總線狀態）
}


/*協議層*/

/**
  * 函    數：I2C起始
  * 參    數：無
  * 返 回 值：無
  */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);							// 釋放SDA，確保SDA為高電平
	MyI2C_W_SCL(1);							// 釋放SCL，確保SCL為高電平
	MyI2C_W_SDA(0);							// 在SCL高電平期間，拉低SDA，產生起始信號
	MyI2C_W_SCL(0);							// 起始後把SCL也拉低，即為了佔用總線，也為了方便總線時序的拼接
}

/**
  * 函    數：I2C終止
  * 參    數：無
  * 返 回 值：無
  */
void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);							// 拉低SDA，確保SDA為低電平
	MyI2C_W_SCL(1);							// 釋放SCL，使SCL呈現高電平
	MyI2C_W_SDA(1);							// 在SCL高電平期間，釋放SDA，產生終止信號
}

/**
  * 函    數：I2C發送一個字節
  * 參    數：Byte 要發送的一個字節數據，範圍：0x00~0xFF
  * 返 回 值：無
  */
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)				// 循環8次，主機依次發送數據的每一位
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));	// 使用掩碼的方式取出Byte的指定一位數據並寫入到SDA線
		MyI2C_W_SCL(1);						// 釋放SCL，從機在SCL高電平期間讀取SDA
		MyI2C_W_SCL(0);						// 拉低SCL，主機開始發送下一位數據
	}
}

/**
  * 函    數：I2C接收一個字節
  * 參    數：無
  * 返 回 值：接收到的一個字節數據，範圍：0x00~0xFF
  */
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;					// 定義接收的數據，並賦初值0x00，此處必須賦初值0x00，後面會用到
	MyI2C_W_SDA(1);							// 接收前，主機先確保釋放SDA，避免干擾從機的數據發送
	for (i = 0; i < 8; i ++)				// 循環8次，主機依次接收數據的每一位
	{
		MyI2C_W_SCL(1);						// 釋放SCL，主機在SCL高電平期間讀取SDA
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}	// 讀取SDA數據，並存儲到Byte變量
														// 當SDA為1時，置變量指定位為1，當SDA為0時，不做處理，指定位為默認的初值0
		MyI2C_W_SCL(0);						// 拉低SCL，從機在SCL低電平期間寫入SDA
	}
	return Byte;							// 返回接收到的一個字節數據
}

/**
  * 函    數：I2C發送應答位
  * 參    數：Byte 要發送的應答位，範圍：0~1，0表示應答，1表示非應答
  * 返 回 值：無
  */
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);					// 主機把應答位數據放到SDA線
	MyI2C_W_SCL(1);							// 釋放SCL，從機在SCL高電平期間，讀取應答位
	MyI2C_W_SCL(0);							// 拉低SCL，開始下一個時序模塊
}

/**
  * 函    數：I2C接收應答位
  * 參    數：無
  * 返 回 值：接收到的應答位，範圍：0~1，0表示應答，1表示非應答
  */
uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;							// 定義應答位變量
	MyI2C_W_SDA(1);							// 接收前，主機先確保釋放SDA，避免干擾從機的數據發送
	MyI2C_W_SCL(1);							// 釋放SCL，主機在SCL高電平期間讀取SDA
	AckBit = MyI2C_R_SDA();					// 將應答位存儲到變量裡
	MyI2C_W_SCL(0);							// 拉低SCL，開始下一個時序模塊
	return AckBit;							// 返回應答位變量
}
