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




    bma_accel_data_t read_val = circular_buff_read(&buff);


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
        bma_accel_data_t stored_samp = circular_buff_read(&buff);
        TEST_ASSERT_EQUAL_INT16(mock_val_arr[s].x,stored_samp.x);
        TEST_ASSERT_EQUAL_INT16(mock_val_arr[s].y,stored_samp.y);
        TEST_ASSERT_EQUAL_INT16(mock_val_arr[s].z,stored_samp.z);
        TEST_ASSERT_EQUAL_INT32(mock_val_arr[s].timestamp_ms,stored_samp.timestamp_ms);
        
    }

    TEST_ASSERT_EQUAL_INT(num_samples,buff.tail);
    TEST_ASSERT_EQUAL_INT(0,buff.count);
    TEST_ASSERT_EQUAL_INT(num_samples,buff.head);

}


void test_circular_buffer_full(){
    //checks that the counter is implementing correctly 
    //to test this have a fixed size circular buffer
    //count == known fixed size then the counter is working properly
    //also test to see that it will erase/overwrite old data?

}

void test_circular_buffer_limit(){
    
}

void test_circular_buffer_overflow(){
    // check the old data is overwritten during overflow //



}

void test_circular_buffer_clear(){



}



int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_empty_after_init);
    RUN_TEST(test_circular_buffer_write);
    RUN_TEST(test_circular_buffer_read);
    RUN_TEST(test_circular_buffer_write_seq);
    RUN_TEST(test_circular_buff_read_seq);

    UNITY_END();
}