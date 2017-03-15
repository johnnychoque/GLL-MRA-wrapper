// -*- Mode:C++; c-basic-indent:4 -*- //

#ifndef _gll_daemon_h
#define _gll_daemon_h

#include <pthread.h>

#include "ConfigurationFile.h"
#include "scheduler.h"
#include "gll_al.h"
#include "gll_im.h"

class gllDaemon {
public:
    gllDaemon();
    virtual ~gllDaemon() {}

    inline pthread_t getThreadID(void) {return m_threadID;}

    int start(ConfigFile &confFile_);
    int stop();

    void waitForEvent(void);
    
protected:
    void main(void);
    void exit(void);
    
private:
    pthread_t m_threadID;
    
    gllAL *m_gllAL;
    gllIM *m_gllIM;

    static void *threadExecute(void *caller);
    static void threadCleanup(void *caller);

    int m_gllAL_ARI_port;
    int m_gllIM_ARI_port;
    int m_wrapper_port;
    int m_mrrm_port;
    int m_mgi_port;
};



#endif  // _gll_daemon_h
