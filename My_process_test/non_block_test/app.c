#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>

int main(int argc, char **argv)
{
    int fd;
    char buff[10];
    
    fd = open("/dev/uart", O_RDONLY | O_NONBLOCK);//以只读且非阻塞形式 打开文件
    if(fd < 0)
    {
        perror("open");
        exit(1);
    }

    while(1)
    {
        read(fd, buff, sizeof(buff) - 1);
        printf("read buf is %s\n", buff);
        sleep(5);
    }
    close(fd);
    return 0;
}