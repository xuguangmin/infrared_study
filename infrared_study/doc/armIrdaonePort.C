#include     <stdio.h>
#include     <stdlib.h>
#include     <unistd.h>
#include     <sys/ioctl.h>
#include     <stropts.h>
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>
#include     <termios.h>
#include     <errno.h>
#include     <string.h>
#include 	 <pthread.h>
#include 	 <semaphore.h>

#define RECV_DATA_BUFFER 4000

int fp;
int irdaFd = -1;						//ºìÍâ¿ÚŸä±ú

pthread_t readSerialPortThreadID;		//¶ÁÈ¡Ž®¿ÚÏß³ÌŸä±ú
pthread_t readInfraredThreadID;			//¶ÁÈ¡ºìÍâ¿ÚÏß³ÌŸä±ú

pthread_mutex_t writeSerialPortMutex = PTHREAD_MUTEX_INITIALIZER;//ÐŽŽ®¿Ú»¥³âËø
pthread_mutex_t irdaFdPortMutex = PTHREAD_MUTEX_INITIALIZER;//µ÷ÓÃºìÍâ»¥³âËø


sem_t semLearnInfrared;					//Ñ§Ï°ºìÍâÐÅºÅÁ¿

int nStopThread = 0;
int nExit = 0;

void* readSerialPortThreadFunc(void *param);			//¶ÁŽ®¿ÚÏß³Ìº¯Êý
void* readInfraredThreadFunc(void *param);			//¶ÁºìÍâ¿ÚÏß³Ìº¯Êý
void  proa_analyse_recv_data(char * data,int nLen); //œâÎöŽ®¿ÚÊýŸÝ(ÍêÕû°ü)
int irdaCode_test_function(char *codeStr,int len); //²âÊÔºìÍâÊýŸÝ
int send_reply_msg(unsigned char cType, int len,char *data);

int send_reply_msg(unsigned char cType, int len,char *data)
{
	if(fp <= 0)
	{
		return -1;
	}

	pthread_mutex_lock(&writeSerialPortMutex);
	char strMsg[RECV_DATA_BUFFER] = {0x7e, cType};
	int i = 0;
	int dataCheck = 0;
	int sendDateLen = 0;
	int dataSum = 0;

	strMsg[2] = len>0?(unsigned char)(len >> 24):0;
	strMsg[3] = len>0?(unsigned char)(len >> 16):0;
	strMsg[4] = len>0?(unsigned char)(len >> 8):0;
	strMsg[5] = len>0?(unsigned char)len:0;

	if(len > 0&&data)
	{
		memcpy(&(strMsg[6]),data, len);
	}

	for(i =0;i<len;i++)
	{
		dataSum +=(int)data[i];
	}

	dataCheck = (int)(0x7e + cType+strMsg[2]+strMsg[3]+strMsg[4]+strMsg[5])+dataSum;
	strMsg[6+len] = dataCheck>0?(unsigned char)((dataCheck&0x0000FF00)>> 8):0;
	strMsg[7+len] = dataCheck>0?(unsigned char)(dataCheck&0x000000FF):0;
	sendDateLen = 8+len;
	if(sendDateLen>1024)
	{
		int lenTmp = 0;
		while(1)
		{
			if(sendDateLen>1024)
			{
				if (write(fp, strMsg+lenTmp, 1024) < 0)
				{
					printf("write data error.\n");
					pthread_mutex_unlock(&writeSerialPortMutex);
					return -1;
				}
				sendDateLen -= 1024;
				lenTmp += 1024;
			}
			else
			{
				if (write(fp,strMsg+lenTmp,sendDateLen) < 0)
				{
					printf("write data error.\n");
					pthread_mutex_unlock(&writeSerialPortMutex);
					return -1;
				}
				break;
			}
		}
	}

	else
	{
		if (write(fp, strMsg, sendDateLen) < 0)
		{
			printf("write data error.\n");
			pthread_mutex_unlock(&writeSerialPortMutex);
			return -1;
		}
	}
	/*
	if (write(fp,strMsg, sendDateLen) < 0)
	{
		printf("write data error.\n");
		return -1;
	}*/

	pthread_mutex_unlock(&writeSerialPortMutex);
	return 0;
}


