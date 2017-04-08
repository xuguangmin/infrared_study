/*
 * task_queue.c
 *
 *  Created on: 2013年8月30日
 *      Author: flx
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util_queue.h"
#include "task_queue.h"

void task_queue_free(TASK_DATA task_data)
{
	if(task_data) free(task_data);
}
TASK_DATA task_queue_get_head()
{
	void *head = NULL;
	if(!util_queue_get_head(&head))
		return NULL;

	return (TASK_DATA)head;
}

int task_queue_append(int task, uint8_t *data, int data_len)
{
	TASK_DATA lp_task = (TASK_DATA)malloc(sizeof(struct task_data));
	if(!lp_task)
		return 0;

	lp_task->task = task;
	lp_task->data = NULL;
	lp_task->data_len = 0;

	if(data && data_len > 0)
	{
		lp_task->data = (uint8_t *)malloc(sizeof(uint8_t) * data_len);
		if(!lp_task->data)
			return 0;

		memcpy(lp_task->data, data, data_len);
		lp_task->data_len  = data_len;
	}

	return util_queue_append((void *)lp_task);
}

void task_queue_clear()
{
	TASK_DATA head;
	while((head = task_queue_get_head()))
	{
		task_queue_free(head);
	}
}
