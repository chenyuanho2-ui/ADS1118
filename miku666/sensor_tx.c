#include "sensor_tx.h"
#include "temp_filter.h" // 引用滤波模块，以获取最新温度
#include <string.h> // 需要包含这个头文件

// 内部 CRC16 计算函数 (保持不变)
static uint16_t Calculate_CRC16(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (uint16_t j = 0; j < 8; j++) {
            if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}

void Sensor_Tx_Process(UART_HandleTypeDef *huart) {
    static uint32_t last_tx_tick = 0;
    SensorData_t packet;

    if (HAL_GetTick() - last_tx_tick >= 100) {
        last_tx_tick = HAL_GetTick();

        // 1. 填充基础信息
        packet.header = 0x55;
        
        // 安全填充时间戳
        uint32_t ts = HAL_GetTick();
        memcpy(&packet.timestamp, &ts, sizeof(uint32_t));
        
        // 安全填充温度
        float temp = TempFilter_GetLatestAvgTemp();
        memcpy(&packet.temperature, &temp, sizeof(float));

        // 3. 计算CRC
        uint16_t crc = Calculate_CRC16((uint8_t *)&packet, sizeof(SensorData_t) - 3);
        memcpy(&packet.crc16, &crc, sizeof(uint16_t));

        // 4. 填充帧尾
        packet.tail = 0xAA;

        // 5. 换成合理的超时时间 50ms
        HAL_UART_Transmit(huart, (uint8_t *)&packet, sizeof(SensorData_t), 50);
    }
}
