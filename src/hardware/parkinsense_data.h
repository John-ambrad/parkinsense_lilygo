#ifndef _PARKINSENSE_DATA_H
    #define _PARKINSENSE_DATA_H

    #include "callback.h"

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else

    #endif

    #define PARKINSENSE_DATA_UPDATE_INTERVAL 1000
    /** How often (ms) the pipeline auto-sends PSRAM blocks over BLE. */
    #define PARKINSENSE_AUTO_SEND_INTERVAL_MS 5000

    #ifndef _BV
    #define _BV(b) (1UL << (b))
    #endif
    #define PARKINSENSE_SEND_DONE   _BV(0)
    #define PARKINSENSE_SEND_ABORT  _BV(1)

    void parkinsense_data_pipeline_init(void);
    void parkinsense_data_pipeline_start(void);
    void parkinsense_data_pipeline_stop(void);
    void parkinsense_data_pipeline_reset(void);
    void parkinsense_data_pipeline_get_data(void);
    void parkinsense_data_pipeline_write_to_psram(void);
    void parkinsense_data_pipeline_drain_if_full(void);
    void parkinsense_data_pipeline_send_data(void);
    int parkinsense_data_pipeline_buff_count(void);
    bool parkinsense_data_pipeline_is_started(void);

#endif // _PARKINSENSE_DATA_H
