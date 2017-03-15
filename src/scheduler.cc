/**
 * \file scheduler.cc
 * \brief It defines the functions that handle a common event queue to be used by
 *        all entities of the implementation
 *
 * Project: AN (Ambient Networks)
 * 
 * Subproject: Activity 1.3.3 - PAN Case Study
 * 
 * This file includes all the functions that are needed for handling all events by means of a
 * common queue, which can be used by the different modules; it gives each event a unique code,
 * and they could be assigned to a file descriptor (i.e. a socket over which we are receiving
 * some data.
 *
 * \author University of Cantabria: Ramon Aguero (ramon@tlmat.unican.es) and 
 *                                  Johnny Choque (jchoque@tlmat.unican.es)
 * \date 2006-08-01
 */
#include <csignal> // signal.h
#include <cstdio> // stdio.h
#include <cstdlib> // malloc
#include <ctime> // time.h

#include <unistd.h> // sleep

#include "scheduler.h"

#define DBG_LEVEL 1
#include "debug.h"


static void sigUser1Handler(int sig)
{
    DBG_IN("sigUser1Handler");

    signal(sig, sigUser1Handler);

    DBG_OUT("sigUser1Handler");
}

scheduler* scheduler::instance_ = 0;

scheduler* scheduler::instance()
{
    DBG_IN("scheduler::instance");

    if(instance_ == 0) {
	instance_ = new scheduler;
    }

    DBG_OUT("scheduler::instance");
    return instance_;
}

void scheduler::endInstance()
{

    DBG_IN("scheduler::endInstance");

    if(instance_ != 0) {
	delete instance_;
    }

    DBG_OUT("scheduler::endInstance");
}


scheduler::scheduler()
{
    DBG_IN("scheduler::scheduler");

    timerQueue = new timer_queue;
    cbQueue = NULL;
    m_mainThreadID = pthread_self();

    DBG_OUT("scheduler::scheduler");
}

/**
 * \brief It inserts an event within the Event Queue
 * 
 * This function inserts an event in the queue of events, unless it was already
 * registered
 *
 * \param code Code of the event that is being registered
 * \param fd File descriptor in which some input is expected to trigger the event
 * \param handler Pointer to the function that has to be called whenever
 *                the event takes place
 * \param data Pointer to the data that will be passed to the handler (as an 
 *             argument to the function
 */
int scheduler::insertInput(unsigned char code, int fd,EventHandler *handler,void *data)
{

    struct callback *newEvent = cbQueue;
  
    DBG_IN("insertInput");

    while(newEvent) {
	if(newEvent->code == code && newEvent->handler == handler && newEvent->fd == fd) {
	    DBG_INFO("Input was already registered");
	    DBG_INFO("The same client cannot be registered to the same event twice");
	    DBG_OUT("insertInput");
	    return(-1);
	}
	newEvent=newEvent ->next;
    }

    newEvent = cbQueue;
  
    if(newEvent) {
	for( ; newEvent->next ; newEvent = newEvent->next) ;
	newEvent->next = (struct callback *) malloc(sizeof(struct callback));
	newEvent = newEvent -> next;
    } else {
	cbQueue = (struct callback *) malloc(sizeof(struct callback));
	newEvent = cbQueue;
    }

    newEvent->code = code;
    newEvent->fd = fd;
    newEvent->handler = handler;
    newEvent->data = data;
    newEvent->next = NULL;
 
    if(fd > 0) {
	unblockMainThread();
    }
   
    DBG_OUT("insertInput");
    return(0);
}

/**
 * \brief It inserts an event on the queue
 *
 * It inserts an event on the queue; a function "subscribes" to the occurence of
 * such event, so whenever it happens, it will be called, with the corresponding 
 * arguments, it uses the insertInput function (with fd = -1)
 *
 * \param code Unique code of the event
 * \param handler Function that will be called when the event happens
 * \param data Pointer to the arguments that will be passed to the function
 *
 */
int scheduler::insertEvent(unsigned char code, EventHandler *handler, void *data)
{
    DBG_IN("scheduler::insertEvent");

    if(insertInput(code, -1, handler, data) == -1) {
	DBG_INFO2("Input was not registered, as it was already in the queue");
	return(-1);
    }

    DBG_OUT("scheduler::insertEvent");
    return 0;
} 


int scheduler::insertTimer(unsigned char code, long msec, TimerHandler *handler, bool permanent, void *data)
{
    struct timer *t;

    DBG_IN("scheduler::addTimer_msec");

    t = new struct timer;
    t->msec = msec;
    t->handler = handler;
    t->code = code;
    t->data = data;
    t->permanent = permanent;

    timerQueue->addTimer(t);

    unblockMainThread();

    DBG_OUT("scheduler::addTimer_msec");

    return 0;
}

int scheduler::deleteInput(unsigned char code, int fd, EventHandler *handler) 
{
    int ret_=-1;
    struct callback *temp;

    DBG_IN("scheduler::deleteInput");

    for(temp = cbQueue ; temp ; temp = temp->next) {
	if(temp->code == code && temp->fd == fd && temp->handler == handler) {
	    delInput(temp);
	    ret_ = 0;
	}
    }

    DBG_OUT("scheduler::deleteInput");
    return ret_;
}


int scheduler::deleteEvent(unsigned char code, EventHandler *handler)
{
    int ret_=-1;
    struct callback *temp;

    DBG_IN("scheduler::deleteEvent");

    for(temp = cbQueue ; temp ; temp = temp->next) {
	if(temp->code == code && temp->fd == -1 && temp->handler == handler) {
	    delInput(temp);
	    ret_ = 0;
	}
    }

    DBG_OUT("scheduler::deleteEvent");
    return ret_;
}


