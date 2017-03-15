/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: gll_im.cc
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
#include <cstdlib>
#include <ctime>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include "mgi_packet.h"
#include "wsi_packet.h"
#include "ari.h"
#include "gll_im.h"
#include "gll_common.h"
#include "candidateEntry.h"

#define DBG_LEVEL 4
#include "debug.h"

//#define TRG_PRODUCER 0

gllIM::gllIM(void)
{
    DBG_IN("gllIM::gllIM");
    
    // Getting the initial Transaction ID
    srand(time(0));
    transactionID_ = rand()%MAX_UINT_16;

    DBG_OUT("gllIM::gllIM");
}

void gllIM::start(void) 
{
    //bool success_;

    DBG_IN("gllIM::start");
    
    if(!openMGI_socket()) {
	scheduler::instance()->insertInput(EVT_MGI_RX_IM, MGI_socket, this, NULL);
    }

    if(!openARI_socket()) {
	scheduler::instance()->insertInput(EVT_ARI_RX_IM, ARI_IMsocket, this, NULL);
    }

#ifdef TRG_PRODUCER
    success_ = m_producer.Register(200);
    if(!success_) {
	DBG_ERR("Couldn't subscribe to Triggering framework");
	DBG_ERR("%s",m_producer.getError().c_str());
    }
	
#endif

    sendRegisterACSFToWrapper();

    DBG_OUT("gllIM::start");
}

void gllIM::stop(void)
{
    DBG_IN("gllIM::stop");

    sendUnregisterACSFToWrapper();

    DBG_OUT("gllIM::stop");
}

void gllIM::sendRegisterACSFToWrapper(void)
{
    WSI_packet *wsiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int8_t FEname;
    u_int64_t TransID_;

    DBG_IN("gllIM::sendRegisterACSFToWrapper");

    wsiPacket_ = new WSI_packet;

    wsiPacket_->setHeader(MSGTYPE_ACSF_REGISTER_FE, REQUEST);

    TransID_ = transactionID_;
    wsiPacket_->addTLV(WSI_ParaTAG_TransactionID,(u_paraValue_t *) &TransID_);

    FEname = GLL_FE;
    wsiPacket_->addTLV(WSI_ParaTAG_FE_ID,(u_paraValue_t *) &FEname, sizeof(u_int8_t));

    length_ = wsiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT WSI_RegisterToACSF.Request");
    sendToWRAPPER(msg_buf, length_);

    DBG_OUT("gllIM::sendRegisterACSFToWrapper");
}


void gllIM::sendUnregisterACSFToWrapper(void)
{
    WSI_packet *wsiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int8_t FEname;

    DBG_IN("gllIM::sendUnregisterACSFToWrapper");

    wsiPacket_ = new WSI_packet;

    wsiPacket_->setHeader(MSGTYPE_ACSF_UNREGISTER_FE, REQUEST);

    FEname = GLL_FE;

    wsiPacket_->addTLV(WSI_ParaTAG_FE_ID,(u_paraValue_t *) &FEname);

    length_ = wsiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT WSI_UnregisterToACSF.Request");
    sendToWRAPPER(msg_buf, length_);

    DBG_OUT("gllIM::sendUnregisterACSFToWrapper");
}


void gllIM::sendRegisterNewDeviceToMRRM(struct interfaceGLLim *iface_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int32_t measCapFlags_;
    u_int32_t linkEventCapFlags_;
    
    DBG_IN("gllIM::sendRegisterNewDeviceToMRRM");
    
    measCapFlags_ = MeasCapability_SNR_DL | MeasCapability_TotalLoad;
    linkEventCapFlags_ = LinkEvent_LinkUP | LinkEvent_LinkDown;

    mgiPacket_ = new MGI_packet;

    mgiPacket_->setHeader(MSGTYPE_DEVICE_REGISTRATION, REQUEST);

    mgiPacket_->addTLV(ParaTAG_DeviceID,(u_paraValue_t *) iface_->m_identifier); 
    mgiPacket_->addTLV(ParaTAG_ContactPort,(u_paraValue_t *) &m_MGIport);
    mgiPacket_->addTLV(ParaTAG_AccessTechnologyType, (u_paraValue_t *) &(iface_->m_techType));
    mgiPacket_->addTLV(ParaTAG_MeasCapabilities, (u_paraValue_t *) &measCapFlags_);
    mgiPacket_->addTLV(ParaTAG_LinkEventCapabilities, (u_paraValue_t *) &linkEventCapFlags_);
    
    length_ = mgiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT MGI_DeviceRegistration.Request");
    sendToMRRM(msg_buf, length_);
    
    iface_->m_state = WAITING_DEVREGISTRATIONRESPONSE;
    scheduler::instance()->insertTimer(TIMER_DEVREG_RESPONSE, 1000, this, false, iface_);

    DBG_OUT("gllIM::sendRegisterNewDeviceToMRRM");
}

void gllIM::sendDeregisterDeviceToMRRM(struct interfaceGLLim *iface_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int32_t attachReason_;
    
    DBG_IN("gllIM::sendDeregisterDeviceToMRRM");
    
    mgiPacket_ = new MGI_packet;

    mgiPacket_->setHeader(MSGTYPE_DEVICE_DEREGISTRATION, REQUEST);

    mgiPacket_->addTLV(ParaTAG_DeviceID,(u_paraValue_t *) iface_->m_identifier); 

    attachReason_ = AttachModifReason_Disconnect;
    mgiPacket_->addTLV(ParaTAG_AttachModifReason, (u_paraValue_t *) &attachReason_);
    mgiPacket_->addTLV(ParaTAG_ContactPort, (u_paraValue_t *) &m_MGIport);

    length_ = mgiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT MGI_DeviceRegistration.Request");
    sendToMRRM(msg_buf, length_);
    
    iface_->m_state = WAITING_DEVREGISTRATIONRESPONSE;
    scheduler::instance()->insertTimer(TIMER_DEVDEREG_RESPONSE, 1000, this, false, iface_);

    DBG_OUT("gllIM::sendDeregisterDeviceToMRRM");
}


