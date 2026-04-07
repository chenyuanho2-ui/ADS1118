#include "temp_filter.h"
#include "ads1118.h"
#include "thermocouple.h"
#include <stdio.h>

#define SAMPLE_INTERVAL_MS   20   // 采样间隔：20ms采样一次
#define PRINT_INTERVAL_MS    100  // 打印间隔：100ms打印一次平均值
// 新增：用于保存最新的平均温度
static float latest_avg_temp = 0.0f;

// 私有变量
static uint32_t last_sample_tick = 0;  // 用于控制约 20ms 的采样频率
static uint32_t last_print_tick = 0;   // 用于控制约 100ms 的打印频率
static float temp_accumulator = 0;      // 温度累加器
static uint32_t sample_count = 0;       // 采样计数器

void TempFilter_Init(void) {
    last_sample_tick = HAL_GetTick();
    last_print_tick = HAL_GetTick();
    temp_accumulator = 0;
    sample_count = 0;
    ADS1118_Init();
}

void TempFilter_Process(void) {
    uint32_t current_tick = HAL_GetTick();

    // --- 任务 A: 高频采样任务 (每 20ms 执行一次) ---
    // 根据 ADS1118 手册，860 SPS 下单次 ADC 转换需 1.16ms，温度转换需 1.2ms
    if (current_tick - last_sample_tick >= SAMPLE_INTERVAL_MS) {
        float current_temp = Get_Actual_Temperature(); // 该函数内部包含两次采样
        temp_accumulator += current_temp;
        sample_count++;
        last_sample_tick = current_tick;
    }

    // --- 任务 B: 打印平均值任务 (每 100ms 执行一次) ---
    if (current_tick - last_print_tick >= PRINT_INTERVAL_MS ) {
        if (sample_count > 0) {
            float average_temp = temp_accumulator / sample_count;
            
            // 更新最新平均温度
            latest_avg_temp = average_temp;
            
            // 打印 100ms 窗口内的平均温度
            printf("[Avg_100ms] Temp: %.2f C | Samples: %u \r\n", 
                   average_temp, sample_count);
            
            // 重置统计器
            temp_accumulator = 0;
            sample_count = 0;
        }
        last_print_tick = current_tick;
    }
}
	// 新增：对外提供获取最新温度的接口
float TempFilter_GetLatestAvgTemp(void) {
	return latest_avg_temp;
	
}
