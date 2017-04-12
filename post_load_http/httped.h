#ifndef _HTTPED_H
#define _HTTPED_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#define ISspace(x) isspace((int)(x))
#define STDIN  0
#define STDOUT 1
#define STDERR 2

typedef struct condition
{
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
} condition_t;

typedef struct task
{
    void* (*run)(void *arg);
    void *arg;
    struct task *next;

} task_t;

typedef struct threadpool
{
    condition_t ready;
    task_t *first;
    task_t *last;
    int counter;    //当前线程数
    int idle;       //空闲线程数
    int max_threads;//最大线程数
    int quit;       //线程即将销毁标志
} threadpool_t;

int get_line(int,char *,int);
void accept_request(void *);   
void execute_cgi(int,const char *,const char *,const char *); 
void serve_file(int,const char *); 
void headers(int,const char *); 
void cat(int,FILE *); 
int startup(u_short *); 

int condition_init(condition_t *cond);
int condition_lock(condition_t *cond);
int condition_unlock(condition_t *cond);
int condition_wait(condition_t *cond);
int condition_timedwait(condition_t *cond,const struct timespec *abstime);
int condition_signal(condition_t *cond);
int condition_broadcast(condition_t *cond);
int condition_destroy(condition_t *cond);
void *thread_routine(void* arg);
void threadpool_init(threadpool_t *pool,int threads);
void threadpool_add_task(threadpool_t *pool,void* (*run)(void *arg),void *arg);
void threadpool_destroy(threadpool_t *pool);

#endif
