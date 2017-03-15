/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: gll_al.cc
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if defined(__linux__)
#include <linux/if.h>
#elif defined(__FreeBSD__)
#include <net/ethernet.h>
#endif

#include <errno.h>

#include "gll_al.h"

#define DBG_LEVEL 2
#include "debug.h"

gllAL::gllAL(void) 
{
    DBG_IN("gllAL::gllAL");
    
    m_ifaceTable = new InterfaceTable;

    DBG_OUT("gllAL::gllAL");
}

void gllAL::start(void)
{
    DBG_IN("gllAL::start");

    scheduler::instance()->insertEvent(EVT_NEW_IFACE, this, NULL);
    scheduler::instance()->insertEvent(EVT_DELETE_IFACE, this, NULL);
    scheduler::instance()->insertTimer(TIMER_CHECK_IFACES,5000,this);

    if(!openARI_socket()) {
	scheduler::instance()->insertInput(EVT_ARI_RX_AL, ARI_ALsocket, this, NULL);
    }

    /*    if(!openNetlink_socket()) {
	scheduler::instance()->insertInput(EVT_RTSOCK_RX, m_netlink_socket, this, NULL);
	}*/

    DBG_OUT("gllAL::start");
}

int gllAL::openARI_socket(void)
{
    struct sockaddr_in sockAddr_;
    int ret_;

    DBG_IN("gllAL::openARI_socket");

    ret_ = 0;
    ARI_ALsocket = socket(PF_INET, SOCK_DGRAM, 0);
    if(ARI_ALsocket < 0) {
	DBG_ERR("Could not create socket for ARI interface");
	ret_ = -1;
    } else {
	memset(&sockAddr_, 0, sizeof(struct sockaddr_in));
	
	sockAddr_.sin_family = PF_INET;
	sockAddr_.sin_port = htons(m_ARI_ALport);
	sockAddr_.sin_addr.s_addr = INADDR_ANY;
	
	if((bind(ARI_ALsocket, (struct sockaddr *) &sockAddr_, sizeof(struct sockaddr_in))) < 0) {
	    DBG_ERR("Could not bind MGI_socket");
	    ret_  = -1;
	}
	DBG_INFO("ARI_AL port %d is open", m_ARI_ALport);
    }

    DBG_OUT("gllAL::openARI_socket");
    return ret_;
}

/*
int gllAL::openNetlink_socket(void) 
{
    int ret_;
    
    DBG_IN("gllAL::openNetLink_socket");

    m_netlink_socket = socket(PF_ROUTE, SOCK_RAW, AF_UNSPEC);
    if(m_netlink_socket < 0) {
	DBG_ERR("Could not create the netlink socket");
	ret_ = -1;
    } else {
	ret_ = 0;
    }

    DBG_OUT("gllAL::openNetLink_socket");
    return ret_;
    }*/

void gllAL::registerNewDeviceToGLLim(NetInterface *iface)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];
    u_int32_t aux32_;
    string deviceID_;
    
    DBG_IN("gllAL::registerNewDeviceToGLLim");
    
    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_NEW_DEVICE, ARI_FLAGS_REQUEST);
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, (iface->getIFname()).c_str(), (iface->getIFname()).length());
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);
    deviceID_ = (char *) aux_;
    DBG_INFO("Iface name: %s", deviceID_.c_str());

    memset(aux_, '\0', MAX_OCTEC_STRING);
#if defined(__linux__)
    memcpy(aux_, iface->getHWaddr(), IFHWADDRLEN);
    ariMessage->addTLV(ARI_ParaTAG_MACaddr, (u_paraValue_t *) aux_, IFHWADDRLEN);
#elif defined(__FreeBSD__)
    memcpy(aux_, iface->getHWaddr(), ETHER_ADDR_LEN);
    DBG_DUMP("MAC Addr",aux_,ETHER_ADDR_LEN);
    ariMessage->addTLV(ARI_ParaTAG_MACaddr, (u_paraValue_t *) aux_, ETHER_ADDR_LEN);
#endif

    aux32_ = (u_int32_t) iface->getTechType();
    DBG_INFO("Technology type = %x",aux32_);
    ariMessage->addTLV(ARI_ParaTAG_AccessTechType, (u_paraValue_t *) &aux32_);

    buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_NewDevice.Request");
    sendToGLL_im(msg_buf, buf_len);
    
    DBG_OUT("gllAL::registerNewDeviceToGLLim");
}

