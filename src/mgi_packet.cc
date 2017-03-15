/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: mgi_packet.cc
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

#include <arpa/inet.h>

#include <string>
using namespace std;

#include "mgi_packet.h"

#define DBG_LEVEL 2
#include "debug.h"

MGI_packet::MGI_packet(void)
{
    DBG_IN("MGI_packet::MGI_packet");

    DBG_OUT("MGI_packet::MGI_packet");
}


MGI_packet::MGI_packet(u_int8_t *msg_buf, int buf_len)
{
    DBG_IN("MGI_packet::MGI_packet");

    parseRxPacket(msg_buf, buf_len);

    DBG_OUT("MGI_packet::MGI_packet");
}

MGI_packet::~MGI_packet(void)
{
    tlv_iter_t iter;

    DBG_IN("MGI_packet::~MGI_packet");

    for(iter=params_.begin() ; iter != params_.end() ; iter++) {
	delete (iter->second);
    }
    params_.clear();

    DBG_OUT("MGI_packet::~MGI_packet");
}

void MGI_packet::setHeader(u_int16_t nslpID, u_int8_t flags)
{
    DBG_IN("MGI_packet::setHeader");

    memset(&header_, 0, sizeof(struct mgiHeader));

    header_.version = CURRENT_VERSION;
    header_.gistHops = MGI_GISTHOPS;
    header_.nslpID = nslpID;
    header_.s_ = 1;

    switch(flags) {
    case INDICATION:
	header_.type = DATA_GIST_TYPE;
	break;
    case REQUEST:
	header_.type = QUERY_GIST_TYPE;
	header_.r_ = 1;
	break;
    case ANSWER:
	header_.type = RESPONSE_GIST_TYPE;
	break;
    default:
	DBG_ERR("Unknown FLAG");
	break;
    }    

    DBG_OUT("MGI_packet::setHeader");
}

void MGI_packet::addTLV(u_int16_t type, u_paraValue_t *data, int len_)
{
    tlv *newTLV;

    DBG_IN("MGI_packet::addTLV");

    newTLV = new tlv;
    newTLV->type() = type;

    switch (type & 0xf00) {
    case MGI_Enumerated:
	// Enumerated
	if (len_) {
	    DBG_INFO2("len_ = %d", len_);
	    newTLV->setEnumerated((u_int32_t *) data, len_);
	} 
	else {
	    DBG_INFO2("len_ = %d", len_);
	    newTLV->setEnumerated((u_int32_t *) data);
	}
	break;
    case MGI_Grouped:
	// Grouped
	newTLV->setGrouped();
	break;
    case MGI_Integer32:
	// Integer32
	newTLV->setSigned32((s_int32_t *) data);
	break;
    case MGI_OctecString:
	// Octec String
	newTLV->setOctecString((u_int8_t *) data);
	break;
    case MGI_Unsigned16:
	// Unsigned 16
	newTLV->setUnsigned16((u_int16_t *) data);
	break;
    case MGI_Unsigned32:
	// Unsigned 32
	newTLV->setUnsigned32((u_int32_t *) data);
	break;
    case MGI_Unsigned8:
	// Unsigned 8
	newTLV->setUnsigned8((u_int8_t *)data);
	break;
    case MGI_UnsignedVariableLength:
	// Unsigned Value Length
	if(len_ != 0) {
	    newTLV->setUnsignedVariableLength((u_int8_t *)data, len_);
	} else {
	    newTLV->setUnsignedVariableLength((u_int32_t *)data);
	}
	break;
    case MGI_UTF8String:
	// UTF8String
	newTLV->setUTF8String((u_int8_t *) data);
	break;
    default:
	DBG_ERR("Unknown Parameter Type");
	break;
    }	

    DBG_INFO2("Added tlv with type = %04x and lenght = %d",type, newTLV->length());
    header_.msgLength += (newTLV->length()+4);
    params_.insert(pair<u_int16_t, tlv*> (type, newTLV));

    DBG_OUT("MGI_packet::addTLV");
}

