#ifndef PER_GATT_WRITE_H
#define PER_GATT_WRITE_H


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

static struct bt_gatt_exchange_params mtu_exchange_params;
static uint32_t write_count;
static uint32_t write_len;
static uint32_t write_rate;
struct bt_conn *conn_connected;
uint32_t last_write_rate;
void (*start_scan_func)(void);

int write_cmd(struct bt_conn *conn);

#ifdef __cplusplus
}
#endif
#endif