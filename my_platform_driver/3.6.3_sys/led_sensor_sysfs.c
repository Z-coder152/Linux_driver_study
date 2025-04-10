#include <linux/init.h>                        
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>

#include <linux/of_gpio.h>
#include <linux/gpio.h>

#include <linux/platform_device.h>
#include <asm/io.h>

#include <dt-bindings/gpio/gpio.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#endif

struct gpio_dev{
	struct platform_device *pdev;
	int led_gpio;
	int led_gpio_output;
	int smoke_sensor_gpio;
};

struct gpio_dev * gpio_dev_pointer = NULL;

static ssize_t led_store(struct device *dev, struct device_attribute *attr,
              const char *buf, size_t count) {
              
    //写文件,控制 gpio 输出 （ echo 1 >   led_gpio)                
    if (buf[0] == '0') {
		gpio_dev_pointer->led_gpio_output = 0;
        gpio_direction_output(gpio_dev_pointer->led_gpio, gpio_dev_pointer->led_gpio_output);
    }else if (buf[0] == '1') {
		gpio_dev_pointer->led_gpio_output = 1;
        gpio_direction_output(gpio_dev_pointer->led_gpio, gpio_dev_pointer->led_gpio_output);
    }    

    printk(KERN_ERR "led_gpio_store %c \n",gpio_dev_pointer->led_gpio_output);   
    return count;
}

static ssize_t smoke_sensor_show(struct device *dev, struct device_attribute *attr,char *buf){  
	int sensor_value = 0;
	sensor_value = gpio_get_value(gpio_dev_pointer->smoke_sensor_gpio);//cat smoke_sensor_gpio时会触发
	return snprintf(buf, PAGE_SIZE, "%d\n",sensor_value);
}

static struct device_attribute gpio_attr[] = {  
    __ATTR(led_gpio, 0664, NULL, led_store), //写：关联属性文件的写回调函数(*store)  echo时触发
    __ATTR(smoke_sensor_gpio, 0664, smoke_sensor_show,NULL), //读：关联属性文件的读回调函数(*show)  cat 时触发
};

static int gpio_init_sysfs(struct device *dev)
{
	int i, ret;
	for(i = 0; i < ARRAY_SIZE(gpio_attr); i++){
		ret = sysfs_create_file(&dev->kobj, &gpio_attr[i].attr);
		if(ret){
			dev_err(dev, "create charger node(%s) error\n",gpio_attr[i].attr.name);
       		return -1;
			}			
		}
	return 0;
}

static int gpio_remove_sysfs(struct device *dev)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(gpio_attr); i++){
		 sysfs_remove_file(&dev->kobj, &gpio_attr[i].attr);
		}
	return 0;
}


static int get_OF_node_property(struct gpio_dev *pdata){
	int gpio;
	int ret;
	enum of_gpio_flags flags;
	struct device *dev = &pdata->pdev->dev;
	struct device_node *node = dev->of_node;


	gpio = of_get_named_gpio_flags(node, "led_gpio", 0, &flags);
	if(gpio_is_valid(gpio))
		{
			pdata->led_gpio = gpio;
			pdata->led_gpio_output = (flags == GPIO_ACTIVE_HIGH)?1:0;
			printk(KERN_ERR"led_gpio: %d\n", pdata->led_gpio);
			ret = devm_gpio_request(dev,gpio,"led_gpio");
			if(ret){
					printk("Failed to get led_gpio gpio.\n");
					return -1;
				}
		}
	
	gpio = of_get_named_gpio_flags(node, "smoke_sensor_gpio", 0, &flags);
    if (gpio_is_valid(gpio))
    {
	pdata->smoke_sensor_gpio        = gpio;
	printk(KERN_ERR"smoke_sensor_gpio: %d\n", pdata->smoke_sensor_gpio);

	ret = devm_gpio_request(dev,gpio, "smoke_sensor_gpio");
	if (ret) {
	   printk("Failed to get smoke_sensor_gpio gpio.\n");
	   return -1;
	}

	ret = gpio_direction_input(gpio);
	if (ret) {
	   printk("Failed to set flame_sensor_gpio gpio.\n");
	   return -1;
	}
    }
	return 0;
	
}

static void gpio_set_default(struct gpio_dev *pdata) {
    if (pdata->led_gpio) {
        gpio_direction_output(pdata->led_gpio, pdata->led_gpio_output);
    }
}


int leds_probe(struct platform_device *pdev)
{
   int  ret = 0;
   printk(KERN_ALERT "%s \n",__func__);      
   gpio_dev_pointer = kmalloc(sizeof(struct gpio_dev), GFP_KERNEL);//为结构体指针申请空间
   if (gpio_dev_pointer == NULL) {
      printk(KERN_ERR"kmalloc struct gpio_dev err \n");
      return -ENOMEM;
   }
   memset(gpio_dev_pointer, 0, sizeof(struct gpio_dev)); //初始化结构体
   
   gpio_dev_pointer->pdev = pdev;
   ret = get_OF_node_property(gpio_dev_pointer);
   if(ret<0){
	  printk(KERN_ERR"gpio_dt err \n");	
	  return -1;
     }
   gpio_set_default(gpio_dev_pointer);
   ret = gpio_init_sysfs(&gpio_dev_pointer->pdev->dev);
   if(ret<0){
	printk(KERN_ERR"gpio_init_sysfs  err \n");	
	return -1;
   }
   printk(KERN_ALERT "%s  ok !!\n",__func__);
   return 0;

}

int leds_remove(struct platform_device *pdev)
{   
    gpio_remove_sysfs(&gpio_dev_pointer->pdev->dev);
    kfree(gpio_dev_pointer);
	printk("leds_remove ok\n");   
    return 0;
}

static const struct of_device_id of_led_match[] = {
    { .compatible = "sys_rw_led_sensor", },
    {},
};

MODULE_DEVICE_TABLE(of, of_led_match);

struct platform_driver leds_drv = {
   .driver = {
      .owner = THIS_MODULE,
      .name = "sys_rw_led driver" ,
      .of_match_table = of_led_match,
   },
   .probe = leds_probe,
   .remove = leds_remove,    
};

module_platform_driver(leds_drv);
MODULE_LICENSE("GPL");