int MGI_packet::addTLVToGrouped(u_int16_t mainType, u_int16_t type, u_paraValue_t *data = NULL)
{
    tlv *newTLV;
    tlv *mainTLV;
    tlv_iter_t iter;
    int ret_;

    DBG_IN("MGI_packet::addTLVToGrouped");

    iter = params_.find(mainType);

    if(iter != params_.end()) {

	mainTLV = iter->second;
	 
	if(mainTLV->format_ == TYPE_Grouped) {
	    newTLV = new tlv;
	    newTLV->type() = type;

	    switch (type & 0xf00) {
	    case MGI_Enumerated:
		// Enumerated
		newTLV->setEnumerated((u_int32_t *) data);
		break;
	    case MGI_Integer32:
		// Integer32
		newTLV->setSigned32((s_int32_t *) data);
		break;
	    case MGI_OctecString:
		// Octec String
		newTLV->setOctecString((u_int8_t *) data);
		break;
	    case MGI_Unsigned8:
		// Unsigned 8
		newTLV->setUnsigned8((u_int8_t *)data);
		break;
	    case MGI_UnsignedVariableLength:
		// Unsigned Value Length
		newTLV->setUnsignedVariableLength((u_int32_t *)data);
		break;
	    default:
		DBG_ERR("Unknown TLV Typed for the Grouped %x",mainType);
		break;
	    }
	    (mainTLV->grouped_).push_back(newTLV);
	    mainTLV->length_ += (newTLV->length_ + 4);
	    header_.msgLength += (newTLV->length_ + 4);
	    ret_ = 0;
	     
	} else {
	    DBG_ERR("Main TLV type (%x) was not of grouped format",mainType);
	    ret_ = -2;
	}
    } else {
	DBG_ERR("Main TLV type (%x) was not registered",mainType);
	ret_ = -1;
    }
    
    DBG_OUT("MGI_packet::addTLVToGrouped");
    return ret_;
}

int MGI_packet::serialize(u_int8_t *msg_buf, int buf_len)
{
    int length_;
    tlv_iter_t iter;
    u_int8_t *position;
    struct mgiHeader auxHeader;

    DBG_IN("MGI_packet::serialize");
    
    // Some chanity checks (about Length) are required
    
    position = msg_buf;

    memcpy(&auxHeader, &header_, sizeof(struct mgiHeader));
    auxHeader.msgLength = htons(auxHeader.msgLength);
    auxHeader.nslpID = htons(auxHeader.nslpID);
    
    memset(position, 0, buf_len);
    memcpy(position, (void *) &auxHeader, sizeof(struct mgiHeader));

    length_ = sizeof(struct mgiHeader);
    position += length_;

    for(iter = params_.begin() ; iter != params_.end() ; iter++) {
	position += (iter->second)->serialize(position);
    }
    length_ = position - msg_buf;

    DBG_OUT("MGI_packet::serialize");
    return length_;
}


void MGI_packet::parseRxPacket(u_int8_t *msg_buf, int buf_len)
{
    DBG_IN("MGI_packet::parseRxPacket");

    memcpy(&header_, (struct mgiHeader *) msg_buf, sizeof(struct mgiHeader));
    header_.msgLength = ntohs(header_.msgLength);
    header_.nslpID = ntohs(header_.nslpID);

    DBG_INFO2("Received packet - Type = %x",header_.nslpID);
    
    processRxTLV(msg_buf+sizeof(struct mgiHeader), header_.msgLength);
    
    DBG_OUT("MGI_packet::parseRxPacket");
}

