#include "utils.h"
#include "stdlib.h"
#include <math.h>

double clamp(double x) { 
    return x < 0 ? 0 : x > 1 ? 1 : x; 
}

int toInt(double x) { 
    return int(pow(clamp(x), 1/2.2) * 255 + 0.5); 
}

// 基于线性同余算法实现erand48()逻辑
double erand48(unsigned short xsubi[3]) {
    const uint64_t a = 0x5DEECE66Dull;
    const uint64_t c = 0xB;
    uint64_t state = ((uint64_t)xsubi[0] << 32) | 
                    ((uint64_t)xsubi[1] << 16) | 
                    xsubi[2];
    
    state = (a * state + c) & 0xFFFFFFFFFFFFull; // 48位掩码
    
    // 更新状态数组
    xsubi[0] = (state >> 32) & 0xFFFF;
    xsubi[1] = (state >> 16) & 0xFFFF;
    xsubi[2] = state & 0xFFFF;
    
    // 生成[0,1)区间的双精度浮点数
    return static_cast<double>(state) / (1ull << 48);
}