int irdaCode_test_function(char *codeStr,int len)
{
	int i;
	int ret;
	unsigned char codeBuf[RECV_DATA_BUFFER] = {0};
	int codeLen = 0;

	if (ioctl(irdaFd, 1, NULL) < 0)
	{
		printf("Call cmd IOCPRINT fail\n");
		return -1;
	}
	ret = write(irdaFd, codeStr, len);
	return 0;
}

void  proa_analyse_recv_data(char * data,int nLen)
{
	int ret;
	unsigned char pbyRevData[nLen];
	memset(pbyRevData,0,nLen);
	memcpy(pbyRevData,data,nLen-2);
	unsigned char *pbyData = pbyRevData;
	int iLen = 0,iTemp=0;
	unsigned char replyFlag[1] = {0};

	iTemp = (int)pbyData[2];
	iLen = iTemp << 24;
	iTemp = (int)pbyData[3];
	iLen = iLen + (iTemp << 16);
	iTemp = (int)pbyData[4];
	iLen = iLen + (iTemp << 8);
	iTemp = (int)pbyData[5];
	iLen = iLen + iTemp;

	if(pbyData[1] == 0x01)
	{
		if(irdaFd < 0)
		{
			irdaFd = open("/dev/irda_Jikong00", O_RDWR);
			if(irdaFd<0)
			{
				replyFlag[0] = 0x01;
				send_reply_msg(0X01,1,replyFlag );
				perror("Cannot Open irda!\n");
				return;
			}
			nStopThread = 0;
			replyFlag[0] = 0x00;
			send_reply_msg(0X01,1,replyFlag);
		}
		else
		{
			replyFlag[0] = 0x00;
			send_reply_msg(0X01,1,replyFlag);
		}
		return;
	}

	else if(pbyData[1] == 0x02)
	{
		sem_post(&semLearnInfrared);
		return;
	}

	else if (pbyData[1] == 0x03)
	{
		close(irdaFd);
		irdaFd = -1;
		nStopThread = 1;
		replyFlag[0] = 0x00;
		send_reply_msg(0X03,1,replyFlag);
		return;
	}

	else if (pbyData[1] == 0x04)
	{
		if(irdaFd<0)
		{
			irdaFd = open("/dev/irda_Jikong00", O_RDWR);
			if(irdaFd<0)
			{
				perror("Cannot Open irda!\n");
				return;
			}
		}
		ret = irdaCode_test_function(&pbyData[6],nLen-8);
		if(ret != 0)
		{
			replyFlag[0] = 0x01;
			send_reply_msg(0X04,1,replyFlag);
			return;
		}
		replyFlag[0] = 0x00;
		send_reply_msg(0X04,1,replyFlag);
		return;
	}
	pbyData = NULL;
}



void* readInfraredThreadFunc(void *param)
{
	int i;
	int codeLen = 0;
	int codeStrLen = 0;
	unsigned char codeBuf[RECV_DATA_BUFFER] = {0};
	unsigned char replyFlag[1] = {0};
	char *list1 = "finish study.";
	char *list2 = "timing out study.";
	while(!nStopThread)
	{
		sem_wait(&semLearnInfrared);
		codeLen = read(irdaFd, codeBuf,RECV_DATA_BUFFER);
		if (codeLen > 0)
		{
			send_reply_msg(0X02,codeLen,codeBuf);
			continue;
		}
		else
		{
			replyFlag[0] = 0x01;
			send_reply_msg(0X02,1,replyFlag);
			continue;
		}
	}
	return ;
}

