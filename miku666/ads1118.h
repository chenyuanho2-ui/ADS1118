#ifndef __ADS1118_H
#define __ADS1118_H

#include "main.h"

// --- ¼Ä´æÆ÷ÅäÖÃºê¶¨Òå (ÌáÈ¡×ÔÉÌ¼ÒÀı³Ì) ---
#define ADS1118_SS_ONCE        (1 << 15) // µ¥´Î×ª»»¿ªÊ¼
#define ADS1118_MUX_01         (0 << 12) // AINP=AIN0, AINN=AIN1 (Ä¬ÈÏ)
#define ADS1118_MUX_0G         (4 << 12) // AINP=AIN0, AINN=GND
#define ADS1118_MUX_1G         (5 << 12) // AINP=AIN1, AINN=GND

#define ADS1118_PGA_256mV      (5 << 9)  // FSR = ¡À0.256V (ÊÊºÏÈÈµçÅ¼Î¢ÈõĞÅºÅ)
#define ADS1118_PGA_2048mV     (2 << 9)  // FSR = ¡À2.048V (Ä¬ÈÏ)

#define ADS1118_MODE_CONTINUE  (0 << 8)  // Á¬Ğø×ª»»Ä£Ê½
#define ADS1118_MODE_SINGLE    (1 << 8)  // µ¥´Î×ª»»Ä£Ê½ (ÍÆ¼ö)

#define ADS1118_DR_860SPS      (7 << 5)

#define ADS1118_TS_ADC         (0 << 4)  // ADC Ä£Ê½ (²âÍâ²¿µçÑ¹)
#define ADS1118_TS_TEMP        (1 << 4)  // ÄÚ²¿ÎÂ¶È´«¸ĞÆ÷Ä£Ê½ (Àä¶Ë²¹³¥)

#define ADS1118_PULLUP_EN      (1 << 3)  // DOUTÉÏÀ­Ê¹ÄÜ (Ä¬ÈÏ)
#define ADS1118_NOP_VALID      (1 << 1)  // Êı¾İÓĞĞ§£¬¸üĞÂÅäÖÃ

// æ›´æ–°å¸¸ç”¨é…ç½®ç»„åˆï¼Œä½¿ç”¨ 860SPS
#define CONFIG_TEMP_SENSOR  (ADS1118_SS_ONCE | ADS1118_MUX_01 | ADS1118_PGA_256mV | \
                             ADS1118_MODE_SINGLE | ADS1118_DR_860SPS | ADS1118_TS_TEMP | \
                             ADS1118_PULLUP_EN | ADS1118_NOP_VALID)

#define CONFIG_TC_ADC       (ADS1118_SS_ONCE | ADS1118_MUX_01 | ADS1118_PGA_256mV | \
                             ADS1118_MODE_SINGLE | ADS1118_DR_860SPS | ADS1118_TS_ADC | \
                             ADS1118_PULLUP_EN | ADS1118_NOP_VALID)

// º¯ÊıÉùÃ÷
void ADS1118_Init(void);
uint16_t ADS1118_WriteRead(uint16_t config_cmd);
float ADS1118_GetInternalTemp(void);
float ADS1118_GetVoltage_mV(void);

// æ–°å¢ï¼šæ¸©åº¦æ ¡å‡†å‡½æ•°
float ADS1118_CalibrateTemp(float measured_temp);

#endif /* __ADS1118_H */
