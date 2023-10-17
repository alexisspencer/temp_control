# temp_control for Raspberry Pi 4B

Forked and updated from original at [https://github.com/OldWang-666/temp_control](https://github.com/OldWang-666/temp_control)

## Automated install
Open Terminal (either locally or via SSH) and run as user "Pi"
`bash <(wget --no-cache -qO- https://raw.githubusercontent.com/alexisspencer/temp_control/main/install.sh)`

## Manual Install
### Install WiringPi (if not installed):
`cd ~`
`wget https://project-downloads.drogon.net/wiringpi-latest.deb`
`sudo dpkg -i wiringpi-latest.deb`

### Clone repo
`git clone https://github.com/alexisspencer/temp_control.git`
`cd temp_control/`

### Compile code
Compile the temp_control code into executable:
`gcc -o temp_control temp_control.c ssd1306_i2c.c -lwiringPi`

## Customised deployment
Instructions for customised deployment and autostart can be found at [https://wiki.dfrobot.com/Smart_Cooling_Hat_For_Raspberry_Pi_4B_SKU_DFR0672#target_2](https://wiki.dfrobot.com/Smart_Cooling_Hat_For_Raspberry_Pi_4B_SKU_DFR0672#target_2)