void gllIM::handleMGI_Rx(void)
{
    char msg_buf[MAX_PKT_SIZE];
    int buf_len;
    struct sockaddr_in src;
    int sockaddr_len;
    MGI_packet *rxPacket;

    DBG_IN("gllIM::handleMGI_Rx");

    sockaddr_len = sizeof(struct sockaddr_in);

    buf_len = recvfrom(MGI_socket, msg_buf, MAX_PKT_SIZE, 0, (struct sockaddr *) &src, (socklen_t *) &sockaddr_len);

    DBG_INFO("gllIM is receiving a packet");
    DBG_DUMP("RX",msg_buf,buf_len);

    rxPacket = new MGI_packet((u_int8_t *)msg_buf, buf_len);
  
    if (rxPacket->gistHops() == WSI_GISTHOPS) { // Packet sent by WRAPPER
	DBG_INFO2("Receiving a packet from WRAPPER ...");
	switch(rxPacket->nslpID()) {
	case MSGTYPE_ACSF_REGISTER_FE:
	    processWSIRegisterACSFAnswer(rxPacket);
	    break;
	default:
	    DBG_ERR("Unknown Packet Type");
	    break;
	};
    }
    else {
	DBG_INFO2("Receiving a packet from MRRM ...");
	switch(rxPacket->nslpID()) {
	case MSGTYPE_DEVICE_REGISTRATION:
	    processMGIDeviceRegistration(rxPacket);
	    break;
	case MSGTYPE_DEVICE_DEREGISTRATION:
	    processMGIDeviceDeregistration(rxPacket);
	    break;
	case MSGTYPE_CANDIDATE_REPORT:
	    processMGICandidateReport(rxPacket);
	break;
	case MSGTYPE_LOAD_REPORT:
	    processMGILoadReport(rxPacket);
	    break;
	case MSGTYPE_LINK_ATTACH:
	    processMGILinkAttach(rxPacket);
	    break;
	case MSGTYPE_LINK_ATTACH_NOTIFICATION:
	    processMGILinkAttachNotification(rxPacket);
	    break;
	case MSGTYPE_LINK_DETACH:
	    processMGILinkDetach(rxPacket);
	    break;
	case MSGTYPE_LINK_DETACH_NOTIFICATION:
	    processMGILinkDetachNotification(rxPacket);
	    break;
	case MSGTYPE_LINK_REPORT:
	    processMGILinkReport(rxPacket);
	    break;
	case MSGTYPE_LINK_CHANGE:
	    processMGILinkChange(rxPacket);
	    break;
	default:
	    DBG_ERR("Unknown Packet Type");
	    break;
	};
    }
    
    DBG_OUT("gllIM::handleMGI_Rx");
}

void gllIM::handleARI_Rx(void)
{
    char msg_buf[MAX_PKT_SIZE];
    int buf_len;
    struct sockaddr_in src;
    int sockaddr_len;
    ARI_message *rxPacket;

    DBG_IN("gllIM::handleARI_Rx");

    sockaddr_len = sizeof(struct sockaddr_in);
    
    buf_len = recvfrom(ARI_IMsocket, msg_buf, MAX_PKT_SIZE, 0, (struct sockaddr *) &src, (socklen_t *) &sockaddr_len);
    //DBG_DUMP("RX",msg_buf,buf_len);

    rxPacket = new ARI_message((u_int8_t *)msg_buf, buf_len);
    switch(rxPacket->type()) {
    case ARI_NEW_DEVICE:
	DBG_INFO("RCVD ARI_NewDevice");
	DBG_DUMP("ARI",msg_buf,buf_len);
	processARINewDevice(rxPacket);
	break;
    case ARI_DELETE_DEVICE:
	DBG_INFO("RCVD ARI_DeleteDevice");
	DBG_DUMP("ARI",msg_buf,buf_len);
	processARIDeleteDevice(rxPacket);
	break;
    case ARI_CANDIDATE_REPORT:
	DBG_INFO("RCVD ARI_CandidateReport");
	DBG_DUMP("ARI",msg_buf,buf_len);
	processARICandidateReport(rxPacket);
	break;
    case ARI_LINK_ATTACH:
	DBG_INFO("RCVD ARI_LinkAttach");
	DBG_DUMP("ARI",msg_buf,buf_len);
	processARILinkAttach(rxPacket);
	break;
    case ARI_LINK_REPORT:
	DBG_INFO("RCVD ARI_LinkReport");
	DBG_DUMP("ARI",msg_buf,buf_len);
	processARILinkReport(rxPacket);
	break;
	/*    case ARI_RTSOCK_INFO:
	processARIRTSockInformation(rxPacket);
	break;*/
    default:
	DBG_ERR("Unknown packet type");
	break;
    }

    DBG_OUT("gllIM::handleARI_Rx");
}


int gllIM::processMGIDeviceRegistration(MGI_packet *packet)
{
    int ret_;
    u_int32_t resultCode_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    string device_;
    struct interfaceGLLim *iface_;
    
    DBG_IN("gllIM::processMGIDeviceRegistration");
    
    switch(packet->type()) {
    case RESPONSE_GIST_TYPE:
	DBG_INFO("RCVD MGI_DeviceRegistration.Answer");
	// I have to delete the timer I had for this 
	memset(devID_, '\0', MAX_OCTEC_STRING);
	packet->getDataFromTLV(ParaTAG_DeviceID, (u_paraValue_t *) devID_, &length_);
	device_ = (char *) devID_;
	
	resultCode_ = 0;
	packet->getDataFromTLV(ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_, &length_);

	DBG_INFO("\tdev = %s - resultcode = %x",device_.c_str(), resultCode_);

	if(iface_=searchInterface(device_)) {
	    if(iface_->m_state == WAITING_DEVREGISTRATIONRESPONSE) {
		scheduler::instance()->deleteTimer(TIMER_DEVREG_RESPONSE, this, iface_);
		iface_->m_state = REGISTERED;
	    }
	    ret_ = 0;
	} else {
	    DBG_ERR("GLL_IM was not aware of that device");
	    ret_ = -2;
	}
	break;
    case DATA_GIST_TYPE:
    case QUERY_GIST_TYPE:
    default:
	DBG_ERR("Received DeviceRegistration.Type, GLL IM is not able to handle type");
	ret_ = -1;
	break;
    }
	
    DBG_OUT("gllIM::processMGIDeviceRegistration");
    return ret_;
}

