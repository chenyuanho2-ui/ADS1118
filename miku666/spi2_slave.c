#include "spi2_slave.h"
#include "spi.h"
#include "temp_filter.h"
#include "ads1118.h"

#include <string.h>        // 修复 memset 警告
#include <stdio.h>         // 修复 printf 警告

extern SPI_HandleTypeDef hspi2;

static SPI2_Frame_t tx_frame;
static uint8_t tx_buffer[SPI2_FRAME_SIZE];

static uint16_t Calculate_CRC16(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (uint16_t j = 0; j < 8; j++) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

static void Build_Frame(void)
{
    tx_frame.header1 = SPI2_FRAME_HEADER1;
    tx_frame.header2 = SPI2_FRAME_HEADER2;
    tx_frame.length   = SPI2_FRAME_SIZE;
    tx_frame.crc16   = 0;
    tx_frame.tail    = SPI2_FRAME_TAIL;
    tx_frame.padding[0] = 0;
    tx_frame.padding[1] = 0;

    uint16_t crc = Calculate_CRC16((uint8_t *)&tx_frame, SPI2_FRAME_SIZE - 3);
    tx_frame.crc16 = crc;
}

void SPI2_Slave_Init(void)
{
    memset(&tx_frame, 0, sizeof(tx_frame));
    tx_frame.cold_junction = 25.0f;
    Build_Frame();
    printf("[SPI2] Slave initialized\r\n");
}

void SPI2_Slave_UpdateData(float temp, float cold_junc)
{
    tx_frame.temperature   = temp;
    tx_frame.cold_junction = cold_junc;
    Build_Frame();
}

void SPI2_Slave_Transmit(void)
{
    Build_Frame();

    uint8_t dummy = 0;
    for (int i = 0; i < SPI2_FRAME_SIZE; i++) {
        uint8_t rx_byte = 0;
        uint8_t tx_byte = ((uint8_t *)&tx_frame)[i];
        HAL_SPI_TransmitReceive(&hspi2, &tx_byte, &rx_byte, 1, 100);
        tx_buffer[i] = tx_byte;
    }
}

void SPI2_Slave_Process(void)
{
    static uint32_t last_tick = 0;

    if (HAL_GetTick() - last_tick >= 100) {
        last_tick = HAL_GetTick();

        float temp = TempFilter_GetLatestAvgTemp();
        float cold = 25.0f;
        SPI2_Slave_UpdateData(temp, cold);
        SPI2_Slave_Transmit();

        printf("[SPI2 TX] ");
        for (int i = 0; i < SPI2_FRAME_SIZE; i++) {
            printf("%02X ", tx_buffer[i]);
        }
        printf("\r\n");
    }
}
