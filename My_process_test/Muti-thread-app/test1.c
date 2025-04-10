#include <stdio.h>
#include <pthread.h>

int tickets = 1000; //用全局变量，让线程间共享数据

pthread_mutex_t lock; //定义锁

void *pth_work(void *arg)
{
  while (1) {
    //pthread_mutex_lock(&lock); //上锁( 不能获得时，会阻塞睡眠)
                              //trylock  尝试请求锁，锁不可用时，直接return，不会线程卡住
    tickets--;
    if(tickets <= 0)
    {
        printf("no tickets!\n");
        //pthread_mutex_unlock(&lock); //解锁
        break;
    }
    usleep(10000);
    printf("pthread id=%ld count=%d\n", pthread_self() , tickets); //pthread_self 获得线程自己的ID
    //pthread_mutex_unlock(&lock); //解锁
    
    
  }
  return NULL;
}


int main(void)
{
  pthread_t id[5];  

  pthread_mutex_init(&lock, NULL); //构造锁

  //创建多个写线程
  for(int i = 0; i < 5; i++)
  {
    pthread_create(id + i, NULL, pth_work, NULL);
  }
  
  //等待子线程退出，回收其资源  
  for(int i = 0; i < 5; i++)
  {
    pthread_join(id[i], NULL);
  }  
  
  pthread_mutex_destroy(&lock); //销毁锁
  return 0;
}