int scheduler::deleteTimer(unsigned char code, TimerHandler *handler, void *data) 
{
    int ret_ = -1;
    struct timer *t;

    DBG_IN("scheduler::deleteTimer");
    
    t=timerQueue->searchTimer(code, handler, data);
    if(t) {
	timerQueue->removeTimer(t);
	ret_ = 0;
    }       

    DBG_OUT("scheduler::deleteTimer");
    return(ret_);
}


int scheduler::deleteTimer(unsigned char code, TimerHandler *handler) 
{
    int ret_ = -1;
    struct timer *t;

    DBG_IN("scheduler::deleteTimer");
    
    t=timerQueue->searchTimer(code, handler);
    if(t) {
	timerQueue->removeTimer(t);
	ret_ = 0;
    }       

    DBG_OUT("scheduler::deleteTimer");
    return(ret_);
}


/**
 * \brief It deletes an input from the event queue
 *
 * It erases an input from the event queue, freeing the corresponding memory
 *
 * \param event Pointer to the callback structure that correponds to the event
 *              we want to delete
 */
void scheduler::delInput(struct callback *event)
{

    struct callback *temp;
  
    DBG_IN("delInput");

    for(temp=cbQueue; temp != event; temp=temp->next) {
	if(temp->next == event) {
	    temp->next = event -> next;
	}
    }
    free(event);
  
    DBG_OUT("delInput");
}


/**
 * \brief It dispatches an event, when it has happened
 * 
 * After an event has happened, this function is used to dispatch is to the
 * corresponding "clients" or "subscribers"
 *
 * \todo We need to define about the need to delete the event when it is dispatched,
 *       i.e. whether it is permanent or not
 */
void scheduler::dispatchEvent(unsigned char code, void *result)
{
    struct callback *temp;
    struct eventMessage *message;
  
    DBG_IN("dispatchEvent");

    message = new struct eventMessage;

    for(temp=cbQueue; temp; temp=temp->next) {
	if(temp->code == code) {
	    message->code = temp->code;
	    message->result = result;
	    temp->handler->onEvent(message);
	}
    }

    DBG_OUT("dispatchEvent");
}

void scheduler::dispatchInput(struct callback *event)
{
    struct eventMessage *message;
  
    DBG_IN("dispatchInput");

    message = new struct eventMessage;
    message->code = event->code;
    message->result = NULL;
    event->handler->onEvent(message);

    DBG_OUT("dispatchInput");
}

/**
 * \brief It provides the main loop, waiting for the different events to happen
 *
 * This should be called from the main program line, and it is based on the select
 * call. It keeps waiting there until some timeout has expired or one of the registered
 * events have taken place. In the first case, it calls the corresponding handler of the 
 * timer (along with the corresponding arguments), while in the other case (an event has
 * happened) the callback function of the event is called (also with the corresponding 
 * arguments.
 */
void
scheduler::mainLoop(void)
{
    int cfds,n;
    fd_set rfds;
    struct callback *event;
    struct timeval timeout, *delaySelect;

    DBG_IN("mainLoop");

    signal(SIGUSR1, sigUser1Handler);
  
    while(1) {
    
	timeout = timerQueue->getTimeout();
	while(timeout.tv_sec < 0 && timeout.tv_usec > 0) {
	    timerQueue->dispatchTimeout();
	    timeout = timerQueue->getTimeout();
	}
	if(timeout.tv_sec == -1 && timeout.tv_usec == -1) {
	    delaySelect = NULL;
	    DBG_INFO("There are not pending timers");
	} else {
	    delaySelect = &timeout;
	    DBG_INFO("Timeout value = %d.%06d",(int) timeout.tv_sec,(int) timeout.tv_usec);
	}
   
	FD_ZERO(&rfds);
	cfds=0;
	for(event = cbQueue; event; event=event->next) {
	    if(event->fd >= 0) {
		//DBG_INFO("There is some pending event (%d)", event->fd);
		FD_SET(event->fd, &rfds);
		if(event->fd >= cfds) {
		    cfds=event->fd + 1;
		}
	    }
	}
    
	if((timeout.tv_sec > 0 || timeout.tv_usec > 0) || cfds > 0) {
	    n = select(cfds, &rfds, NULL, NULL, delaySelect);
	    //DBG_INFO("Out from the select call");
	    if(n==0) {
		timerQueue->dispatchTimeout();
	    }
	    else if(n < 0) {
		continue;
	    }
	    else {
		for(event = cbQueue; event; event=event->next) {
		    if(event->fd > 0 && FD_ISSET(event->fd, &rfds)) {
			DBG_INFO("Out from the select call, with n = %d", n);
			dispatchInput(event);
		    }
		}
	    }
	}
	else {
	    sleep(5);
	}
    }
}

void scheduler::unblockMainThread(void)
{
    DBG_IN("schedule::runblockMainThread");

    if(!pthread_equal(m_mainThreadID, pthread_self())) {
	DBG_INFO("Sending SIGUSR1");
	pthread_kill(m_mainThreadID, SIGUSR1);
    }

    DBG_OUT("scheduler::unblockMainThread");
}


pthread_t scheduler::getMainThreadID(void)
{
    DBG_IN("scheduler::getMainThreadID");
    
    DBG_OUT("scheduler::getMainThreadID");
    return m_mainThreadID;
}