int gllIM::processMGIDeviceDeregistration(MGI_packet *packet)
{
    int ret_;
    u_int32_t resultCode_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    string device_;
    struct interfaceGLLim *iface_;
    
    DBG_IN("gllIM::processMGIDeviceDeregistration");

    switch(packet->type()) {
    case RESPONSE_GIST_TYPE:
	DBG_INFO("RCVD MGI_DeviceDeregistration.Answer");
	// I have to erase the information for this interface
	memset(devID_, '\0', MAX_OCTEC_STRING);
	packet->getDataFromTLV(ParaTAG_DeviceID, (u_paraValue_t *) devID_, &length_);
	device_ = (char *) devID_;
	
	resultCode_ = 0;
	packet->getDataFromTLV(ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_, &length_);
	
	if(iface_ = searchInterface(device_)) {
	    deleteInterface(device_);
	} else {
	    DBG_ERR("GLL IM was not aware of that device");
	    ret_ = -2;
	}
	break;
    case DATA_GIST_TYPE:
    case QUERY_GIST_TYPE:
    default:
	DBG_ERR("Received DeviceRegistration.Type, GLL IM is not able to handle type");
	ret_ = -1;
	break;
    }
	
    
    DBG_OUT("gllIM::processMGIDeviceDeregistration");
    return ret_;
}

int gllIM::processMGICandidateReport(MGI_packet *packet)
{
    int ret_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    string device_;
    struct interfaceGLLim *iface_;
    
    DBG_IN("gllIM::processMGICandidateReport");

    switch(packet->type()) {
    case QUERY_GIST_TYPE:
	DBG_INFO("RCVD MGI_CandidateReport.Request");
	
	memset(devID_, '\0', MAX_OCTEC_STRING);
	packet->getDataFromTLV(ParaTAG_DeviceID, (u_paraValue_t *) devID_, &length_);
	device_ = (char *) devID_;

	if(iface_ = searchInterface(device_)) {
	    if(packet->getMeasConfiguration(&(iface_->m_measConfiguration)) < 0) {
		DBG_ERR("The candidate report did not carry any configuration");
	    } else {
		configureReporting(iface_);
	    }

	    sendCandidateReportAnswerToMRRM(iface_,ResultCode_SUCCESS);
	} else {
	    DBG_ERR("GLL IM was not aware of such device");
	    ret_ = -2;
	}
	break;
    case DATA_GIST_TYPE:
    case RESPONSE_GIST_TYPE:
    default:
	ret_ = -1;
	DBG_ERR("Received CandidateReport.Type, gllIM is not able to handle type (%X)", packet->type());
	break;
    }
    
    DBG_OUT("gllIM::processMGICandidateReport");
    return ret_;
}


int gllIM::processMGILoadReport(MGI_packet *packet)
{
    struct measConfiguration measConfig;
    int ret_;

    DBG_IN("gllIM::processMGILoadReport");
    
    switch(packet->type()) {
    case QUERY_GIST_TYPE:
	DBG_INFO("RCVD MGI_LoadReport.Request");
	packet->getMeasConfiguration(&measConfig);
	break;
    case DATA_GIST_TYPE:
    case RESPONSE_GIST_TYPE:
    default:
	DBG_INFO("Received LoadReport.Type, GLL IM is not able to handle type");
	break;
    }
    


    DBG_OUT("gllIM::processMGILoadReport");
    return ret_;
}
    
int gllIM::processMGILinkAttach(MGI_packet *packet)
{
    int ret_;
    u_int8_t aux_[MAX_OCTEC_STRING];
    int length_;
    string device_;
    struct interfaceGLLim *iface_;

    DBG_IN("gllIM::processMGILinkAttach");

    switch(packet->type()) {
    case QUERY_GIST_TYPE:
	DBG_INFO("RCVD MGI_LinkAttach.Request");
	
	memset(aux_, '\0', MAX_OCTEC_STRING);
	packet->getDataFromTLV(ParaTAG_DeviceID, (u_paraValue_t *) aux_, &length_);
	device_ = (char *) aux_;
	
	if(iface_ = searchInterface(device_)) {
	    memset(aux_, '\0', MAX_OCTEC_STRING);
	    packet->getDataFromTLV(ParaTAG_NetworkID, (u_paraValue_t *) aux_, &length_);
	    iface_->m_networkID = (char *) aux_;

	    memset(aux_, '\0', MAX_OCTEC_STRING);
	    packet->getDataFromTLV(ParaTAG_CellID, (u_paraValue_t *) aux_, &length_);
	    memcpy(iface_->m_cellID, aux_, CELLID_LEN);

	    packet->getDataFromTLV(ParaTAG_LocalLinkID, (u_paraValue_t *) &(iface_->m_localLinkID), &length_);

	    
	    sendConnectNetworkToGLLal(device_, iface_->m_networkID);
	    iface_->m_state = WAITING_LINKATTACH;
	    
	    // I need to elapse the timer for getting candidate report measurements 
	    // I start another one to wait for the attachment
	    scheduler::instance()->deleteTimer(TIMER_CANDIDATEREPORT_INDICATION, this, iface_);
	    scheduler::instance()->insertTimer(TIMER_WAITING_LINKATTACH, 10000, this, false, iface_);

	    DBG_INFO("Device %s - is waiting link attach",iface_->m_identifier);
	    DBG_INFO("\t- Network ID = %s",iface_->m_networkID.c_str());
	    DBG_INFO("\t- Local Link ID = %u",iface_->m_localLinkID);
	    DBG_DUMP("BSSID",iface_->m_cellID,CELLID_LEN);
	    
	}
	break;
    case RESPONSE_GIST_TYPE:
    default:
	DBG_ERR("Received LinkAttach.Type, GLL IM is not able to handle type");
	break;
    }

    DBG_OUT("gllIM::processMGILinkAttach");

    return ret_;
}

int gllIM::processMGILinkAttachNotification(MGI_packet *packet)
{
    int ret_;

    DBG_IN("gllIM::processMGILinkAttachNotification");

    DBG_OUT("gllIM::processMGILinkAttachNotification");

    return ret_;
}

