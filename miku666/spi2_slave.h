#ifndef __SPI2_SLAVE_H__
#define __SPI2_SLAVE_H__

#include "main.h"

#define SPI2_FRAME_SIZE    16
#define SPI2_FRAME_HEADER1 0x55
#define SPI2_FRAME_HEADER2 0xAA
#define SPI2_FRAME_TAIL    0xBB

#pragma pack(push, 1)
typedef struct {
    uint8_t  header1;
    uint8_t  header2;
    uint8_t  length;
    float    temperature;
    float    cold_junction;
    uint16_t crc16;
    uint8_t  tail;
    uint8_t  padding[2];
} __attribute__((packed)) SPI2_Frame_t;
#pragma pack(pop)

void SPI2_Slave_Init(void);
void SPI2_Slave_UpdateData(float temp, float cold_junc);
void SPI2_Slave_Transmit(void);
void SPI2_Slave_Process(void);

#endif
