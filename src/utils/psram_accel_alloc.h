#ifndef PSRAM_ACCEL_ALLOC
#define PSRAM_ACCEL_ALLOC

#include <stdint.h>
#include <stdbool.h>
#include "utils/alloc.h"


#ifdef NATIVE_64BIT
#include "utils/circular_buff.h"  /* provides bma_accel_data_t */
#else
#include "../hardware/motion.h"
#endif

/** Sentinel value in header_t.magic_number to validate that a PSRAM block
 *  is a valid accel block (not garbage or wrong pointer). Use when writing
 *  the header and when checking after wake from deep sleep. */
#define PSRAM_ACCEL_HEADER_MAGIC  0xAC
#define PSRAM_ACCEL_HEADER_VERSION  1


// DEFINE HEADER STRUCTURE
typedef struct header_t {
    uint8_t magic_number;  /* set to PSRAM_ACCEL_HEADER_MAGIC */
    uint8_t version;
    uint8_t header_size;    
    uint32_t num_elements;
    bool sync;
    uint32_t start_time_ms;
    uint32_t end_time_ms;
    uint32_t seq_num;
    uint32_t checksum;
    void* next;   /* next block (newer), nullptr for latest */

} header_t;


// RTC MEMORY ALLOCATION (declared here, defined once in psram_accel_alloc.cpp) //
extern header_t* current_accel_block_ptr;
extern header_t* first_accel_block_ptr;
extern uint32_t accel_block_valid_magic;
extern uint32_t accel_seq_counter;

// FUNCTIONS //
header_t* psram_write(int num_elements, bma_accel_data_t* buf);
void psram_init(void);
header_t* create_header(int num_elements, bma_accel_data_t* buf, void* ptr);
header_t* get_latest_block(void);
uint32_t checksum(const void* ptr, size_t len);
header_t *psram_read(int num_elements);



#endif