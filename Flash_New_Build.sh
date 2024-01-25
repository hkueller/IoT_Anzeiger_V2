#!/bin/bash

#set -x

if sed -e "/^#/d" platformio.ini|grep -q upload_protocol; then
	if [ "$(sed -e "/^#/d" platformio.ini|grep upload_protocol|cut -d "=" -f 2|cut -d " " -f 2)" == "espota" ]; then
		node=$(sed -e "/^#/d" platformio.ini|grep upload_port|cut -d"=" -f 2|cut -d " " -f 2)
		./espota.py -i $node -f .pio/build/nodemcu/firmware.bin -d -r
	fi
else
	dev=$(ls /dev/ttyUSB*)
	esptool --port $dev --baud 921600 write_flash 0x00000 .pio/build/nodemcu/firmware.bin
fi
