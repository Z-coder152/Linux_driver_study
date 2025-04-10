#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>  
#include <asm/io.h>  
#include <asm/uaccess.h> 

#define DEV_MA 500 //主设备号
#define DEV_MI 0    //次设备号
#define DEV_NAME "uart" //设备名
struct cdev uart_dev;   //声明字符设备 结构体

#define C_BUF_LEN 64
char c_buf[C_BUF_LEN];
int data_len = 0;

//文件操作--write() 的函数实现  *驱动程序从 buf 中读取数据并将其写入到设备中（如硬盘、串口、显示屏等）*
static ssize_t write_dev(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)//`__user`用于标识缓冲区是用户空间内存
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

  if (file->f_flags & O_NONBLOCK) {  //判断文件是否以非阻塞模式打开的关键代码,表示文件操作（如 read 或 write）将不会因为数据不可用或其他资源未准备好而阻塞进程
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

struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .write = write_dev,
    .read = read_dev,
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
	cdev_init(&uart_dev,&dev_fops); 
	ret =cdev_add(&uart_dev,devno,1); 
	if(ret<0){
		printk("cdev_add fail \n");
		return -1;
	}

	printk("BBB dev init ok\n");
	return 0;
}

static void dev_exit(void)
{
	int  devno = MKDEV(DEV_MA,DEV_MI); 
	cdev_del(&uart_dev);
	unregister_chrdev_region(devno,1);
	printk("AAA dev exit \n");
}

module_init(dev_init); 
module_exit(dev_exit); 
MODULE_LICENSE("GPL");