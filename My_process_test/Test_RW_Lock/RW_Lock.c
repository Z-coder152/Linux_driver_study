#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/rwlock.h>

rwlock_t lock;       //定义锁
struct task_struct *task1_id = NULL;
struct task_struct *task2_id = NULL;
struct task_struct *task3_id = NULL;
struct task_struct *task4_id = NULL;

int num = 1;
int thread_num=1;
int read_thread(void *arg)
{
  int n = 0;
  int id = thread_num++;
  while(1)
  {
    read_lock(&lock); //读锁
    printk("%s %d  %d read num= %d \n",__func__,id,n++,num);
    read_unlock(&lock); //释放读锁
	
    ssleep(3); 
    if(kthread_should_stop())  //接收停止信号
    {
	    break;
    }		 
  }
  return 0;
}

int write_thread(void *arg)
{
  int n = 0;
  int id = thread_num++;
  while(1)
  {  
    write_lock(&lock); //写锁
    num++;
    printk("%s %d %d write num= %d \n",__func__,id,n++,num);
    write_unlock(&lock); //释放写锁
	
    ssleep(3);  
    if(kthread_should_stop())  //接收停止信号
    {
	    break;
    }		
  }
  return 0;
}


static int __init base_init(void)
{
	printk("%s:\n",__func__);
	task1_id = kthread_run(read_thread,NULL,"thread1"); //创建内核线程  
	task2_id = kthread_run(read_thread,NULL,"thread2"); //创建内核线程  
	task3_id = kthread_run(write_thread,NULL,"thread3"); //创建内核线程  	
	task4_id = kthread_run(write_thread,NULL,"thread4"); //创建内核线程  	
	rwlock_init(&lock);  //初始化锁	
	return 0;
}

static void __exit base_exit(void)
{
  printk("%s:\n",__func__);
  kthread_stop(task1_id);  //停止内核线程(通过id指定)
  kthread_stop(task2_id);
  kthread_stop(task3_id);
  kthread_stop(task4_id);    
}

module_init(base_init);
module_exit(base_exit);