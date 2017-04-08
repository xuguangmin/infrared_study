/*
 * serial.c
 *
 *  Created on: 2013年8月26日
 *      Author: flx
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <stdarg.h>
#include "log.h"

#define SERIAL_PORT_NAME         "/dev/s3c2410_serial0"

static int g_fd_serial = -1;

/* 把控制台转移到tty1 */
static int transfer_console()
{
	int fd_tty = open("/dev/tty1",O_RDONLY); // 改变console
	if(fd_tty < 0)
	{
		to_log("open /dev/tty1 failed");
		return 0;
	}

	if(ioctl(fd_tty, TIOCCONS) != 0)
		return 0;

	close(fd_tty);
	return 1;
}
/* 恢复控制台 */
static int internal_restore_console()
{
	int fd_serial = open(SERIAL_PORT_NAME, O_RDWR);         //恢复console 到串口0
	if(fd_serial < 0)
	{
		to_log("%s failed", __FUNCTION__);
		return 0;
	}

	if(ioctl(fd_serial, TIOCCONS) != 0)
		return 0;

	close(fd_serial);
	return 1;
}

static int internal_serial_config(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct  termios new_ios,old_ios;

    if ( tcgetattr ( fd , &new_ios ) !=0 )
	{
		return -1;
	}

    bzero( &old_ios , sizeof( struct termios ));
    old_ios=new_ios;

    tcflush(fd,TCIOFLUSH) ;
    new_ios.c_cflag |= CLOCAL |CREAD ;
    new_ios.c_cflag &= ~CSIZE ;

    switch (nBits)
    {
        case 5:
            new_ios.c_cflag |=CS5 ;
            break ;
        case 6:
            new_ios.c_cflag |=CS6 ;
            break ;
        case 7:
            new_ios.c_cflag |=CS7 ;
            break ;
        case 8:
            new_ios.c_cflag |=CS8 ;
            break ;
        default:
            return -1 ;
    }
    switch (nSpeed )
    {
        case 2400:
            cfsetispeed(&new_ios , B2400);
            cfsetospeed(&new_ios , B2400);
            break;
        case 4800:
            cfsetispeed(&new_ios , B4800);
            cfsetospeed(&new_ios , B4800);
            break;
        case 9600:
            cfsetispeed(&new_ios , B9600);
            cfsetospeed(&new_ios , B9600);
            break;
        case 19200:
            cfsetispeed(&new_ios , B19200);
            cfsetospeed(&new_ios , B19200);
            break;
        case 115200:
            cfsetispeed(&new_ios , B115200);
            cfsetospeed(&new_ios , B115200);
            break;
        case 460800:
            cfsetispeed(&new_ios , B460800);
            cfsetospeed(&new_ios , B460800);
            break;
        default:
            return -1;
    }
    switch (nEvent )
    {
        case 'o':
        case 'O':
            new_ios.c_cflag |= PARENB ;
            new_ios.c_cflag |= PARODD ;
            new_ios.c_iflag |= (INPCK | ISTRIP);
            break ;
        case 'e':
        case 'E':
            new_ios.c_iflag |= (INPCK | ISTRIP);
            new_ios.c_cflag |= PARENB ;
            new_ios.c_cflag &= ~PARODD ;
            break ;
        case 'n':
        case 'N':
            new_ios.c_cflag &= ~PARENB ;
            new_ios.c_iflag &= ~INPCK  ;
            break ;
        default:
            return -1 ;
    }
    if ( nStop == 1 )
        new_ios.c_cflag &= ~CSTOPB ;
    else if ( nStop == 2 )
        new_ios.c_cflag |= CSTOPB ;

    /*No hardware control*/
    new_ios.c_cflag &= ~CRTSCTS;
    /*No software control*/
    new_ios.c_iflag &= ~(IXON | IXOFF | IXANY);
    /*delay time set */
    //new_ios.c_cc[VTIME] = 0 ;
    //new_ios.c_cc[VMIN] = 0 ;

    /*raw model*/
    new_ios.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_ios.c_oflag  &= ~OPOST;

    new_ios.c_iflag &= ~(INLCR|IGNCR|ICRNL);
    new_ios.c_iflag &= ~(ONLCR|OCRNL);

    new_ios.c_oflag &= ~(INLCR|IGNCR|ICRNL);
    new_ios.c_oflag &= ~(ONLCR|OCRNL);


    tcflush(fd, TCIOFLUSH) ;
    if (tcsetattr(fd,TCSANOW,&new_ios) != 0 )
    {
        tcsetattr(fd,TCSANOW,&old_ios);
        return -1 ;
    }

    return  0;
}

static int internal_serial_open()
{
	g_fd_serial = open(SERIAL_PORT_NAME, O_RDWR|O_NOCTTY);// |O_NONBLOCK 打开串口0读写
	if(g_fd_serial < 0)
	{
		to_log("open %s failed", SERIAL_PORT_NAME);
		return 0;
	}

	if(internal_serial_config(g_fd_serial, 115200, 8, 'n', 1) != 0)
	{
		to_log("internal_serial_config failed");
		return 0;
	}
	return 1;
}

int serial_write(const unsigned char *buffer, int data_len)
{
	int k, dist=0;
	int split = 1024;        /* 按1024分割 */
	if(!buffer || data_len <= 0)
		return 0;

	for(k = 0; k < (data_len/split); ++k)
	{
		dist = k*split;

		if(write(g_fd_serial, buffer + dist, split) < 0)
		{
			//printf("%s write failed. %s\n", __FUNCTION__, strerror(errno));
			return 0;
		}
	}

	if(data_len % split)
	{
		int len = data_len % split;
		if(data_len/split) dist += split;

		if(write(g_fd_serial, buffer + dist, len) < 0)
		{
			//printf("%s write failed. %s\n", __FUNCTION__, strerror(errno));
			return 0;
		}
	}
	return 1;
}

int serial_read(unsigned char *buffer, int size)
{
	if(!buffer || size <= 0)
		return 0;

	if(g_fd_serial < 0)
		return 0;

	return read(g_fd_serial, buffer, size);
}

void serial_close()
{
	if(g_fd_serial >= 0) close(g_fd_serial);
	g_fd_serial = -1;
}

int serial_open()
{
	if(!transfer_console())
		return 0;

	if(!internal_serial_open())
	{
		serial_close();
		internal_restore_console();
		return 0;
	}

	to_log("serial port opened.\n");
	return 1;
}

/* 恢复控制台 */
int restore_console()
{
	serial_close();
	internal_restore_console();
	return 1;
}

void serial_log(const char *format, ...)
{
	va_list args;
	char buffer[1000];

	va_start(args, format);
	vsnprintf(buffer, BUFSIZ, format, args);
	va_end(args);

	serial_write((unsigned char *)buffer, strlen(buffer));
}


