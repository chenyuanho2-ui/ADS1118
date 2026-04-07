#ifndef __SENSOR_TX_H__
#define __SENSOR_TX_H__

#include "main.h"

// 结构体总长度：1(头) + 4(时间戳) + 4(温度float) + 2(CRC) + 1(尾) = 12 字节
#pragma pack(push, 1)
typedef struct {
    uint8_t  header;      // 帧头：0x55
    uint32_t timestamp;   // 时间戳
    float    temperature; // 温度数据 (二进制IEEE 754格式)
    uint16_t crc16;       // CRC-16 (Modbus)
    uint8_t  tail;        // 帧尾：0xAA
} SensorData_t;
#pragma pack(pop)

// 处理从机发送任务
void Sensor_Tx_Process(UART_HandleTypeDef *huart);

#endif /* __SENSOR_TX_H__ */
