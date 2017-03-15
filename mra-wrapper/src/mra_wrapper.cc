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
#include <resolv.h>

#include <errno.h>

#include "wsi_packet.h"
#include "wsi.h"
#include "mra_wrapper.h"

#include "ANifaces.nsmap"

#include "ANifacesRegistrationComponentSoapBindingProxy.h"
#include "ANifacesLookupComponentSoapBindingProxy.h"
#include "ANifacestrg_USCOREproducerProxy.h"

#include "WSwrapper.nsmap"
#include "WSwrapperMRAservicesObject.h"

#define DBG_LEVEL 2
#include "debug.h"

struct soap soap;

MRAwrapper::MRAwrapper(void)
{
    DBG_IN("MRAwrapper::MRAwrapper");
    

    DBG_OUT("MRAwrapper::MRAwrapper");
}

void MRAwrapper::start(void) 
{
    string trgFullName;
    struct trigger MHItrigger_;
    struct trigger MHI_HOexec_;

    DBG_IN("MRAwrapper::start");

    soap_init(&soap);

    if(!openMRAwrapper_socket()) {
	scheduler::instance()->insertInput(EVT_WSWRAPPER_RX,m_wrapperSocket, this, NULL);
    }

    trgFullName = m_trgANid + ".TRG.Producer";

    DBG_INFO("Resolving URI for %s", trgFullName.c_str());

    /* Quitar comentario cuando funcione LookupURI */
    //m_trgProdURI = LookupURI(trgFullName);
    //DBG_INFO("Result of lookupURI is %s", m_trgProdURI.c_str());


    /* Quitar comentario cuando funcione TRG */
    /*MHItrigger_.triggerId = m_MHItriggerID;
    DBG_INFO("Subscribing MHI_Trigger with producer ID %d", MHItrigger_.triggerId);
    subscribeTRGproducer(MHItrigger_.triggerId, m_trgProdURI);

    MHI_HOexec_.triggerId = m_MHI_HOexecID;
    DBG_INFO("Subscribing MHI_HOexecution with producer ID %d", MHI_HOexec_.triggerId);
    subscribeTRGproducer(MHI_HOexec_.triggerId, m_trgProdURI);
    */

    /*    
    { // Only por testing
	struct trigger testTrg;
	string trgFullName;

	testTrg.triggerId = 6666;
	testTrg.type = 6666;
	testTrg.value = 6666;

	trgFullName = m_trgANid + ".TRG.Producer";

	DBG_INFO("Resolving URI for %s", trgFullName.c_str());

	m_trgProdURI = LookupURI(trgFullName);

	DBG_INFO("Result of lookupURI for %s: %s",
		 trgFullName.c_str(),
		 m_trgProdURI.c_str());

	subscribeTRGproducer(testTrg.triggerId, m_trgProdURI);
	sendTriggering(testTrg, m_trgProdURI);
    }
    */

    DBG_OUT("MRAwrapper::start");
}

void MRAwrapper::startServer(void) 
{
    int m,s;

    DBG_IN("MRAwrapper::startServer");

    DBG_INFO("WS server-side is running...");

    WSwrapper::MRAservices wsServer;
    
    m = wsServer.bind("localhost", 8081, 100);
    if (m < 0) {
	DBG_ERR("Error wsServer.bind");
    }
    else {
	DBG_INFO("Socket connection successful: master socket = %d", m);
	for (int i = 1; ; i++) {
	    s = wsServer.accept(); // !!Waiting for soap request!!
	    if (s < 0) {
		DBG_ERR("Error wsServer.accept");
		break;
	    }
	    DBG_INFO("Connection OK");
	    if (wsServer.serve() != SOAP_OK) // process RPC request
		DBG_ERR("Error wsServer.serve");
	    DBG_INFO("request served");
	}
    }
    
    DBG_OUT("MRAwrapper::startServer");
}

