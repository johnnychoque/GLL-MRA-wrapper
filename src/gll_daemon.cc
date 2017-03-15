// -*- Mode:C++; c-basic-indent:4 -*- //


#include <signal.h>

#include "gll_daemon.h"
#include "scheduler.h"

#include "gll_al.h"
#include "gll_im.h"

#define DBG_LEVEL 2
#include "debug.h"

gllDaemon::gllDaemon(void) 
{
    DBG_IN("gllDaemon::gllDaemon");

    //DBG_INFO("pthreadMain = %d - pthread_self = %d",(int) scheduler::instance()->getMainThreadID(), (int) pthread_self());

    DBG_OUT("gllDaemon::gllDaemon");
}

int gllDaemon::start(ConfigFile &confFile_) 
{
    int ret_;
    string value;

    DBG_IN("gllDaemon::start");
    
    // We get here all configuration parameters
    // For the time being we get only the ports

    // (1) Getting the MRRM port
    if(confFile_.getKeyValue("PORTS","MRRM",value) < 0) {
	DBG_ERR("MRRM port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_mrrm_port = atoi(value.c_str());
	DBG_INFO("MRRM is listening at port %d",m_mrrm_port);
    }

    // (2) Getting the gllAL_ARI port
    if(confFile_.getKeyValue("PORTS","GLLAL_ARI",value) < 0) {
	DBG_ERR("gllAL_ARI port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_gllAL_ARI_port = atoi(value.c_str());
	//DBG_INFO("gllAL_ARI port is setting as %d",m_gllAL_ARI_port);
    }

    // (3) Getting the gllIM_ARI port
    if(confFile_.getKeyValue("PORTS","GLLIM_ARI",value) < 0) {
	DBG_ERR("gllIM_ARI port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_gllIM_ARI_port = atoi(value.c_str());
	//DBG_INFO("gllIM_ARI port is setting as %d",m_gllIM_ARI_port);
    }

    // (4) Getting the WRAPPER port
    if(confFile_.getKeyValue("PORTS","WRAPPER",value) < 0) {
	DBG_ERR("WRAPPER port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_wrapper_port = atoi(value.c_str());
	DBG_INFO("WRAPPER is listening at port %d",m_wrapper_port);
    }

    // (5) Getting the MGI port
    if(confFile_.getKeyValue("PORTS","MGI",value) < 0) {
	DBG_ERR("MGI port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_mgi_port = atoi(value.c_str());
	//DBG_INFO("MGI port is setting as %d",m_wrapper_port);
    }

    signal(SIGUSR1, SIG_IGN);
    
    ret_ = pthread_create(&m_threadID, NULL, threadExecute, (void *)this);

    DBG_INFO("m_threadID = %d - ret_ = %d",(int) m_threadID,ret_);
    sleep(3);

 theEND:
    DBG_OUT("gllDaemon::start");
    return ret_;
}

int gllDaemon::stop(void)
{
    int ret_;

    DBG_IN("gllDaemon::stop");

    DBG_INFO("m_threadID = %d, pthread_self = %d",(int) m_threadID, (int) pthread_self());
    
    m_gllIM->stop();

    pthread_cancel(m_threadID);
    pthread_join(m_threadID, NULL);

    DBG_OUT("gllDaemon::stop");
    return ret_;
}

void *gllDaemon::threadExecute(void *caller) 
{
    sigset_t sigmask;
    gllDaemon *this_;

    DBG_IN("gllDaemon::threadExecute");

    this_ = (gllDaemon *) caller;

    this_->m_threadID = pthread_self();

    sigfillset(&sigmask);
    pthread_sigmask(SIG_BLOCK, &sigmask, NULL);
    
    pthread_cleanup_push(threadCleanup, caller);
    
    this_->main();
    
    pthread_cleanup_pop(1);
    
    //this_->m_threadID = 0;

    DBG_OUT("gllDaemon::threadExecute");
    return NULL;
}

void gllDaemon::threadCleanup(void *caller)
{
    gllDaemon *this_;
    
    DBG_IN("gllDaemon::threadCleanup");
    
    this_ = (gllDaemon *) caller;
    this_->exit();

    DBG_OUT("gllDaemon::threadCleanup");
}

void gllDaemon::main(void)
{
    DBG_IN("gllDaemon::main");

    m_gllAL = new gllAL;
    m_gllIM = new gllIM;

    m_gllAL->ARI_ALport() = m_gllAL_ARI_port;
    m_gllAL->ARI_IMport() = m_gllIM_ARI_port;

    m_gllIM->ARI_ALport() = m_gllAL_ARI_port;
    m_gllIM->ARI_IMport() = m_gllIM_ARI_port;
    m_gllIM->MRRM_port() = m_mrrm_port;
    m_gllIM->MGI_port() = m_mgi_port;    
    m_gllIM->WRAPPER_port() = m_wrapper_port;

    m_gllAL->start();
    m_gllIM->start();

    waitForEvent();
    while(1) { }

    DBG_OUT("gllDaemon:main");
}

void gllDaemon::exit(void) 
{
    DBG_IN("gllDaemon::exit");


    DBG_OUT("gllDaemon::exit");
}

void gllDaemon::waitForEvent(void)
{
    DBG_IN("gllDaemon::waitForEvent");

    if(pthread_equal(scheduler::instance()->getMainThreadID(), pthread_self())) {
	DBG_INFO("Going to main loop");
	scheduler::instance()->mainLoop();
    } else {
	pthread_join(scheduler::instance()->getMainThreadID(), NULL);
    }

    DBG_OUT("gllDaemon::waitForEvent");
}