void gllAL::deregisterDeviceToGLLim(NetInterface *iface)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];

    DBG_IN("gllAL::deregisterDeviceToGLLim");

    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_DELETE_DEVICE, ARI_FLAGS_REQUEST);
    
    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, (iface->getIFname()).c_str(), (iface->getIFname()).length());
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);
    
    buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_DeleteDevice.Request");
    sendToGLL_im(msg_buf, buf_len);

    m_ifaceTable->deleteInterface(iface->getIFname());

    DBG_OUT("gllAL::deregisterDeviceToGLLim");
}

void gllAL::sendToGLL_im(u_int8_t *msg_buf, int buf_len)
{
    struct sockaddr_in gllIM_addr;
    
    DBG_IN("gllAL::sendToGLL_im");
    
    gllIM_addr.sin_family = PF_INET;
    gllIM_addr.sin_port = htons(m_ARI_IMport);
    gllIM_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    DBG_INFO2("Sending info to GLL_im ...");
    DBG_DUMP("ARI",msg_buf, buf_len);
    if((sendto(ARI_ALsocket, msg_buf, buf_len, 0, (struct sockaddr *)&gllIM_addr, sizeof(struct sockaddr_in))) < 0) {
	DBG_ERR("Could not send message to GLL_im");
	DBG_INFO("errno = %s",strerror(errno));
    }

    DBG_OUT("gllAL::sendToGLL_im");
    
}

void gllAL::onEvent(struct eventMessage *message)
{
    DBG_IN("gllAL::onEvent");

    switch(message->code) {
    case EVT_NEW_IFACE:
	DBG_INFO("New interface available");
	registerNewDeviceToGLLim((NetInterface *) message->result);
	break;
    case EVT_DELETE_IFACE:
	DBG_INFO("Deleting Interface");
	deregisterDeviceToGLLim((NetInterface *) message->result);
	break;
    case EVT_ARI_RX_AL:
	handleARI_Rx();
	break;
	/*    case EVT_RTSOCK_RX:
	handleRTSock_Rx();
	break;*/
    default:
	DBG_ERR("Unrecognized code - %d",message->code);
	break;
    }
    
    DBG_OUT("gllAL::onEvent");
}

void gllAL::handleARI_Rx(void)
{
    char msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    struct sockaddr_in src;
    int sockaddr_len;
    ARI_message *rxPacket;
    
    DBG_IN("gllALL::handleARI_Rx");

    sockaddr_len = sizeof(struct sockaddr_in);
	
    buf_len = recvfrom(ARI_ALsocket, msg_buf, ARI_MAX_PKT_SIZE, 0, (struct sockaddr *) &src, (socklen_t *) &sockaddr_len);
    //DBG_DUMP("RX",msg_buf,buf_len);

    rxPacket = new ARI_message((u_int8_t *)msg_buf, buf_len);
    switch(rxPacket->type()) {
    case ARI_CANDIDATE_REPORT:
	DBG_INFO("RCVD ARI_CandidateReport");
	DBG_DUMP("RX",msg_buf,buf_len);
	processARICandidateReport(rxPacket);
	break;
    case ARI_LINK_ATTACH:
	DBG_INFO("RCVD ARI_LinkAttach");
	DBG_DUMP("RX",msg_buf,buf_len);
	processARILinkAttach(rxPacket);
	break;
    case ARI_LINK_DETACH:
	DBG_INFO("RCVD ARI_LinkDetach");
	DBG_DUMP("RX",msg_buf,buf_len);
	processARILinkDetach(rxPacket);
	break;
    case ARI_LINK_REPORT:
	DBG_INFO("RCVD ARI_LinkReport");
	DBG_DUMP("RX",msg_buf,buf_len);
	processARILinkReport(rxPacket);
	break;
    default:
	DBG_ERR("Unknown packet type");
	break;
    }

    DBG_OUT("gllAL::handleARI_Rx");
}

int gllAL::processARICandidateReport(ARI_message *ariMessage_)
{
    int ret_, i;
    u_int8_t devID_[MAX_OCTEC_STRING];
    string deviceID_;
    int length_;
    vector<struct candidateEntry *> detectedSet_;
    struct candidateEntry *currentEntry_;
    
    DBG_IN("gllAL::processARICandidateReport");

    memset(devID_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) devID_, &length_);

    deviceID_ = (char *) devID_;
    
    m_ifaceTable->getDetectedSet(deviceID_, detectedSet_);

    for( i = 0 ; i < (signed) detectedSet_.size() ; i++) {
	currentEntry_ = detectedSet_[i];
	deliverCandidateReportToGLLim(deviceID_, currentEntry_);
    }

    DBG_OUT("gllAL::processARICandidateReport");
    return ret_;
}

