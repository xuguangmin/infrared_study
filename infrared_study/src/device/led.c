/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 ******************************************************************************
  文件名称 : led.c
  作者    : 贾延刚
  生成日期 : 2014-01-09

  版本    : 1.0
  功能描述 : 前面板的指示灯

******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define LED_STATUS_RED        0x0
#define LED_STATUS_GREEN      0x1
#define LED_DEV_NAME          "/dev/Jikong_study_Led_v2"


static int l_led_fd = -1;
static int internal_led_open()
{
	l_led_fd = open(LED_DEV_NAME, O_RDWR); //270
	if(l_led_fd < 0)
	{
		perror("internal_led_open()");
		return 0;
	}
	return 1;
}

static int internal_led_status_switch(unsigned char status)
{
	unsigned char buffer[1];
	if(l_led_fd < 0)
	{
		if(!internal_led_open())
		{
			l_led_fd = -1;
			return 0;
		}
	}

	buffer[0] = status;
	printf("status = %d\n", status);
	return (write(l_led_fd,buffer, 1) >= 0) ? 1:0;
}


int led_green()
{
	return internal_led_status_switch(LED_STATUS_GREEN);
}
int led_red()
{
	return internal_led_status_switch(LED_STATUS_RED);
}

void led_twinkle()
{
	int loop = 10;
	while(loop > 0)
	{
		led_red();
		usleep(500*1000);
		led_green();
		usleep(500*1000);
		loop--;
	}

}


