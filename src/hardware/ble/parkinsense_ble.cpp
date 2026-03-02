#include "config.h"
#include "parkinsense_ble.h"
#include "hardware/blectl.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include "NimBLEDevice.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    static NimBLECharacteristic *pParkinsenseTXCharacteristic = NULL;

    #define PARKINSENSE_BLE_MIN_CHUNK    20
    #define PARKINSENSE_BLE_CHUNK_DELAY  15
#endif

void parkinsense_ble_setup(void) {
#ifndef NATIVE_64BIT
    NimBLEServer *pServer = blectl_get_ble_server();
    NimBLEAdvertising *pAdvertising = blectl_get_ble_advertising();

    NimBLEService *pService = pServer->createService(NimBLEUUID(PARKINSENSE_SERVICE_UUID));
    pParkinsenseTXCharacteristic = pService->createCharacteristic(
        NimBLEUUID(PARKINSENSE_CHARACTERISTIC_UUID_TX),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();
    pAdvertising->addServiceUUID(pService->getUUID());
#endif
}

bool parkinsense_ble_send(const char *data, size_t len) {
#ifndef NATIVE_64BIT
    if (!pParkinsenseTXCharacteristic)
        return false;

    NimBLEServer *pServer = blectl_get_ble_server();
    if (!pServer || pServer->getConnectedCount() == 0)
        return false;

    size_t chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    std::vector<uint16_t> peers = pServer->getPeerDevices();
    if (!peers.empty()) {
        uint16_t mtu = pServer->getPeerMTU(peers[0]);
        if (mtu > 3)
            chunk_size = mtu - 3;
        if (chunk_size < PARKINSENSE_BLE_MIN_CHUNK)
            chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    }

    size_t pos = 0;
    while (pos < len) {
        if (pServer->getConnectedCount() == 0)
            return false;

        size_t remaining = len - pos;
        size_t chunk_len = (remaining > chunk_size) ? chunk_size : remaining;
        pParkinsenseTXCharacteristic->setValue((const uint8_t*)(data + pos), chunk_len);
        pParkinsenseTXCharacteristic->notify();
        pos += chunk_len;

        if (pos < len)
            vTaskDelay(pdMS_TO_TICKS(PARKINSENSE_BLE_CHUNK_DELAY));
    }

    const uint8_t newline = '\n';
    pParkinsenseTXCharacteristic->setValue(&newline, 1);
    pParkinsenseTXCharacteristic->notify();

    return true;
#else
    log_i("parkinsense_ble_send (native stub): %.*s", (int)len, data);
    return true;
#endif
}
