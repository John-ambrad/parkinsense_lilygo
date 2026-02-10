/*
 * Shared test entry point for both native (main) and device (setup/loop).
 * Runs all circular buffer tests, plus PSRAM tests on ESP32.
 */
#include "unity.h"
#include "utils/circular_buff.h"
#include "utils/psram_accel_alloc.h"

/* ---- Unity hooks ---- */
void setUp(void) {
    psram_init();
}

void tearDown(void) {
}

/* ---- Declare test functions from test_circular_buffer.cpp ---- */
void test_circular_buffer_empty_after_init(void);
void test_circular_buffer_write(void);
void test_circular_buffer_read(void);
void test_circular_buffer_write_seq(void);
void test_circular_buff_read_seq(void);
void test_circular_buffer_sequential_readwrite(void);
void test_circular_buffer_overwrite(void);
void test_circular_buffer_read_empty(void);
void test_circular_buffer_multi_overwrite(void);
void test_circular_buffer_count_invariants(void);
void test_circular_buffer_wrap_around(void);

/* ---- Declare test functions from test_psram.cpp (ESP32 only) ---- */
#ifdef ESP32
void test_psram_get_memory(void);
void test_psram_init(void);
void test_write_to_psram(void);
void test_read_from_psram(void);
#endif

/* ---- Run all tests ---- */
static void run_all_tests(void) {
    UNITY_BEGIN();

    /* Circular buffer tests (all platforms) */
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

    /* PSRAM tests (device only) */
#ifdef ESP32
    RUN_TEST(test_psram_get_memory);
    RUN_TEST(test_psram_init);
    RUN_TEST(test_write_to_psram);
    RUN_TEST(test_read_from_psram);
#endif

    UNITY_END();
}

/* ---- Entry points ---- */
#ifdef ESP32
#include <Arduino.h>
void setup(void) {
    delay(2000);  /* let Serial connect before Unity output */
    run_all_tests();
}

void loop(void) {
}
#else
int main(int argc, char **argv) {
    run_all_tests();
    return 0;
}
#endif
