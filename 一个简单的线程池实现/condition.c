#include "condition.h"

int condition_init(condition_t *cond)
{
    int ret = 0;
    ret = pthread_mutex_init(&cond->pmutex, NULL);
    if(ret != 0){
        return ret;
    }
    ret = pthread_cond_init(&cond->pcond, NULL);
    if(ret != 0){
        return ret;
    }
    return ret;
}

int condition_lock(condition_t *cond)
{
    return pthread_mutex_lock(&cond->pmutex);
}

int condition_unlock(condition_t *cond)
{
    return pthread_mutex_unlock(&cond->pmutex);
}

int condition_wait(condition_t *cond)
{
    return pthread_cond_wait(&cond->pcond, &cond->pmutex);
}

int condition_timedwait(condition_t *cond, const struct timespec *abstime)
{
    return pthread_cond_timedwait(&cond->pcond, &cond->pmutex, abstime);
}

int condition_signal(condition_t *cond)
{
    return pthread_cond_signal(&cond->pcond);
}

int condition_broadcast(condition_t *cond)
{
    return pthread_cond_broadcast(&cond->pcond);
}

int condition_destory(condition_t *cond)
{
    int ret;
	if((ret = pthread_mutex_destroy(&cond->pmutex)))
		return ret;
		
	if((ret = pthread_cond_destroy(&cond->pcond)))
		return ret;
		
	return ret;
}