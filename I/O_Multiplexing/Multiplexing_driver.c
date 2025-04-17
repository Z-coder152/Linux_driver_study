#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>  
#include <asm/io.h>  
#include <asm/uaccess.h> 
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/poll.h>

#define DEV_MA   500  //定义主设备号(区分不同的类别)
#define DEV_MI   0  //次设备号
#define DEV_NAME "uart" //设备名
struct  cdev cdev; //定义字符设备

#define C_BUF_LEN 64
char c_buf[C_BUF_LEN];

static wait_queue_head_t  queue;   //定义等待队列
struct fasync_struct *async_queue; //定义异步通知

int data_len=0; //设备初始数据为空
static ssize_t   write_dev(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret;

	if(count>C_BUF_LEN){
		printk("cout overflow\n");
		return -ENOMEM;
	}

	if(count<0){
		printk("cout err args\n");
		return -EINVAL;
	}

    if(copy_from_user (c_buf,buf,count)){
		printk(" copy from user fail\n");			  
		ret = -EFAULT; 
	}else{
		c_buf[63]='\0';    
		printk ("Received: %s, length is %ld\n", c_buf, count);  
		ret = count;		
	}
    wake_up_interruptible(&queue); //唤醒在等待队列上等待的进程
    kill_fasync(&async_queue, SIGIO, POLL_IN);  //发送SIGIO 异步通知信号
	data_len = count;  //写入后，有数据了
	return ret;

}


static ssize_t read_dev(struct file *file, char __user *buf, size_t count,  loff_t *f_pos){
	int ret;
	
	
	if(count > C_BUF_LEN -1 ) { 
		count = C_BUF_LEN -1; 
	}	
	if(count<0){
		printk("cout err args\n");
		return -EINVAL;
	}

  if (file->f_flags & O_NONBLOCK) { 
  	  //非阻塞访问时
      if(data_len == 0) {
      	  printk("dev no data \n");
          return -EAGAIN; //无数据，直接返回
      }
  } else{
    /*int wait_event_interruptible(wait_queue_head_t queue, int condition);
    *condition: 这是条件表达式，通常是一个逻辑表达式。如果该条件为假，线程会被阻塞在等待队列中
    */
    if(wait_event_interruptible(queue, data_len != 0))
    {
        return -ERESTARTSYS;
    }
  }

  //有数据则读数据
	if( copy_to_user(buf,c_buf,count)){
		printk(" copy to user fail\n");
		ret = -EFAULT; 
	}else{
		printk ("read %ld bytes from device\n", count); 
		ret = count;
	}
    data_len = 0;
	return ret;
}

/* dev_poll() 函数通过检查设备的当前状态，
 * 并通过 poll_wait() 将进程挂起到设备的等待队列，直到设备准备好进行 I/O 操作。
 * mask 变量根据设备的状态（如是否有数据可读、是否可以写数据）设置不同的事件标志，
 * 并将这些标志返回给 poll() 系统调用，允许用户进程知道设备的 I/O 操作情况。
 * poll_wait(): 使当前进程进入阻塞状态，等待设备的状态变化（如数据可读、可写等）。
 * mask: 返回的标志，表示当前设备可执行的 I/O 操作类型
*/
static unsigned int dev_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;

	poll_wait(filp, &queue, wait);  /*
  当设备有数据时，会唤醒再其等待队列上等待的所有进程，去执行。
  这实现了，多对多，poll_table里存放多个设备，等待队列里，存放访问设备的多个任务。
  poll_wait函数并不阻塞， 真正的阻塞动作是在poll系统调用中完成
					*/
	if (data_len != 0)
	{
		mask |= POLLIN | POLLRDNORM; //POLLIN:可阻塞读  POLLRDNORM:可读
		                         
	}
	if (data_len < C_BUF_LEN)
	{
		mask |= POLLOUT | POLLWRNORM; //POLLOUT:可阻塞写  POLLRDNORM:可写
	}
	return mask;      //通过立即unsigned int mask = 0;返回标志
}

static int init_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, &async_queue);  //初始化异步通知
}

struct file_operations dev_fops={
  .owner = THIS_MODULE,
  .write = write_dev, //当设备被写入，有数据后，可唤醒等待队列上的进程
  .read =read_dev,    //读设备，没有数据时，当前进程阻塞睡眠，放入等待队列
  .fasync = init_fasync,
  .poll = dev_poll, // poll 接口
};	

static int dev_init(void)
{
	int  devno = MKDEV(DEV_MA,DEV_MI); 
	int  ret; 

	//注册设备号
	ret = register_chrdev_region(devno,1,DEV_NAME);
	if(ret<0){
		printk("register_chrdev_region fail \n");
		return ret;
	}

	//初始化字符设备
	cdev_init(&cdev,&dev_fops); 
	ret =cdev_add(&cdev,devno,1); 
	if(ret<0){
		printk("cdev_add fail \n");
		return -1;
	}
    init_waitqueue_head(&queue);   //初始化等待队列
	printk("zzzz dev init ok\n");
	return 0;
}

static void dev_exit(void)
{
	int  devno = MKDEV(DEV_MA,DEV_MI); 
	cdev_del(&cdev);
	unregister_chrdev_region(devno,1);
	printk("zzzz dev exit \n");
}

module_init(dev_init); 
module_exit(dev_exit); 
MODULE_LICENSE("GPL"); 
