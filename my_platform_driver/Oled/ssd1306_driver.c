#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include <asm/uaccess.h>

#include "ssd1306_driver.h"
#include "Oled_Font.h"

MODULE_LICENSE("GPL");


#define SSD1306_MAJOR 500
#define SSD1306_MINOR 6

struct ssd1306_device {
	struct cdev cdev;
	struct i2c_client *client;
};
struct ssd1306_device *ssd1306; 

static int ssd1306_write_command_byte(struct i2c_client *client, unsigned char val)
{
    int ret = 0;
	char txbuf[2] = {0x00, val};

	struct i2c_msg msg[1] = {
		{client->addr, 0, sizeof(txbuf), txbuf}, //指定这条消息是 “写消息”
	};

	//printk("i2c write: addr=0x%02x, val=0x%02x\n", client->addr, val);

	ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0) {
        printk("i2c write failed: %d\n", ret);
        return ret;
    }
	//printk("i2c write success\n");

	return 0;
}

static int ssd1306_write_data_byte(struct i2c_client *client, unsigned char val)
{
    int ret = 0;
	char txbuf[2] = {0x40, val};

	struct i2c_msg msg[1] = {
		{client->addr, 0, sizeof(txbuf), txbuf}, //指定这条消息是 “写消息”
	};

	//printk("i2c write: addr=0x%02x, val=0x%02x\n", client->addr, val);

	ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0) {
        printk("i2c write failed: %d\n", ret);
        return ret;
    }
	//printk("i2c write success\n");

	return 0;
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(unsigned char Y, unsigned char X)
{
    struct i2c_client *client = ssd1306->client;
	ssd1306_write_command_byte(client, 0xB0 | Y);					//设置Y位置
	ssd1306_write_command_byte(client, 0x10 | ((X & 0xF0) >> 4));	//设置X位置低4位
	ssd1306_write_command_byte(client, 0x00 | (X & 0x0F));			//设置X位置高4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
    struct i2c_client *client = ssd1306->client;
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			ssd1306_write_data_byte(client, 0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	struct i2c_client *client = ssd1306->client;
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		ssd1306_write_data_byte(client, OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		ssd1306_write_data_byte(client, OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

static int ssd1306_open(struct inode *inode, struct file *file) 
{
	return 0;
}

static int ssd1306_release(struct inode *inode, struct file *file) 
{
	return 0;
}

static long ssd1306_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	
	struct i2c_client *client = ssd1306->client;
	struct ShowChar Oled_Char_arg;
	struct ShowString Oled_String_arg;
	struct ShowNum Oled_Num_arg;
	switch(cmd) {
	case OLED_OPEN:
		ssd1306_write_command_byte(client, 0xAF);
		break;
	case OLED_CLOSE:
        ssd1306_write_command_byte(client, 0xAE);
		break;
    case OLED_CLEAR:
        OLED_Clear();
        //printk("Oled is cleared now!\n");
		break;
	case OLED_Show_Char:
		if(copy_from_user(&Oled_Char_arg, (struct ShowChar *)arg, sizeof(Oled_Char_arg))){
			return -EFAULT;
		}
    	OLED_ShowChar(Oled_Char_arg.Line, Oled_Char_arg.Column, Oled_Char_arg.Char);
		break;
	case OLED_Show_String:
		if(copy_from_user(&Oled_String_arg, (struct ShowString *)arg, sizeof(Oled_String_arg))){
			return -EFAULT;
		}		
    	OLED_ShowString(Oled_String_arg.Line, Oled_String_arg.Column, Oled_String_arg.String);
		break;
	case OLED_Show_Num:
		if(copy_from_user(&Oled_Num_arg, (struct ShowNum *)arg, sizeof(Oled_Num_arg))){
			return -EFAULT;
		}		
    	OLED_ShowNum(Oled_Num_arg.Line, Oled_Num_arg.Column, Oled_Num_arg.Number, Oled_Num_arg.Length);
		break;
	default:
		printk("invalid argument\n");
		return -EINVAL;
	}
	return 0;
}

struct file_operations ssd1306_fops = {
	.owner 		= THIS_MODULE,
	.open		= ssd1306_open,
	.release 	= ssd1306_release,
	.unlocked_ioctl = ssd1306_ioctl,
};

static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	dev_t devno = MKDEV(SSD1306_MAJOR, SSD1306_MINOR);
	printk("match OK!\n");

	ssd1306 = kzalloc(sizeof(*ssd1306), GFP_KERNEL);
	if (ssd1306 == NULL) {
		return -ENOMEM;
	}

	ssd1306->client = client;

	ret = register_chrdev_region(devno, 1, "ssd1306_zcl");
	if (ret < 0) {
		printk("failed to register char device region!\n");
		goto err1;
	}

	cdev_init(&ssd1306->cdev, &ssd1306_fops);
	ssd1306->cdev.owner = THIS_MODULE;
	ret = cdev_add(&ssd1306->cdev, devno, 1);
	if (ret < 0) {
		printk("failed to add device\n");
		goto err2;
	}
	
	ssd1306_write_command_byte(client, 0xAE);
    ssd1306_write_command_byte(client, 0xD5);
    ssd1306_write_command_byte(client, 80);
    ssd1306_write_command_byte(client, 0xA8);
    ssd1306_write_command_byte(client, 0x3F);

    ssd1306_write_command_byte(client, 0xD3);
    ssd1306_write_command_byte(client, 0x00);

    ssd1306_write_command_byte(client, 0x40);//起始行

    ssd1306_write_command_byte(client, 0x8D);
    ssd1306_write_command_byte(client, 0x14);

    ssd1306_write_command_byte(client, 0x20);
    ssd1306_write_command_byte(client, 0x00);

    ssd1306_write_command_byte(client, 0xA0);//扫描方向，设置为从左到右
    ssd1306_write_command_byte(client, 0xC0);//扫描方向，设置为从上到下

    ssd1306_write_command_byte(client, 0xDA);
    ssd1306_write_command_byte(client, 0x12);

    ssd1306_write_command_byte(client, 0x81);
    ssd1306_write_command_byte(client, 0xEF);

    ssd1306_write_command_byte(client, 0xD9);
    ssd1306_write_command_byte(client, 0xF1);

    ssd1306_write_command_byte(client, 0xDB);
    ssd1306_write_command_byte(client, 0x30);

    ssd1306_write_command_byte(client, 0xA4);
    ssd1306_write_command_byte(client, 0xA6);
    ssd1306_write_command_byte(client, 0xAF); //开启显示


	return 0;
err2:
	unregister_chrdev_region(devno, 1);
err1:
	kfree(ssd1306);
	return ret;
}

static int ssd1306_remove(struct i2c_client *client)
{
	dev_t devno = MKDEV(SSD1306_MAJOR, SSD1306_MINOR);
	cdev_del(&ssd1306->cdev);
	unregister_chrdev_region(devno, 1);
	kfree(ssd1306);

	return 0;
}

static const struct i2c_device_id ssd1306_id[] = {
	{ "ssd1306", 0},
	{}
}; 

static struct of_device_id ssd1306_dt_match[] = {
	{.compatible = "oled_ssd1306" },
	{/*nothing to be done*/},
};

struct i2c_driver ssd1306_driver = {
	.driver = {
		.name 			= "ssd1306",
		.owner 			= THIS_MODULE,
		.of_match_table = of_match_ptr(ssd1306_dt_match),
	},
	.probe 		= ssd1306_probe,
	.remove 	= ssd1306_remove,
	.id_table 	= ssd1306_id,
};

module_i2c_driver(ssd1306_driver);