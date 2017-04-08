#include <stdlib.h>
#include <pthread.h>
#include "util_queue.h"

typedef struct queue_node *QUEUE_NODE;
struct queue_node
{
	void       *data;
	QUEUE_NODE  next;
};

struct util_queue
{
	QUEUE_NODE   head;
	QUEUE_NODE   tail;

	pthread_mutex_t queue_mutex;
};
typedef struct util_queue UTIL_QUEUE;

static UTIL_QUEUE   g_util_queue = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER};

static QUEUE_NODE make_queue_node()
{
	QUEUE_NODE lp_node = (QUEUE_NODE)malloc(sizeof(struct queue_node));
	if(!lp_node)
		return NULL;

	lp_node->data = 0;
	lp_node->next = NULL;
	return lp_node;
}

static void free_queue_node(QUEUE_NODE lp_node)
{
	free(lp_node);
}

static void insert_queue_node(UTIL_QUEUE *lp_util_queue, QUEUE_NODE lp_node)
{
	lp_node->next = NULL;
	if(!lp_util_queue->head)
	{
		lp_util_queue->head = lp_node;
		lp_util_queue->tail = lp_node;
	}
	else
	{
		lp_util_queue->tail->next = lp_node;
		lp_util_queue->tail       = lp_node;
	}
}
static void internal_util_queue_clear(UTIL_QUEUE *lp_util_queue)
{
	if(lp_util_queue)
	{
		QUEUE_NODE tempnode;
		while(lp_util_queue->head)
		{
			tempnode = lp_util_queue->head;
			lp_util_queue->head = lp_util_queue->head->next;

			free_queue_node(tempnode);
		}
		lp_util_queue->head = 0;
		lp_util_queue->tail = 0;
	}
}

static int internal_util_queue_append(UTIL_QUEUE *lp_util_queue, void *data)
{
	QUEUE_NODE idle_node;
	if(!lp_util_queue || !data)
		return 0;

	idle_node = make_queue_node();
	if(!idle_node)
		return 0;

	idle_node->data = data;
	insert_queue_node(lp_util_queue, idle_node);
	return 1;
}

static int internal_util_queue_get_head(UTIL_QUEUE *lp_util_queue, void **data)
{
	QUEUE_NODE lp_node;
	if(!lp_util_queue || !lp_util_queue->head)
		return 0;

	lp_node = lp_util_queue->head;
	*data = lp_node->data;

	lp_util_queue->head = lp_util_queue->head->next;
	free_queue_node(lp_node);
	return 1;
}

#if 0
static void internal_util_queue_init(UTIL_QUEUE *lp_util_queue)
{
	if(!lp_util_queue)
		return;

	lp_util_queue->head  = 0;
	lp_util_queue->tail  = 0;
	pthread_mutex_init(&lp_util_queue->queue_mutex, NULL);
}
#endif
int util_queue_append(void *data)
{
	pthread_mutex_lock(&g_util_queue.queue_mutex);
	int result = internal_util_queue_append(&g_util_queue, data);
	pthread_mutex_unlock(&g_util_queue.queue_mutex);
	return result;
}

int util_queue_get_head(void **data)
{
	pthread_mutex_lock(&g_util_queue.queue_mutex);
	int result = internal_util_queue_get_head(&g_util_queue, data);
	pthread_mutex_unlock(&g_util_queue.queue_mutex);
	return result;
}

void util_queue_clear()
{
	pthread_mutex_lock(&g_util_queue.queue_mutex);
	internal_util_queue_clear(&g_util_queue);
	pthread_mutex_unlock(&g_util_queue.queue_mutex);

	pthread_mutex_destroy(&g_util_queue.queue_mutex);
}
void util_queue_init()
{
	//internal_util_queue_init(&g_util_queue);
}
