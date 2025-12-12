#include "unity.h"
#include "../src/utils/circular_buff.cpp"   // include the implementation directly
#include "utils/circular_buff.h"           // include the header
void setUp(void){

}

void tearDown(void){

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


void test_circular_buffer_remove(){

}

void test_circular_buffer_full(){
    //checks that the counter is implementing correctly 
    //to test this have a fixed size circular buffer
    //count == known fixed size then the counter is working properly
    //also test to see that it will erase/overwrite old data?

}

void test_circular_buffer_overflow(){



}

void test_circular_buffer_clear(){



}



int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_empty_after_init);
    RUN_TEST(test_circular_buffer_write);
    RUN_TEST(test_circular_buffer_read);
    UNITY_END();
}