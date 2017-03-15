#include "timer.h"

#define DBG_LEVEL 0
#include <debug.h>

timer_queue::timer_queue()
{
    timerQueue_ = NULL;

}

struct timer *timer_queue::searchTimer(unsigned char code, TimerHandler *handler, void *data)
{
    struct timer *t;

    DBG_IN("timer_queue::searchTimer");

    for(t = timerQueue_; t ; t=t->next) {
	if(t->code == code && t->handler == handler && t->data == data) {
	    break;
	}
    }

    DBG_OUT("timer_queue::searchTimer");
    return t;
}

struct timer *timer_queue::searchTimer(unsigned char code, TimerHandler *handler)
{
    struct timer *t;

    DBG_IN("timer_queue::searchTimer");

    for(t = timerQueue_; t ; t=t->next) {
	if(t->code == code && t->handler == handler && t->data == NULL ) {
	    break;
	}
    }

    DBG_OUT("timer_queue::searchTimer");
    return t;
}

void timer_queue::removeTimer(struct timer *t)
{
    struct timer *curr, *prev = NULL;
    bool earliest_ = false;
  
    DBG_IN("timer_queue::removeTimer");

    t->used = false;
  
    for(curr = timerQueue_; curr; curr=curr->next) {
	if(curr == t) {
	    if(prev == NULL) {
		timerQueue_ = t->next;
		earliest_ = true;
	    } else {
		prev->next = t->next;
	    }
	    t->next = NULL;
	}
	prev = curr;
    }
  
    if(earliest_) {
	updateTimer();
    }

    if(DBG_LEVEL > 2) {
	printTQ();
    }

    DBG_OUT("timer_queue::removeTimer");
}

void timer_queue::getCurrentTime(struct timeval *tv)
{
    struct tm now_;
    
    DBG_IN("timer_queue::getCurrentTime");

    gettimeofday(tv, NULL);

    if(DBG_LEVEL > 1) {
	localtime_r((const time_t *)&(tv->tv_sec), &now_);
	DBG_INFO("Current time is %02d:%02d.%02d",now_.tm_hour, now_.tm_min, now_.tm_sec);
    }

    DBG_OUT("timer_queue::getCurrentTime");
}

void timer_queue::printTQ(void)
{
    struct timer *curr;
    struct timeval now;

    DBG_IN("timer_queue::printTQ");
  
    getCurrentTime(&now);

    for(curr = timerQueue_; curr; curr=curr->next) {
	DBG_INFO("Current = %d.%6.6d, diff = %d",(int) curr->timeout.tv_sec, (int) curr->timeout.tv_usec, (int) curr->timeout.tv_sec-(int) now.tv_sec);	//  = %d",curr, 
    }

    DBG_OUT("timer_queue::printTQ");
}

int timer_queue::addTimer(struct timer *t)
{
    struct timer *prev, *curr;
    bool earliest_ = false;

    DBG_IN("timer_queue::addTimer");

    if(t->used == true) {
	removeTimer(t);
    }

    getCurrentTime(&t->timeout);
    t->timeout.tv_usec += (t->msec*1000);
    t->timeout.tv_sec += (t->timeout.tv_usec / 1000000);
    t->timeout.tv_usec = t->timeout.tv_usec % 1000000;

    if(!timerQueue_) {
	timerQueue_ = t;
	t->next = NULL;
	earliest_ = true;
	DBG_INFO("Added timer at the beginning, the queue was empty");
    } else {
	for(curr= timerQueue_; curr; curr=curr->next) {
	    if(timeval_diff(&(t->timeout), &(curr->timeout)) < 0) {
		break;
	    }
	    prev = curr;
	}
    
	if(curr==timerQueue_) {
	    t->next = timerQueue_;
	    timerQueue_ = t;
	    earliest_ = true;
	} else {
	    t->next = curr;
	    prev->next = t;
	}
    }

    t->used = true;

    if(earliest_) {
	updateTimer();
    }

    if(DBG_LEVEL > 2) {
	printTQ();
    }

    DBG_INFO("Added timer to happen in %ld.%03ld",t->msec/1000, t->msec - (t->msec/1000)*1000);
    DBG_OUT("timer_queue::addTimer");
    return 0;
}

void timer_queue::dispatchTimeout(void)
{
    struct timer *aux;
    struct timeval auxTime_;
    struct timerMessage *message;

    DBG_IN("timer_queue::dispatchTimeout");

    message = new struct timerMessage;

    aux = timerQueue_;
    memcpy(&auxTime_,&(aux->timeout),sizeof(struct timeval));

    message->code = timerQueue_->code;
    message->data = timerQueue_->data;

    timerQueue_->handler->onTimeout(message);

    if(timerQueue_->permanent) {
	addTimer(aux);
    } else {
	removeTimer(aux);
    }

    DBG_OUT("timer_queue::dispatchTimeout");
}

void timer_queue::updateTimer(void)
{
    float tmo_;
    struct timeval pending_;

    DBG_IN("timer_queue::updateNStimer");

    pending_ = getTimeout();
  
    if(pending_.tv_usec == -1) {
	DBG_INFO("No more timers pending");
    } else {
	if(pending_.tv_sec == -1) {
	    DBG_INFO("Apparently, there is some timer we have not dealt with..."); // Need to rework this	//INFO2
	    dispatchTimeout();
	} else {
	    tmo_ = pending_.tv_sec + (float) pending_.tv_usec / 1000000;
	    //nsTimerAPI.resched(tmo_);
	}
    }

    DBG_OUT("timer_queue::updateNStimer");
}

struct timeval timer_queue::getTimeout(void) 
{
    struct timeval now;
    struct timeval remaining;
  
    DBG_IN("timer_queue::getTimeout");

    if(timerQueue_) {
	getCurrentTime(&now);

	//DBG_INFO("timerQueue->time is %d.%6d",(int) timerQueue_->timeout.tv_sec,(int) timerQueue_->timeout.tv_usec);
	//DBG_INFO("Now is %d.%6.06d",(int) now.tv_sec,(int) (now.tv_usec % 100000));

	remaining.tv_sec = timerQueue_->timeout.tv_sec - now.tv_sec;
	remaining.tv_usec = timerQueue_->timeout.tv_usec - now.tv_usec;

	if(remaining.tv_usec < 0) {
	    remaining.tv_usec += 1000000;
	    remaining.tv_sec -= 1;
	}
    }
    else {
	remaining.tv_sec = -1;
	remaining.tv_usec = -1;
    }

    DBG_OUT("timer_queue::getTimeout");

    return remaining;

}
