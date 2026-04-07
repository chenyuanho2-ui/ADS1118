#ifndef __TEMP_FILTER_H
#define __TEMP_FILTER_H

#include "main.h"

// 初始化统计变量
void TempFilter_Init(void);

// 核心处理函数：每 20ms 左右采样一次，每 100ms 打印一次平均值
// 建议在 main 循环中持续调用
void TempFilter_Process(void);
// 获取最新计算出的平均温度 (供 sensor_tx 引用)
float TempFilter_GetLatestAvgTemp(void);

#endif /* __TEMP_FILTER_H */
