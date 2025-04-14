#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void sig_handler(int signum) //异步通知的处理程序（接收到异步读信号后的动作）
{
  printf("sig_handler go ,signalnum:%d\n",signum);
  //read(fd)  收到异步通知的信号后，再去读取具体的数据
}

int main()
{
	int fd = 0;
	int oflags;

	fd = open("/dev/uart",O_RDWR);
	if (fd < 0)
	{
		printf("Device open failure\n");
		return -1;
	}

	//启动信号驱动机制
	signal(SIGIO, sig_handler); //注册信号处理函数(让sig_handler来处理SIGIO信号)
	fcntl(fd, F_SETOWN, getpid());  //将本应用程序的进程号告诉给内核（设置设备文件的所有者为本进程）
	oflags = fcntl(fd, F_GETFL);    //获取当前的进程状态（会调用 驱动中的 .fasync）  
	fcntl(fd, F_SETFL, oflags | FASYNC);  //开启当前进程异步通知功能（FASYNC  设置支持异步通知模式 ）

	while (1)
	{
		sleep(100);
	}  
}