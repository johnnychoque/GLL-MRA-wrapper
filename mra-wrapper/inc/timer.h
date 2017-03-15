/**
 * \file timer.h
 * \brief It declares the functions that handle a common timer queue to be used by
 *        all entities of the implementation, and it specifies the structures that will be
 *        required for managing that
 *
 * Project: AN-P2 (Ambient Networks Phase 2)
 * 
 * Subproject: WPC Task 3 - Multi Radio Access Implementation
 * 
 * This file includes the declaration of the timer_queue class, with all the methods that are needed 
 * for handling all timers by means of a common queue, which can be used by the different modules; 
 * all these methods are defined in the timer.cc file. It also specifies the structure used for managing 
 * all the information required in the timer queue.
 * 
 * \author University of Cantabria: Ramon Aguero (ramon@tlmat.unican.es) and 
 *                                  Johnny Choque (jchoque@tlmat.unican.es)
 * \date 2005-08-01
 */

#ifndef _timer_h
#define _timer_h

#include <sys/time.h>

enum {
    TIMER_IDLE,
    TIMER_PENDING,
    TIMER_HANDLING,
};

struct timerMessage {
    unsigned char code;
    void *data;
};

class TimerHandler {
 public:
    virtual void onTimeout(struct timerMessage *message) = 0;
    virtual ~TimerHandler(void) {}
};


/**
 * \brief Structure used to manage the timers
 *
 * This structure gathers all the information that is needed for handling the timer queue
 * information, including the expiring time and its subscriber (function to be called upon timer
 * expiration along with the corresponding parameters)
 */
struct timer {
    long int msec;
    struct timeval timeout;           /**< Expiration time */
    TimerHandler *handler;            /**< Function that is to be called upon timer expiration */
    unsigned char code;
    void *data;                       /**< Pointer to the data -> client function argument */
    int used;                         /**< Flag that indicates whether the timer is used or not*/
    bool permanent;
    struct timer *next;               /**< Pointer to the next timer in the timer queue*/
};


class scheduler;

class timer_queue {
    friend class scheduler;
 public:
    timer_queue();
    //~timer_queue();
  
 private:
    struct timer *timerQueue_;
  
    void getCurrentTime(struct timeval *tv);
    void printTQ(void);

    static inline long timeval_diff(struct timeval *t1, struct timeval *t2) {return (1000000 * (t1->tv_sec - t2->tv_sec) + (t1->tv_usec - t2->tv_usec));}

    int addTimer(struct timer *t);
    struct timer *searchTimer(unsigned char code, TimerHandler *handler);
    struct timer *searchTimer(unsigned char code, TimerHandler *handler, void *data);
    void removeTimer(struct timer *t);

    void dispatchTimeout(void);

    void updateTimer(void);
    struct timeval getTimeout(void);

};

#endif // _timer_h
