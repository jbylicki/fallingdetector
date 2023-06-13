#ifndef PER_GATT_WRITE_H
#define PER_GATT_WRITE_H

#include "gatt_write_common.h"

#include <stdio.h>
#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/types.h>
#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TX_CHARACTERISTIC_UUID                                                                \
    0xED, 0x69, 0x20, 0x11, 0x92, 0xE7, 0x43, 0x5A, 0xAA, 0xE9, 0x94, 0x43, 0x35, 0x6A, 0xD4, \
        0xD3
#define MY_SERVICE_UUID                                                                       \
    0xd4, 0x86, 0x48, 0x24, 0x54, 0xB3, 0x43, 0xA1, 0xBC, 0x20, 0x97, 0x8F, 0xC3, 0x76, 0xC2, \
        0x75

#define BT_UUID_MY_SERVICE BT_UUID_DECLARE_128(MY_SERVICE_UUID)
#define BT_UUID_MY_SERVICE_TX BT_UUID_DECLARE_128(TX_CHARACTERISTIC_UUID)

uint32_t peripheral_gatt_write(uint32_t count);
void peripheral_gatt_prepare_ble();
void peripheral_gatt_send_ble(int count, int data);

#ifdef __cplusplus
}
#endif
#endif