int gllIM::processMGILinkDetach(MGI_packet *packet)
{
    int ret_;
    int length_;
    string device_; 
    u_int8_t devID_[MAX_OCTEC_STRING];
    struct interfaceGLLim *Iface_;

    DBG_IN("gllIM::processMGILinkDetach");

    switch(packet->type()) {
    case QUERY_GIST_TYPE:
	DBG_INFO("Received LinkDettach.Request");
	
	Iface_ = new struct interfaceGLLim;

	memset(devID_, '\0', MAX_OCTEC_STRING);
	packet->getDataFromTLV(ParaTAG_DeviceID, (u_paraValue_t *) devID_, &length_);

	memset(Iface_->m_identifier, '\0', MAX_OCTEC_STRING);
	memcpy(Iface_->m_identifier, devID_, length_);
	device_ = (char *) devID_;
	DBG_INFO("Detach device Identifier = %s",Iface_->m_identifier); 

	packet->getDataFromTLV(ParaTAG_LocalLinkID, (u_paraValue_t *) &(Iface_->m_localLinkID), &length_);
	DBG_INFO("\t- Local Link ID = %u",Iface_->m_localLinkID);

	/*	sendDisconnectNetworkToGLLal(device_);
	Iface_->m_state = WAITING_DISCONNECT;
	    
	scheduler::instance()->insertTimer(TIMER_WAITING_DISCONNECT, 10000, this, false, Iface_);
	*/
	sendLinkDetachAnswerToMRRM(Iface_, ResultCode_SUCCESS);	
	
	break;
    case RESPONSE_GIST_TYPE:
    default:
	DBG_ERR("Received LinkDetach.Type, GLL IM is not able to handle type");
	break;
	}

    DBG_OUT("gllIM::processMGILinkDetach");

    return ret_;
}

void gllIM::sendLinkDetachAnswerToMRRM(struct interfaceGLLim *iface_, u_int8_t result_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int32_t resultCode_;
    
    DBG_IN("gllIM::sendLinkDetachAnswerToMRRM");

    mgiPacket_ = new MGI_packet;
    
    mgiPacket_->setHeader(MSGTYPE_LINK_DETACH, ANSWER);
    
    mgiPacket_->addTLV(ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);
    resultCode_ = 0 | result_;
    mgiPacket_->addTLV(ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_);

    //    DBG_INFO("Local Link ID = %u",iface_->m_localLinkID);
    mgiPacket_->addTLV(ParaTAG_LocalLinkID, (u_paraValue_t *) &(iface_->m_localLinkID));

    length_ = mgiPacket_ -> serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT MGI_LinkDetach.Answer");
    sendToMRRM(msg_buf, length_);

    DBG_OUT("gllIM::sendLinkDetachAnswerToMRRM");
}

int gllIM::processMGILinkDetachNotification(MGI_packet *packet)
{
    int ret_;

    DBG_IN("gllIM::processMGILinkDetachNotification");

    DBG_OUT("gllIM::processMGILinkDetachNotification");

    return ret_;
}

int gllIM::processMGILinkChange(MGI_packet *packet)
{
    int ret_;

    DBG_IN("gllIM::processMGILinkChange");
    
    DBG_OUT("gllIM::processMGILinkChange");

    return ret_;
}

int gllIM::processMGILinkChangeNotification(MGI_packet *packet)
{
    int ret_;

    DBG_IN("gllIM::processMGILinkChangeNotification");

	
    DBG_OUT("gllIM::processMGILinkChangeNotification");

    return ret_;
}

int gllIM::processMGILinkReport(MGI_packet *packet)
{
    int ret_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    string device_;
    struct interfaceGLLim *iface_;

    DBG_IN("gllIM::processMGILinkReport");

    switch(packet->type()) {
    case QUERY_GIST_TYPE:
	DBG_INFO("RCVD MGI_LinkReport.Request");
	
	memset(devID_, '\0', MAX_OCTEC_STRING);
	packet->getDataFromTLV(ParaTAG_DeviceID, (u_paraValue_t *) devID_, &length_);
	device_ = (char *) devID_;
	
	if(iface_ = searchInterface(device_)) {
	    if(packet->getMeasConfiguration(&(iface_->m_measConfiguration)) < 0) {
		DBG_ERR("The LinkReport did not carry any configuration");
	    } else {
		configureReporting(iface_);
	    }

	    sendLinkReportAnswerToMRRM(iface_, ResultCode_SUCCESS);
	    ret_ = 0;
	} else {
	    DBG_ERR("GLL IM was not aware of such device");
	    ret_ = -2;
	}
	break;
    case DATA_GIST_TYPE:
    case RESPONSE_GIST_TYPE:
    default:
	ret_ = -1;
	DBG_ERR("Received LinkReport.Type, gllIM is not able to handle type (%X)", packet->type());
	break;
    }

    DBG_OUT("gllIM::processMGILinkReport");
    return ret_;
}

int gllIM::processWSIRegisterACSFAnswer(MGI_packet *packet)
{
    int ret_;
    u_int32_t resultCode_ = 0;
    u_int64_t TransID_;
    int length_;
    
    DBG_IN("gllIM::processWSIRegisterACSFAnswer");

    switch(packet->type()) {
    case RESPONSE_GIST_TYPE:
	DBG_INFO("RCVD WSI_RegisterACSF.Answer");

	packet->getDataFromTLV(ParaTAG_TransactionID, (u_paraValue_t *) &TransID_, &length_);
	
	resultCode_ = 0;
	packet->getDataFromTLV(ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_, &length_);
	
	if(resultCode_ == ResultCode_SUCCESS) {
	    DBG_INFO("GLL has been registered successfully to ACSF");
	} else {
	    DBG_ERR("An error has happened registering GLL to ACSF");
	    ret_ = -2;
	}
	break;
    case DATA_GIST_TYPE:
    case QUERY_GIST_TYPE:
    default:
	DBG_ERR("Received RegisterACSFAnswer.Type, GLL_IM is not able to handle type");
	ret_ = -1;
	break;
    }
	
    DBG_OUT("gllIM::processWSIRegisterACSFAnswer");    

    return ret_;
}

int gllIM::processARINewDevice(ARI_message *ariMessage_)
{
    int ret_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    string newDevice;
    struct interfaceGLLim *newIface_;

    DBG_IN("gllIM::processARINewDevice");

    memset(devID_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) devID_, &length_);

    newIface_ = new struct interfaceGLLim;

    memset(newIface_->m_identifier, '\0', MAX_OCTEC_STRING);
    memcpy(newIface_->m_identifier, devID_, length_);
    DBG_INFO("Device Identifier = %s",newIface_->m_identifier); 

    newDevice = (char *) devID_;
    m_interfaces.insert(pair<string,struct interfaceGLLim*>(newDevice, newIface_));

    ariMessage_->getDataFromTLV(ARI_ParaTAG_AccessTechType, (void *) &(newIface_->m_techType), &length_);
    DBG_INFO("Technology Type = %x",newIface_->m_techType);

    if(newIface_->m_techType != Tech_FIXED_Ethernet) {
	sendRegisterNewDeviceToMRRM(newIface_);
    }

#ifdef TRG_PRODUCER
    registerNewDeviceToTRG(devID_);