void MRAwrapper::handleMRAwrapper_Rx(void)
{
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int buf_len;
    struct sockaddr_in src;
    int sockaddr_len;
    WSI_packet *rxPacket;

    DBG_IN("MRAwrapper::handleMRAwrapper_Rx");

    sockaddr_len = sizeof(struct sockaddr_in);

    buf_len = recvfrom(m_wrapperSocket, msg_buf, MAX_PKT_SIZE, 0, (struct sockaddr *) &src, (socklen_t *) &sockaddr_len);

    DBG_INFO("Receiving a packet throught the WSI...");
    DBG_DUMP("RX",msg_buf,buf_len);

    rxPacket = new WSI_packet(msg_buf, buf_len);

    switch(rxPacket->nslpID() & 0xFF00) {
      case MSGTYPE_MHI:
	  processWrapper_MHI(rxPacket);
	  break;
      case MSGTYPE_GHI:
	  processWrapper_GHI(rxPacket);
	  break;
      case MSGTYPE_ACSI:
	  processWrapper_ACSI(rxPacket);
	  break;
      default:
	  DBG_ERR("Unknown Packet Type");
	  break;
      };

    DBG_OUT("MRAwrapper::handleMRAwrapper_Rx");

}

void MRAwrapper::processWrapper_MHI(WSI_packet *packet)
{
    DBG_IN("MRAwrapper::processWrapper_MHI");

    switch(packet->nslpID()) {
    case MSGTYPE_MHI_Trigger:
	DBG_INFO("RX MHI_Trigger");
	processMHI_Trigger(packet);
	break;
    case MSGTYPE_MHI_HandoverExecution:
	DBG_INFO("RX MHI_HandoverExecution");
	processMHI_HandoverExecution(packet);
	break;
    case MSGTYPE_MHI_LocatorDeletion:
	DBG_INFO("RX MHI_LocatorDeletion");
	processMHI_LocatorDeletion(packet);
	break;
    default:
	DBG_ERR("Unknown packet type (%0x)",packet->nslpID());
	break;
    }

    DBG_OUT("MRAwrapper::processWrapper_MHI");
}

void MRAwrapper::processMHI_Trigger(WSI_packet *packet)
{
    char buf_[BUFBASE64];
    int buflen_=0;
    //string trgFullName;
    struct trigger MHItrigger_;
    u_int8_t devID_[MAX_OCTEC_STRING];
    string deviceID_;
    int length_;
    u_int32_t linkEventType_;
    u_int32_t accessTechType_;


    DBG_IN("MRAwrapper::processMHI_Trigger");

    memset(devID_, '\0', MAX_OCTEC_STRING);
    packet->getDataFromTLV(WSI_ParaTAG_TargetDeviceID, (u_paraValue_t *) devID_, &length_);
    linkEventType_ = 0;
    packet->getDataFromTLV(WSI_ParaTAG_LinkEventType, (u_paraValue_t *) &linkEventType_, &length_);

    packet->getDataFromTLV(WSI_ParaTAG_AccessTechnologyType, (u_paraValue_t *) &accessTechType_, &length_);

    DBG_INFO("\t- devID_ = %s", devID_);
    DBG_INFO("\t- linkEventtype_ = %0X", linkEventType_);
    DBG_INFO("\t- accessTechType_ = %0X", accessTechType_);

    memset(buf_,'\0', BUFBASE64);

    DBG_INFO("Buf_ is of length %d",buflen_);

    MHItrigger_.triggerId = m_MHItriggerID;
    MHItrigger_.type = m_MHItriggerID;
    MHItrigger_.value = buf_;

    //sendTriggering(MHItrigger_, m_trgProdURI);

    DBG_OUT("MRAwrapper::processMHI_Trigger");
}

void MRAwrapper::processMHI_LocatorDeletion(WSI_packet *packet)
{
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    u_int64_t transID_;
    struct trigger MHI_LOdel_;
    string device_;

    DBG_IN("MRAwrapper::processMHI_LocatorDeletion");

    packet->getDataFromTLV(WSI_ParaTAG_TransactionID, (void *) &transID_, &length_);
    DBG_DUMP("\t- TransactionID = ", &transID_, length_);
    memset(devID_, '\0', MAX_OCTEC_STRING);
    packet->getDataFromTLV(WSI_ParaTAG_TargetDeviceID, (u_paraValue_t *) devID_, &length_);
    device_ = (char *) devID_;

    DBG_INFO("\t- Target devID_ = %s", devID_);

    MHI_LOdel_.triggerId = m_MHI_LOdelID;
    MHI_LOdel_.type = m_MHI_LOdelID;
    MHI_LOdel_.value = device_.c_str();
    //MHI_HOexec_.value = "wlan0"; // temporal hardcode for devID_

    //sendTriggering(MHI_LOdel_, m_trgProdURI);

    /* Missing: Wait for answer from HOLM */

    sendMHI_LocatorDeletionAnswer(transID_, ResultCode_SUCCESS);

    DBG_OUT("MRAwrapper::processMHI_LocatorDeletion");
}

