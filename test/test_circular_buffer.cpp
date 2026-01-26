#include "unity.h"
#include "../src/utils/circular_buff.cpp"   // include the implementation directly
#include "utils/circular_buff.h"           // include the header
#include <stdint.h>

#define MAX_G 2047
#define MIN_G -2048

// STRUCT FOR PSEUDORANDOM NUMBER GENERATOR //
typedef struct {
    uint32_t state;
} rng_t;





void setUp(void){

}

void tearDown(void){

}

// HELPER FUNCTIONS //

static inline void rng_seed(rng_t *rng, uint32_t seed)
{
    rng->state = seed ? seed : 0xDEADBEEF;
}

static inline uint32_t rng_next(rng_t *rng)
{
    uint32_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

static int16_t clamp_12bit(int32_t v)
{
    if (v > MAX_G) return MAX_G;
    if (v < MIN_G) return MIN_G;
    return (int16_t)v;
}

bma_accel_data_t gen_mock_val(void)
{
    static rng_t rng;
    static uint32_t ts = 0;
    static int seeded = 0;

    if (!seeded) {
        rng_seed(&rng, 1234); // deterministic seed
        ts = 0;
        seeded = 1;
    }

    bma_accel_data_t v;

    v.x = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
    v.y = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
    v.z = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));

    v.timestamp_ms = ts;
    ts += 40; // assuming 50 Hz sample rate

    return v;
}


void gen_mock_array(bma_accel_data_t* arr, int n) {
    for (int i=0;i<n;i++)
        arr[i] = gen_mock_val();
}

//BASIC LOGIC AND FUNCTIONAL TESTING//
void test_circular_buffer_empty_after_init(){

    circular_buff_t buff;

    circular_buff_init(&buff);

    TEST_ASSERT_TRUE(circular_buff_empty(&buff));
    


}

void test_circular_buffer_write(){

    circular_buff_t buff;
    circular_buff_init(&buff);



    bma_accel_data_t mock_val = {1,2,3,1000};
    circular_buff_write(mock_val,&buff);

    TEST_ASSERT_EQUAL_INT(1,buff.head);
    TEST_ASSERT_EQUAL_INT(1,buff.count);
    TEST_ASSERT_EQUAL_INT(0,buff.tail);


}


void test_circular_buffer_read(){

    circular_buff_t buff; 
    bma_accel_data_t mock_val;


    circular_buff_init(&buff);

    // CREATE MOCK VALUES //
    mock_val = {1,2,3,1000};

    circular_buff_write(mock_val,&buff);
    int count_exp = buff.count-1;
    int tail_exp = buff.tail+1;




    bma_accel_data_t read_val;
    circular_buff_read(&buff,&read_val);


    // check if tail is incrementing // 

    TEST_ASSERT_EQUAL(1,read_val.x);
    TEST_ASSERT_EQUAL(2,read_val.y);
    TEST_ASSERT_EQUAL(3,read_val.z);
    TEST_ASSERT_EQUAL(1000,read_val.timestamp_ms);
    TEST_ASSERT_EQUAL(count_exp,buff.count);
    TEST_ASSERT_EQUAL(tail_exp, buff.tail);


}


void test_circular_buffer_write_seq(){

    circular_buff_t buff;
    circular_buff_init(&buff);
    int num_samples = 100;

    bma_accel_data_t mock_val_arr[num_samples];

    gen_mock_array(mock_val_arr,num_samples);

    for(int s = 0; s<num_samples;s++){
        circular_buff_write(mock_val_arr[s],&buff);
    }

    TEST_ASSERT_EQUAL_INT(num_samples,buff.head);
    TEST_ASSERT_EQUAL_INT(num_samples,buff.count);
    TEST_ASSERT_EQUAL_INT(0,buff.tail);



}

