#include "unity.h"
#include "utils/circular_buff.h"
#include <stdint.h>
#include "utils/alloc.h"
#include "utils/psram_accel_alloc.h"

/* setUp/tearDown in test_setup.cpp */


//-------- NATIVE TESTS --------//
// get memory
// get free memory?
// basically status checks
// initialization checks? 

//-------- ON DEVICE TESTS -------//

#ifdef ESP32
void test_psram_get_memory(void) {
    TEST_ASSERT_GREATER_THAN(0,ESP.getPsramSize());
    TEST_ASSERT_GREATER_THAN(0,ESP.getFreePsram());
}

void test_psram_init(void) {
    psram_init();
    TEST_ASSERT_EQUAL_INT(0, accel_block_valid_magic);
    TEST_ASSERT_EQUAL_INT(0, accel_seq_counter);
    TEST_ASSERT_NULL(first_accel_block_ptr);
    TEST_ASSERT_NULL(current_accel_block_ptr);
}


void test_write_to_psram(void) {
    bma_accel_data_t mock_val = {1, 2, 3, 1000};

    TEST_ASSERT_NOT_NULL(psram_write(1, &mock_val));

    TEST_ASSERT_EQUAL_INT(PSRAM_ACCEL_HEADER_MAGIC, current_accel_block_ptr->magic_number);
    TEST_ASSERT_EQUAL_INT(1, current_accel_block_ptr->version);
    TEST_ASSERT_EQUAL_INT(sizeof(header_t), current_accel_block_ptr->header_size);
    TEST_ASSERT_EQUAL_INT(1, current_accel_block_ptr->num_elements);
    TEST_ASSERT_EQUAL_INT(false, current_accel_block_ptr->sync);
    TEST_ASSERT_EQUAL_INT(1000, current_accel_block_ptr->start_time_ms);
    TEST_ASSERT_EQUAL_INT(1000, current_accel_block_ptr->end_time_ms);
    TEST_ASSERT_EQUAL_INT(0, current_accel_block_ptr->seq_num);
    TEST_ASSERT_NOT_EQUAL(0, current_accel_block_ptr->checksum);  /* computed by psram_write */
}

void test_read_from_psram(void) {
    bma_accel_data_t mock_val = {1, 2, 3, 1000};

    TEST_ASSERT_NOT_NULL(psram_write(1, &mock_val));
    header_t* read_header = psram_read(1);
    TEST_ASSERT_NOT_NULL(read_header);

    TEST_ASSERT_EQUAL_INT(PSRAM_ACCEL_HEADER_MAGIC, read_header->magic_number);
    TEST_ASSERT_EQUAL_INT(1, read_header->version);
    TEST_ASSERT_EQUAL_INT(sizeof(header_t), read_header->header_size);
    TEST_ASSERT_EQUAL_INT(1, read_header->num_elements);
    TEST_ASSERT_EQUAL_INT(false, read_header->sync);
    TEST_ASSERT_EQUAL_INT(1000, read_header->start_time_ms);
    TEST_ASSERT_EQUAL_INT(1000, read_header->end_time_ms);
    TEST_ASSERT_EQUAL_INT(0, read_header->seq_num);
    /* psram_read returning non-null already proves checksum validated */
    TEST_ASSERT_NOT_EQUAL(0, read_header->checksum);
}
#endif /* ESP32 */