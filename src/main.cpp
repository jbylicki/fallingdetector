#include "main_functions.hpp"
#include <zephyr/kernel.h>
int main(int argc, char *argv[])
{
  printk("Starting TensorFlow Lite Micro\n");
	setup();
	while (true) {
		loop();
	}
}

