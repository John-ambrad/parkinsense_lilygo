#ifndef _PARKINSENSE_BLE_H
    #define _PARKINSENSE_BLE_H

    #include <stddef.h>
    #include <stdbool.h>

    #define PARKINSENSE_SERVICE_UUID              "4a6c5e01-7b8f-4a9d-b2c3-d4e5f6a7b8c9"
    #define PARKINSENSE_CHARACTERISTIC_UUID_TX    "4a6c5e02-7b8f-4a9d-b2c3-d4e5f6a7b8c9"

    void parkinsense_ble_setup(void);
    /**
     * @brief Send a buffer over the dedicated parkinsense BLE characteristic.
     *        Chunks the data to fit the negotiated MTU and appends '\n' as a
     *        message delimiter so any client can reassemble.
     *
     * @param data  null-terminated string to send
     * @param len   length of data (excluding null terminator)
     * @return true if at least one notify was sent
     */
    bool parkinsense_ble_send(const char *data, size_t len);

#endif // _PARKINSENSE_BLE_H
