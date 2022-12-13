#!/bin/bash

dev=$(ls /dev/ttyUSB*)
esptool --port $dev --baud 921600 write_flash 0x00000 .pio/build/nodemcu/firmware.bin
