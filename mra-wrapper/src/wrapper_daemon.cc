// -*- Mode:C++; c-basic-indent:4 -*- //

#include <signal.h>

#include "wrapper_daemon.h"
#include "scheduler.h"

#define DBG_LEVEL 3
#include "debug.h"

wrapperDaemon::wrapperDaemon(void) 
{
    DBG_IN("wrapperDaemon::wrapperDaemon");

    DBG_OUT("wrapperDaemon::wrapperDaemon");
}

int wrapperDaemon::start(ConfigFile &confFile_) 
{
    int ret_;
    string value;

    DBG_IN("wrapperDaemon::start");
    
    // We get here all configuration parameters
    // For the time being we get only the ports
    // (1) Getting the Wrapper port    
    if(confFile_.getKeyValue("PORT","WRAPPER",value) < 0) {
	DBG_ERR("WRAPPER port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_wrapperPort = atoi(value.c_str());
	DBG_INFO("The WRAPPER port is setting as %d",m_wrapperPort);
    }

    if(confFile_.getKeyValue("PORT","GLLIM",value) < 0) {
	DBG_ERR("GLLIM port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_gllIM_port = atoi(value.c_str());
	DBG_INFO("The GLL_IM is listening at port %d",m_gllIM_port);
    }

    if(confFile_.getKeyValue("PORT","MRRM",value) < 0) {
	DBG_ERR("MRRM port not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_mrrm_port = atoi(value.c_str());
	DBG_INFO("The MRRM is listening at port %d",m_mrrm_port);
    }

    if(confFile_.getKeyValue("TRIGGERSID","RTSOCK",value) < 0) {
	DBG_ERR("Trigger ID for RTSOCK  not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_rtsockTriggerID = atoi(value.c_str());
	DBG_INFO("The Trigger ID for rtsock is %d",m_rtsockTriggerID);
    }

    if(confFile_.getKeyValue("TRIGGERSID","MHI_TRG",value) < 0) {
	DBG_ERR("Trigger ID for MHI_Trigger.Indication  not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_MHItriggerID = atoi(value.c_str());
	DBG_INFO("The Trigger ID for MHI_Trigger.Indication is %d",m_MHItriggerID);
    }

    if(confFile_.getKeyValue("TRIGGERSID","MHI_HO",value) < 0) {
	DBG_ERR("Trigger ID for MHI_HandoverExecution.Request  not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_MHI_HOexecID = atoi(value.c_str());
	DBG_INFO("The Trigger ID for MHI_HandoverExecution.Request is %d",m_MHI_HOexecID);
    }

    if(confFile_.getKeyValue("TRIGGERSID","MHI_LO",value) < 0) {
	DBG_ERR("Trigger ID for MHI_LocatorDeletion.Request  not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_MHI_LOdelID = atoi(value.c_str());
	DBG_INFO("The Trigger ID for MHI_LocatorDeletion.Request is %d",m_MHI_LOdelID);
    }

    if(confFile_.getKeyValue("TRGANID","ANID",value) < 0) {
	DBG_ERR("AN Identifier for Triggering not specified... Please fix!!!");
	ret_ = -1;
	goto theEND;
    } else {
	m_trgANid = value;
	DBG_INFO("The AN Identifier for TRG FE is %s",m_trgANid.c_str());
    }

    signal(SIGUSR1, SIG_IGN);
    
    ret_ = pthread_create(&m_threadID, NULL, threadExecute, (void *)this);

    DBG_INFO("m_threadID = %d - ret_ = %d",(int) m_threadID,ret_);
    sleep(3);

    ret_ = pthread_create(&m_threadIDServer, NULL, threadExecuteServer, (void *)this);

    DBG_INFO("m_threadIDServer = %d - ret_ = %d",(int) m_threadIDServer,ret_);
    sleep(3);

 theEND:
    DBG_OUT("wrapperDaemon::start");
    return ret_;
}

int wrapperDaemon::stop(void)
{
    int ret_;

    DBG_IN("wrapperDaemon::stop");

    DBG_INFO("m_threadID = %d, pthread_self = %d",(int) m_threadID, (int) pthread_self());

    pthread_cancel(m_threadID);
    pthread_join(m_threadID, NULL);

    DBG_INFO("m_threadIDServer = %d, pthread_self = %d",(int) m_threadIDServer, (int) pthread_self());

    pthread_cancel(m_threadIDServer);
    pthread_join(m_threadIDServer, NULL);

    DBG_OUT("wrapperDaemon::stop");
    return ret_;
}

void *wrapperDaemon::threadExecute(void *caller) 
{
    sigset_t sigmask;
    wrapperDaemon *this_;

    DBG_IN("wrapperDaemon::threadExecute");

    this_ = (wrapperDaemon *) caller;

    this_->m_threadID = pthread_self();

    sigfillset(&sigmask);
    pthread_sigmask(SIG_BLOCK, &sigmask, NULL);
    
    pthread_cleanup_push(threadCleanup, caller);
    
    this_->main();
    
    pthread_cleanup_pop(1);
    
    DBG_OUT("wrapperDaemon::threadExecute");
    return NULL;
}


void *wrapperDaemon::threadExecuteServer(void *caller) 
{
    wrapperDaemon *this_;
    
    DBG_IN("wrapperDaemon::threadExecuteServer");

    this_ = (wrapperDaemon *) caller;

    this_->mainServer();

    DBG_OUT("wrapperDaemon::threadExecuteServer");
    return NULL;
}

void wrapperDaemon::threadCleanup(void *caller)
{
    wrapperDaemon *this_;
    
    DBG_IN("wrapperDaemon::threadCleanup");
    
    this_ = (wrapperDaemon *) caller;
    this_->exit();

    DBG_OUT("wrapperDaemon::threadCleanup");
}

void wrapperDaemon::main(void)
{
    DBG_IN("wrapperDaemon::main");

    m_mraWrapper = new MRAwrapper;

    m_mraWrapper->mraWrapper_port() = m_wrapperPort;
    m_mraWrapper->gllIM_port() = m_gllIM_port;
    m_mraWrapper->mrrm_port() = m_mrrm_port;
    m_mraWrapper->rtsock_triggerID() = m_rtsockTriggerID;
    m_mraWrapper->MHI_triggerID() = m_MHItriggerID;
    m_mraWrapper->MHI_HOexecID() = m_MHI_HOexecID;
    m_mraWrapper->MHI_LOdelID() = m_MHI_LOdelID;
    m_mraWrapper->trgFE_ANid() = m_trgANid;

    m_mraWrapper->start();

    waitForEvent();
    while(1) { }

    DBG_OUT("wrapperDaemon:main");
}

void wrapperDaemon::mainServer(void)
{
    DBG_IN("wrapperDaemon::mainServer");

    m_mraWrapperServer = new MRAwrapper;

    m_mraWrapperServer->startServer();

    DBG_OUT("wrapperDaemon::mainServer");
}

void wrapperDaemon::exit(void) 
{
    DBG_IN("wrapperDaemon::exit");


    DBG_OUT("wrapperDaemon::exit");
}

void wrapperDaemon::waitForEvent(void)
{
    DBG_IN("wrapperDaemon::waitForEvent");

    if(pthread_equal(scheduler::instance()->getMainThreadID(), pthread_self())) {
	DBG_INFO("Going to main loop");
	scheduler::instance()->mainLoop();
    } else {
	pthread_join(scheduler::instance()->getMainThreadID(), NULL);
    }

    DBG_OUT("wrapperDaemon::waitForEvent");
}