void MRAwrapper::sendMHI_LocatorDeletionAnswer(u_int64_t transID_, u_int32_t resultCode_)
{
    WSI_packet *wsiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;

    DBG_IN("MRAwrapper::sendMHI_LocatorDeletionAnswer");

    DBG_DUMP("send TransID = ", &transID_, 8);
    wsiPacket_ = new WSI_packet;
    wsiPacket_->setHeader(MSGTYPE_MHI_LocatorDeletion, ANSWER);
    wsiPacket_->addTLV(WSI_ParaTAG_TransactionID, (u_paraValue_t *) &transID_);
    wsiPacket_->addTLV(WSI_ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_);
    length_ = wsiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("Sending MHI_LocatorDeletion.Answer to MRRM");
    sendToMRRM(msg_buf, length_);

    DBG_OUT("MRAwrapper::sendMHI_LocatorDeletionAnswer");
}

void MRAwrapper::processMHI_HandoverExecution(WSI_packet *packet)
{
    u_int8_t devID_[MAX_OCTEC_STRING];
    int length_;
    u_int64_t transID_;
    struct trigger MHI_HOexec_;
    string device_;

    DBG_IN("MRAwrapper::processMHI_HandoverExecution");

    packet->getDataFromTLV(WSI_ParaTAG_TransactionID, (void *) &transID_, &length_);
    DBG_DUMP("\t- TransactionID = ", &transID_, length_);
    memset(devID_, '\0', MAX_OCTEC_STRING);
    packet->getDataFromTLV(WSI_ParaTAG_TargetDeviceID, (u_paraValue_t *) devID_, &length_);
    device_ = (char *) devID_;

    DBG_INFO("\t- Target devID_ = %s", devID_);

    MHI_HOexec_.triggerId = m_MHI_HOexecID;
    MHI_HOexec_.type = m_MHI_HOexecID;
    MHI_HOexec_.value = device_.c_str();
    //MHI_HOexec_.value = "wlan0"; // temporal hardcode for devID_

    //sendTriggering(MHI_HOexec_, m_trgProdURI);

    /* Missing: Wait for answer from HOLM */

    sendMHI_HandoverExecutionAnswer(transID_, ResultCode_SUCCESS);

    DBG_OUT("MRAwrapper::processMHI_HandoverExecution");
}

void MRAwrapper::sendMHI_HandoverExecutionAnswer(u_int64_t transID_, u_int32_t resultCode_)
{
    WSI_packet *wsiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;

    DBG_IN("MRAwrapper::sendMHI_HandoverExecutionAnswer");

    DBG_DUMP("send TransID = ", &transID_, 8);
    wsiPacket_ = new WSI_packet;
    wsiPacket_->setHeader(MSGTYPE_MHI_HandoverExecution, ANSWER);
    wsiPacket_->addTLV(WSI_ParaTAG_TransactionID, (u_paraValue_t *) &transID_);
    wsiPacket_->addTLV(WSI_ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_);
    length_ = wsiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    DBG_INFO("Sending MHI_HandoverExecution.Answer to MRRM");
    sendToMRRM(msg_buf, length_);

    DBG_OUT("MRAwrapper::sendMHI_HandoverExecutionAnswer");
}

void MRAwrapper::processWrapper_GHI(WSI_packet *packet)
{
    DBG_IN("MRAwrapper::processWrapper_GHI");

    switch(packet->nslpID()) {
    case MSGTYPE_GHI_LINK_ATTACH_GLL:
	DBG_INFO("RX GHI_LinkAttachGLL");
	processGHI_linkAttach(packet);
	break;
    case MSGTYPE_GHI_RTSOCKINFO_GLL:
	DBG_INFO("RX GHI_RTsockInfoGLL");
	processGHI_RTSockInformation(packet);
	break;
    default:
	DBG_ERR("Unknown packet type (%0x)",packet->nslpID());
	break;
    }

    DBG_OUT("MRAwrapper::processWrapper_GHI");
}

void MRAwrapper::processGHI_linkAttach(WSI_packet *packet)
{
    DBG_IN("MRAwrapper::processGHI_linkAttach");


    DBG_OUT("MRAwrapper::processGHI_linkAttach");
}

