#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef NATIVE_64BIT
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
    uint32_t timestamp_ms;
} bma_accel_data_t;
#else
#include "../hardware/motion.h"
#endif



#endif