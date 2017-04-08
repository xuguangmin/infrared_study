/*
 * /home/flx/developer/workspace_c/infrared_study/build
 */
#include <stdio.h>
#include <pthread.h>
#include "log.h"
#include "led.h"
#include "key_reset.h"
#include "ir_study.h"


static void* thread_func_reset(void *param)
{
	printf("reset key waiting\n");
	while(1)
	{
		if(1 == key_reset_launch())
		{
			led_twinkle();
		}
	}
	return NULL;
}

int main(int argc,char **argv)
{
	pthread_t thread_id;
	to_log("app start ...");

	if(pthread_create(&thread_id, NULL, thread_func_reset, NULL) != 0)
	{
		printf("thread_func_reset not start\n");
		return -1;
	}

	if(!ir_study_init())
		return -1;

	if(!led_green())
		return -1;

	ir_study_start();
	ir_study_close();
	led_red();

	return 0;
}