void MRAwrapper::processGHI_RTSockInformation(WSI_packet *packet)
{
    char buf_base64_[BUFBASE64];
    struct trigger rtSockTrigger_;
    u_int8_t buf_[MAX_UVALUE_LEN];
    int buf_len_;
    //string m_trgProdURI;
    string trgFullName;

    DBG_IN("MRAwrapper::processGHI_RTSockInformation");
    
    memset(buf_,'\0', MAX_UVALUE_LEN);
    packet->getDataFromTLV(WSI_ParaTAG_RTSockInfo, (void *) buf_, &buf_len_);

    DBG_INFO("Buf_ is of length %d",buf_len_);

    b64_ntop(buf_, buf_len_, buf_base64_, BUFBASE64);

    rtSockTrigger_.triggerId = m_rtsockTriggerID;
    rtSockTrigger_.type = m_rtsockTriggerID;
    rtSockTrigger_.value = buf_base64_;

    trgFullName = m_trgANid + ".TRG.Producer";

    DBG_INFO("Resolving URI for %s", trgFullName.c_str());

    /* Quitar comentario cuando funcione el TRG */
    //m_trgProdURI = LookupURI(trgFullName);

    DBG_INFO("Result of lookupURI for %s: %s",
	     trgFullName.c_str(),
	     m_trgProdURI.c_str());

    subscribeTRGproducer(rtSockTrigger_.triggerId, m_trgProdURI);
    sendTriggering(rtSockTrigger_, m_trgProdURI);

    DBG_OUT("MRAwrapper::processGHI_RTSockInformation");
}

void MRAwrapper::processWrapper_ACSI(WSI_packet *packet)
{
    u_int8_t fe_;
    int length_;
    u_int64_t transID_;

    DBG_IN("MRAwrapper::processWrapper_ACSI");

    packet->getDataFromTLV(WSI_ParaTAG_TransactionID, (void *) &transID_, &length_);
    //DBG_DUMP("TransactionID = ", &transID_, length_);
    packet->getDataFromTLV(WSI_ParaTAG_FE, (void *) &fe_, &length_);
    DBG_INFO("processWrapper_ACSI FE = %u",fe_);

    switch(packet->nslpID()) {
    case MSGTYPE_ACSI_REGISTER_FE:
	switch(fe_) {
	case MRRM_FE:
	    DBG_INFO("Registering MRRM FE");
	    //RegisterFE("MRRM");
	    DBG_INFO("MRRM registerFE sucessfully");
	    sendACSI_RegisterFEAnswer(fe_, transID_, ResultCode_SUCCESS);
	    break;
	case GLL_FE:
	    DBG_INFO("Registering GLL FE");
	    //RegisterFE("GLL");
	    DBG_INFO("GLL registerFE sucessfully");
	    //resultCode_ = ResultCode_SUCCESS;
	    sendACSI_RegisterFEAnswer(fe_, transID_, ResultCode_SUCCESS);
	    break;
	default:
	    DBG_ERR("Unknown FE");
	    break;
	}
	break;
    case MSGTYPE_ACSI_UNREGISTER_FE:
	switch(fe_) {
	case MRRM_FE:
	    UnRegisterFE("MRRM");
	    break;
	case GLL_FE:
	    UnRegisterFE("GLL");
	    break;
	default:
	    DBG_ERR("Unknown FE");
	    break;
	}
	break;
    default:
	DBG_ERR("Unknown type");
	break;
    }

    DBG_OUT("MRAwrapper::processWrapper_ACSI");
}


int MRAwrapper::openMRAwrapper_socket(void)
{
    struct sockaddr_in sockAddr_;
    int ret_;

    DBG_IN("MRAwrapper::openMRAwrapper_socket");

    ret_ = 0;
    m_wrapperSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if(m_wrapperSocket < 0) {
	DBG_ERR("Could not create socket for GLLIM_WS interface");
	ret_ = -1;
    } else {

	memset(&sockAddr_, 0, sizeof(struct sockaddr_in));

	sockAddr_.sin_family = PF_INET;
	sockAddr_.sin_port = htons(m_wrapperPort);
	sockAddr_.sin_addr.s_addr = INADDR_ANY;
	
	if((bind(m_wrapperSocket, (struct sockaddr *) &sockAddr_, sizeof(struct sockaddr_in))) < 0) {
	    DBG_ERR("Could not bind WS_socket");
	    ret_  = -1;
	}
	
    }
    
    DBG_OUT("MRAwrapper::openMRAwrapper_socket");
    return ret_;
}

