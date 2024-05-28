#include "stm32f10x.h"                  // 設備頭文件
#include "Delay.h"
#include "PWM.h"

uint8_t i;  // 定義 for 循環的變量

int main(void) {
    /* 模塊初始化 */
    PWM_Init();   // PWM 初始化

    while (1) {
        for (i = 0; i <= 100; i++) {
            PWM_SetCompare1(i);  // 依次將定時器的 CCR 寄存器設置為 0~100，PWM 占空比逐漸增大，LED 逐漸變亮
            Delay_ms(30);        // 延時 30ms
        }
        for (i = 0; i <= 100; i++) {
            PWM_SetCompare1(100 - i);  // 依次將定時器的 CCR 寄存器設置為 100~0，PWM 占空比逐漸減小，LED 逐漸變暗
            Delay_ms(30);              // 延時 30ms
        }
    }
}
