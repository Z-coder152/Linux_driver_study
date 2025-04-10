#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/of.h>

static int led_gpiod_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    const char *name;	
	unsigned int tmp;
	unsigned int tmparray[3];

    if(!of_property_read_string(np, "zhan_addr", &name)){
        printk("zhan's address = %s\n", name);
    }
    if(!of_property_read_u32(np, "zhan_age", &tmp)){
        printk("zhan's address = %d\n", tmp);
    }
    if (!of_property_read_u32_array(np, "zhan_infos", tmparray, 3)) {
		printk("yh_infor[0] =%d\n",tmparray[0]);
		printk("yh_infor[1] =%d\n",tmparray[1]);		
		printk("yh_infor[2] =%x\n",tmparray[2]);			
	}
    printk("led_gpiod_probe  ok 1\n");
	return 0;

};

int led_gpiod_remove(struct platform_device *pdev)
{
	printk("led_gpiod_remove  ok\n");   
	return 0;
}

static struct of_device_id ids[] = {
    {.compatible = "zhan-led-gpiod"},
    {},
};

static struct platform_driver led_gpiod_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "led_gpiod_driver",
        .of_match_table = ids,
    },
    .probe = led_gpiod_probe,
    .remove = led_gpiod_remove,
};

module_platform_driver(led_gpiod_driver);
MODULE_LICENSE("GPL");