void MGI_packet::processRxTLV(u_int8_t *msg_buf, int len)
{
    struct tlvHead *aux;
    tlv *newTLV;
    char temp[250];
    int i;
    u_paraValue_t *auxData;
    
    DBG_IN("MGI_packet::processRxTLV");

    while(len > 0) {

	newTLV = new tlv;
	aux = (struct tlvHead *) msg_buf;
	
	newTLV->type() = ntohs(aux->type);
	newTLV->length() = ntohs(aux->length);

	if(DBG_LEVEL > 1) {
	    memset(temp, '\0', 250);
	    for(i = 0; i < newTLV->length()+4; i++) {
		sprintf(temp+i*3, "%02X ", msg_buf[i]);
	    }
	}
	auxData = (u_paraValue_t *) (msg_buf + 4);
	DBG_INFO2("TLV is of type %x, with length = %d",newTLV->type(),newTLV->length());

	switch (newTLV->type() & 0xf00) {
	case MGI_Enumerated:
	    // Enumerated
	    newTLV->rxEnumerated((u_int32_t *)auxData);
	    break;
	case MGI_Grouped:
	    // Grouped
	    newTLV->rxGrouped((u_int8_t *) auxData);
	    processGroupedTLV(msg_buf+4, newTLV);
	    break;
	case MGI_Integer32:
	    // Integer32
	    newTLV->rxSigned32((s_int32_t *) auxData);
	    break;
	case MGI_OctecString:
	    // Octec String
	    newTLV->rxOctecString((u_int8_t *) auxData);
	    break;
	case MGI_Unsigned16:
	    // Unsigned 16
	    newTLV->rxUnsigned16((u_int16_t *) auxData);
	    break;
	case MGI_Unsigned32:
	    // Unsigned 32
	    newTLV->rxUnsigned32((u_int32_t *) auxData);
	    break;
	case MGI_Unsigned64:
	    // Unsigned 64
	    newTLV->rxUnsigned64((u_int64_t *) auxData);
	    break;
	case MGI_Unsigned8:
	    // Unsigned 8
	    newTLV->rxUnsigned8((u_int8_t *)auxData);
	    break;
	case MGI_UnsignedVariableLength:
	    // Unsigned Value Length
	    newTLV->rxUnsignedVariableLength((u_int8_t *)auxData);
	    break;
	case MGI_UTF8String:
	    newTLV->rxUTF8String((u_int8_t *) auxData);
	    // UTF8String
	    break;
	default:
	    DBG_ERR("Rx packet with unknown Parameter Type");
	    break;
	};

	len -= (4 + newTLV->length());
	msg_buf += (4 + newTLV->length());

	DBG_INFO2("processing tlv -- length = %d",len);

	params_.insert(pair<u_int16_t, tlv*> (newTLV->type(), newTLV));

    }

    DBG_OUT("MGI_packet::processRxTLV");
}


void MGI_packet::processGroupedTLV(u_int8_t *msg_buf, tlv *currentTLV)
{
    int len, i;
    char temp[250];
    tlv *newTLV;
    struct tlvHead *aux;
    u_paraValue_t *auxData;
    
    DBG_IN("MGI_packet::processGroupedTLV");

    len = (int) currentTLV->length(); // lenght has been fixed before
    while(len > 0) {

	newTLV = new tlv;
	aux = (struct tlvHead *) msg_buf;
	
	newTLV->type() = ntohs(aux->type);
	newTLV->length() = ntohs(aux->length);

	if(DBG_LEVEL > 1) {
	    memset(temp, '\0', 250);
	    for(i = 0; i < newTLV->length()+4; i++) {
		sprintf(temp+i*3, "%02X ", msg_buf[i]);
	    }
	}

	auxData = (u_paraValue_t *) (msg_buf + 4);

	switch (newTLV->type() & 0xf00) {
	case MGI_Enumerated:
	    // Enumerated
	    newTLV->rxEnumerated((u_int32_t *)auxData);
	    break;
	case MGI_Integer32:
	    // Integer32
	    newTLV->rxSigned32((s_int32_t *) auxData);
	    break;
	case MGI_OctecString:
	    // Octec String
	    newTLV->rxOctecString((u_int8_t *) auxData);
	    break;
	case MGI_Unsigned8:
	    // Unsigned 8
	    newTLV->rxUnsigned8((u_int8_t *)auxData);
	    break;
	case MGI_UnsignedVariableLength:
	    // Unsigned Value Length
	    newTLV->rxUnsignedVariableLength((u_int8_t *)auxData);
	    break;
	default:
	    DBG_ERR("Unknown TLV Typed for the Grouped %x",currentTLV->type());
	}
	
	len -= (4 + newTLV->length());
	msg_buf += (4 + newTLV->length());

	currentTLV->addGroupedTLV(newTLV);
    }

    DBG_OUT("MGI_packet::processGroupedTLV");
}

