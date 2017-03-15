/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: ari.cc
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
#include <netinet/in.h>

#include "ari.h"

#define DBG_LEVEL 2
#include "debug.h"

ARI_message::ARI_message(void)
{
    DBG_IN("ARI_message::ARI_message");

    DBG_OUT("ARI_message::ARI_message");
}

ARI_message::ARI_message(u_int8_t *msg_buf, int buf_len)
{
    DBG_IN("ARI_message::ARI_message");

    parseRxPacket(msg_buf, buf_len);

    DBG_OUT("ARI_message::ARI_message");
}

ARI_message::~ARI_message(void)
{
    tlv_iter_t iter;

    DBG_IN("ARI_message::~ARI_message");
   
    for(iter=params_.begin() ; iter != params_.end() ; iter++) {
	delete (iter->second);
    }
    params_.clear();

    DBG_OUT("ARI_message::~ARI_message");
}

void ARI_message::setHeader(u_int16_t type, u_int8_t flags)
{
    DBG_IN("ARI_message::setHeader");
    
    memset(&header_, 0, sizeof(struct ARI_header));

    header_.version = ARI_CURRENT_VERSION;
    header_.type = type;
    header_.flags = flags;

    DBG_OUT("ARI_message::setHeader");
}

void ARI_message::addTLV(u_int16_t type, u_paraValue_t *data, int len)
{
    tlv *newTLV;

    DBG_IN("ARI_message::addTLV");
    
    newTLV = new tlv;
    newTLV->type() = type;
    
    switch(type & 0xf000 ) {
    case ARI_Enumerated:
	// Enumerated
	if(len > 0) {
	    newTLV->setEnumerated((u_int32_t *) data, len);
	} else {
	    newTLV->setEnumerated((u_int32_t *) data);
	}
	break;
    case ARI_Grouped:
	// Grouped
	//newTLV->setGrouped(data);
	break;
    case ARI_Signed32:
	// Integer32
	newTLV->setSigned32((s_int32_t *) data);
	break;
    case ARI_OctecString:
	// Octec String
	if(len > 0) {
	    newTLV->setOctecString((u_int8_t *) data,len);
	} else {
	    newTLV->setOctecString((u_int8_t *) data);
	}
	break;
    case ARI_Unsigned16:
	// Unsigned 16
	newTLV->setUnsigned16((u_int16_t *) data);
	break;
    case ARI_Unsigned32:
	// Unsigned 32
	newTLV->setUnsigned32((u_int32_t *) data);
	break;
    case ARI_Unsigned8:
	// Unsigned 8
	newTLV->setUnsigned8((u_int8_t *)data);
	break;
    case ARI_UnsignedVariableLength:
	// Unsigned Variable Length
	if(len>0) {
	    newTLV->setUnsignedVariableLength((u_int8_t *)data,len);
	} else {
	    newTLV->setUnsignedVariableLength((u_int32_t *)data);
	}
	break;
    case ARI_UTF8String:
	// UTF8String
	if(len > 0) {
	    newTLV->setUTF8String((u_int8_t *) data,len);
	} else {
	    newTLV->setUTF8String((u_int8_t *) data);
	}
	break;
    default:
	DBG_ERR("Unknown Parameter Type");
	break;
    }	
    
    header_.length += (newTLV->length()+4);
    params_.insert(pair<u_int16_t, tlv*> (type, newTLV));

    DBG_INFO2("Adding tlv --- length = %d",header_.length);

    DBG_OUT("ARI_message::addTLV");
}

int ARI_message::serialize(u_int8_t *msg_buf, int buf_len)
{
    int length_;
    tlv_iter_t iter;
    u_int8_t *position;
    struct ARI_header auxHeader;

    DBG_IN("ARI_message::serialize");
    
    // Some chanity checks (about Length) are required
    
    position = msg_buf;

    memcpy(&auxHeader, &header_, sizeof(struct ARI_header));
    auxHeader.length = htons(auxHeader.length);
    auxHeader.type = htons(auxHeader.type);
    
    memset(position, 0, buf_len);
    memcpy(position, (void *) &auxHeader, sizeof(struct ARI_header));

    length_ = sizeof(struct ARI_header);
    position += length_;

    for(iter = params_.begin() ; iter != params_.end() ; iter++) {
	position += (iter->second)->serialize(position);
    }
    length_ = position - msg_buf;
    //DBG_INFO("Length at serialize = %d",length_);


    DBG_OUT("ARI_message::serialize");
    return length_;
}


void ARI_message::parseRxPacket(u_int8_t *msg_buf, int buf_len)
{
    DBG_IN("ARI_message::parseRxPacket");

    memcpy(&header_, (struct ARI_header *) msg_buf, sizeof(struct ARI_header));
    header_.length = ntohs(header_.length);
    header_.type = ntohs(header_.type);

    DBG_INFO2("Received packet - Type = %x - Length = %d",header_.type, (int) header_.length);
    
    processRxTLV(msg_buf+sizeof(struct ARI_header), header_.length);
    
    DBG_OUT("ARI_message::parseRxPacket");
}

