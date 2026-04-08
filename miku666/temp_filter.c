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

/* temp_filter.c */
void TempFilter_Process(void) {
    uint32_t current_tick = HAL_GetTick();

    // --- 任务 A: 高频采样、校准、滤波 (每 20ms 执行一次) ---
    if (current_tick - last_sample_tick >= SAMPLE_INTERVAL_MS) {
        // 1. 获取原始热电偶计算值
        float raw_temp = Get_Actual_Temperature(); 
        
        // 2. 立即进行 11 点校准
        float cal_temp = ADS1118_CalibrateTemp(raw_temp);
        
        // 3. 一阶低通滤波 (EMA) - 核心优化点
        // alpha 取 0.1~0.2，能彻底消除 ADC 噪声且保持曲线连续，不产生阶跃
        static float filtered_val = -999.0f;
        const float alpha = 0.15f; 
        if (filtered_val == -999.0f) filtered_val = cal_temp;
        filtered_val = alpha * cal_temp + (1.0f - alpha) * filtered_val;
        
        // 4. 更新供 PID 使用的全局变量 (20ms 更新一次，非常丝滑)
        latest_avg_temp = filtered_val;
        
        // 5. 累加用于 100ms 打印任务的平均值
        temp_accumulator += filtered_val;
        sample_count++;
        last_sample_tick = current_tick;
    }

    // --- 任务 B: 打印平均值任务 (每 100ms 执行一次，仅做显示) ---
    if (current_tick - last_print_tick >= PRINT_INTERVAL_MS ) {
        if (sample_count > 0) {
            float display_avg = temp_accumulator / sample_count;
            printf("[Monitor] Real-time Temp: %.2f C \r\n", display_avg);
            
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


