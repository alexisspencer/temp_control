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

int main(void)
{
	int fd_temp;
	double temp = 0;
	char buf[MAX_SIZE];

	// get system usage / info
	struct sysinfo sys_info;
	struct statfs disk_info;

	struct ifaddrs *ifAddrStruct = NULL;
	void *tmpAddrPtr = NULL;
	getifaddrs(&ifAddrStruct);

	ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
	// ssd1306_display();      //show logo
	// ssd1306_clearDisplay();
	// delay(500);
	printf("init ok!\n");

	while (1)
	{
		// 读取系统信息
		if (sysinfo(&sys_info) != 0) // sysinfo(&sys_info) != 0
		{
			printf("sysinfo-Error\n");
			ssd1306_clearDisplay();
			char *text = "sysinfo-Error";
			ssd1306_drawString(text);
			ssd1306_display();
			delay(500);
			continue;
		}
		else
		{
			// 清除屏幕内容
			ssd1306_clearDisplay();

			// CPU占用率
			char CPUInfo[MAX_SIZE];
			unsigned long avgCpuLoad = sys_info.loads[0] / 1000;
			sprintf(CPUInfo, "CPU:%ld%%", avgCpuLoad);

			// 运行内存占用率，剩余/总内存
			char RamInfo[MAX_SIZE];
			unsigned long totalRam = sys_info.totalram >> 20;
			unsigned long freeRam = sys_info.freeram >> 20;
			sprintf(RamInfo, "RAM:%ld/%ld MB", freeRam, totalRam);

			// 获取IP地址
			char IPInfo[MAX_SIZE];
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
						break;
					}
					else if (strcmp(ifAddrStruct->ifa_name, "wlan0") == 0)
					{
						sprintf(IPInfo, "wlan0:%s", addressBuffer);
						break;
					}
				}
				ifAddrStruct = ifAddrStruct->ifa_next;
			}

			// 读取CPU温度
			char CPUTemp[MAX_SIZE];
			fd_temp = open(TEMP_PATH, O_RDONLY);
			if (fd_temp < 0)
			{
				temp = 0;
				printf("failed to open thermal_zone0/temp\n");
			}
			else
			{
				// 读取温度并判断
				if (read(fd_temp, buf, MAX_SIZE) < 0)
				{
					temp = 0;
					printf("fail to read temp\n");
				}
				else
				{
					// 转换为浮点数打印
					temp = atoi(buf) / 1000.0;
					// printf("temp: %.1f\n", temp);
					sprintf(CPUTemp, "Temp:%.1fC", temp);
				}
			}
			close(fd_temp);

			// 读取磁盘空间，剩余/总空间
			char DiskInfo[MAX_SIZE];
			statfs("/", &disk_info);
			unsigned long long totalBlocks = disk_info.f_bsize;
			unsigned long long totalSize = totalBlocks * disk_info.f_blocks;
			size_t mbTotalsize = totalSize >> 20;
			unsigned long long freeDisk = disk_info.f_bfree * totalBlocks;
			size_t mbFreedisk = freeDisk >> 20;
			sprintf(DiskInfo, "Disk:%ld/%ldMB", mbFreedisk, mbTotalsize);

			// 在显示屏上要显示的内容
			ssd1306_drawText(0, 0, CPUInfo);
			ssd1306_drawText(56, 0, CPUTemp);
			ssd1306_drawText(0, 8, RamInfo);
			ssd1306_drawText(0, 16, DiskInfo);
			ssd1306_drawText(0, 24, IPInfo);

			// 刷新显示
			ssd1306_display();
		}
		delay(500);
	}

	return 0;
}
