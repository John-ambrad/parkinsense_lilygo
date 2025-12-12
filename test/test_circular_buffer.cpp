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

void test_circular_buffer_add(){

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

void test_circular_buffer_get(){




}



int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_empty_after_init);
    UNITY_END();
}