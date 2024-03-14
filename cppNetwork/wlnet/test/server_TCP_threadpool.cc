#include "wlnet.h"

#define MAX_EPOLLSIZE	100000
#define MAX_THREAD		10


#define LL_ADD(item, list) \
{ \
	item->prev = NULL; \
	item->next = list; \
	list = item; \
}

#define LL_REMOVE(item, list) \
{ \
	if (item->prev != NULL) item->prev->next = item->next; \
	if (item->next != NULL) item->next->prev = item->prev; \
	if (list == item) list = item->next; \
	item->prev = item->next = NULL; \
}
typedef struct worker_s 
{	
	pthread_t thread;	
	int terminate;	
	struct workqueue *workqueue;	
	struct worker_s *prev;	
	struct worker_s *next;
} worker_t;

typedef struct job 
{	
	void (*job_function)(struct job *job);	
	void *user_data;	
	struct job *prev;	
	struct job *next;
}job_t;

typedef struct workqueue 
{	
	struct worker_s *workers;	
	struct job *waiting_jobs;	
	pthread_mutex_t jobs_mutex;	
	pthread_cond_t jobs_cond;
}workqueue_t;

static void *worker_function(void *ptr) 
{	
	worker_t *worker = (worker_t *)ptr;	
	job_t *job;	

	while (1) 
    {			
		pthread_mutex_lock(&worker->workqueue->jobs_mutex);		
		while (worker->workqueue->waiting_jobs == NULL) 
        {			
			if (worker->terminate) break;			
			pthread_cond_wait(&worker->workqueue->jobs_cond, &worker->workqueue->jobs_mutex);		
		}			
		if (worker->terminate) break;		
		job = worker->workqueue->waiting_jobs;		
		if (job != NULL) 
        {			
			LL_REMOVE(job, worker->workqueue->waiting_jobs);		
		}		
		pthread_mutex_unlock(&worker->workqueue->jobs_mutex);		
			
		if (job == NULL) continue;	
		
		/* Execute the job. */		
		job->job_function(job);	
	}	
	
	free(worker);	
	pthread_exit(NULL);
}

int workqueue_init(workqueue_t *workqueue, int numWorkers) 
{	
	int i;	
	worker_t *worker;	
	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;	
	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;	

	if (numWorkers < 1) numWorkers = 1;	
	
	memset(workqueue, 0, sizeof(*workqueue));	
	memcpy(&workqueue->jobs_mutex, &blank_mutex, sizeof(workqueue->jobs_mutex));	
	memcpy(&workqueue->jobs_cond, &blank_cond, sizeof(workqueue->jobs_cond));	

	for (i = 0; i < numWorkers; i++) 
    {		
		if ((worker = (worker_t*)malloc(sizeof(worker_t))) == NULL) 
        {			
			perror("Failed to allocate all workers");			
			return 1;		
		}		

		memset(worker, 0, sizeof(*worker));		
		worker->workqueue = workqueue;		
		
		if (pthread_create(&worker->thread, NULL, worker_function, (void *)worker)) 
        {			
			perror("Failed to start all worker threads");			
			free(worker);			
			return 1;		
		}		

		LL_ADD(worker, worker->workqueue->workers);	
	}	

	return 0;
}

void workqueue_shutdown(workqueue_t *workqueue) 
{	

	worker_t *worker = NULL;		
	for (worker = workqueue->workers; worker != NULL; worker = worker->next) 
    {		
		worker->terminate = 1;	
	}	

	pthread_mutex_lock(&workqueue->jobs_mutex);	
	workqueue->workers = NULL;	
	workqueue->waiting_jobs = NULL;	
	pthread_cond_broadcast(&workqueue->jobs_cond);	
	pthread_mutex_unlock(&workqueue->jobs_mutex);

}

void workqueue_add_job(workqueue_t *workqueue, job_t *job) 
{	

	pthread_mutex_lock(&workqueue->jobs_mutex);	

	LL_ADD(job, workqueue->waiting_jobs);	

	pthread_cond_signal(&workqueue->jobs_cond);	
	pthread_mutex_unlock(&workqueue->jobs_mutex);

}

static workqueue_t workqueue;
void threadpool_init(void) 
{
	workqueue_init(&workqueue, MAX_THREAD);
}

typedef struct client {
	int fd;
    wl_reactor_t* reactor;
    int events;
} client_t;

void client_job(job_t *job) 
{
	client_t *rClient = (client_t*)job->user_data;
	int clientfd = rClient->fd;
	int events = rClient->events;
    wl_reactor_t* reactor = rClient->reactor;

    wl_connect_t* conn = wl_connect_idx(reactor, clientfd);

    conn->cb(clientfd, events, reactor);

	free(rClient);
	free(job);
}

int wl_run_reactor(wl_reactor_t* reactor)
{

    struct epoll_event events[MAX_EPOLLSIZE] = {0};

    while (1)
    {
        int nready = epoll_wait(reactor->epfd, events, MAX_EPOLLSIZE, -1);

        for(int i = 0; i < nready; ++i)
        {
            client_t *rClient = (client_t*)malloc(sizeof(client_t));
            memset(rClient, 0, sizeof(client_t));				
            rClient->fd = events[i].data.fd;
            rClient->reactor = reactor;
            rClient->events = events[i].events;
            
            job_t *job = (job_t*)malloc(sizeof(job_t));
            job->job_function = client_job;
            job->user_data = rClient;
            workqueue_add_job(&workqueue, job);
        }
    }
}

typedef struct port_data_s
{
    int port_length;

    int buff[];
}port_data_t;

int main(int argc, char* argv[])
{
    if(argc < 2)
        return -1;
    
    int port = atoi(argv[1]);
    int port_length = atoi(argv[2]);

	threadpool_init(); 

    wl_reactor_t reactor;
    wl_init_reactor(&reactor);

    port_data_t* portData = (port_data_t*)malloc(sizeof(port_data_t) + sizeof(int)*port_length);
    portData->port_length = port_length;
    for (int i = 0; i < port_length; i++)
    {
        int sockfd = init_server(port + i);
        portData->buff[i] = sockfd;
        set_listener(&reactor, sockfd, accept_cb);
    }
    
    wl_run_reactor(&reactor);

    wl_dest_reactor(&reactor);

    for (int i = 0; i < port_length; i++)
    {
        close(portData->buff[i]);
    }
    free(portData);

    return 0;
}