void ARI_message::processRxTLV(u_int8_t *msg_buf, int len)
{
    struct tlvHead *aux;
    tlv *newTLV;
    char temp[MAX_UVALUE_LEN*3];
    int i;
    u_paraValue_t *auxData;
    
    DBG_IN("ARI_message::processRxTLV");

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

	switch(newTLV->type() & 0xf000) {
	case ARI_Enumerated:
	    // Enumerated
	    newTLV->rxEnumerated((u_int32_t *)auxData);
	    break;
	case ARI_Grouped:
	    // Grouped
	    newTLV->rxGrouped((u_int8_t *) auxData);
	    processGroupedTLV(msg_buf+4, newTLV);
	    break;
	case ARI_Signed32:
	    // Integer32
	    newTLV->rxSigned32((s_int32_t *) auxData);
	    break;
	case ARI_OctecString:
	    // Octec String
	    newTLV->rxOctecString((u_int8_t *) auxData);
	    break;
	case ARI_Unsigned16:
	    // Unsigned 16
	    newTLV->rxUnsigned16((u_int16_t *) auxData);
	    break;
	case ARI_Unsigned32:
	    // Unsigned 32
	    newTLV->rxUnsigned32((u_int32_t *) auxData);
	    break;
	case ARI_Unsigned8:
	    // Unsigned 8
	    newTLV->rxUnsigned8((u_int8_t *)auxData);
	    break;
	case ARI_UnsignedVariableLength:
	    // Unsigned Value Length
	    newTLV->rxUnsignedVariableLength((u_int8_t *)auxData);
	    break;
	case ARI_UTF8String:
	    newTLV->rxUTF8String((u_int8_t *) auxData);
	    // UTF8String
	    break;
	default:
	    DBG_ERR("Unknown Parameter Type");
	    break;

	};


	len -= (4 + newTLV->length());
	msg_buf += (4 + newTLV->length());

	DBG_INFO2("processing tlv (%04x) -- length = %d",newTLV->type(), len);

	params_.insert(pair<u_int16_t, tlv*> (newTLV->type(), newTLV));
	

    }

    DBG_OUT("ARI_message::processRxTLV");
}


void ARI_message::processGroupedTLV(u_int8_t *msg_buf, tlv *currentTLV)
{
    int len, i;
    char temp[250];
    tlv *newTLV;
    struct tlvHead *aux;
    u_paraValue_t *auxData;
    
    DBG_IN("ARI_message::processGroupedTLV");

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
	DBG_INFO("\t%s",temp);

	auxData = (u_paraValue_t *) (msg_buf + 4);

	switch(newTLV->type() & 0xf000) {
	case ARI_Enumerated:
	    // Enumerated
	    newTLV->rxEnumerated((u_int32_t *)auxData);
	    break;
	case ARI_Grouped:
	    // Grouped
	    newTLV->rxGrouped((u_int8_t *) auxData);
	    processGroupedTLV(msg_buf+4, newTLV);
	    break;
	case ARI_Signed32:
	    // Integer32
	    newTLV->rxSigned32((s_int32_t *) auxData);
	    break;
	case ARI_OctecString:
	    // Octec String
	    newTLV->rxOctecString((u_int8_t *) auxData);
	    break;
	case ARI_Unsigned16:
	    // Unsigned 16
	    newTLV->rxUnsigned16((u_int16_t *) auxData);
	    break;
	case ARI_Unsigned32:
	    // Unsigned 32
	    newTLV->rxUnsigned32((u_int32_t *) auxData);
	    break;
	case ARI_Unsigned8:
	    // Unsigned 8
	    newTLV->rxUnsigned8((u_int8_t *)auxData);
	    break;
	case ARI_UnsignedVariableLength:
	    // Unsigned Variable Length
	    newTLV->rxUnsignedVariableLength((u_int8_t *)auxData);
	    break;
	case ARI_UTF8String:
	    newTLV->rxUTF8String((u_int8_t *) auxData);
	    // UTF8String
	    break;
	default:
	    DBG_ERR("Unknown TLV Typed for the Grouped %u",currentTLV->type());
	}
	
	len -= (4 + newTLV->length());
	msg_buf += (4 + newTLV->length());

	currentTLV->addGroupedTLV(newTLV);
    }

    DBG_OUT("ARI_message::processGroupedTLV");
}


void ARI_message::getDataFromTLV(u_int16_t type, void *data, int *len)
{
    tlv_iter_t iter;
    vector<tlv*>::iterator grouped_iter;
    tlv *current;
    //int i;

    DBG_IN("ARI_message::getDataFromTLV");

    //i = 0;
    // for(iter = params_.begin() ; iter != params_.end() ; iter++) {
    // 	DBG_INFO("(%d) - Type = %u",i, (iter->second)->type());
    // 	i++;
    //     }
    
    iter = params_.find(type);

    
    if(iter != params_.end()) {
	current = iter->second;
	//DBG_INFO("Getting information of type %d",current->format());
	switch(current->format() ) {
	case TYPE_OctecString:
	case TYPE_UTF8String:
	    *len = (int) current->length_;
	    memcpy(data, current->octecString_data, current->length_);
	    //DBG_INFO("We have copied something");
	    break;
	case TYPE_UnsignedVariableLength:
	case TYPE_Enumerated:
	    // We need to rework this
	    *len = (int) current->length_;
	    
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
    
    
    DBG_OUT("ARI_message::getDataFromTLV");
}
