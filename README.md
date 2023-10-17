# temp_control for Raspberry Pi 4B

Forked and updated from original at [https://github.com/OldWang-666/temp_control](https://github.com/OldWang-666/temp_control)

## Automated install
`bash <(wget --no-cache -qO- https://raw.githubusercontent.com/alexisspencer/temp_control/main/install.sh)`

## Prerequisits
Install WiringPi (if not installed):
`cd ~`
`wget https://project-downloads.drogon.net/wiringpi-latest.deb`
`sudo dpkg -i wiringpi-latest.deb`

## Installation
Clone this repo
`git clone https://github.com/alexisspencer/temp_control.git`
`cd temp_control/`

Then compile, run from terminal in temp_control folder:
`gcc -o temp_control temp_control.c ssd1306_i2c.c -lwiringPi`


Instructions for customised deployment can be found at https://wiki.dfrobot.com/Smart_Cooling_Hat_For_Raspberry_Pi_4B_SKU_DFR0672#target_2