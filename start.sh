#!/bin/sh
sleep 5s
cd /home/pi/temp_control/
./fan_temp & ./oled & ./rgb_temp &