#endif

    //scheduler::instance()->insertTimer(TIMER_CANDIDATEREPORT_INDICATION, 1000, this, true, devID_);
    ret_ = 0;

    DBG_OUT("gllIM::processARINewDevice");
    return ret_;
}

int gllIM::processARIDeleteDevice(ARI_message *ariMessage_)
{
    int ret_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    string device_;
    struct interfaceGLLim *iface_;

    DBG_IN("gllIM::processARIDeleteDevice");

    memset(devID_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) devID_, &length_);
    device_ = (char *) devID_;

    if(iface_ = searchInterface(device_)) {
	if(iface_->m_techType != Tech_FIXED_Ethernet) {
	    switch(iface_->m_state) {
	    case REGISTERED:
		// I have to delete the candidate report timer
		scheduler::instance()->deleteTimer(TIMER_CANDIDATEREPORT_INDICATION, this, iface_);
		break;
	    case ATTACHED:
		// I have to delete the link report timer
		scheduler::instance()->deleteTimer(TIMER_LINKREPORT, this, iface_);
		break;
	    default:
		DBG_ERR("Unknown state");
		break;
	    }
	    sendDeregisterDeviceToMRRM(iface_);
	} 
#ifdef TRG_PRODUCER
	deregisterDeviceToTRG(devID_);
#endif
    } else {
	DBG_ERR("Interface was not registered in the GLL IM");
    }

    

    ret_ = 0;

    DBG_OUT("gllIM::processARIDeleteDevice");
    return ret_;
}


int gllIM::processARICandidateReport(ARI_message *ariMessage_) 
{
    int ret_;
    u_int8_t aux_[MAX_OCTEC_STRING];
    int length_;
    struct interfaceGLLim *iface_;
    struct candidateReportIndication candidateReportInfo_;

    DBG_IN("gllIM::processARICandidateReport");

    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) aux_, &length_);

    candidateReportInfo_.m_deviceID = (char *) aux_;
    iface_ = searchInterface(candidateReportInfo_.m_deviceID);

    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_NetworkID, (void *) aux_, &length_);
    candidateReportInfo_.m_networkID = (char *) aux_;
    DBG_INFO("Network ID = %s",candidateReportInfo_.m_networkID.c_str());
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_CellID, (void *) aux_, &length_);
    memcpy(candidateReportInfo_.m_cellID, aux_, CELLID_LEN);
    DBG_DUMP("CellID",candidateReportInfo_.m_cellID, CELLID_LEN);

    ariMessage_->getDataFromTLV(ARI_ParaTAG_LinkQual, (void *) &(candidateReportInfo_.m_rssi), &length_);
    DBG_INFO("Rssi = %u",candidateReportInfo_.m_rssi);

    if(iface_) {
	sendCandidateReportIndicationToMRRM(iface_, candidateReportInfo_);
	ret_ = 0;
    }
    
    DBG_OUT("gllIM::processARICandidateReport");
    return ret_;
}

int gllIM::processARILinkAttach(ARI_message *ariMessage_)
{
    int ret_;
    u_int8_t aux_[MAX_OCTEC_STRING];
    int length_;
    struct interfaceGLLim *iface_;
    string device_, networkID_;
    u_int8_t cellID_[CELLID_LEN];

    DBG_IN("gllIM::processARILinkAttachReport");
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) aux_, &length_);
    device_ = (char *) aux_;

    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_NetworkID, (void *) aux_, &length_);
    networkID_ = (char *) aux_;
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    memset(cellID_, '\0', CELLID_LEN);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_CellID, (void *) aux_, &length_);

    memcpy(cellID_, aux_, length_);


    if(iface_ = searchInterface(device_)) {
	if(iface_->m_state == WAITING_LINKATTACH && \
	   iface_->m_networkID == networkID_ && \
	   !memcmp(iface_->m_cellID, cellID_, CELLID_LEN)) {

	    DBG_INFO("Connected with network %s", iface_->m_networkID.c_str());

	    // I delete the timer for candidate reporting
	    scheduler::instance()->deleteTimer(TIMER_WAITING_LINKATTACH, this, iface_);
	    sendLinkAttachAnswerToMRRM(iface_, ResultCode_SUCCESS);

	    // We send the information also to the MRA wrapper
	    sendLinkAttachAnswerToWrapper(iface_);

	    iface_->m_state = ATTACHED;

	} else {
	    DBG_INFO2("The interface attached to another network");
	}
	ret_ = 0;
    } else {
	DBG_ERR("GLL IM was not aware of the device");
	ret_ = -1;
    }
    
    DBG_OUT("gllIM::processARILinkAttachReport");
    return ret_;
}

void gllIM::sendLinkAttachAnswerToWrapper(struct interfaceGLLim *iface_)
{
    WSI_packet *wsiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int8_t aux_[MAX_OCTEC_STRING];
    
    DBG_IN("gllIM::sendLinkAttachAnswerToWrapper");

    wsiPacket_ = new WSI_packet;
    
    wsiPacket_->setHeader(MSGTYPE_WSI_LINK_ATTACH_GLL, ANSWER);
    
    wsiPacket_->addTLV(ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);
    memset(aux_,'\0',MAX_OCTEC_STRING);
    memcpy(aux_,(iface_->m_networkID).c_str(), (iface_->m_networkID).size());
    wsiPacket_->addTLV(ParaTAG_NetworkID, (u_paraValue_t *) aux_);

    length_ = wsiPacket_ -> serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT WSI_LinkAttach.Answer");
    sendToWRAPPER(msg_buf, length_);

    DBG_OUT("gllIM::sendLinkAttachAnswerToWrapper");
}


/*void gllIM::sendRTSockInformationToWrapper(u_int8_t *buf_, int buf_len_)
{
    WSI_packet *wsiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;

    DBG_IN("gllIM::sendRTSockInformationToWrapper");

    wsiPacket_ = new WSI_packet;

    wsiPacket_->setHeader(MSGTYPE_WSI_RTSOCKINFO_GLL, INDICATION);
    wsiPacket_->addTLV(WSI_ParaTAG_RTSockInfo, (u_paraValue_t *) buf_, buf_len_);

    length_ = wsiPacket_->serialize(msg_buf, MAX_PKT_SIZE);
    sendToWRAPPER(msg_buf, length_);

    DBG_OUT("gllIM::sendRTSockInformationToWrapper");
    
}
*/

