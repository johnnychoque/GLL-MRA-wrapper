// -*- Mode: C++; c-basic-offset: 4 -*- //

#ifndef _gll_al_h
#define _gll_al_h

#include "scheduler.h"
#include "timer.h"
#include "ari.h"
#include "ifaceTable.h"
#include "gll_common.h"

#define EVT_ARI_RX_AL 0x01 | GLL_AL_ID
//#define EVT_RTSOCK_RX 0x02 | GLL_AL_ID

#define TIMER_CHECK_IFACES 0x01 | GLL_AL_ID
#define TIMER_CHECK_IFACE_ASSOCIATED 0x02 | GLL_AL_ID

#define BUFSIZE 4096

class gllAL : public EventHandler, public TimerHandler {
    
 public:
    gllAL(void);
    virtual ~gllAL(void){}

    void onEvent(struct eventMessage *message);
    void onTimeout(struct timerMessage *message);

    void start(void);
    
    inline int &ARI_ALport(void) { return m_ARI_ALport; }
    inline int &ARI_IMport(void) { return m_ARI_IMport; } 

 private:
    int m_ARI_ALport;
    int m_ARI_IMport;
    
    int ARI_ALsocket;
    int openARI_socket(void);
    
    int m_netlink_socket;
    int openNetlink_socket(void);

    InterfaceTable *m_ifaceTable;

    void handleARI_Rx(void);
    //    void handleRTSock_Rx(void);

    int processARICandidateReport(ARI_message *ariMessage_);
    int processARILinkAttach(ARI_message *ariMessage_);
    int processARILinkDetach(ARI_message *ariMessage_);
    int processARILinkReport(ARI_message *ariMessage_);

    void registerNewDeviceToGLLim(NetInterface *iface);
    void deregisterDeviceToGLLim(NetInterface *iface);

    void deliverCandidateReportToGLLim(string &deviceID_, struct candidateEntry *currentEntry_);
    void deliverLinkAttachResponseToGLLim(NetInterface *iface_);
    void deliverLinkReportToGLLim(string &device_, int rssi_);

    //    void deliverRTSockInformationToGLLim(u_int8_t *buf_, int len_);

    void sendToGLL_im(u_int8_t *msg_buf, int buf_len);
};



#endif // _gll_al_h