int MGI_packet::getMeasConfiguration(struct measConfiguration *measConfig)
{
    tlv *MeasConfigTLV;
    vector<tlv *>::iterator iter;
    u_int32_t *aux, temp1, temp2;
    int *configItem, ret_;
    tlv_iter_t iterTLV;

    DBG_IN("MGI_packet::getMeasConfiguration");

    memset(measConfig, 0, sizeof(struct measConfiguration));
    // Sanity checks required
    if((iterTLV = params_.find(ParaTAG_MeasConfiguration)) == params_.end()) {
	DBG_ERR("Candidate report without configuration");
	ret_ = -1;
    } else {
	ret_ = 0;
	MeasConfigTLV = iterTLV->second;
	for(iter = MeasConfigTLV->grouped_.begin() ; iter != MeasConfigTLV->grouped_.end() ; iter++) {
	    switch((*iter)->type()) {
	    case ParaTAG_MeasParaType:
		configItem = &(measConfig->MeasurementParaType);
		switch((*iter)->length_) {
		case 1:
		    *configItem = (int) (*iter)->u8_data_;
		    break;
		case 2:
		    *configItem = (int) (*iter)->u16_data_;
		    break;
		case 4:
		    *configItem = (int) (*iter)->u32_data_;
		    break;
		}
		break;
	    case ParaTAG_MeasReportReason:
		configItem = &(measConfig->MeasurementReportEventType);
		switch((*iter)->length_) {
		case 1:
		    *configItem = (int) (*iter)->u8_data_;
		    break;
		case 2:
		    *configItem = (int) (*iter)->u16_data_;
		    break;
		case 4:
		    *configItem = (int) (*iter)->u32_data_;
		    break;
		}
		break;
	    case ParaTAG_ReportPeriod:
		configItem = &(measConfig->ReportingPeriodicity);
		switch((*iter)->length_) {
		case 1:
		    *configItem = (int) (*iter)->u8_data_;
		    break;
		case 2:
		    *configItem = (int) (*iter)->u16_data_;
		    break;
		case 4:
		    *configItem = (int) (*iter)->u32_data_;
		    break;
		}
		break;
	    case ParaTAG_NumberOfSamples:
		configItem = &(measConfig->NumberOfSamples);
		switch((*iter)->length_) {
		case 1:
		    *configItem = (int) (*iter)->u8_data_;
		    break;
		case 2:
		    *configItem = (int) (*iter)->u16_data_;
		    break;
		case 4:
		    *configItem = (int) (*iter)->u32_data_;
		    break;
		}
		break;
	    case ParaTAG_AveragSamples:
		configItem = &(measConfig->AveragingOfSamples);
		switch((*iter)->length_) {
		case 1:
		    *configItem = (int) (*iter)->u8_data_;
		    break;
		case 2:
		    *configItem = (int) (*iter)->u16_data_;
		    break;
		case 4:
		    *configItem = (int) (*iter)->u32_data_;
		    break;
		}
		break;
	    case ParaTAG_SamplePeriod:
		configItem = &(measConfig->SamplePeriodicity);
		switch((*iter)->length_) {
		case 1:
		    *configItem = (int) (*iter)->u8_data_;
		    break;
		case 2:
		    *configItem = (int) (*iter)->u16_data_;
		    break;
		case 4:
		    *configItem = (int) (*iter)->u32_data_;
		    break;
		}
		break;
	    case ParaTAG_Threshold:
		if((*iter)->length_ == 4) { // There is only one threshold
		    aux = (u_int32_t *) (*iter)->octecString_data;
		    temp1 = ntohl(*aux);
		    measConfig->lowerThreshold = (int) (0x00FFFFFF & temp1);
		    measConfig->upperThreshold = -1;
		} else {
		    aux = (u_int32_t *) ((*iter)->octecString_data);
		    temp1 = ntohl(*aux);
		    measConfig->lowerThreshold = (int) (0x00FFFFFF & temp1);
		    temp2 = measConfig->lowerThreshold;
		    aux = (u_int32_t *) ((*iter)->octecString_data + 4);
		    temp1 = ntohl(*aux);
		    measConfig->upperThreshold = (int) (0x00FFFFFF & temp1);
		    if((int) temp2 >  measConfig->upperThreshold) {
			temp2 = (unsigned) measConfig->upperThreshold;
			measConfig->upperThreshold = measConfig->lowerThreshold;
			measConfig->lowerThreshold = (int) temp2;
		    }
		}
		break;
	    default:
		DBG_ERR("Unknown parameter in Measurement Configuration");
		break;
	    }
	}
    }


    DBG_OUT("MGI_packet::getMeasConfiguration");
    return ret_;
}

