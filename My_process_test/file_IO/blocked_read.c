#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
	char buf[10];
	int n;
	int fd;

	fd = open("/dev/tty",O_RDWR); //打开控制终端，文件权限可读可写，默认阻塞方式打开
	                       //ttyS0 表示的是串口 uart0
	if (fd < 0) {
		perror("open /dev/tty");
		exit(1);
	}

	n = read(fd, buf, 10); //读10个字符
	if(n < 0){
		perror("read ");
		exit(1);
	}
	printf("%s /n",buf);

	return 0;
}