// 导入wiringPi/I2C库
#include <wiringPi.h>
#include <wiringPiI2C.h>

// 导入oled显示屏库
#include "ssd1306_i2c.h"

// 导入文件控制函数库
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
// 读取IP库
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
// 读取磁盘库
#include <sys/vfs.h>
#include <unistd.h>

#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define MAX_SIZE 32

#define Max_LED  3
#define RGB_Effect 0x04 
#define RGB_Speed  0x05
#define RGB_Color  0x06
int fd_i2c;

void setRGB(int num, int R, int G, int B);
void closeRGB();

void setRGBEffect(int effect);
void setRGBSpeed(int speed);
void setRGBColor(int color);

int main(void)
{
	int readed_ip = 0;
	int count = 0;
	int fd_temp;
	double temp = 0, level_temp = 0;
	char buf[MAX_SIZE];

	// get system usage / info
	struct sysinfo sys_info;
	struct statfs disk_info;

	struct ifaddrs *ifAddrStruct = NULL;
	void *tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

	// Define I2C parameters
	//int fd_i2c;
	wiringPiSetup();
	fd_i2c = wiringPiI2CSetup(0x0d);
	while (fd_i2c < 0)
	{
		fd_i2c = wiringPiI2CSetup(0x0d);
		fprintf(stderr, "fail to init I2C\n");
		delay(500);
	}

	// Turn on the RGB light effect
	wiringPiI2CWriteReg8(fd_i2c, 0x04, 0x03);

	// wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);   // close fan
	// ssd1306_display();      //show logo
	// ssd1306_clearDisplay();
	// delay(500);
	printf("init ok!\n");

	while (1)
	{
		// Read system information
		if (sysinfo(&sys_info) != 0) // sysinfo(&sys_info) != 0
		{
			printf("sysinfo-Error\n");
			ssd1306_clearDisplay();
			char *text = "sysinfo-Error";
			ssd1306_drawString(text);
			ssd1306_display();
			continue;
		}
		else
		{
			// Clear screen content
			ssd1306_clearDisplay();

			// CPU usage
			char CPUInfo[MAX_SIZE] = {0};
			unsigned long avgCpuLoad = sys_info.loads[0] / 1000;
			sprintf(CPUInfo, "CPU:%ld%%", avgCpuLoad);

			// Running memory usage, remaining/total memory
			char RamInfo[MAX_SIZE];
			unsigned long totalRam = sys_info.totalram >> 20;
			unsigned long freeRam = sys_info.freeram >> 20;
			sprintf(RamInfo, "RAM:%ld/%ld MB", freeRam, totalRam);

			// Obtain an IP address
			char IPInfo[MAX_SIZE];
			if (readed_ip == 0)
			{
				while (ifAddrStruct != NULL)
				{
					if (ifAddrStruct->ifa_addr->sa_family == AF_INET)
					{ // check it is IP4 is a valid IP4 Address
						tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
						char addressBuffer[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

						if (strcmp(ifAddrStruct->ifa_name, "eth0") == 0)
						{
							sprintf(IPInfo, "eth0:IP:%s", addressBuffer);
							readed_ip = 1;
							break;
						}
						else if (strcmp(ifAddrStruct->ifa_name, "wlan0") == 0)
						{
							sprintf(IPInfo, "wlan0:%s", addressBuffer);
							readed_ip = 1;
							break;
						}else 
						{
							readed_ip = 0;
						}
					}
					ifAddrStruct = ifAddrStruct->ifa_next;
				}
				getifaddrs(&ifAddrStruct);
			}

			// Read the CPU temperature
			char CPUTemp[MAX_SIZE];
			fd_temp = open(TEMP_PATH, O_RDONLY);
			if (fd_temp < 0)
			{
				temp = 0;
				printf("failed to open thermal_zone0/temp\n");
			}
			else
			{
				// Read the temperature and judge
				if (read(fd_temp, buf, MAX_SIZE) < 0)
				{
					temp = 0;
					printf("fail to read temp\n");
				}
				else
				{
					// Convert to floating-point printing
					temp = atoi(buf) / 1000.0;
					// printf("temp: %.1f\n", temp);
					sprintf(CPUTemp, "Temp:%.1fC", temp);
				}
			}
			close(fd_temp);

			// Read disk space, remaining/total space
			char DiskInfo[MAX_SIZE];
			statfs("/", &disk_info);
			unsigned long long totalBlocks = disk_info.f_bsize;
			unsigned long long totalSize = totalBlocks * disk_info.f_blocks;
			size_t mbTotalsize = totalSize >> 20;
			unsigned long long freeDisk = disk_info.f_bfree * totalBlocks;
			size_t mbFreedisk = freeDisk >> 20;
			sprintf(DiskInfo, "Disk:%ld/%ldMB", mbFreedisk, mbTotalsize);

			// What to display on the display
			ssd1306_drawText(0, 0, CPUInfo);
			ssd1306_drawText(56, 0, CPUTemp);
			ssd1306_drawText(0, 8, RamInfo);
			ssd1306_drawText(0, 16, DiskInfo);
			ssd1306_drawText(0, 24, IPInfo);

			// Refresh the display
			ssd1306_display();
			delay(10);
		}

		if (abs(temp - level_temp) >= 1)
        {
            if (temp <= 45)
            {
                level_temp = 45;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x00);
				setRGB(Max_LED, 0x00, 0x00, 0xff);
				setRGBEffect(0);
    			setRGBSpeed(1);
            }
            else if (temp <= 47)
            {
                level_temp = 47;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x02);
				setRGB(Max_LED, 0x1e, 0x90, 0xff);
				setRGBEffect(0);
    			setRGBSpeed(1);
            }
            else if (temp <= 49)
            {
                level_temp = 49;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x03);
				setRGB(Max_LED, 0x00, 0xbf, 0xff);
				setRGBEffect(1);
    			setRGBSpeed(1);
            }
            else if (temp <= 51)
            {
                level_temp = 51;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x04);
				setRGB(Max_LED, 0x5f, 0x9e, 0xa0);
				setRGBEffect(1);
    			setRGBSpeed(2);
            }
            else if (temp <= 53)
            {
                level_temp = 53;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x05);
				setRGB(Max_LED, 0xff, 0xff, 0x00);
				setRGBEffect(2);
    			setRGBSpeed(2);
            }
            else if (temp <= 55)
            {
                level_temp = 55;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x06);
				setRGB(Max_LED, 0xff, 0xd7, 0x00);
				setRGBEffect(2);
    			setRGBSpeed(2);
            }
            else if (temp <= 57)
            {
                level_temp = 57;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x07);
				setRGB(Max_LED, 0xff, 0xa5, 0x00);
				setRGBEffect(3);
    			setRGBSpeed(3);
            }
            else if (temp <= 59)
            {
                level_temp = 59;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x08);
				setRGB(Max_LED, 0xff, 0x8c, 0x00);
				setRGBEffect(3);
    			setRGBSpeed(3);
            }
            else if (temp <= 61)
            {
                level_temp = 61;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x09);
				setRGB(Max_LED, 0xff, 0x45, 0x00);
				setRGBEffect(4);
    			setRGBSpeed(3);
            }
            else
            {
                level_temp = 63;
                wiringPiI2CWriteReg8(fd_i2c, 0x08, 0x01);
				setRGB(Max_LED, 0xff, 0x00, 0x00);
				setRGBEffect(4);
    			setRGBSpeed(3);
            }
        }

		delay(500);

		// if (count == 10)
		// {
		// 	// Turn on the RGB light effect
		// 	wiringPiI2CWriteReg8(fd_i2c, 0x04, 0x04);
		// }
		// else if (count == 20)
		// {
		// 	// Turn on the RGB light effect
		// 	wiringPiI2CWriteReg8(fd_i2c, 0x04, 0x02);
		// }
		// else if (count == 30)
		// {
		// 	// Turn on the RGB light effect
		// 	wiringPiI2CWriteReg8(fd_i2c, 0x04, 0x01);
		// }
		// else if (count == 40)
		// {
		// 	// Turn on the RGB light effect
		// 	wiringPiI2CWriteReg8(fd_i2c, 0x04, 0x03);
		// 	count = 0;
		// }
		
		// count++;
		// delay(500);
	}

	return 0;
}


