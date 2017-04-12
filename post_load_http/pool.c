#include "httped.h"

//条件变量初始化
int condition_init(condition_t *cond)
{
    int status;
    if((status=pthread_mutex_init(&cond->pmutex,NULL)))
        return status;
    if((status=pthread_cond_init(&cond->pcond,NULL)))
        return status;
    return 0;
}

//线程锁
int condition_lock(condition_t *cond)
{
    return pthread_mutex_lock(&cond->pmutex);
}

//解锁
int condition_unlock(condition_t *cond)
{
    return pthread_mutex_unlock(&cond->pmutex);
}

//条件变量等待唤醒函数
int condition_wait(condition_t *cond)
{
   return pthread_cond_wait(&cond->pcond,&cond->pmutex);
}

//设定时钟唤醒
int condition_timedwait(condition_t *cond,const struct timespec *abstime)
{
    return pthread_cond_timedwait(&cond->pcond,&cond->pmutex,abstime);
}

//条件变量唤醒信号
int condition_signal(condition_t *cond)
{
    return pthread_cond_signal(&cond->pcond);
}

//唤醒信号广播
int condition_broadcast(condition_t *cond)
{
    return pthread_cond_broadcast(&cond->pcond);
}

//条件变量释放
int condition_destroy(condition_t *cond)
{
    int status;
    if((status=pthread_mutex_destroy(&cond->pmutex)))
        return status;
        if((status=pthread_cond_destroy(&cond->pcond)))
        return status;
    return 0;
}

//检测线程池的状态,并将相应任务交给线程执行
void *thread_routine(void* arg)
{
    printf("thread 0x%x is starting on task\n",(int)pthread_self(),*(int *)arg);
    threadpool_t *pool=(threadpool_t *)arg;
//	pool->idle++;
    while(1)
    {
        struct timespec abstime;
        int timeout=0;
        condition_lock(&pool->ready);
//		if(pool->first == NULL&&!pool->quit)
//			pool->idle++;
		//等待队列有任务到达
		while(pool->first == NULL && !pool->quit)
        {
//            printf("thread 0x%x is waiting\n",(int)pthread_self());
            clock_gettime(CLOCK_REALTIME,&abstime);
            abstime.tv_sec+=2;
            int status=condition_timedwait(&pool->ready,&abstime);
            if(status == ETIMEDOUT)
            {
                printf("thread 0x%x is wait timeout\n",(int)pthread_self());
                timeout=1;
                break;
            }
        }
		//线程开始工作
//		pool->idle--;
		//执行任务
		if(pool->first!=NULL)
        {
            task_t *t=pool->first;
            pool->first=t->next;
            condition_unlock(&pool->ready);
            t->run(t->arg);
            free(t);
            condition_lock(&pool->ready);
        }
		//线程池为空且销毁指令到达，唤醒执行销毁线程池
		if(pool->quit && pool->first==NULL)
        {
            pool->counter--;
            if(pool->counter==0)
                condition_signal(&pool->ready);
            condition_unlock(&pool->ready);
            break;
        }
		//执行销毁线程
		if(timeout && pool->first==NULL)
        {
            pool->counter--;
            condition_unlock(&pool->ready);
            break;
        }
        condition_unlock(&pool->ready);
    }
    printf("thread 0x%x is exitting\n",(int)pthread_self());
}

//线程池初始化
void threadpool_init(threadpool_t *pool,int threads)
{
    condition_init(&pool->ready);
    pool->first=NULL;
    pool->last=NULL;
    pool->counter=0;
//    pool->idle=0;
    pool->max_threads=threads;
    pool->quit=0;
}

//注册任务到线程池
void threadpool_add_task(threadpool_t *pool,void* (*run)(void *arg),void *arg)
{
    condition_lock(&pool->ready);
    task_t *newtask=(task_t *)malloc(sizeof(task_t));
    newtask->run=run;
    newtask->arg=arg;
    newtask->next=NULL;

    if(pool->first==NULL)
        pool->first=newtask;
    else
        pool->last->next=newtask;
    pool->last=newtask;

//    if(pool->idle>0)
//	{
//		printf("ready %d",pool->idle);
//        condition_signal(&pool->ready);
//	}
	condition_signal(&pool->ready);
	if(pool->counter < pool->max_threads)
    {
        pthread_t tid;
//		pool->idle++;
        pthread_create(&tid,NULL,thread_routine,pool);
        pool->counter++;
    }
    condition_unlock(&pool->ready);
}

//线程池销毁
void threadpool_destroy(threadpool_t *pool)
{
    if(pool->quit)
        return;
    condition_lock(&pool->ready);
    pool->quit=1;
    if(pool->counter>0)
    {
//        if(pool->idle > 0)
//            condition_broadcast(&pool->ready);
        while(pool->counter>0)
        {
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);
    condition_destroy(&pool->ready);
}

