#include <stdio.h>  //for printf
#include <unistd.h> //for sleep
int main(int argc,char **argv)
{
	while(1){
		printf("app1 go\n");
		sleep(5);
	}	
	return 0;
}