void MRAwrapper::sendToMRRM(u_int8_t *msg_buf, int buf_len)
{
    struct sockaddr_in mrrmAddr;
    
    DBG_IN("MRAwrapper::sendToMRRM");
    
    mrrmAddr.sin_family = PF_INET;
    mrrmAddr.sin_port = htons(m_mrrm_port);
    mrrmAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    DBG_DUMP("SEND",msg_buf,buf_len);
    if((sendto(m_wrapperSocket, msg_buf, buf_len, 0, (struct sockaddr *)&mrrmAddr, sizeof(struct sockaddr_in))) < 0) {
	DBG_ERR("Could not send message to MRRM");
    }

    DBG_OUT("MRAwrapper::sendToMRRM");
}

void MRAwrapper::sendToGLLim(u_int8_t *msg_buf, int buf_len)
{
    struct sockaddr_in gllAddr;
    
    DBG_IN("MRAwrapper::sendToGLLim");
    
    gllAddr.sin_family = PF_INET;
    gllAddr.sin_port = htons(m_gllIM_port);
    gllAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    DBG_DUMP("SEND",msg_buf,buf_len);
    if((sendto(m_wrapperSocket, msg_buf, buf_len, 0, (struct sockaddr *)&gllAddr, sizeof(struct sockaddr_in))) < 0) {
	DBG_ERR("Could not send message to GLLim");
    }

    DBG_OUT("MRAwrapper::sendToGLLim");
}

void MRAwrapper::subscribeTRGproducer(int triggerID, string trgProdURI)
{
    string result;


    DBG_INFO("Subscribing Trigger ID %d in order to produce", triggerID);

    ANifaces::soap_call_ns2__register(&soap, (char *)trgProdURI.c_str(),
			    NULL, triggerID, result);

    if (!soap.error) {
	DBG_INFO("TRG producer subscribed: %s", result.c_str());
    }
    else {
	DBG_ERR("Suscribing TRG producer ID");
	soap_print_fault(&soap, stderr);
    }
}

void MRAwrapper::sendTriggering(struct trigger &trgProd, string trgProdURI) 
{
    ANifaces::ns2__WSANTrigger trg;
    string result;

    DBG_IN("MRAwrapper::sendTriggering");

    trg.triggerID = trgProd.triggerId ;
    trg.value= trgProd.value;
    trg.type = trgProd.type;

    DBG_INFO("Sending Trigger producer");
   
    ANifaces::soap_call_ns2__sendTrigger(&soap, (char *)trgProdURI.c_str(),
			       NULL, &trg, result);

    if (!soap.error) {
	DBG_INFO("TRG producer sent successfully: %s", result.c_str());
    }
    else {
	DBG_ERR("Sending TRG producer");
	soap_print_fault(&soap, stderr);
    }

    DBG_OUT("MRAwrapper::sendTriggering");
}

void MRAwrapper::RegisterFE(std::string FEname)
{
    std::string  strString = FEname;
    std::string* strArray = (std::string*) malloc (sizeof(std::string*)); 
    u_int8_t fe_;
    u_int32_t resultCode_;

    DBG_IN("MRAwrapper::RegisterFE");

    DBG_INFO("FEname %s received", FEname.c_str());

    strArray[0] = strString;

    ANifaces::ArrayOf_USCOREsoapenc_USCOREstring s_arr;
    s_arr.__size = 1;
    s_arr.__ptr = &strArray;
  
    ANifaces::RegistrationComponentSoapBinding acs;
    ANifaces::ns1__registerFEResponse response;

    if (FEname == "MRRM") fe_=MRRM_FE; else fe_=GLL_FE;

    /*	DBG_INFO("%s registerFE sucessfully",FEname.c_str());
	resultCode_ = ResultCode_SUCCESS;
	sendACSI_RegisterFEAnswer(fe_, resultCode_); */
  
    if (acs.ns1__registerFE(FEname, FEname, &s_arr, &s_arr, &s_arr, response) == SOAP_OK) {
	DBG_INFO("%s registerFE sucessfully",FEname.c_str());
	resultCode_ = ResultCode_SUCCESS;
	//sendACSI_RegisterFEAnswer(fe_, transID_, resultCode_); // falta definir transID_
    }
    else {
	DBG_ERR("Something went wrong with the remote method, SOAP_FAULT %d", SOAP_FAULT);
	soap_print_fault(acs.soap, stderr);
    }  

    free(strArray);

    DBG_OUT("MRAwrapper::RegisterFE");	
}

