#ifndef WORKQUEUE_H
#define WORKQUEUE_H


#include <stdio.h>

typedef void (*work_t) (void *param);

void workqueue_init(void);

void workqueue_run(work_t work,void *param);

#endif
/*WORKQUEUE_H */
