/*
 * infrared.c
 *
 *  Created on: 2013年8月26日
 *      Author: flx
 */

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "config.h"
#include "log.h"


#ifdef DEBUG_IN_JIKONG
#define INFRARED_PORT_NAME     "/dev/FPGA_Jikong11"      /* 集控主板的红外端口，仅用来调试 */
#define INFRARED_PORT_NO       2
#else
#define INFRARED_PORT_NAME     "/dev/irda_Jikong00"
#define INFRARED_PORT_NO       1
#endif

static int g_fd_infrared = -1;


#if 0
/*
 * 一次发送数据不能超过1000个字节
 */
static int internal_infrared_write(const unsigned char *buffer, int data_len)
{
	int k, dist = 0;
	int split = 1000;        /* 按1000分割 */
	if(!buffer || data_len <= 0)
		return 0;

	to_log("data_len=%d\n", data_len);
	for(k = 0; k < (data_len/split); ++k)
	{
		dist = k*split;

		to_log("k = %d, dist=%d\n", k, dist);
		if(write(g_fd_infrared, buffer + dist, split) < 0)
		{
			to_log("%s write failed. %s\n", __FUNCTION__, strerror(errno));
			return 0;
		}
	}

	if(data_len % split)
	{
		int len = data_len % split;
		if(data_len/split) dist += split;

		to_log("2 dist=%d, len = %d\n", dist, len);
		if(write(g_fd_infrared, buffer + dist, len) < 0)
		{
			to_log("%s write failed. %s\n", __FUNCTION__, strerror(errno));
			return 0;
		}
	}
	return 1;
}
#endif

static int infrared_is_valid()
{
	return (g_fd_infrared < 0) ? 0:1;
}

int infrared_write(const unsigned char *buffer, int data_len)
{
	int result;
	if(!buffer || data_len <= 0)
		return 0;

	if(!infrared_is_valid())
	{
		to_log("infrared port is invalid\n");
		return 0;
	}

	result = write(g_fd_infrared, buffer, data_len);

#ifdef DEBUG_IN_JIKONG
	return (result < 0) ? 0:1;
#else
	return 1;        /* 红外学习板驱动固定返回了 -1 */
#endif
}

int infrared_read(unsigned char *buffer, int size)
{
	if(!infrared_is_valid())
		return 0;

	return read(g_fd_infrared, buffer, size);
}

int infrared_close()
{
	close(g_fd_infrared);
	g_fd_infrared = -1;
	return 1;
}

int infrared_open()
{
	g_fd_infrared = open(INFRARED_PORT_NAME, O_RDWR);
	if(g_fd_infrared < 0)
	{
		perror("open infrared port");
		return 0;
	}

	if(ioctl(g_fd_infrared, (short)INFRARED_PORT_NO, NULL) < 0)
	{
		perror("ioctl infrared port");
		return 0;
	}
	printf("infrared port opened.\n");
	return 1;
}



