#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include "ssd1306_driver.h"
#include "mpu_driver.h"


int main(int argc, const char *argv[])
{
	int fd, fd1;
	union mpu6050_data Mpu_data;
	fd = open("/dev/ssd1306", O_RDWR);
	if (fd < 0) {
		perror("open");
		exit(1);
	}
	fd1 = open("/dev/mpu6050", O_RDWR);
	if (fd1 < 0) {
		perror("open");
		exit(1);
	}
	struct ShowNum Num_data[3] = {
		{.Line = 1,
		.Column = 8,
		.Number = 0000,
		.Length = 4},
		{.Line = 2,
		.Column = 8,
		.Number = 0000,
		.Length = 4},
		{.Line = 3,
		.Column = 8,
		.Number = 0000,
		.Length = 4}
	};
	struct ShowString String_data[3] = {
		{.Line = 1,
		.Column = 1,
		.String = "Acc-X:"},
		{.Line = 2,
		.Column = 1,
		.String = "Acc-Y:"},
		{.Line = 3,
		.Column = 1,
		.String = "Acc-Z:"}
	};
	
	ioctl(fd, OLED_CLOSE);
	printf("oled is closed!\n");
    sleep(2);
    ioctl(fd, OLED_OPEN);
    printf("oled is opend!\n");
    sleep(2);
    ioctl(fd, OLED_CLEAR);
    printf("Oled is cleared now!\n");

	ioctl(fd, OLED_Show_String, &String_data[0]);
	ioctl(fd, OLED_Show_String, &String_data[1]);
	ioctl(fd, OLED_Show_String, &String_data[2]);
	ioctl(fd, OLED_Show_Num, &Num_data[0]);
	ioctl(fd, OLED_Show_Num, &Num_data[1]);
	ioctl(fd, OLED_Show_Num, &Num_data[2]);
    printf("Show String is done!\n");

	while(1) {

		ioctl(fd1, GET_ACCEL, &Mpu_data);
		Num_data[0].Number = Mpu_data.accel.x;
		Num_data[1].Number = Mpu_data.accel.y;
		Num_data[2].Number = Mpu_data.accel.z;
		ioctl(fd, OLED_Show_Num, &Num_data[0]);
		ioctl(fd, OLED_Show_Num, &Num_data[1]);
		ioctl(fd, OLED_Show_Num, &Num_data[2]);
				
	}

	close(fd);
	close(fd1);

	return 0;
}