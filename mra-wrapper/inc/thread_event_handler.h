// Mode:C++; c-basic-indent:4 -*- //

#ifndef _thread_event_handler_h
#define _thread_event_handler_h

#include <pthread.h>
#include <deque>
using namespace std;

#include "event_handler.h"

class ThreadEventHandler : public EventHandler {
 public:
    ThreadEventHandler() : m_threadID(0) {}
    virtual ~ThreadEventHandler() {}
	
    int start(void);
    void stop(void);

    void eventLoop();

    static void *startThread(void *caller);
    static void finishThread(void *caller);

    virtual void run(void) = 0;
    virtual void end(void) = 0;

    virtual void eventReceived(void) = 0;

 private:
    pthread_t m_threadID;

    deque<Message *> m_queue;
    pthread_cond_t m_condition;
    pthread_mutex_t m_mutex;
    Message *dequeue();
};

#endif  // _thread_event_handler_h
