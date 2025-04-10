#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int SSD1306_send_cmd (int hi2c, unsigned char cmd)
{
  unsigned char sending_buffer[2];

  sending_buffer[0] = 0x00;
  sending_buffer[1] = cmd;

  return write (hi2c, sending_buffer, 2);
}

void SSD1306_Init(int fd)
{
  SSD1306_send_cmd(fd,0xAE);//关闭显示

  SSD1306_send_cmd(fd,0xD5);//设置时钟分频因子,震荡频率
  SSD1306_send_cmd(fd,80);  //[3:0],分频因子;[7:4],震荡频率

  SSD1306_send_cmd(fd,0xA8);//设置驱动路数
  SSD1306_send_cmd(fd,0X3F);//默认0X3F(hi2c,1/64)
  SSD1306_send_cmd(fd,0xD3);//设置显示偏移
  SSD1306_send_cmd(fd,0X00);//默认为0

  SSD1306_send_cmd(fd,0x40);//设置显示开始行 [5:0],行数.

  SSD1306_send_cmd(fd,0x8D);//电荷泵设置
  SSD1306_send_cmd(fd,0x14);//bit2，开启/关闭

  SSD1306_send_cmd(fd,0x20);//设置内存地址模式
  SSD1306_send_cmd(fd,0x00);

  SSD1306_send_cmd(fd,0xA1);//段重定义设置,bit0:0,0->0;1,0->127;
  SSD1306_send_cmd(fd,0xC0);

  SSD1306_send_cmd(fd,0xDA);//设置COM硬件引脚配置
  SSD1306_send_cmd(fd,0x12);//[5:4]配置

  SSD1306_send_cmd(fd,0x81);//对比度设置
  SSD1306_send_cmd(fd,0xEF);//1~255;默认0X7F (hi2c,亮度设置,越大越亮)

  SSD1306_send_cmd(fd,0xD9);//设置预充电周期
  SSD1306_send_cmd(fd,0xf1);//[3:0],PHASE 1;[7:4],PHASE 2;

  SSD1306_send_cmd(fd,0xDB);//设置VCOMH 电压倍率
  SSD1306_send_cmd(fd,0x30);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

  SSD1306_send_cmd(fd,0xA4);//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)

  SSD1306_send_cmd(fd,0xA6);//设置显示方式;bit0:1,反相显示;0,正常显示

  SSD1306_send_cmd(fd,0xAF);//开启显示	
}
int main(int argc, const char *argv[])
{
	int ret;
	int fd;

	
	fd = open("/dev/i2c-0", O_WRONLY);  //打开i2c控制器(下面可以挂多个i2c设备)
	if (fd < 0) {
		perror("open");
		exit(1);
	}

  ret = ioctl (fd, I2C_SLAVE_FORCE, 0x3c);  //指定i2c从机设备的id  (id为0x3c 的是oled屏)
  if (ret < 0) {
  	perror("ioctl");
  	exit(1);
  }		

  SSD1306_Init(fd);  //初始化oled屏
  sleep(2);
  SSD1306_send_cmd(fd,0xAE);//关闭显示

  printf("test i2c oled \n");
  close(fd);
  return 0;
}