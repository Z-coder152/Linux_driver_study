
/*
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

	fd = open("/dev/tty",O_RDONLY|O_NONBLOCK); //以只读，非阻塞方式，打开文件
	if (fd < 0) {
		perror("open /dev/tty");
		exit(1);
	}

	while(read(fd,&buf,1)!=1){//不断循环读，直到读到一个字符  *如果设备没有数据，则read函数马上返回，不等待*
		sleep(1);    
	}
	printf("%s \n",buf);

	return 0;
}
*/

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

    // 以只读，非阻塞方式打开文件
    fd = open("/dev/tty", O_RDONLY ); 
    if (fd < 0) {
        perror("open /dev/tty");
        exit(1);
    }

    while (1) {
        // 读取数据
        n = read(fd, &buf, 2);

        // 如果读取成功，打印数据
        if (n == 2) {
            printf("Read characters: %s\n", buf);
            break;
        }
        // 如果没有数据可读，返回 -1，且 errno 是 EAGAIN
        else if (n == -1) {
            if (errno == EAGAIN) {
                printf("No data available, will try again later...\n");
            } else {
                perror("read error");
                break;
            }
        }
        
        // 每次没有数据时，可以执行其他任务，或者可以继续轮询
        // 这里使用 sleep(1)，模拟其他任务
        sleep(2);
    }

    // 关闭文件
    close(fd);

    return 0;
}
