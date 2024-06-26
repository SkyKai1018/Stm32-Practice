## 目的

本文檔旨在說明如何在 STM32F10x 微控制器上結合 MPU6050 加速度計和 OLED 顯示屏實現一個基於運動控制的目標追踪遊戲。本應用利用 MPU6050 的加速度感測功能，實現通過傾斜設備來控制 OLED 上顯示的字符移動，以達到指定的目標位置。

## 流程說明

### 系統初始化

- **OLED 初始化**：配置 OLED 顯示屏，用於顯示遊戲的圖形和文本。
- **MPU6050 初始化**：設置 MPU6050 模組，用於實時捕捉和處理加速度數據。
- **隨機數初始化**：設置隨機數發生器的種子，用於生成遊戲中的隨機目標位置。

### 遊戲邏輯處理

- **目標生成**：隨機生成一個目標位置在 OLED 顯示屏上，玩家需通過移動設備來將顯示的字符對齊到此位置。
- **位置更新**：
  - 讀取加速度計數據，根據數據計算顯示字符的新位置。
  - 更新顯示字符的位置，根據設備的傾斜方向動態調整字符在顯示屏上的位置。

### 主循環

- **數據讀取與處理**：持續從 MPU6050 讀取加速度數據並更新字符位置。
- **顯示更新**：清空 OLED 顯示屏並重新繪製目標和字符。
- **目標達成檢查**：
  - 檢查字符是否達到了目標位置。
  - 如果目標達成，則重新生成一個目標位置。
- **延遲**：為了使遊戲運行平滑，設置一個短暫的延遲。

## 流程總結

1. **初始化階段**：設定所需的硬件模塊和變數，為遊戲運行做準備。
2. **遊戲邏輯處理**：透過 MPU6050 的加速度數據控制字符在 OLED 上的移動，並試圖將其導向隨機生成的目標位置。
3. **顯示和反饋**：透過 OLED 實時顯示遊戲狀態，並根據用戶操作提供即時的視覺反饋。
4. **互動反應**：遊戲會根據玩家的物理操作動態調整，提供一個互動性強的遊戲體驗。

![項目演示](images/example.gif)
