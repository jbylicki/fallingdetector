/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "peripheral_gatt_write.h"

// fucking stolen

extern int mtu_exchange(struct bt_conn *conn);
extern int write_cmd(struct bt_conn *conn);
extern struct bt_conn *conn_connected;
extern uint32_t last_write_rate;

// DATA TO SEND OVER BLE GATT
static uint8_t current_state = 0x69;

// XXX: change these values

/* This function is called whenever the CCCD register has been changed by the client */
void on_cccd_changed(const struct bt_gatt_attr *attr, uint16_t value) {
    ARG_UNUSED(attr);
    switch (value) {
    case BT_GATT_CCC_NOTIFY:
        // Start sending stuff!
        break;
    case BT_GATT_CCC_INDICATE:
        // Start sending stuff via indications
        break;
    case 0:
        // Stop sending stuff
        break;
    default:
        printf("Error, CCCD has been set to an invalid value");
    }
}

BT_GATT_SERVICE_DEFINE(send_service,
											BT_GATT_PRIMARY_SERVICE(BT_UUID_MY_SERVICE),
                      BT_GATT_CHARACTERISTIC(BT_UUID_MY_SERVICE_TX, BT_GATT_CHRC_BROADCAST | BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ, NULL, NULL, NULL),
                      BT_GATT_CCC(on_cccd_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE_ENCRYPT));
//


static void on_sent(struct bt_conn *conn, void *user_data) {
    ARG_UNUSED(user_data);

    const bt_addr_le_t *addr = bt_conn_get_dst(conn);

    // printf("Data sent to Address 0x %02X %02X %02X %02X %02X %02X \n", addr->a.val[0],
    //        addr->a.val[1], addr->a.val[2], addr->a.val[3], addr->a.val[4], addr->a.val[5]);
}

void send_service_send(struct bt_conn *conn) {
    const struct bt_gatt_attr *attr = &send_service.attrs[2];

    struct bt_gatt_notify_params params = {.uuid = BT_UUID_MY_SERVICE_TX,
                                           .attr = attr,
                                           .data = &current_state,
                                           .len = sizeof(current_state),
                                           .func = on_sent};

    // // Check whether notifications are enabled or not
    if (conn && bt_gatt_is_subscribed(conn, attr, BT_GATT_CCC_NOTIFY)) {
        // // Send the notification
        if (bt_gatt_notify_cb(conn, &params)) {
            printf("Error, unable to send notification\n");
        }
    } else {
        // printf("Warning, notification not enabled on the selected attribute\n");
    }
}

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

static void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	printf("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
}

#if defined(CONFIG_BT_SMP)
static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_callbacks = {
	.cancel = auth_cancel,
};
#endif /* CONFIG_BT_SMP */

static struct bt_gatt_cb gatt_callbacks = {
	.att_mtu_updated = mtu_updated
};

uint32_t peripheral_gatt_write(uint32_t count)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return 0U;
	}

	printf("Bluetooth initialized\n");

	bt_gatt_cb_register(&gatt_callbacks);

#if defined(CONFIG_BT_SMP)
	(void)bt_conn_auth_cb_register(&auth_callbacks);
#endif /* CONFIG_BT_SMP */

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printf("Advertising failed to start (err %d)\n", err);
		return 0U;
	}

	printf("Advertising successfully started\n");

	conn_connected = NULL;
	last_write_rate = 0U;

	while (true) {
		struct bt_conn *conn = NULL;

		if (conn_connected) {
			/* Get a connection reference to ensure that a
			 * reference is maintained in case disconnected
			 * callback is called while we perform GATT Write
			 * command.
			 */
			conn = bt_conn_ref(conn_connected);
		}

		if (conn) {
			write_cmd(conn);
			bt_conn_unref(conn);

			// Actual sending shit
			send_service_send(conn);
			// current_state++;
			if (count) {
				count--;
				if (!count) {
					break;
				}
			}

			k_yield();
		} else {
			k_sleep(K_SECONDS(1));
		}
	}

	return last_write_rate;
}

void peripheral_gatt_prepare_ble()
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printf("Bluetooth initialized\n");

	bt_gatt_cb_register(&gatt_callbacks);

#if defined(CONFIG_BT_SMP)
	(void)bt_conn_auth_cb_register(&auth_callbacks);
#endif /* CONFIG_BT_SMP */

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printf("Advertising failed to start (err %d)\n", err);
		return 0U;
	}

	printf("Advertising successfully started\n");

	conn_connected = NULL;
	last_write_rate = 0U;
}

void peripheral_gatt_send_ble(int count, int data)
{
	// printf("well hello there!\n");
	// conn_connected = NULL;
	// last_write_rate = 0U;

	// printf("entering loop\n");
	while (true) {
		struct bt_conn *conn = NULL;

		if (conn_connected) {
			/* Get a connection reference to ensure that a
			 * reference is maintained in case disconnected
			 * callback is called while we perform GATT Write
			 * command.
			 */
			conn = bt_conn_ref(conn_connected);
		}

		if (conn)
		{
			// printf("seinding... ");
			write_cmd(conn);
			bt_conn_unref(conn);
			send_service_send(conn);
			// printf("sent!\n");

			current_state = data;

			if (count)
			{
				count--;
				if (!count)
				{
					// printf("!count!\n");
					// conn_connected = NULL;
					return;
					// break;
				}
			}

			// printf("end of the lopp!\n");
			// bt_conn_unref(conn);
		}
		else
		{
			// printf("else\n");
			// k_sleep(K_SECONDS(1));
			return;
		}
	}
}