// Set the RGB light, num if greater than or equal to Max_LED(3), all lights are set together
// num=(0~3),R=(0~255),G=(0~255),B=(0~255)
void setRGB(int num, int R, int G, int B)
{
    if (num >= Max_LED)
    {
        wiringPiI2CWriteReg8(fd_i2c, 0x00, 0xff);
        wiringPiI2CWriteReg8(fd_i2c, 0x01, R);
        wiringPiI2CWriteReg8(fd_i2c, 0x02, G);
        wiringPiI2CWriteReg8(fd_i2c, 0x03, B);
    }
    else if (num >= 0)
    {
        wiringPiI2CWriteReg8(fd_i2c, 0x00, num);
        wiringPiI2CWriteReg8(fd_i2c, 0x01, R);
        wiringPiI2CWriteReg8(fd_i2c, 0x02, G);
        wiringPiI2CWriteReg8(fd_i2c, 0x03, B);
    }
}

// 关闭RGB
void closeRGB()
{
    wiringPiI2CWriteReg8(fd_i2c, 0x07, 0x00);
    delay(10);
}


// Set RGB lighting effects, 0 running water lights, 1 breathing lights, 2 marquee lights, 3 rainbow lights, 4 dazzling lights
void setRGBEffect(int effect)
{
    if (effect >= 0 && effect <= 4)
    {
        wiringPiI2CWriteReg8(fd_i2c, RGB_Effect, effect);
    }  
}
// Set RGB speed: 1 low speed, 2 medium speed (default), 3 high speed
void setRGBSpeed(int speed)
{
    if (speed >= 1 && speed <= 3)
    {
        wiringPiI2CWriteReg8(fd_i2c, RGB_Speed, speed);
    }
}
// Set the running light/breathing lamp color: 0 red, 1 green (default), 2 blue, 3 yellow, 4 purple, 5 cyan, 6 white
void setRGBColor(int color)
{
    if (color >= 0 && color <= 6)
    {
        wiringPiI2CWriteReg8(fd_i2c, RGB_Color, color);
    }
}