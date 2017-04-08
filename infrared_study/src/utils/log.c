/*
 * log.c
 *
 *  Created on: 2013年8月26日
 *      Author: flx
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"

/*
 FILE *debugFp = NULL;                   //调试句柄
 void writeLog(const char *log)
{
	if(debugFp && log)
	{
		int nDataLen = strlen(log);
		fwrite(log, sizeof(char), nDataLen, debugFp);
	}
}

 需要按照一定的大小进行分割
void to_log(const char *format, ...)
{
#ifdef DEBUG_IN_JIKONG
	va_list args;
	char buffer[8192];

	va_start(args, format);
	vsnprintf(buffer, BUFSIZ, format, args);
	va_end(args);

	debug_serial_log((unsigned char *)buffer, strlen(buffer));
#endif
}

int log_close()
{
	return fclose(debugFp);
}
int log_open()
{
	debugFp = fopen("./log.txt","a+");
	if(debugFp == NULL)
	{
		printf("open log.txt failed\n");
		return 0;
	}
	return 1;
}

*/


/*
 * 参数：
 *
 */
void to_log_byte_array(const unsigned char *buffer, int data_len)
{
	int k;
	char logstr[8192];
	char temp[256];
	//const unsigned char *p = buffer;
	//int out_len = data_len;

	memset(logstr, 0, 8192);
	/*
	while(out_len > 0)
	{
		//to_log("%02X ", *p);
		sprintf(temp, "%02X ", *p);
		strcat(logstr, temp);
		out_len--;
		p++;
	}
	*/


	for(k = 0; k < 5; ++k)
	{
		sprintf(temp, "%02X ", buffer[k]);
		strcat(logstr, temp);
	}

	for(k = (data_len-5); k < data_len; ++k)
	{
		sprintf(temp, "%02X ", buffer[k]);
		strcat(logstr, temp);
	}

	to_log("%s\n", logstr);
}


