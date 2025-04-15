#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

int main()
{
  int fd;
  int ret;
  fd_set rfds;
  char buff[15];

  fd = open("/dev/uart", O_RDONLY|O_NONBLOCK); //只读，非阻塞
  if (fd < 0)
  {
    printf("Device open failure\n");
    return -1;
  }

  while (1)
  {
    FD_ZERO(&rfds);  //清空读集合
    FD_SET(fd, &rfds); //添加设备到读即可
    int nready = select(fd + 1, &rfds , NULL, NULL, NULL);  /*阻塞等待某设备有事件发生 -> 交由内核去遍历解除阻塞
    I/O多路复用:  select -> sys_poll -> 驱动里.poll 函数          
    1. 拷贝 fds到内核空间 
    2. 遍历 fds，把当前进程添加到，每个fd设备的等待队列中  (fd：是设备的文件句柄)
    3. 当某个fd设备有I/O数据时，其驱动会唤醒该设备等待队列上睡眠的进程（ 通过驱动的.write函数)
    4. 系统调用 sys_poll 去轮询,调用驱动.poll 通过返回值知道，是否有I/O事件发生，有则解除阻塞，返回应用层。	
   */
    if (nready<=0) {
      printf("select err or timeout\n");
      return -1;			
    }

    if (FD_ISSET(fd, &rfds)) //数据可获得
    {
      printf("Poll monitor:can be read\n");	  			
      ret = read (fd, buff, sizeof(buff) - 1); //读数据（无数据则马上返回）
      if (ret < 0) {
        printf("read err\n");
        return -1;	        
      } 
      printf("read buf is %s\n",buff);
    }

    printf("select go !!!\n");
  }
}


/*多个设备时的做法
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#define DEV_COUNT 5
const char *device_paths[DEV_COUNT] = {
    "/dev/uart0", "/dev/uart1", "/dev/uart2", "/dev/uart3", "/dev/uart4"
};

int main() {
    int fds[DEV_COUNT];
    int i, ret;
    char buf[64];
    fd_set rfds;

    int maxfd = 0;

    // 打开所有设备文件
    for (i = 0; i < DEV_COUNT; i++) {
        fds[i] = open(device_paths[i], O_RDONLY | O_NONBLOCK);
        if (fds[i] < 0) {
            perror("open");
            return -1;
        }
        if (fds[i] > maxfd)
            maxfd = fds[i];
    }

    while (1) {
        FD_ZERO(&rfds);

        for (i = 0; i < DEV_COUNT; i++) {
            FD_SET(fds[i], &rfds);
        }

        // 等待任意一个设备可读
        ret = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select");
            break;
        }

        for (i = 0; i < DEV_COUNT; i++) {
            if (FD_ISSET(fds[i], &rfds)) {
                int n = read(fds[i], buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    printf("[fd %d] read: %s\n", fds[i], buf);
                } else {
                    perror("read");
                }
            }
        }

        printf("select loop end\n");
    }

    // 关闭所有设备文件
    for (i = 0; i < DEV_COUNT; i++) {
        close(fds[i]);
    }

    return 0;
}


*/