int gllIM::processARILinkReport(ARI_message *ariMessage_)
{
    int ret_;
    u_int8_t aux_[MAX_OCTEC_STRING];
    int length_;
    struct interfaceGLLim *iface_;
    string device_;
    u_int16_t linkQual_;
    
    DBG_IN("gllIM::processARILinkReport");

    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) aux_, &length_);
    device_ = (char *) aux_;

    if(iface_ = searchInterface(device_)) {
	ariMessage_->getDataFromTLV(ARI_ParaTAG_LinkQual, (u_paraValue_t *) &linkQual_, &length_);
	DBG_INFO("LINK QUAL at GLL IM is %u", linkQual_);

	sendLinkReportIndicationToMRRM(iface_, linkQual_);
	ret_ = 0;
    } else {
	DBG_ERR("GLL IM was not aware of the device");
	ret_ = -1;
    }

    DBG_OUT("gllIM::processARILinkReport");
    return ret_;
}

/*
int gllIM::processARIRTSockInformation(ARI_message *ariMessage_)
{
    int ret_;
    u_int8_t aux_[MAX_CHUNK_SIZE];
    int length_;
 
    DBG_IN("gllIM::processARIRTSockInformation");

    memset(aux_, '\0', MAX_CHUNK_SIZE);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_RTSockInfo, (void *) aux_, &length_);

    if(length_ > 0) {
	sendRTSockInformationToWrapper(aux_, length_);
	ret_ = 0;
    } else {
	ret_ = -2;
    }

    DBG_OUT("gllIM::processARIRTSockInformation");
    return ret_;
}
*/

void gllIM::onEvent(struct eventMessage *message)
{
    DBG_IN("gllIM::onEvent");

    switch(message->code) {
    case EVT_MGI_RX_IM:
	handleMGI_Rx();
	break;
    case EVT_ARI_RX_IM:
	handleARI_Rx();
	break;
    default:
	DBG_ERR("Unrecognized code");
	break;
    }
    
    DBG_OUT("gllIM::onEvent");
}

void gllIM::onTimeout(struct timerMessage *message)
{
    struct interfaceGLLim *iface_;

    DBG_IN("gllIM::onTimeout");

    switch(message->code) {
    case TIMER_DEVREG_RESPONSE:
	iface_ = (struct interfaceGLLim *) message->data;
	break;
    case TIMER_CANDIDATEREPORT_INDICATION:
	iface_ = (struct interfaceGLLim *) message->data;
	DBG_INFO("Timer expired, get SNR of device %s",iface_->m_identifier);
	sendCandidateReportToGLLal(iface_->m_identifier);
	break;
    case TIMER_WAITING_LINKATTACH:
	iface_ = (struct interfaceGLLim *) message->data;
	unsuccessAttach(iface_);
	break;
    case TIMER_LINKREPORT:
	iface_ = (struct interfaceGLLim *) message->data;
	sendGetLinkQualityToGLLal(iface_);
	break;
    default:
	DBG_ERR("Unrecognized code -- %X",message->code);
	break;
    }

    DBG_OUT("gllIM::onTimeout");
}

void gllIM::sendLinkQualityToTRG(u_int8_t snr_)
{
    ANTrigger trigger;
    bool succeeded;
    char snrValue[10];

    DBG_IN("gllIM::sendLinkQualToTRG");

    memset(snrValue, '\0',10);
    sprintf(snrValue,"%d",snr_);
    trigger.setID(200);
    trigger.setValue(snrValue);
    trigger.setType(100);
    
    succeeded = m_producer.send( trigger );
    if ( !succeeded ) {
	DBG_ERR("Couldn't send trigger to Triggering framework");
	DBG_ERR("%s",m_producer.getError().c_str());
    }
    
    DBG_OUT("gllIM::sendLinkQualToTRG");
}

void gllIM::registerNewDeviceToTRG(u_int8_t devID_[MAX_OCTEC_STRING])
{
    ANTrigger trigger;
    bool succeeded;
    
    DBG_IN("gllIM::registerNewDeviceToTRG");

    trigger.setID(200);
    trigger.setValue((const char*) devID_);
    trigger.setType(101);
    
    succeeded = m_producer.send( trigger );
    if ( !succeeded ) {
	DBG_ERR("Couldn't send trigger to Triggering framework");
	DBG_ERR("%s",m_producer.getError().c_str());
    }
    
    DBG_OUT("gllIM::registerNewDeviceToTRG");
}

void gllIM::deregisterDeviceToTRG(u_int8_t devID_[MAX_OCTEC_STRING]) 
{
    ANTrigger trigger;
    bool succeeded;
    
    DBG_IN("gllIM::deregisterDeviceToTRG");

    trigger.setID(200);
    trigger.setValue((const char*) devID_);
    trigger.setType(102);
    
    succeeded = m_producer.send( trigger );
    if ( !succeeded ) {
	DBG_ERR("Couldn't send trigger to Triggering framework");
	DBG_ERR("%s",m_producer.getError().c_str());
    }
    
    DBG_OUT("gllIM::deregisterDeviceToTRG");

}

void gllIM::sendCandidateReportToGLLal(string devID_)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];

    DBG_IN("gllIM::sendCandidateReportToGLLal");

    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_CANDIDATE_REPORT, ARI_FLAGS_REQUEST);
	
    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, devID_.c_str(), (int) devID_.size());
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);
    
    buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_CandidateReport.Request");
    sendToGLL_al(msg_buf, buf_len);

    DBG_OUT("gllIM::sendCandidateReportToGLLal");
}

void gllIM::sendGetLinkQualityToGLLal(struct interfaceGLLim *iface_)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];

    DBG_IN("gllIM::SendGetLinkQualityToGLLal");

    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_LINK_REPORT, ARI_FLAGS_REQUEST);
	
    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);
    
    buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_LinkReport.Request");
    sendToGLL_al(msg_buf, buf_len);

    DBG_OUT("gllIM::SendGetLinkQualityToGLLal");
}

void gllIM::sendConnectNetworkToGLLal(string device_, string networkID_)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];
    
    DBG_IN("gllIM::sendConnectNetworkToGLLal");

    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_LINK_ATTACH, ARI_FLAGS_REQUEST);
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, device_.c_str(), (int) device_.size());
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, networkID_.c_str(), (int) networkID_.size());
    ariMessage->addTLV(ARI_ParaTAG_NetworkID, (u_paraValue_t *) aux_);

    buf_len= ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_LinkAttach.Request");
    sendToGLL_al(msg_buf, buf_len);

    DBG_OUT("gllIM::sendConnectNetworkToGLLal");
}

