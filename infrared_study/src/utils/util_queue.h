#ifndef __UTIL_QUEUE_H__
#define __UTIL_QUEUE_H__


extern int util_queue_append(void *data);
extern int util_queue_get_head(void **data);
extern void util_queue_clear();

#endif  /* __UTIL_QUEUE_H__ */