void MGI_packet::getDataFromTLV(u_int16_t type, void *data, int *len)
{
    tlv_iter_t iter;
    vector<tlv*>::iterator grouped_iter;
    tlv *current;

    DBG_IN("MGI_packet::getDataFromTLV");
    
    iter = params_.find(type);
    
    if(iter != params_.end()) {
	current = iter->second;

	switch(current->format()) {
	case TYPE_OctecString:
	case TYPE_UTF8String:
	    DBG_INFO2("Length = %d",(int) current->length_);
	    *len = (int) current->length_;
	    memcpy(data, current->octecString_data, current->length_);
	    //DBG_INFO("We have copied something");
	    break;
	case TYPE_UnsignedVariableLength:
	  // There is a difference here with the unsigned variable length
	  // some of them are sort of array (as Cell ID, MAC address), while
	  // others refer to single variables
	  *len = (int) current->length_;

	  // We assume that if len > 4, then this is an array, otherwise it
	  // is a single variable
	  if(*len > 4) {
	    memcpy(data, current->uvl_data_, current->length_);
	  } else {
	    switch(*len) {
	    case 1:
		memcpy(data, &(current->u8_data_), current->length_);
		break;
	    case 2:
		memcpy(data, &(current->u16_data_), current->length_);
		break;
	    case 4:
		memcpy(data, &(current->u32_data_), current->length_);
		break;
	    }
	  }
	  break;
	case TYPE_Enumerated:
	    // We need to rework this
	    *len = (int) current->length_;
	    DBG_INFO2("Parsing an enumerated tlv - length = %d",*len);
	    switch(*len) {
	    case 1:
		memcpy(data, &(current->u8_data_), current->length_);
		break;
	    case 2:
		memcpy(data, &(current->u16_data_), current->length_);
		break;
	    case 4:
		memcpy(data, &(current->u32_data_), current->length_);
		break;
	    }
	    break;
	case TYPE_Unsigned8:
	    *len = (int) current->length_;
	    memcpy(data, &(current->u8_data_), current->length_);
	    break;
	case TYPE_Unsigned16:
	    *len = (int) current->length_;
	    memcpy(data, &(current->u16_data_), current->length_);
	    break;
	case TYPE_Unsigned32:
	    *len = (int) current->length_;
	    memcpy(data, &(current->u32_data_), current->length_);
	    break;
	case TYPE_Unsigned64:
	    *len = (int) current->length_;
	    memcpy(data, &(current->u64_data_), current->length_);
	    break;	    
	case TYPE_Grouped:
	    *len = 0;
	    //memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	    //grouped_iter = 
	    //for(iter = grouped_.begin() ; iter != grouped_.end() ; iter++) {
	    //	len_ += (*iter)->serialize(msg_buf + 4 + len_);
	    //}
	    break;
	default:
	    DBG_ERR("Unsupported format");
	    break;

	}


    } else {
	DBG_ERR("Type not found in the message");
	*len = 0;
    }

    DBG_OUT("MGI_packet::getDataFromTLV");

}
