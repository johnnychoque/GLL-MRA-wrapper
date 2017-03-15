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

#ifndef _gll_im_h
#define _gll_im_h

#include <map>
#include <string>
using namespace std;

#include "trg.h"

#include "config.h"
#include "scheduler.h"
#include "timer.h"
#include "ari.h"
#include "mgi_packet.h"
#include "gll_common.h"

#define EVT_MGI_RX_IM  0x01 | GLL_IM_ID
#define EVT_ARI_RX_IM  0x02 | GLL_IM_ID

#define TIMER_CANDIDATEREPORT_INDICATION  0x01 | GLL_IM_ID
#define TIMER_DEVREG_RESPONSE             0x02 | GLL_IM_ID
#define TIMER_DEVDEREG_RESPONSE           0x05 | GLL_IM_ID
#define TIMER_WAITING_LINKATTACH          0x03 | GLL_IM_ID
#define TIMER_LINKREPORT                  0x04 | GLL_IM_ID
#define TIMER_WAITING_DISCONNECT          0x06 | GLL_IM_ID

#define WAITING_DEVREGISTRATIONRESPONSE  0x0001
#define REGISTERED                       0x0002
#define WAITING_LINKATTACH               0x0003
#define ATTACHED                         0x0004
#define WAITING_DISCONNECT               0x0005
#define DISCONNECTED                     0x0006

struct interfaceGLLim {
    char m_identifier[MAX_OCTEC_STRING];
    u_int8_t m_state;
    u_int16_t m_techType;
    
    string m_networkID;
    u_int8_t m_cellID[CELLID_LEN];
    u_int32_t m_localLinkID;

    struct measConfiguration m_measConfiguration;
};    

class gllIM : public EventHandler, public TimerHandler {
    
 public:
    gllIM(void);

    void onEvent(struct eventMessage *message);
    void onTimeout(struct timerMessage *message);

    void start();
    void stop();

    inline int &MRRM_port(void) { return m_mrrm_port; }
    inline int &WRAPPER_port(void) { return m_wrapper_port; }

    inline int &MGI_port(void) { return m_MGIport; }
    inline int &ARI_ALport(void) { return m_ARI_ALport; }
    inline int &ARI_IMport(void) { return m_ARI_IMport; }

 private:
    int openMGI_socket(void);
    int openARI_socket(void);

    void handleMGI_Rx(void);
    void handleARI_Rx(void);

    void sendToMRRM(u_int8_t *msg_buf, int buf_len);
    void sendToGLL_al(u_int8_t *msg_buf, int buf_len);
    void sendToWRAPPER(u_int8_t *msg_buf, int buf_len);

    // Send primitives to WRAPPER
    void sendRegisterACSFToWrapper(void);
    void sendUnregisterACSFToWrapper(void);
    void sendLinkAttachAnswerToWrapper(struct interfaceGLLim *iface_);
    //    void sendRTSockInformationToWrapper(u_int8_t *buf_, int buf_len_);

    // Primitives received from WRAPPER
    int processWSIRegisterACSFAnswer(MGI_packet *packet);
    int processWSIUnregisterACSFAnswer(MGI_packet *packet);
    
    // Send primitives to MRRM
    void sendRegisterNewDeviceToMRRM(struct interfaceGLLim *iface_);
    void sendDeregisterDeviceToMRRM(struct interfaceGLLim *iface_);
    void sendCandidateReportAnswerToMRRM(struct interfaceGLLim *iface, u_int8_t result_);
    void sendCandidateReportIndicationToMRRM(struct interfaceGLLim *iface_,struct candidateReportIndication &info_);
    void sendLinkAttachAnswerToMRRM(struct interfaceGLLim *iface_, u_int8_t result_);
    void sendLinkReportAnswerToMRRM(struct interfaceGLLim *iface_, u_int8_t result_);
    void sendLinkReportIndicationToMRRM(struct interfaceGLLim *iface_, u_int16_t linkQual_);
    void sendLinkDetachAnswerToMRRM(struct interfaceGLLim *iface_, u_int8_t result_);

    // Primitives received from MRRM
    int processMGIDeviceRegistration(MGI_packet *packet);
    int processMGIDeviceDeregistration(MGI_packet *packet);
    int processMGICandidateReport(MGI_packet *packet);
    int processMGILoadReport(MGI_packet *packet);
    int processMGILinkAttach(MGI_packet *packet);
    int processMGILinkAttachNotification(MGI_packet *packet);
    int processMGILinkDetach(MGI_packet *packet);
    int processMGILinkDetachNotification(MGI_packet *packet);
    int processMGILinkChange(MGI_packet *packet);
    int processMGILinkChangeNotification(MGI_packet *packet);
    int processMGILinkReport(MGI_packet *packet);
    
    // Primitives received from GLL_AL
    int processARINewDevice(ARI_message *ariMessage_);
    int processARIDeleteDevice(ARI_message *ariMessage_);
    int processARICandidateReport(ARI_message *ariMessage_);
    int processARILinkAttach(ARI_message *ariMessage_);
    int processARILinkReport(ARI_message *ariMessage_);
    //int processARIRTSockInformation(ARI_message *ariMessage_);

    // Send primitives to GLL_AL
    void sendCandidateReportToGLLal(string devID_);
    void sendConnectNetworkToGLLal(string device_, string networkID_);
    void sendDisconnectNetworkToGLLal(string device_);
    void sendGetLinkQualityToGLLal(struct interfaceGLLim *iface_);

    void unsuccessAttach(struct interfaceGLLim *iface_);    
    
    // Miscelaneous functions
    void configureReporting(struct interfaceGLLim *iface_);

    // Miscelaneous primitives to TRG
    void sendLinkQualityToTRG(u_int8_t snr_);
    void registerNewDeviceToTRG(u_int8_t devID_[MAX_OCTEC_STRING]);
    void deregisterDeviceToTRG(u_int8_t devID_[MAX_OCTEC_STRING]);
    
    int m_ARI_ALport;
    int m_ARI_IMport;
    int m_MGIport;
    int m_mrrm_port;
    int m_wrapper_port;
    
    int MGI_socket;
    int ARI_IMsocket;

    u_int8_t transactionID_;

    int role;

    map<string,struct interfaceGLLim*> m_interfaces;
    typedef map<string,struct interfaceGLLim*>::iterator iface_iter_t;

    struct interfaceGLLim *searchInterface(string &device_);
    void deleteInterface(string &device_);

    TRGProducer m_producer;
};


#endif  // _gll_im_h
