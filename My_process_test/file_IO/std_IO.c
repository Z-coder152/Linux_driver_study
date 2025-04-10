#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
 
int main(void)
{
	char buf[10];
	int n;
 
	n = read(STDIN_FILENO, buf, 10);   //STDIN_FILENO 代表标准输入（键盘输入）
	                       //会阻塞等待，终端输入换行符
	if(n < 0){
		perror("read STDIN_FILENO");
		exit(1);
	}
	write(STDOUT_FILENO, buf, n);  //STDOUT_FILENO 代表标准输出(终端显示)
	
	return 0;
}