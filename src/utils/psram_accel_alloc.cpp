#include "psram_accel_alloc.h"
#include "utils/alloc.h"
#include <sys/time.h>
#include <stddef.h>
#include <string.h>



// what exactly am i doing here?
// so we take data from circ
// allocate a malloc
// memcpy to allocated malloc
// to test see if block size is the same?


void psram_init(void){
    first_accel_block_ptr = nullptr;
    current_accel_block_ptr = nullptr;
    accel_block_valid_magic = 0;
    accel_seq_counter = 0;
}

header_t* psram_write(int num_elements, bma_accel_data_t* buf){
    void* ptr = MALLOC(sizeof(header_t) + (num_elements * sizeof(bma_accel_data_t)));

    if (!ptr) {
        log_w("No memory available for psram write");
        return nullptr;
    }

    if (!first_accel_block_ptr) {
        first_accel_block_ptr = (header_t*)ptr;
    }


    header_t* h = create_header(num_elements, buf, ptr);
    /* Copy payload into block (after header). */
    if (num_elements > 0 && buf) {
        memcpy((char*)ptr + sizeof(header_t), buf, num_elements * sizeof(bma_accel_data_t));
    }

    /* Checksum last: over whole block with checksum field treated as zero. */
    h->checksum = checksum(ptr, sizeof(header_t) + num_elements * sizeof(bma_accel_data_t));
    

    if (current_accel_block_ptr) {
        header_t* old = (header_t*)current_accel_block_ptr;
        old->next = ptr;
    }

    current_accel_block_ptr = (header_t*)ptr;

    return (header_t*)ptr;

}

uint32_t checksum(const void* ptr, size_t len){
    uint32_t sum = 0;
    const uint8_t* p = (const uint8_t*)ptr;
    for (size_t i = 0; i < len; i++) {
        sum += p[i];
    }
    return sum;
}

/** Compute checksum over block excluding the 4-byte checksum field (for validation). */
static uint32_t checksum_block_for_validate(const void* ptr, size_t block_len){
    size_t cs_off = offsetof(header_t, checksum);
    uint32_t sum = 0;
    const uint8_t* p = (const uint8_t*)ptr;
    for (size_t i = 0; i < block_len; i++) {
        if (i >= cs_off && i < cs_off + sizeof(uint32_t)) continue;
        sum += p[i];
    }
    return sum;
}

header_t* create_header(int num_elements, bma_accel_data_t* buf, void* ptr){
    header_t* h = (header_t*)ptr;
    h->magic_number = PSRAM_ACCEL_HEADER_MAGIC;
    h->version = 1;
    h->header_size = sizeof(header_t);
    h->num_elements = num_elements;
    h->sync = false;
    h->next = nullptr;  /* this block is the latest */
    /* Block start/end from first/last sample so date is stored once per block. */
    if (num_elements > 0) {
        h->start_time_ms = buf[0].timestamp_ms;
        h->end_time_ms = buf[num_elements - 1].timestamp_ms;
    } else {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint32_t now_ms = (uint32_t)(tv.tv_sec * 1000UL + tv.tv_usec / 1000);
        h->start_time_ms = now_ms;
        h->end_time_ms = now_ms;
    }
    h->seq_num = accel_seq_counter++;
    h->checksum = 0;
    return h;

}
header_t* get_latest_block(void){
    if (!current_accel_block_ptr) return nullptr;
    header_t* current = (header_t*)current_accel_block_ptr;
    while (current->next) {
        current = (header_t*)current->next;
    }
    return current;
}
    



header_t *psram_read(int num_elements){

    //returned pointer is the block start, header then payload, head is at (bma_accel_data_t*)latest + sizeof(header_t)
    header_t* latest = get_latest_block();
    if (!latest) {
        log_w("No latest block found");
        return nullptr;
    }
    if (latest->magic_number != PSRAM_ACCEL_HEADER_MAGIC) {
        log_w("Invalid magic number in latest block");
        return nullptr;
    }
    size_t block_len = sizeof(header_t) + latest->num_elements * sizeof(bma_accel_data_t);
    if (latest->checksum != checksum_block_for_validate(latest, block_len)) {
        log_w("Invalid checksum in latest block");
        return nullptr;
    }
    if (latest->num_elements != num_elements) {
        log_w("Invalid number of elements in latest block");
        return nullptr;
    }
    return latest;
}