void MRAwrapper::sendACSI_RegisterFEAnswer(u_int8_t fe_, u_int64_t transID_, u_int32_t resultCode_)
{
    WSI_packet *wsiPacket_;
    u_int8_t msg_buf[MAX_PKT_SIZE];
    int length_;

    DBG_IN("MRAwrapper::sendACSI_RegisterFEAnswer");

    //DBG_DUMP("send transID_", &transID_, 8);
    wsiPacket_ = new WSI_packet;
    wsiPacket_->setHeader(MSGTYPE_ACSI_REGISTER_FE, ANSWER);
    wsiPacket_->addTLV(WSI_ParaTAG_TransactionID, (u_paraValue_t *) &transID_);
    wsiPacket_->addTLV(WSI_ParaTAG_ResultCode, (u_paraValue_t *) &resultCode_);
    length_ = wsiPacket_->serialize(msg_buf, MAX_PKT_SIZE);

    //DBG_DUMP("**addTLV", msg_buf, length_);

    DBG_INFO("sendACSI_RegisterFEAnswer fe=%d", fe_);
    switch (fe_) {
    case MRRM_FE:
	DBG_INFO("Sending ACSI_RegisterFE.Answer to MRRM");
	sendToMRRM(msg_buf, length_);
	break;
    case GLL_FE:
	DBG_INFO("Sending ACSI_RegisterFE.Answer to GLL");
	sendToGLLim(msg_buf, length_);
	break;
    default:
	DBG_ERR("Unknown FE");
	break;
    }

    DBG_OUT("MRAwrapper::sendACSI_RegisterFEAnswer");
}

void MRAwrapper::UnRegisterFE(string FEname) 
{
    std::string  strString = FEname;
    std::string* strArray = (std::string*) malloc (sizeof(std::string*)); 

    DBG_IN("MRAwrapper::UnRegisterFE");

    strArray[0] = strString;

    ANifaces::ArrayOf_USCOREsoapenc_USCOREstring s_arr;
    s_arr.__size = 1;
    s_arr.__ptr = &strArray;
  
    ANifaces::RegistrationComponentSoapBinding acs;
    ANifaces::ns1__unRegisterFEResponse response;
  
    if (acs.ns1__unRegisterFE(FEname, response) == SOAP_OK) {
	DBG_INFO("%s UnRegisterFE sucessfully",FEname.c_str());
    }
    else {
	DBG_ERR("Something went wrong with the remote method, SOAP_FAULT %d", SOAP_FAULT);
	soap_print_fault(acs.soap, stderr);
    }  

    free(strArray);

    DBG_OUT("MRAwrapper::UnRegisterFE");
}

string MRAwrapper::LookupURI(string nameToResolve) 
{

    ANifaces::LookupComponentSoapBinding acs;
    string feURI;

    DBG_IN("MRAwrapper::LookupURI");

    if (acs.ns7__lookupURI(nameToResolve, feURI) == SOAP_OK) {
	DBG_INFO("LookupURI sucessfully: %s", feURI.c_str());
	return feURI;
    }
    else {
    	soap_print_fault(acs.soap, stderr);
	return "\0";
    }

    DBG_OUT("MRAwrapper::LookupURI");
}

void MRAwrapper::Tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters) {

    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos) 
	{
	    tokens.push_back(str.substr(lastPos, pos - lastPos));
	    lastPos = str.find_first_not_of(delimiters, pos);
	    pos = str.find_first_of(delimiters, lastPos);
	}
}


void MRAwrapper::onEvent(struct eventMessage *message)
{
    DBG_IN("MRAwrapper::onEvent");

    switch(message->code) {
    case EVT_WSWRAPPER_RX:
	handleMRAwrapper_Rx();
	break;
    default:
	DBG_ERR("Unrecognized code");
	break;
    }
    
    DBG_OUT("MRAwrapper::onEvent");
}

void MRAwrapper::onTimeout(struct timerMessage *message)
{
    DBG_IN("MRAwrapper::onTimeout");

    switch(message->code) {
    default:
	DBG_INFO2("No timers declared for the moment");
	break;
    }

    DBG_OUT("MRAwrapper::onTimeout");
}

// Server-side Methods
namespace WSwrapper {

    int ns__getsnr(struct soap *soap, double ifname, double &result)
    { 
	result = 1234;
	return SOAP_OK;
    }

}
