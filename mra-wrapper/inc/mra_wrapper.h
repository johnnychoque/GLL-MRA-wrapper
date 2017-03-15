/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: gll_im.h
 *
 * Description: 
 *
 * Authors: Ramon Aguero (ramon@tlmat.unican.es)
 *          Johnny Choque (jchoque@tlmat.unican.es)
 *
 * Organisation: University of Cantabria
 *
 * This software 
 *
 */

#ifndef _mra_wrapper_h
#define _mra_wrapper_h

#include <map>
#include <string>
using namespace std;

#include "config.h"
#include "scheduler.h"
#include "timer.h"
#include "wsi_packet.h"

#define MAX_PKT_SIZE 512
#define BUFBASE64 MAX_UVALUE_LEN*4

#define EVT_WSWRAPPER_RX 0x01 

class MRAwrapper : public EventHandler, public TimerHandler {
    
 public:
    MRAwrapper(void);

    void onEvent(struct eventMessage *message);
    void onTimeout(struct timerMessage *message);

    void start();
    void startServer();

    inline int &mraWrapper_port(void) { return m_wrapperPort; }
    inline int &gllIM_port(void) { return m_gllIM_port; }
    inline int &mrrm_port(void) { return m_mrrm_port; }
    inline int &rtsock_triggerID(void) { return m_rtsockTriggerID; }
    inline int &MHI_triggerID(void) { return m_MHItriggerID; }
    inline int &MHI_HOexecID(void) {return m_MHI_HOexecID; }
    inline int &MHI_LOdelID(void) {return m_MHI_LOdelID; }
    inline string &trgFE_ANid(void) { return m_trgANid; }

 private:
    int openMRAwrapper_socket(void);

    void handleMRAwrapper_Rx(void);

    void sendToGLLim(u_int8_t *msg_buf, int buf_len);
    void sendToMRRM(u_int8_t *msg_buf, int buf_len);
    
    // Primitives received from MRRM / GLL
    void processWrapper_MHI(WSI_packet *packet);
    void processWrapper_GHI(WSI_packet *packet);
    void processWrapper_ACSI(WSI_packet *packet);

    //    int processWSI_ACSI_registerFE(WSI_packet *packet);
    //    int processWSI_ACSI_unRegisterFE(WSI_packet *packet);

    void processGHI_linkAttach(WSI_packet *packet);
    void processGHI_RTSockInformation(WSI_packet *packet);

    void processMHI_Trigger(WSI_packet *packet);
    void processMHI_HandoverExecution(WSI_packet *packet);
    void processMHI_LocatorDeletion(WSI_packet *packet);

    // Send primitives to MRRM / GLL
    void sendACSI_RegisterFEAnswer(u_int8_t fe_, u_int64_t transID_, u_int32_t resultCode_);
    void sendMHI_HandoverExecutionAnswer(u_int64_t transID_, u_int32_t resultCode_);
    void sendMHI_LocatorDeletionAnswer(u_int64_t transID_, u_int32_t resultCode_);

    void subscribeTRGproducer(int triggerID, string trgProdURI);
    void sendTriggering(struct trigger &trgProd, string trgProdURI);
    void RegisterFE(string FEname);
    void UnRegisterFE(string FEname);
    void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);
    string LookupURI(string nameToResolve);

    int m_wrapperPort;	
    int m_gllIM_port;
    int m_mrrm_port;
    int m_wrapperSocket;
    int m_rtsockTriggerID;
    int m_MHItriggerID;
    int m_MHI_HOexecID;
    int m_MHI_LOdelID;
    string m_trgProdURI;
    string m_trgANid;
};


#endif  // _mra_wrapper_h
