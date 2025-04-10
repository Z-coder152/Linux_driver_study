#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "mpu_driver.h"

int main(int argc, const char *argv[])
{
	int fd;
	union mpu6050_data data; 
	
	fd = open("/dev/mpu6050", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	while(1) {
		//三轴加速度
		ioctl(fd, GET_ACCEL, &data);
		printf("1 acceleration data: x = %04x, y = %04x, z = %04x\n", 
				data.accel.x, data.accel.y, data.accel.z);
                //三轴陀螺仪(三轴角速度)
		ioctl(fd, GET_GYRO, &data);
		printf("2 gyroscope data: x = %04x, y = %04x, z = %04x\n", 
				data.accel.x, data.accel.y, data.accel.z);
		//温度值	
		ioctl(fd, GET_TEMP, &data);
		printf("3 temp data: = %x \n", data.temp);
		sleep(1);
	}

	close(fd);

	return 0;
}