void gllIM::sendDisconnectNetworkToGLLal(string device_)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];
    
    DBG_IN("gllIM::sendConnectNetworkToGLLal");

    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_LINK_DETACH, ARI_FLAGS_REQUEST);
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, device_.c_str(), (int) device_.size());
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);
    
    buf_len= ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_LinkDetach.Request");
    sendToGLL_al(msg_buf, buf_len);

    DBG_OUT("gllIM::sendDisconnectNetworkToGLLal");
}

void gllIM::sendToGLL_al(u_int8_t *msg_buf, int buf_len)
{
    struct sockaddr_in gllIM_addr;
    
    DBG_IN("gllIM::sendToGLL_al");
    
    gllIM_addr.sin_family = PF_INET;
    gllIM_addr.sin_port = htons(m_ARI_ALport);
    gllIM_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    DBG_INFO2("Sending info to GLL_al ...");
    DBG_DUMP("ARI",msg_buf, buf_len);
    if((sendto(ARI_IMsocket, msg_buf, buf_len, 0, (struct sockaddr *)&gllIM_addr, sizeof(struct sockaddr_in))) < 0) {
	DBG_ERR("Could not send message to GLL AL");
	DBG_INFO("errno = %s",strerror(errno));
    }

    DBG_OUT("gllIM::sendToGLL_al");
}

void gllIM::sendCandidateReportAnswerToMRRM(struct interfaceGLLim *iface_, u_int8_t result_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int32_t resultCode_;
    
    DBG_IN("gllIM::sendCandidateReportAnswerToMRRM");

    mgiPacket_ = new MGI_packet;
    
    mgiPacket_->setHeader(MSGTYPE_CANDIDATE_REPORT, ANSWER);
    

    mgiPacket_->addTLV(ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);
    resultCode_ = 0 | result_;
    mgiPacket_->addTLV(ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_);
    
    length_ = mgiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT MGI_CandidateReport.Answer");
    DBG_INFO("\t- ResultCode = %x",resultCode_);
    sendToMRRM(msg_buf, length_);
    
    DBG_OUT("gllIM::sendCandidateReportAnswerToMRRM");
}

void gllIM::sendCandidateReportIndicationToMRRM(struct interfaceGLLim *iface_, struct candidateReportIndication &info_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    char aux_[MAX_OCTEC_STRING];

    DBG_IN("gllIM::sendCandidateReportIndicationToMRRM");

    mgiPacket_ = new MGI_packet;
    
    mgiPacket_->setHeader(MSGTYPE_CANDIDATE_REPORT, INDICATION);
    
    mgiPacket_->addTLV(ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);

    memset(aux_,'\0', MAX_OCTEC_STRING);
    memcpy(aux_, info_.m_networkID.c_str(), info_.m_networkID.size());
    //DBG_DUMP("AUX", aux_, MAX_OCTEC_STRING);
    mgiPacket_->addTLV(ParaTAG_NetworkID, (u_paraValue_t *) aux_);

    memset(aux_,'\0', MAX_OCTEC_STRING);
    memcpy(aux_, info_.m_cellID, CELLID_LEN);
    mgiPacket_->addTLV(ParaTAG_CellID, (u_paraValue_t *) aux_, CELLID_LEN);

    mgiPacket_->addTLV(ParaTAG_AccessTechnologyType, (u_paraValue_t *) &(iface_->m_techType));

    mgiPacket_->addTLV(ParaTAG_MeasValueReport);
    mgiPacket_->addTLVToGrouped(ParaTAG_MeasValueReport, ParaTAG_MeasValueSignalStrength, (u_paraValue_t *) &(info_.m_rssi));

    length_ = mgiPacket_->serialize(msg_buf, MAX_PKT_SIZE);
    
    DBG_INFO("SENT MGI_CandidateReport.Indication");
    sendToMRRM(msg_buf, length_);

    DBG_OUT("gllIM::sendCandidateReportIndicationToMRRM");
}

void gllIM::sendLinkAttachAnswerToMRRM(struct interfaceGLLim *iface_, u_int8_t result_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int32_t resultCode_;
    
    DBG_IN("gllIM::sendLinkAttachAnswerToMRRM");

    mgiPacket_ = new MGI_packet;
    
    mgiPacket_->setHeader(MSGTYPE_LINK_ATTACH, ANSWER);
    
    mgiPacket_->addTLV(ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);
    resultCode_ = 0 | result_;
    mgiPacket_->addTLV(ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_);

    DBG_INFO("Local Link ID = %u",iface_->m_localLinkID);
    mgiPacket_->addTLV(ParaTAG_LocalLinkID, (u_paraValue_t *) &(iface_->m_localLinkID));

    length_ = mgiPacket_ -> serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT MGI_LinkAttach.Answer");
    sendToMRRM(msg_buf, length_);

    DBG_OUT("gllIM::sendLinkAttachAnswerToMRRM");
}

void gllIM::sendLinkReportAnswerToMRRM(struct interfaceGLLim *iface_, u_int8_t result_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    u_int32_t resultCode_;
    
    DBG_IN("gllIM::sendLinkReportAnswerToMRRM");

    mgiPacket_ = new MGI_packet;
    
    mgiPacket_->setHeader(MSGTYPE_LINK_REPORT, ANSWER);
    
    mgiPacket_->addTLV(ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);
    resultCode_ = 0 | result_;
    mgiPacket_->addTLV(ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_);

    DBG_INFO("Local Link ID = %u",iface_->m_localLinkID);
    mgiPacket_->addTLV(ParaTAG_LocalLinkID, (u_paraValue_t *) &(iface_->m_localLinkID));

    length_ = mgiPacket_ -> serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("SENT MGI_LinkReport.Answer");
    sendToMRRM(msg_buf, length_);

    DBG_OUT("gllIM::sendLinkReportAnswerToMRRM");
}

void gllIM::sendLinkReportIndicationToMRRM(struct interfaceGLLim *iface_, u_int16_t rssi_)
{
    MGI_packet *mgiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;
    
    DBG_IN("gllIM::sendLinkReportIndicationToMRRM");

    mgiPacket_ = new MGI_packet;
    
    mgiPacket_->setHeader(MSGTYPE_LINK_REPORT, INDICATION);
    
    mgiPacket_->addTLV(ParaTAG_DeviceID, (u_paraValue_t *) iface_->m_identifier);

    DBG_INFO("Local Link ID = %u",iface_->m_localLinkID);
    mgiPacket_->addTLV(ParaTAG_LocalLinkID, (u_paraValue_t *) &(iface_->m_localLinkID));

    mgiPacket_->addTLV(ParaTAG_MeasValueReport);
    mgiPacket_->addTLVToGrouped(ParaTAG_MeasValueReport, ParaTAG_MeasValueSignalStrength, (u_paraValue_t *) &rssi_);
    
    length_ = mgiPacket_ -> serialize(msg_buf, MAX_PKT_SIZE);
    DBG_INFO("SENT MGI_LinkReport.Indication");
    sendToMRRM(msg_buf, length_);

    DBG_OUT("gllIM::sendLinkReportIndicationToMRRM");

}