void test_circular_buff_read_seq(){

    circular_buff_t buff;
    circular_buff_init(&buff);
    int num_samples = 100;

    bma_accel_data_t mock_val_arr[num_samples];
    gen_mock_array(mock_val_arr,num_samples);

    for(int s = 0; s<num_samples; s++){
        circular_buff_write(mock_val_arr[s],&buff);
    }

    for(int s = 0; s<num_samples; s++){
        bma_accel_data_t stored_samp;
        circular_buff_read(&buff,&stored_samp);
        TEST_ASSERT_EQUAL_INT16(mock_val_arr[s].x,stored_samp.x);
        TEST_ASSERT_EQUAL_INT16(mock_val_arr[s].y,stored_samp.y);
        TEST_ASSERT_EQUAL_INT16(mock_val_arr[s].z,stored_samp.z);
        TEST_ASSERT_EQUAL_INT32(mock_val_arr[s].timestamp_ms,stored_samp.timestamp_ms);
        
    }

    TEST_ASSERT_EQUAL_INT(num_samples,buff.tail);
    TEST_ASSERT_EQUAL_INT(0,buff.count);
    TEST_ASSERT_EQUAL_INT(num_samples,buff.head);

}

void test_circular_buffer_sequential_readwrite(void)
{
    circular_buff_t buff;
    circular_buff_init(&buff);

    const int n = 100;

    bma_accel_data_t a[n];
    bma_accel_data_t b[n];

    gen_mock_array(a, n);
    gen_mock_array(b, n);

    /* -------------------------------------------------
     * Phase 1: Write A
     * ------------------------------------------------- */
    for (int i = 0; i < n; i++) {
        circular_buff_write(a[i], &buff);
    }

    /* -------------------------------------------------
     * Phase 2: Read half of A
     * ------------------------------------------------- */
    for (int i = 0; i < n / 2; i++) {

        bma_accel_data_t out;
        circular_buff_read(&buff,&out);

        TEST_ASSERT_EQUAL_MEMORY(
            &a[i],
            &out,
            sizeof(bma_accel_data_t)
        );
    }

    /* -------------------------------------------------
     * Phase 3: Write B
     * ------------------------------------------------- */
    for (int i = 0; i < n; i++) {
        circular_buff_write(b[i], &buff);
    }

    /* -------------------------------------------------
     * Phase 4: Read remainder of A
     * ------------------------------------------------- */
    for (int i = n / 2; i < n; i++) {
        bma_accel_data_t out;
        circular_buff_read(&buff,&out);

        TEST_ASSERT_EQUAL_MEMORY(
            &a[i],
            &out,
            sizeof(bma_accel_data_t)
        );
    }

    /* -------------------------------------------------
     * Phase 5: Read all of B
     * ------------------------------------------------- */
    for (int i = 0; i < n; i++) {
        bma_accel_data_t out;
        circular_buff_read(&buff,&out);

        TEST_ASSERT_EQUAL_MEMORY(
            &b[i],
            &out,
            sizeof(bma_accel_data_t)
        );
    }
}


void test_circular_buffer_overwrite(void)
{
    circular_buff_t buff;
    circular_buff_init(&buff);

    const int capacity = 1024;
    const int total_writes = capacity + 1; // 1 overflow

    bma_accel_data_t data[total_writes];
    gen_mock_array(data, total_writes);

    for (int i = 0; i < total_writes; i++) {
        circular_buff_write(data[i], &buff);
    }

    bma_accel_data_t out;

    // The first element should have been overwritten
    // Buffer should now contain data[1] ... data[1024]
    for (int i = 1; i < total_writes; i++) {
        bool ok = circular_buff_read(&buff, &out);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_EQUAL_MEMORY(&data[i], &out, sizeof(bma_accel_data_t));
    }
}

void test_circular_buffer_read_empty(void) {
    circular_buff_t buff;
    circular_buff_init(&buff);

    bma_accel_data_t out;
    bool ok = circular_buff_read(&buff, &out);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL_INT(0, buff.count);
    TEST_ASSERT_EQUAL_INT(0, buff.head);
    TEST_ASSERT_EQUAL_INT(0, buff.tail);
}