int gllAL::processARILinkAttach(ARI_message *ariMessage_)
{
    int ret_;
    u_int8_t aux_[MAX_OCTEC_STRING];
    string deviceID_, networkID_;
    int length_;
    NetInterface *iface_;
    
    DBG_IN("gllAL::processARILinkAttach");

    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) aux_, &length_);
    deviceID_ = (char *) aux_;

    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_NetworkID, (void *) aux_, &length_);
    networkID_ = (char *) aux_;

    ret_ = m_ifaceTable->connectToNetwork(deviceID_, networkID_, &iface_);

    if(!ret_) {
	// The command was a success, I've to check in some time 
	// that the interface was succcessfully associated
	scheduler::instance()->insertTimer(TIMER_CHECK_IFACE_ASSOCIATED, 9000, this, false, iface_);
    } else {
	DBG_ERR("Could not send the connect command to the interface");
	// tbd... Sending a error message to GLL_IM
    }

    DBG_OUT("gllAL::processARILinkAttach");
    return ret_;
}

int gllAL::processARILinkDetach(ARI_message *ariMessage_)
{
    int ret_;
    u_int8_t aux_[MAX_OCTEC_STRING];
    string deviceID_;
    int length_;
    NetInterface *iface_;
    
    DBG_IN("gllAL::processARILinkDetach");

    memset(aux_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) aux_, &length_);
    deviceID_ = (char *) aux_;

    ret_ = m_ifaceTable->disconnectFromNetwork(deviceID_, &iface_);

    if(!ret_) {
	DBG_INFO("Disconnection of %s was a success", deviceID_.c_str());
    } else {
	DBG_ERR("Could not send the connect command to the interface");
	// tbd... Sending a error message to GLL_IM
    }

    DBG_OUT("gllAL::processARILinkDetach");
    return ret_;
}

int gllAL::processARILinkReport(ARI_message *ariMessage_) 
{
    int ret_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    string device_;
    int rssi_;
    int length_;
    NetInterface *iface_;
    
    DBG_IN("gllAL::processARILinkReport");

    memset(devID_, '\0', MAX_OCTEC_STRING);
    ariMessage_->getDataFromTLV(ARI_ParaTAG_DeviceID, (void *) devID_, &length_);
    device_ = (char *) devID_;

    iface_ = m_ifaceTable->getInterface(device_);

    if(iface_) {
	rssi_ = iface_->getLinkQual();
	deliverLinkReportToGLLim(device_,rssi_);
	ret_ = 0;
    } else {
	DBG_ERR("Interface was not known");
	ret_ = -1;
    }

    DBG_OUT("gllAL::processARILinkReport");
    return ret_;
}

/*
void gllAL::handleRTSock_Rx(void)
{
    int len_;
    u_int8_t buf_[BUFSIZE];
    
    DBG_IN("gllAL::handleRTSock_Rx");
    
    len_ = read(m_netlink_socket, buf_,BUFSIZE);
    if(len_ > 0) {
	DBG_INFO("Read something from the netlink socket (%d) bytes", len_);
	deliverRTSockInformationToGLLim(buf_,len_);
    }

    DBG_OUT("gllAL::handleRTSock_Rx");
}
*/

void gllAL::deliverCandidateReportToGLLim(string &deviceID_, struct candidateEntry *currentEntry_)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];

    DBG_IN("gllAL::deliverLinkQualToGLLim");

    ariMessage = new ARI_message;

    ariMessage->setHeader(ARI_CANDIDATE_REPORT,ARI_FLAGS_INDICATION);

    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, deviceID_.c_str(), deviceID_.size());
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);

    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, (currentEntry_->m_networkID).c_str(), (currentEntry_->m_networkID).size());
    ariMessage->addTLV(ARI_ParaTAG_NetworkID, (u_paraValue_t *) aux_);

    memset(aux_, 0, MAX_OCTEC_STRING);
    memcpy(aux_, currentEntry_->m_cellID, IFACEHWADDRLEN);
    ariMessage->addTLV(ARI_ParaTAG_CellID, (u_paraValue_t *) aux_, IFACEHWADDRLEN);

    ariMessage->addTLV(ARI_ParaTAG_LinkQual, (u_paraValue_t *) &(currentEntry_->m_rssi));

    buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_CandidateReport.Indication");
    DBG_INFO("\t- Device ID : %s", deviceID_.c_str());
    sendToGLL_im(msg_buf, buf_len);

    DBG_OUT("gllAL::deliverLinkQualToGLLim");
}

