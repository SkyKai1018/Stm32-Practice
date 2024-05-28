#include "stm32f10x.h"  // 設備頭文件

void PWM_Init(void) {
    // 開啟時鐘
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // 開啟 TIM2 的時鐘
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // 開啟 GPIOA 的時鐘

    // GPIO 初始化
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_CNF0_1;  // PA0 為復用推挽輸出

    // 配置時基單元
    TIM2->PSC = 720 - 1;  // 預分頻器
    TIM2->ARR = 100 - 1;  // 自動重裝載寄存器

    // 配置輸出比較單元
    TIM2->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_CC1S);
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;  // PWM 模式 1
    TIM2->CCER |= TIM_CCER_CC1E;  // 使能捕獲比較 1 輸出
    TIM2->CCR1 = 0;  // 初始占空比

    // 使能 TIM2
    TIM2->CR1 |= TIM_CR1_CEN;  // 使能 TIM2
}

/**
 * 函    數：PWM 設置 CCR
 * 參    數：Compare 要寫入的 CCR 的值，範圍：0~100
 * 返 回 值：無
 * 注意事項：CCR 和 ARR 共同決定占空比，此函數僅設置 CCR 的值，並不直接是占空比
 *           占空比 Duty = CCR / (ARR + 1)
 */
void PWM_SetCompare1(uint16_t Compare) {
    TIM2->CCR1 = Compare;  // 設置 CCR1 的值
}
