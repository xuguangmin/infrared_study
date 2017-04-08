/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 ******************************************************************************
  文件名称 : key_reset.c
  作者    : 贾延刚
  生成日期 : 2014-01-09

  版本    : 1.0
  功能描述 : reset键

******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define KEY_RESET_DEV_NAME          "/dev/Jikong_Study_Reset"

static int l_key_reset_fd = -1;
static int internal_key_reset_open()
{
	l_key_reset_fd = open(KEY_RESET_DEV_NAME, O_RDWR); //270
	if(l_key_reset_fd < 0)
	{
		perror("internal_key_reset_open");
		return 0;
	}
	return 1;
}

/* 如果键按下有效，则返回1 */
int key_reset_launch()
{
	unsigned char buffer[1];
	if(l_key_reset_fd < 0)
	{
		if(!internal_key_reset_open())
		{
			l_key_reset_fd = -1;
			return 0;
		}
	}

	return read(l_key_reset_fd, buffer, 1);
}
