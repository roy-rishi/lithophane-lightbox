#ifndef BLE_
#define BLE_

#include <freertos/FreeRTOS.h>

void ble_init(QueueHandle_t itc_queue);
void ble_start();

#endif  // BLE_
