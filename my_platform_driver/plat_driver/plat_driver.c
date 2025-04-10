#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>

static struct resource *res1;
static struct resource *res2;


static int plat_probe(struct platform_device *pdev)
{
	printk("Entering function plat_prob, which means match is ok\n");
	/*
	 *platform_get_resource(struct platform_device * dev, unsigned int type, unsigned int num)
	 */
	//获取第一个资源(也即设备树对应节点reg中的第一个<>内的)
	res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);/*从平台设备里获取资源
       pdev: 指向平台设备的指针
       IORESOURCE_MEM:资源类型
       0: 资源序号
    */
    if(res1 == NULL){
		printk("res1 platform_get_resource fail \n");
		return -ENODEV;
    	}
	printk("res1->start=%llx\n",res1->start);
	
	//获取第二个资源
	res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    if(res2 == NULL){
		printk("res2 platform_get_resource fail \n");
		return -ENODEV;
    	}
	printk("res2->start=%llx\n",res2->start);
	return 0;
}

static int plat_remove(struct platform_device *pdev)
{
    printk("plat_remove go\n");
    return 0;
}


/*下面这几行代码充当驱动和平台设备之间的桥梁。通过compatible参数，
 *内核自动找到与之对应的设备树节点，也即设备信息或称resource
 */
static const struct of_device_id of_plat_match[] = {
	{.compatible = "yhai-plat",},  //此驱动所支持的设备列表
	{},
};
MODULE_DEVICE_TABLE(of, of_plat_match);//这个宏让驱动程序公开其 ID 表，该表描述它可以支持哪些
											 //设备，用于匹配设备

static struct platform_driver plat_driver = {
	.driver = {
		.name = "yhai plat driver",
		.owner = THIS_MODULE,
		.of_match_table = of_plat_match,
		},
	.probe = plat_probe, 
	.remove = plat_remove, 
};

module_platform_driver(plat_driver);//声明平台设备驱动的入口
MODULE_LICENSE("GPL"); //模块免费开源声明
MODULE_AUTHOR("Chanlgin"); //模块作者声明(可选)


