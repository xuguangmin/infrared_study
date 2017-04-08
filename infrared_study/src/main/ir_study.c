/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 ******************************************************************************
  文件名称 : ir_study.c
  作者    : 贾延刚
  生成日期 : 2014-01-09

  版本    : 1.0
  功能描述 : 学习功能的实现

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <syslog.h>
#include <errno.h>

#include "serial.h"
#include "infrared.h"
#include "protocol.h"
#include "task_queue.h"
#include "log.h"

#define DATA_BUFFER_MAX               8192
#define RECV_DATA_BUFFER              2048

#define INFRARED_TASK_READ            0
#define INFRARED_TASK_WRITE           1
#define SERIAL_TASK_WRITE             2

static sem_t    g_task_sem_t;
static uint8_t  ir_read_buf[DATA_BUFFER_MAX];           /* 红外的读缓存 */



static int send_reply_data(unsigned char cmd, unsigned char *data, int data_len)
{
	unsigned char buffer[DATA_BUFFER_MAX];
	int len = create_protocol_packet(cmd, data, data_len, buffer, DATA_BUFFER_MAX);
	if(len <= 0)
		return 0;

	return serial_write(buffer, len);
}

/*
 * b_success 取值： PCS_EX_ERR or PCS_EX_OK
 */
static int send_reply_msg(unsigned char cmd, uint8_t b_success)
{
	uint8_t replyFlag[1];
	uint8_t buffer[128];

	replyFlag[0] = b_success;
	int len = create_protocol_packet(cmd, replyFlag, 1, buffer, 128);
	if(len <= 0)
		return 0;

	return serial_write(buffer, len);
}

static void infrared_task_read()
{
	int len = infrared_read(ir_read_buf, DATA_BUFFER_MAX);
	if (len > 0)
	{
		send_reply_data(PCS_START_STUDY, ir_read_buf, len);
	}
	else
	{
		send_reply_msg(PCS_START_STUDY, PCS_EX_ERR);
	}
}
static void infrared_task_write(uint8_t *data, int data_len)
{
	uint8_t replyFlag = infrared_write(data, data_len) ? PCS_EX_OK:PCS_EX_ERR;

	send_reply_msg(PCS_TEST_IRDA_DATA, replyFlag);
}

static void process_task(int taskId, uint8_t *data, int data_len)
{
	switch(taskId)
	{
	case INFRARED_TASK_READ:
		infrared_task_read();
		break;
	case INFRARED_TASK_WRITE:
		infrared_task_write(data, data_len);
		break;

	case SERIAL_TASK_WRITE:
		serial_write(data, data_len);
		break;
	}
}
static void* thread_func_task(void *param)
{
	TASK_DATA task;

	printf("%s started\n", __FUNCTION__);
	while(1)
	{
		sem_wait(&g_task_sem_t);
		task = task_queue_get_head();
		if(task)
		{
			process_task(task->task, task->data, task->data_len);
			task_queue_free(task);
		}
	}
	return NULL;
}
/* 向红外口输出的任务 */
static int process_protocol_pdu_test(PDU *pdu)
{
	task_queue_append(INFRARED_TASK_WRITE, pdu->data, pdu->data_len);
	sem_post(&g_task_sem_t);
	return 1;
}

/* 向串口输出的任务 */
static int send_to_task_queue(unsigned char cmd, uint8_t b_success)
{
	uint8_t replyFlag[1], buffer[128];
	if(b_success != PCS_EX_OK && b_success != PCS_EX_ERR)
		return 0;

	replyFlag[0] = b_success;
	int len = create_protocol_packet(cmd, replyFlag, 1, buffer, 128);
	if(len <= 0)
		return 0;

	if(!task_queue_append(SERIAL_TASK_WRITE, buffer, len))
		return 0;

	sem_post(&g_task_sem_t);
	return 1;
}

static int process_protocol_pdu_end_study(PDU *pdu)
{
	return send_to_task_queue(PCS_END_STUDY, PCS_EX_OK);
}
/* 读红外口的任务 */
static int process_protocol_pdu_start_study(PDU *pdu)
{
	task_queue_append(INFRARED_TASK_READ, NULL, 0);
	sem_post(&g_task_sem_t);
	return 1;
}

static int process_protocol_pdu_initialize(PDU *pdu)
{
	return send_to_task_queue(PCS_IRDA_INITIALIZE, PCS_EX_OK);
}
static int process_protocol_pdu(PDU *pdu)
{
	switch(pdu->cmd)
	{
	case PCS_IRDA_INITIALIZE:return process_protocol_pdu_initialize(pdu);
	case PCS_START_STUDY    :return process_protocol_pdu_start_study(pdu);
	case PCS_END_STUDY      :return process_protocol_pdu_end_study(pdu);
	case PCS_TEST_IRDA_DATA :return process_protocol_pdu_test(pdu);
	}
	return 0;
}

static void loop_read_from_serial()
{
	PDU g_pdu;
	int consume;
	int len_recv, len_total = 0;
	uint8_t main_buf[DATA_BUFFER_MAX];
	uint8_t recv_buf[RECV_DATA_BUFFER];

	while(1)
	{
		len_recv = serial_read(recv_buf, RECV_DATA_BUFFER);
		if (len_recv > 0)
		{
			if((len_total + len_recv) >= DATA_BUFFER_MAX)
			{
				to_log("serial recv buffer is filled\n");
				continue;
			}

			memcpy(&main_buf[len_total], recv_buf, len_recv);
			len_total += len_recv;

			while(len_total > 0)
			{
				g_pdu.b_enable = 0;
				consume = parse_packet(main_buf, len_total, &g_pdu);
				if(consume <= 0)
					break;

				if(g_pdu.b_enable) process_protocol_pdu(&g_pdu);
				memmove(main_buf, main_buf+consume, consume);
				len_total -= consume;
			}
		}
	}
}

void ir_study_close()
{
	infrared_close();
	sem_destroy(&g_task_sem_t);
	restore_console();

}
void ir_study_start()
{
	loop_read_from_serial();
}

int ir_study_init()
{
	pthread_t thread_id;

	if(!infrared_open())
		return 0;

	if(sem_init(&g_task_sem_t, 0, 0) != 0)
	{
		printf("sem_init error. %s\n", strerror(errno));
		return 0;
	}

	if(pthread_create(&thread_id, NULL, thread_func_task, NULL) != 0)
	{
		printf("thread_func_task not start\n");
		return 0;
	}
	pthread_detach(thread_id);

	if(!serial_open())
	{
		to_log("serial open failed");
		return 0;
	}
	return 1;
}
