#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0		// MPU6050的I2C從機地址

/**
  * 函    數：MPU6050寫寄存器
  * 參    數：RegAddress 寄存器地址，範圍：參考MPU6050手冊的寄存器描述
  * 參    數：Data 要寫入寄存器的數據，範圍：0x00~0xFF
  * 返 回 值：無
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();						// I2C起始
	MyI2C_SendByte(MPU6050_ADDRESS);	// 發送從機地址，讀寫位為0，表示即將寫入
	MyI2C_ReceiveAck();					// 接收應答
	MyI2C_SendByte(RegAddress);			// 發送寄存器地址
	MyI2C_ReceiveAck();					// 接收應答
	MyI2C_SendByte(Data);				// 發送要寫入寄存器的數據
	MyI2C_ReceiveAck();					// 接收應答
	MyI2C_Stop();						// I2C終止
}

/**
  * 函    數：MPU6050讀寄存器
  * 參    數：RegAddress 寄存器地址，範圍：參考MPU6050手冊的寄存器描述
  * 返 回 值：讀取寄存器的數據，範圍：0x00~0xFF
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();						// I2C起始
	MyI2C_SendByte(MPU6050_ADDRESS);	// 發送從機地址，讀寫位為0，表示即將寫入
	MyI2C_ReceiveAck();					// 接收應答
	MyI2C_SendByte(RegAddress);			// 發送寄存器地址
	MyI2C_ReceiveAck();					// 接收應答
	
	MyI2C_Start();						// I2C重複起始
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);	// 發送從機地址，讀寫位為1，表示即將讀取
	MyI2C_ReceiveAck();					// 接收應答
	Data = MyI2C_ReceiveByte();			// 接收指定寄存器的數據
	MyI2C_SendAck(1);					// 發送應答，給從機非應答，終止從機的數據輸出
	MyI2C_Stop();						// I2C終止
	
	return Data;
}

/**
  * 函    數：MPU6050初始化
  * 參    數：無
  * 返 回 值：無
  */
void MPU6050_Init(void)
{
	MyI2C_Init();									// 先初始化底層的I2C
	
	/* MPU6050寄存器初始化，需要對照MPU6050手冊的寄存器描述配置，此處僅配置了部分重要的寄存器 */
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		// 電源管理寄存器1，取消睡眠模式，選擇時鐘源為X軸陀螺儀
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		// 電源管理寄存器2，保持默認值0，所有軸均不待機
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		// 采樣率分頻寄存器，配置采樣率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			// 配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	// 陀螺儀配置寄存器，選擇滿量程為±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	// 加速度計配置寄存器，選擇滿量程為±16g
}

/**
  * 函    數：MPU6050獲取ID號
  * 參    數：無
  * 返 回 值：MPU6050的ID號
  */
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);		// 返回WHO_AM_I寄存器的值
}

/**
  * 函    數：MPU6050獲取數據
  * 參    數：AccX AccY AccZ 加速度計X、Y、Z軸的數據，使用輸出參數的形式返回，範圍：-32768~32767
  * 參    數：GyroX GyroY GyroZ 陀螺儀X、Y、Z軸的數據，使用輸出參數的形式返回，範圍：-32768~32767
  * 返 回 值：無
  */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;								// 定義數據高8位和低8位的變量
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);		// 讀取加速度計X軸的高8位數據
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);		// 讀取加速度計X軸的低8位數據
	*AccX = (DataH << 8) | DataL;						// 數據拼接，通過輸出參數返回
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);		// 讀取加速度計Y軸的高8位數據
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);		// 讀取加速度計Y軸的低8位數據
	*AccY = (DataH << 8) | DataL;						// 數據拼接，通過輸出參數返回
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);		// 讀取加速度計Z軸的高8位數據
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);		// 讀取加速度計Z軸的低8位數據
	*AccZ = (DataH << 8) | DataL;						// 數據拼接，通過輸出參數返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);		// 讀取陀螺儀X軸的高8位數據
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);		// 讀取陀螺儀X軸的低8位數據
	*GyroX = (DataH << 8) | DataL;						// 數據拼接，通過輸出參數返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);		// 讀取陀螺儀Y軸的高8位數據
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);		// 讀取陀螺儀Y軸的低8位數據
	*GyroY = (DataH << 8) | DataL;						// 數據拼接，通過輸出參數返回
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);		// 讀取陀螺儀Z軸的高8位數據
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);		// 讀取陀螺儀Z軸的低8位數據
	*GyroZ = (DataH << 8) | DataL;						// 數據拼接，通過輸出參數返回
}
