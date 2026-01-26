#include "unity.h"
#include "../src/utils/circular_buff.cpp"   // include the implementation directly
#include "utils/circular_buff.h"           // include the header
#include <stdint.h>
#include "utils/alloc.h"

void setUp(void){

}

void tearDown(void){

}


//-------- NATIVE TESTS --------//
// get memory
// get free memory?
// basically status checks
// initialization checks? 

//-------- ON DEVICE TESTS -------//

void test_psram_get_memory(void) {
    TEST_ASSERT_GREATER_THAN(0,ESP.getPsramSize());
    TEST_ASSERT_GREATER_THAN(0,ESP.getFreePsram());
}

void test_write_to_psram(void) {
    void *ptr = MALLOC(100);
    TEST_ASSERT_NOT_NULL(ptr);
    free(ptr);
}

void test_read_from_psram(void) {

    int expected[] = {1,2,3,4,5};
    void *ptr = MALLOC(5);
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected,ptr,sizeof(int),5);
    free(ptr);


}
