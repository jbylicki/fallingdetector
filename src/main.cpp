#include "main_functions.hpp"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <cstdio>

USBD_DEVICE_DEFINE(sample_usbd,
		   DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
		   0x2fe3, 0x0001);


int main(int argc, char *argv[])
{
  if (usb_enable(NULL)) {
		return 1;
	}
  k_sleep(K_SECONDS(2));
  printf("Starting TensorFlow Lite Micro\n");
  k_sleep(K_SECONDS(1));
	setup();
	while (true) {
		loop();
	}
}