void test_circular_buffer_multi_overwrite(void) {
    circular_buff_t buff;
    circular_buff_init(&buff);

    const int capacity = 1024;
    const int total_writes = capacity + 10; // multiple overwrites

    bma_accel_data_t data[total_writes];
    gen_mock_array(data, total_writes);

    for (int i = 0; i < total_writes; i++) {
        circular_buff_write(data[i], &buff);
    }

    bma_accel_data_t out;
    // Only the last 'capacity' elements should remain
    for (int i = total_writes - capacity; i < total_writes; i++) {
        bool ok = circular_buff_read(&buff, &out);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_EQUAL_MEMORY(&data[i], &out, sizeof(bma_accel_data_t));
    }

    // Buffer should now be empty
    bool ok = circular_buff_read(&buff, &out);
    TEST_ASSERT_FALSE(ok);
}

void test_circular_buffer_count_invariants(void) {
    circular_buff_t buff;
    circular_buff_init(&buff);

    const int capacity = 1024;

    // Write more than capacity
    for (int i = 0; i < capacity + 5; i++) {
        bma_accel_data_t val = gen_mock_val();
        circular_buff_write(val, &buff);
        TEST_ASSERT_TRUE(buff.count <= capacity);
        TEST_ASSERT_TRUE(buff.count >= 0);
    }

    // Read until empty
    bma_accel_data_t out;
    while (circular_buff_read(&buff, &out)) {
        TEST_ASSERT_TRUE(buff.count <= capacity);
        TEST_ASSERT_TRUE(buff.count >= 0);
    }
}

void test_circular_buffer_wrap_around(void) {
    circular_buff_t buff;
    circular_buff_init(&buff);

    const int capacity = 1024;           // small buffer to force wrap-around
    const int total_writes = capacity * 2; // write twice the capacity
    bma_accel_data_t data[total_writes];

    // Deterministic array generation
    rng_t rng;
    rng_seed(&rng, 4321);  // fixed seed for repeatability
    uint32_t ts = 0;
    for (int i = 0; i < total_writes; i++) {
        data[i].x = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
        data[i].y = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
        data[i].z = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
        data[i].timestamp_ms = ts;
        ts += 40;
    }

    // Write all elements into the buffer
    for (int i = 0; i < total_writes; i++) {
        circular_buff_write(data[i], &buff);
    }

    // Only the last 'capacity' elements should remain
    bma_accel_data_t out;
    for (int i = total_writes - capacity; i < total_writes; i++) {
        bool ok = circular_buff_read(&buff, &out);
        TEST_ASSERT_TRUE(ok);

        // Field-by-field comparison avoids padding issues
        TEST_ASSERT_EQUAL_INT16(data[i].x, out.x);
        TEST_ASSERT_EQUAL_INT16(data[i].y, out.y);
        TEST_ASSERT_EQUAL_INT16(data[i].z, out.z);
        TEST_ASSERT_EQUAL_INT32(data[i].timestamp_ms, out.timestamp_ms);
    }

    // After reading everything, buffer should be empty and head == tail
    TEST_ASSERT_EQUAL_INT(0, buff.count);
    TEST_ASSERT_EQUAL_INT(buff.head, buff.tail);
}


//----------------------------- TEST WRITES TO THE PSRAM ----------------------------// 









int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_empty_after_init);
    RUN_TEST(test_circular_buffer_write);
    RUN_TEST(test_circular_buffer_read);
    RUN_TEST(test_circular_buffer_write_seq);
    RUN_TEST(test_circular_buff_read_seq);
    RUN_TEST(test_circular_buffer_sequential_readwrite);
    RUN_TEST(test_circular_buffer_overwrite);
    RUN_TEST(test_circular_buffer_read_empty);
    RUN_TEST(test_circular_buffer_multi_overwrite);
    RUN_TEST(test_circular_buffer_count_invariants);
    RUN_TEST(test_circular_buffer_wrap_around);

    UNITY_END();
}