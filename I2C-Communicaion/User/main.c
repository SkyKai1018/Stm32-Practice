#include "stm32f10x.h"
#include "MPU6050.h" 
#include "OLED.h"
#include "Delay.h"
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b)) //取a和b中較大值的宏定義

#define ACCEL_X_OFFSET 800 //加速度計X軸偏移量
#define ACCEL_Y_OFFSET 1200 //加速度計Y軸偏移量

#define POSITION_X_MIN 1 //X軸位置最小值
#define POSITION_X_MAX 4 //X軸位置最大值
#define POSITION_Y_MIN 1 //Y軸位置最小值 
#define POSITION_Y_MAX 15 //Y軸位置最大值

int16_t AX, AY, AZ, GX, GY, GZ; //定義存放MPU6050數據的變數
int target_X, target_Y; //目標字串位置
int position_X=1, position_Y=15; //初始化位置
static uint32_t seed = 123456789; // 初始種子


//隨機生成目標字串位置和字符
void generate_target() {
    target_X = rand() % 4 + 1; // 生成1到4的隨機數
    target_Y = rand() % 15 + 1; // 生成1到15的隨機數
}

void update_position(int16_t ax, int16_t ay) {
	position_X = POSITION_X_MAX - (POSITION_X_MAX - POSITION_X_MIN) * (ax + ACCEL_X_OFFSET) / (ACCEL_X_OFFSET*2); //根據加速度計X軸數值更新X軸位置
	position_Y = POSITION_Y_MAX - (POSITION_Y_MAX - POSITION_Y_MIN) * (ay + ACCEL_Y_OFFSET) / (ACCEL_Y_OFFSET*2); //根據加速度計Y軸數值更新Y軸位置
}


int main(void) {
	SystemInit(); //系統初始化
	OLED_Init(); //OLED初始化
	MPU6050_Init(); //MPU6050初始化
	generate_target();
   
	while (1) {
	   MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ); //獲取MPU6050數據
	   update_position(AX, AY); //更新位置
	   OLED_Clear(); //清除OLED顯示

	   OLED_ShowString(target_X, target_Y, "T"); //在OLED上顯示目標
	   OLED_ShowString(position_X, position_Y, "o"); //在OLED上顯示字符"o"
	   
	   //判斷當前位置是否與目標字串位置相同
	   if (position_X == target_X && position_Y == target_Y) {
		   generate_target(); //生成新的目標字串
	   }
		
	   Delay_ms(50); //延遲100ms
	}
}