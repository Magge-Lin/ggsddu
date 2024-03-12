#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREAD		80

typedef struct job job_t;

typedef struct workqueue workqueue_t;

static void *worker_function(void *ptr);

int workqueue_init(workqueue_t *workqueue, int numWorkers);

void workqueue_shutdown(workqueue_t *workqueue);

void workqueue_add_job(workqueue_t *workqueue, job_t *job);

void threadpool_init(void);
