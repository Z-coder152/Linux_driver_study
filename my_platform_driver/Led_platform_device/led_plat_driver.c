#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>

#define LED_MA 500 //定义主设备号，区分不同设备类别
#define LED_MI 0   //次设备号

#define LED_MAGIC 'L'
#define LED_ON _IOW(LED_MAGIC, 0, int) //使用幻术定义和LED对应唯一操作数
#define LED_OFF _IOW(LED_MAGIC, 1, int)

//#define PINMUX_AUX_DAP4_SCLK_0 0x70003150 //定义管脚复用寄存器地址
//#define GPIO3 0x6000D200 //定义GPIO3寄存器的基地址
#define CNF 0x04 	//下面几个宏 定义了J组第7号GPIO引脚的基本配置寄存器偏移地址
#define OE 0x14
#define OUT 0x24	
#define MSK_CNF 0x84 
#define MSK_OE 0x94
#define MSK_OUT 0xA4

struct cdev led_zhan; //声明字符设备

unsigned char *gpio_base;
unsigned char *gpio_pinmux;

static struct resource *res1;
static struct resource *res2;

static int led_open(struct inode* innod, struct file * file)
{
	printk("led is opened\n ");
	return 0;
}
static int led_release(struct inode * innod, struct file * file)
{
	printk("led is closed\n");
	return 0;
}
static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)//文件定制操作
{
	switch(cmd){
		case LED_ON:
			writel(readl(gpio_base+OUT) | 1 << 7, gpio_base+OUT);  //指定输出高电平
			printk("LED state is ON! \n");
			break;
		case LED_OFF:
			writel(readl(gpio_base+OUT) & ~(1 << 7), gpio_base+OUT);  //指定输出低电平
			printk("LED state is OFF! \n");
			break;
		default:
			printk("no found this cmd!!!\n");
	}
    return 0;
}
struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_release,
	.unlocked_ioctl = led_ioctl,	
};
static int led_plat_probe(struct platform_device *pdev)
{
	printk("Entering function plat_prob, which means match is ok\n");
    res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);//对应GPIO基地址
    if(res1 == NULL){
		printk("res1 platform_get_resource fail \n");
		return -ENODEV;
    	}
	
	//获取第二个资源
	res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);//对应管脚复用
    if(res2 == NULL){
		printk("res2 platform_get_resource fail \n");
		return -ENODEV;
    	}

    int ret, ret_1;
	int devnum = MKDEV(LED_MA, LED_MI); //合并主次设备号，生成新的32位设备ID，避免资源浪费

	ret = register_chrdev_region(devnum, 1, "zhan_char_dev");
	if(ret < 0) //添加异常判断
	{
		printk("register_chrdev_region failed! \n");
		return ret;
	}
    //初始化字符设备(init 和 add 函数)
	cdev_init(&led_zhan,&led_fops); //将字符设备与文件操作绑定
	ret_1 = cdev_add(&led_zhan, devnum, 1);	//将字符设备注册到内核中
	if(ret_1 < 0) //添加异常判断
	{
		printk("cdev_add failed! \n");
		goto err1;
	}

    //动态映射物理地址到内核虚拟地址，返回值
	gpio_pinmux = ioremap(res2->start, 8);//指定管脚复用寄存器地址，‘8’为映射范围大小，单位字节
	if(gpio_pinmux == NULL){
		printk("ioremap gpio_pinmux error!\n");
		goto err2;
	}
	writel((readl(gpio_pinmux)& ~(1<<4)) | 1, gpio_pinmux);

	gpio_base = ioremap(res1->start,0xFF);
	if(gpio_base == NULL){
		printk("ioremap gpio_base error! \n");
		goto err3;
	}
	writel(readl(gpio_base+CNF) | 1 << 7, gpio_base+CNF);   //配置引脚GPIO3_PJ.07 为 GPIO模式
    writel(readl(gpio_base+OE) | 1 << 7, gpio_base+OE);	  //使能引脚(7号)
    writel(readl(gpio_base+OUT) | ~(1 << 7), gpio_base+OUT); //输出低电平，保持初始状态为：灯不亮
    writel(readl(gpio_base+MSK_CNF) | 1 << 7, gpio_base+MSK_CNF); //取消对GPIO模下引脚的屏蔽
    writel(readl(gpio_base+MSK_OE) | 1 << 7, gpio_base+MSK_OE);  //取消引脚 使能屏蔽
    printk("led_probe(1) is done\n");
	return 0;
err3: //顺序申请，逆序释放(成功的需要释放，失败的自动释放了)
	iounmap(gpio_pinmux); 
err2:
	cdev_del(&led_zhan);
err1:
	unregister_chrdev_region(devnum, 1);
	return -1;

}

static int led_plat_remove(struct platform_device *pdev)
{
    int devnum = MKDEV(LED_MA, LED_MI);
	iounmap(gpio_pinmux); 
	iounmap(gpio_base);
	cdev_del(&led_zhan); //和字符设备添加相配对
	unregister_chrdev_region(devnum, 1);//卸载设备时，需要取消注册，释放资源（与注册行为配对）
    printk("led_platform exit\n");
    return 0;
}

static const struct of_device_id of_plat_match[] = {
	{.compatible = "led-device-zhan",},  //此驱动所支持的设备列表
	{},
};
MODULE_DEVICE_TABLE(of, of_plat_match);//这个宏让驱动程序公开其 ID 表，该表描述它可以支持哪些
											 //设备，用于匹配设备

static struct platform_driver led_driver = {
	.driver = {
		.name = "zhan led-plat-driver",
		.owner = THIS_MODULE,
		.of_match_table = of_plat_match,
		},
	.probe = led_plat_probe, 
	.remove = led_plat_remove, 
};

module_platform_driver(led_driver);//声明平台设备驱动的入口
MODULE_LICENSE("GPL"); //模块免费开源声明
MODULE_AUTHOR("Chanlgin"); //模块作者声明(可选)