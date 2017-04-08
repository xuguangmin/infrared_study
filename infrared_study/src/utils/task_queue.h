/*
 * task_queue.h
 *
 *  Created on: 2013年8月30日
 *      Author: flx
 */

#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__

#include <stdio.h>
#include <stdint.h>

struct task_data
{
	int         task;
	uint8_t    *data;
	int         data_len;
};
typedef struct task_data *TASK_DATA;


extern int task_queue_append(int task, uint8_t *data, int data_len);
extern TASK_DATA task_queue_get_head();
extern void task_queue_free(TASK_DATA task_data);
extern void task_queue_clear();

#endif /* __TASK_QUEUE_H__ */
