#!/usr/bin/env bash

west build -p=auto -b arduino_nano_33_ble .

read -p "Press enter to flash"

west -v flash --bossac=./bossac/bossac