int gllIM::openMGI_socket(void)
{
    struct sockaddr_in sockAddr_;
    int ret_;

    DBG_IN("gllIM::openMGI_socket");

    ret_ = 0;
    MGI_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if(MGI_socket < 0) {
	DBG_ERR("Could not create socket for MGI interface");
	ret_ = -1;
    } else {

	memset(&sockAddr_, 0, sizeof(struct sockaddr_in));

	sockAddr_.sin_family = PF_INET;
	sockAddr_.sin_port = htons(m_MGIport);
	sockAddr_.sin_addr.s_addr = INADDR_ANY;
	
	if((bind(MGI_socket, (struct sockaddr *) &sockAddr_, sizeof(struct sockaddr_in))) < 0) {
	    DBG_ERR("Could not bind MGI_socket");
	    ret_  = -1;
	}
	
	DBG_INFO("MGI port %d is open", m_MGIport);
    }
    
    DBG_OUT("gllIM::openMGI_socket");
    return ret_;
}

int gllIM::openARI_socket(void)
{
    struct sockaddr_in sockAddr_;
    int ret_;

    DBG_IN("gllIM::openARI_socket");

    ret_ = 0;
    ARI_IMsocket = socket(PF_INET, SOCK_DGRAM, 0);
    if(ARI_IMsocket < 0) {
	DBG_ERR("Could not create socket for MGI interface");
	ret_ = -1;
    } else {
	memset(&sockAddr_, 0, sizeof(struct sockaddr_in));

	sockAddr_.sin_family = PF_INET;
	sockAddr_.sin_port = htons(m_ARI_IMport);
	sockAddr_.sin_addr.s_addr = INADDR_ANY;
	
	if((bind(ARI_IMsocket, (struct sockaddr *) &sockAddr_, sizeof(struct sockaddr_in))) < 0) {
	    DBG_ERR("Could not bind ARI_IM socket");
	    ret_  = -1;
	}
	DBG_INFO("ARI_IM port %d is open", m_ARI_IMport);	
    }
    
    DBG_OUT("gllIM::openARI_socket");
    return ret_;
}

void gllIM::configureReporting(struct interfaceGLLim *iface_)
{
    struct measConfiguration *measConfig_;

    DBG_IN("gllIM::configureReporting");

    measConfig_ = &(iface_->m_measConfiguration);

    DBG_INFO2("Configuring to measure %X, every %d milliseconds",measConfig_->MeasurementParaType, measConfig_->ReportingPeriodicity);
    if(measConfig_->ReportingPeriodicity > 0) {
	switch(iface_->m_state) {
	case REGISTERED:
	    DBG_INFO("Report every %d msec from %s", measConfig_->ReportingPeriodicity, iface_->m_identifier);
	    scheduler::instance()->insertTimer(TIMER_CANDIDATEREPORT_INDICATION, measConfig_->ReportingPeriodicity, this, true, iface_);
	    break;
	case ATTACHED:
	    DBG_INFO("Report every %d msec from %s", measConfig_->ReportingPeriodicity, iface_->m_identifier);
	    scheduler::instance()->insertTimer(TIMER_LINKREPORT,measConfig_->ReportingPeriodicity, this, true, iface_);
	    break;
	default:
	    DBG_INFO("There is not any reason to create a timer");
	    break;
	}
    }

    DBG_OUT("gllIM::configureReporting");
}

void gllIM::sendToMRRM(u_int8_t *msg_buf, int buf_len)
{
    struct sockaddr_in mrrmAddr;
    
    DBG_IN("gllIM::sendToMRRM");
    
    mrrmAddr.sin_family = PF_INET;
    mrrmAddr.sin_port = htons(m_mrrm_port);
    mrrmAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    DBG_DUMP("SEND",msg_buf,buf_len);
    if((sendto(MGI_socket, msg_buf, buf_len, 0, (struct sockaddr *)&mrrmAddr, sizeof(struct sockaddr_in))) < 0) {
	DBG_ERR("Could not send message to MRRM");
    }

    DBG_OUT("gllIM::sendToMRRM");
}

void gllIM::sendToWRAPPER(u_int8_t *msg_buf, int buf_len)
{
    struct sockaddr_in wrapperAddr;
    
    DBG_IN("gllIM::sendToWRAPPER");
    
    wrapperAddr.sin_family = PF_INET;
    wrapperAddr.sin_port = htons(m_wrapper_port);
    wrapperAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    DBG_DUMP("SEND",msg_buf,buf_len);
    if((sendto(MGI_socket, msg_buf, buf_len, 0, (struct sockaddr *) &wrapperAddr, sizeof(struct sockaddr_in))) < 0) {
	DBG_ERR("Could not send message to WRAPPER");
    }

    DBG_OUT("gllIM::sendToWRAPPER");
}


struct interfaceGLLim *gllIM::searchInterface(string &device_)
{
    iface_iter_t iter_;
    struct interfaceGLLim *ret_;

    DBG_IN("gllIM::searchInterface");
    
    iter_ = m_interfaces.find(device_);
    if(iter_ != m_interfaces.end()) {
	ret_ = iter_->second;
    } else {
	ret_ = NULL;
    }
	
    DBG_OUT("gllIM::searchInterface");
    return ret_;
}

void gllIM::deleteInterface(string &device_) 
{
    iface_iter_t iter_;
    struct interfaceGLLim *entry_;

    DBG_IN("gllIM::deleteInterface");

    iter_ = m_interfaces.find(device_);
    if(iter_ != m_interfaces.end()) {
	entry_ = iter_->second;
	scheduler::instance()->deleteTimer(TIMER_DEVDEREG_RESPONSE, this, entry_);
	delete entry_;
    } 

    DBG_OUT("gllIM::deleteInterface");
}

void gllIM::unsuccessAttach(struct interfaceGLLim *iface_)
{
    

    DBG_IN("gllIM::unsuccessAttach");



    DBG_OUT("gllIM::unsuccessAttach");
}
