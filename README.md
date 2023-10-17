# temp_control for Raspberry Pi 4B

Forked and updated from original at [https://github.com/OldWang-666/temp_control](https://github.com/OldWang-666/temp_control)

Instructions for deployment at https://wiki.dfrobot.com/Smart_Cooling_Hat_For_Raspberry_Pi_4B_SKU_DFR0672#target_2

To install:

To compile, run from terminal in temp_control folder:
`gcc -o temp_control temp_control.c ssd1306_i2c.c -lwiringPi`