void* readSerialPortThreadFunc(void *param)
{
	int err;
	int ret;
	int i=0;
	int len = 0;
	int readLen = 0;
	int tempLen = 0;
	int dateSum = 0;

	int datalenTemp;
	int proaDatelen;
	int pthreadFlag = 0;
	int sumProaDatelen = 0;

	char dataTmp[1] = {0};
	unsigned char data[RECV_DATA_BUFFER] = {0};
	unsigned char temp[2];
	int readLenFlag = 0;
	char buf[10];
	//write(fp,"ABC",3);
	while(1)
	{
		memset(buf,0,10);
		ret = read(fp,dataTmp,1);
		if(ret <= 0)
			continue;

		data[readLen] = dataTmp[0];
		readLen++;

		if(readLen == 6)
		{
			readLenFlag = 1;
			datalenTemp = (int)data[2];
			proaDatelen = datalenTemp << 24;
			datalenTemp = (int)data[3];
			proaDatelen = proaDatelen + (datalenTemp << 16);
		   	datalenTemp = (int)data[4];
			proaDatelen = proaDatelen + (datalenTemp<< 8);
			datalenTemp = (int)data[5];
			proaDatelen = proaDatelen + datalenTemp;
			sumProaDatelen = proaDatelen + 8;
		}

		else if(readLen > 6 && readLen == sumProaDatelen)	//for a complete package
		{
			for(i = 0;i<(sumProaDatelen - 2);i++)
			{
				dateSum +=(int)data[i];

			}
			temp[0] = dateSum>0?(unsigned char)dateSum:0;
			temp[1] = dateSum>0?(unsigned char)(dateSum>> 8):0;
			if((temp[0] !=data[sumProaDatelen-1]) || (temp[1] !=data[sumProaDatelen-2]))
			{
				printf("error package.\n");
				memset(data,0,sizeof(data));
				sumProaDatelen = 0;
				readLen = 0;
				dateSum = 0;
		   		continue;
			}
			proa_analyse_recv_data(data,sumProaDatelen);
			memset(data,0,sizeof(data));
			sumProaDatelen = 0;
		    readLen = 0;
		    dateSum = 0;
		}
	}
	return;
}

int  tty_set_port (int  fd , int nSpeed , int  nBits , char nEvent , int  nStop )
{
    struct  termios new_ios,old_ios;

    if ( tcgetattr ( fd , &new_ios ) !=0 )
        perror("Save the terminal error");

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
            perror("Wrong  nBits");
            break ;
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
            perror("Wrong  nSpeed");
            break;
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
            perror("Wrong nEvent");
            break ;
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
    new_ios.c_cc[VTIME] = 0 ;
    new_ios.c_cc[VMIN] = 0 ;

    /*raw model*/
    new_ios.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_ios.c_oflag  &= ~OPOST;

    new_ios.c_iflag &= ~(INLCR|IGNCR|ICRNL);
    new_ios.c_iflag &= ~(ONLCR|OCRNL);

    new_ios.c_oflag &= ~(INLCR|IGNCR|ICRNL);
    new_ios.c_oflag &= ~(ONLCR|OCRNL);


    tcflush(fd,TCIOFLUSH) ;
    if (tcsetattr(fd,TCSANOW,&new_ios) != 0 )
    {
        perror("Set the terminal error");
        tcsetattr(fd,TCSANOW,&old_ios);
        return -1 ;
    }

    return  0;
}

int main(int argc,char **argv)
{
	int ret;

	struct termios options;
	fp = open("/dev/tty1",O_RDONLY); // 改变console
	ioctl(fp,TIOCCONS);
	close(fp);

	fp = open("/dev/s3c2410_serial0",O_RDWR|O_NOCTTY|O_NDELAY); //打开串口0读写
	if(fp == -1) exit(0);
	tty_set_port(fp,115200,8,'n',1);

	sem_init(&semLearnInfrared,0,0);

	if(pthread_create(&readSerialPortThreadID,NULL,&readSerialPortThreadFunc,NULL) != 0)
	{
		close(fp);                       //关闭串口0
		fp = open("/dev/s3c2410_serial0",O_RDONLY);  //恢复console 到串口0
		ioctl(fp,TIOCCONS);
		close(fp);
		return -1;
	}

	if(pthread_create(&readInfraredThreadID,NULL,&readInfraredThreadFunc,NULL) != 0)
	{
		close(fp);                       //关闭串口0
		fp = open("/dev/s3c2410_serial0",O_RDONLY);  //恢复console 到串口0
		ioctl(fp,TIOCCONS);
		close(fp);
		return -1;
	}

	while(!nExit)
	{
		sleep(1);
	}

	//stop threads
	close(fp);                       //关闭串口0
	pthread_join(readSerialPortThreadID,NULL);
	close(irdaFd);
	pthread_join(readInfraredThreadID,NULL);
	sem_destroy(&semLearnInfrared);
	return 0;
}



