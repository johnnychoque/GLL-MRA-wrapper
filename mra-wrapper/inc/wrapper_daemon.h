// -*- Mode:C++; c-basic-indent:4 -*- //

#ifndef _wrapper_daemon_h
#define _wrapper_daemon_h

#include <pthread.h>

#include "ConfigurationFile.h"
#include "scheduler.h"

#include "mra_wrapper.h"

class wrapperDaemon {
public:
    wrapperDaemon();
    virtual ~wrapperDaemon() {}

    inline pthread_t getThreadID(void) {return m_threadID;}

    int start(ConfigFile &confFile_);
    int stop();

    void waitForEvent(void);
    
protected:
    void main(void);
    void mainServer(void);
    void exit(void);
    
private:
    pthread_t m_threadID;
    pthread_t m_threadIDServer;
    
    MRAwrapper *m_mraWrapper, *m_mraWrapperServer;

    static void *threadExecute(void *caller);
    static void *threadExecuteServer(void *caller);
    static void threadCleanup(void *caller);

    int m_wrapperPort;
    int m_gllIM_port;
    int m_mrrm_port;
    int m_rtsockTriggerID;
    int m_MHItriggerID;
    int m_MHI_HOexecID;
    int m_MHI_LOdelID;
    string m_trgANid;
};



#endif  // _wrapper_daemon_h
