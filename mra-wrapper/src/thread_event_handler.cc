#include "thread_event_handler.h"

int ThreadEventHandler::start(void) 
{
    int ret_;

    DBG_IN("ThreadEventHandler::start");
    
    //signal(SIGUSR1, SIG_IGN);
    
    ret_ = pthread_create(&m_threadID, NULL, threadExecute, (void *)this);

    DBG_INFO("m_threadID = %d - ret_ = %d",(int) m_threadID,ret_);
    
    DBG_OUT("ThreadEventHandler::start");
    return ret_;
}
   
void ThreadEventHandler::stop(void) 
{
    int ret_;
    
    DBG_IN("ThreadEventHandler");

    pthread_cancel(m_threadID);
    pthread_join(m_threadID, NULL));

    DBG_OUT("ThreadEventHandler");
}

void *ThreadEventHandler::startThread(void *caller)
{
    sigset_t sigmask;
    ThreadEventHandler *this_;

    DBG_IN("ThreadEventHandler::threadExecute");

    this_ = (ThreadEventHandler *) caller;

    sigfillset(&sigmask);
    pthread_sigmask(SIG_BLOCK, &sigmask, NULL);
    
    pthread_cleanup_push(finishThread, caller);
    
    this_->run();
    
    pthread_cleanup_pop(1);
    

    DBG_OUT("ThreadEventHandler::threadExecute");
    return NULL;
}


void ThreadEventHandler::finishThread(void *caller)
{
    ThreadEventHandler *this_;
    
    DBG_IN("ThreadEventHandler::threadCleanup");
    
    this_ = (ThreadEventHandler *) caller;
    this_->end();

    DBG_OUT("ThreadEventHandler::threadCleanup");
}

    
void ThreadEventHandler::eventLoop(void)
{

    Message *

}