void gllAL::deliverLinkAttachResponseToGLLim(NetInterface *iface_)
{
    int assoc_;
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];
    u_int8_t cellID_[IFACEHWADDRLEN];
    string ssid_;

    DBG_IN("gllAL::deliverLinkAttachResponseToGLLIM");

    assoc_ = iface_->checkAssociated();

    if(assoc_ == 1) {
	// The interface is already associated; send the information to GLL_IM
	ariMessage = new ARI_message;
	
	ariMessage->setHeader(ARI_LINK_ATTACH, ARI_FLAGS_INDICATION);

	memset(aux_, '\0', MAX_OCTEC_STRING);
	memcpy(aux_, (char *) iface_->getIFname().c_str(), iface_->getIFname().size());
	ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);

	memset(aux_, '\0', MAX_OCTEC_STRING);
	memset(cellID_, '\0', IFACEHWADDRLEN);

	DBG_DUMP("SSID",aux_,6);
	iface_->getCurrentConnection(cellID_, aux_);
	ariMessage->addTLV(ARI_ParaTAG_CellID, (u_paraValue_t *) cellID_, IFACEHWADDRLEN);
	ariMessage->addTLV(ARI_ParaTAG_NetworkID, (u_paraValue_t *) aux_);

	buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
	DBG_INFO("SENT ARI_LinkAttach.Indication");
	sendToGLL_im(msg_buf, buf_len);

    } else if(assoc_ == 0) {
	DBG_INFO2("The interface did not have the time to associate");
    } else {
	DBG_ERR("There was some error while checking the association");
    }

    DBG_OUT("gllAL::deliverLinkAttachResponseToGLLIM");
}

void gllAL::deliverLinkReportToGLLim(string &device_, int rssi_)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;
    u_int8_t aux_[MAX_OCTEC_STRING];
    u_int16_t linkQual_;

    DBG_IN("gllAL::deliverLinkReportToGLLim");

    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_LINK_REPORT, ARI_FLAGS_INDICATION);

    memset(aux_, '\0', MAX_OCTEC_STRING);
    memcpy(aux_, (char *) device_.c_str(), device_.size());
    ariMessage->addTLV(ARI_ParaTAG_DeviceID, (u_paraValue_t *) aux_);
    
    linkQual_ = (u_int16_t) rssi_;
    ariMessage->addTLV(ARI_ParaTAG_LinkQual, (u_paraValue_t *) &linkQual_);

    buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    DBG_INFO("SENT ARI_LinkReport.Indication");
    sendToGLL_im(msg_buf, buf_len);

    DBG_OUT("gllAL::deliverLinkReportToGLLim");

}

/*
void gllAL::deliverRTSockInformationToGLLim(u_int8_t *buf_, int len_)
{
    ARI_message *ariMessage;
    u_int8_t msg_buf[ARI_MAX_PKT_SIZE];
    int buf_len;

    DBG_IN("gllAL::deliverRTSockInformationToGLLim");

    ariMessage = new ARI_message;
    
    ariMessage->setHeader(ARI_RTSOCK_INFO, ARI_FLAGS_INDICATION);
    
    ariMessage->addTLV(ARI_ParaTAG_RTSockInfo, (u_paraValue_t *) buf_, len_);

    buf_len = ariMessage->serialize(msg_buf, ARI_MAX_PKT_SIZE);
    sendToGLL_im(msg_buf, buf_len);

    DBG_OUT("gllAL::deliverRTSockInformationToGLLim");
}
*/

void gllAL::onTimeout(struct timerMessage *message)
{
    NetInterface *iface_;
    
    DBG_IN("gllAL::onTimeout");

    switch(message->code) {
    case TIMER_CHECK_IFACES:
	m_ifaceTable->checkInterfaces();
	break;
    case TIMER_CHECK_IFACE_ASSOCIATED:
	iface_ = (NetInterface *) message->data;
	deliverLinkAttachResponseToGLLim(iface_);
	break;
    default:
	break;
    }

    DBG_OUT("gllAL::onTimeout");

}


