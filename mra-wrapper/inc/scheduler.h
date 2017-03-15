/**
 * \file scheduler.h
 * \brief It declares the class and corresponding methods that handle a common event queue 
 *        to be used by all entities of the implementation, and it specifies the structures 
 *        that will be required for managing that
 *
 * Project: AN-P2 (Ambient Networks Phase 2)
 * 
 * Subproject: WPC Task 3 - Multi Radio Access Implementation
 * 
 * This file includes the declaration of the scheduler class that has all the methods that are 
 * needed for handling all events by means of a common queue, which can be used by the different 
 * modules; all these functions are defined in the scheduler.c file. It also specifies the 
 * structure used for managing all the information required in the event queue.
 * 
 * \author University of Cantabria: Ramon Aguero (ramon@tlmat.unican.es) and 
 *                                  Johnny Choque (jchoque@tlmat.unican.es)
 * \date 2005-08-01
 */
#ifndef _scheduler_h
#define _scheduler_h

#include <pthread.h>

#include "event_handler.h"
#include "timer.h"


/**
 * Maximum # of events we could register
 */
#define MAX_CALLBACKS 16

/**
 * \brief This structure is used to manage the events
 *
 * Within this structure we gather all the information that is needed to manage the events.
 * It keeps the unique code of the event, the corresponding file descriptor, as well as the 
 * subscriber to the event (both the handler and the arguments)
 */
struct callback {
    unsigned char code;            /**< Unique identifier of the event */
    EventHandler *handler;
    int fd;                        /**< File descriptor in which we are expecting some input */
    void *data;                    /**< Pointer to the arguments that should be passed to the function */
    struct callback *next;         /**< Pointer to the next event */
};

/**
 * \brief This structure is passed to an event costumer
 * 
 * The structure keeps the code of the event that has happened, as well as the information
 * that the "client" is willing to receive
 */
struct eventMessage {
    unsigned char code;           /**< Code of the event */
    void *result;                 /**< Information that the client is expecting */
};


class scheduler {

 public:
    scheduler();
    //~scheduler();

    static scheduler *instance();
  
    void mainLoop();
    void endInstance(void);

    int insertInput(unsigned char code, int fd, EventHandler *handler, void *data);
    int insertEvent(unsigned char code, EventHandler *handler, void *data);
    int insertTimer(unsigned char code, long msec, TimerHandler *handler, bool permanent = true, void *data = NULL);

    int deleteInput(unsigned char code, int fd, EventHandler *handler);
    int deleteEvent(unsigned char code, EventHandler *handler);
    int deleteTimer(unsigned char code, TimerHandler *handler);
    int deleteTimer(unsigned char code, TimerHandler *handler, void *data);
		    
    void dispatchEvent(unsigned char code, void *result);

    pthread_t getMainThreadID(void);

 private:

    static scheduler* instance_;
    struct callback *cbQueue;

    int eventQueue(struct callback *events);
    void delInput(struct callback *event);

    void dispatchInput(struct callback *event);

    pthread_t m_mainThreadID;
    void unblockMainThread(void);

    timer_queue *timerQueue;
};




#endif // _scheduler_h



