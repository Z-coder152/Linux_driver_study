#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>  
#include <asm/io.h>  
#include <asm/uaccess.h> 
#include <linux/signal.h>

#define DEV_MA   500  //定义主设备号(区分不同的类别)
#define DEV_MI   0  //次设备号
#define DEV_NAME "uart" //设备名
struct  cdev cdev; //定义字符设备

#define C_BUF_LEN 64
char c_buf[C_BUF_LEN];

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
		printk ("Received: %s\n", c_buf);  
		ret = count;		
	}

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
  } 

  //有数据则读数据
	if( copy_to_user(buf,c_buf,count)){
		printk(" copy to user fail\n");
		ret = -EFAULT; 
	}else{
		printk ("read %ld bytes from device\n", count); 
		ret = count;
	}
	return ret;
}

static int init_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, &async_queue);  //初始化异步通知
}

struct file_operations dev_fops={
  .owner = THIS_MODULE,
  .write = write_dev,
  .read =read_dev, 
  .fasync = init_fasync,
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