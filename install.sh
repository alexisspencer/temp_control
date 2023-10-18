#!/bin/sh

echo " "
echo " "
echo " "
echo "--------------------------------------------------------------------------------------"
echo "$(date +\%Y-\%m-\%d\ \%H:\%M) INSTALLATION STARTED"
echo "--------------------------------------------------------------------------------------"

echo " "
echo " "
echo " "
echo "--------------------------------------------------------------------------------------"
echo "$(date +\%Y-\%m-\%d\ \%H:\%M) Setting hdmi_force_hotplug=1 in /boot/config.txt"
echo "--------------------------------------------------------------------------------------"

sudo su -c "echo 'hdmi_force_hotplug=1' >> /boot/config.txt" root

echo " "
echo " "
echo " "
echo "--------------------------------------------------------------------------------------"
echo "$(date +\%Y-\%m-\%d\ \%H:\%M) Installing WiringPi"
echo "--------------------------------------------------------------------------------------"

cd ~
wget https://project-downloads.drogon.net/wiringpi-latest.deb
sudo su -c "dpkg -i wiringpi-latest.deb" root

echo " "
echo " "
echo " "
echo "--------------------------------------------------------------------------------------"
echo "$(date +\%Y-\%m-\%d\ \%H:\%M) Cloning git repository"
echo "--------------------------------------------------------------------------------------"

git clone https://github.com/alexisspencer/temp_control.git
cd temp_control/

echo " "
echo " "
echo " "
echo "--------------------------------------------------------------------------------------"
echo "$(date +\%Y-\%m-\%d\ \%H:\%M) Compiling applications"
echo "--------------------------------------------------------------------------------------"
gcc -o temp_control temp_control.c ssd1306_i2c.c -lwiringPi
gcc -o fan_temp fan_temp.c ssd1306_i2c.c -lwiringPi
gcc -o oled oled.c ssd1306_i2c.c -lwiringPi
gcc -o rgb_temp rgb_temp.c ssd1306_i2c.c -lwiringPi

echo " "
echo " "
echo " "
echo "--------------------------------------------------------------------------------------"
echo "$(date +\%Y-\%m-\%d\ \%H:\%M) Setting up autostart for temp_control"
echo "--------------------------------------------------------------------------------------"
cd /home/pi/.config/
mkdir /home/pi/.config/autostart
cd /home/pi/.config/autostart
cp /home/pi/temp_control/start.desktop /home/pi/.config/autostart/
echo " "
echo " "
echo " "
echo "--------------------------------------------------------------------------------------"
echo "$(date +\%Y-\%m-\%d\ \%H:\%M) INSTALLATION COMPLETE"
echo "--------------------------------------------------------------------------------------"
echo " "
echo " "
echo " "
read -p "Press [Enter] key to restart Pi or [CTRL-C] to return to terminal..."
sudo reboot
