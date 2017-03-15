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

#include "wsi_packet.h"

#define DBG_LEVEL 2
#include "debug.h"

WSI_packet::WSI_packet(void)
{
    DBG_IN("WSI_packet::WSI_packet");

    DBG_OUT("WSI_packet::WSI_packet");
}


WSI_packet::WSI_packet(u_int8_t *msg_buf, int buf_len)
{
    DBG_IN("WSI_packet::WSI_packet");

    parseRxPacket(msg_buf, buf_len);

    DBG_OUT("WSI_packet::WSI_packet");
}

WSI_packet::~WSI_packet(void)
{
    tlv_iter_t iter;

    DBG_IN("WSI_packet::~WSI_packet");

    for(iter=params_.begin() ; iter != params_.end() ; iter++) {
	delete (iter->second);
    }
    params_.clear();

    DBG_OUT("WSI_packet::~WSI_packet");
}

void WSI_packet::setHeader(u_int16_t nslpID, u_int8_t flags)
{
    DBG_IN("WSI_packet::setHeader");

    memset(&header_, 0, sizeof(struct wsiHeader));

    header_.version = CURRENT_VERSION;
    header_.gistHops = WSI_GISTHOPS;
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

    DBG_OUT("WSI_packet::setHeader");
}

void WSI_packet::addTLV(u_int16_t type, u_paraValue_t *data, int len_)
{
    tlv *newTLV;

    DBG_IN("WSI_packet::addTLV");

    newTLV = new tlv;
    newTLV->type() = type;

    switch( type & 0xf00 ) {
    case WSI_Enumerated:
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
    case WSI_Grouped:
	// Grouped
	newTLV->setGrouped();
	break;
    case WSI_Signed32:
	// Integer32
	newTLV->setSigned32((s_int32_t *) data);
	break;
    case WSI_OctecString:
	// Octec String
	newTLV->setOctecString((u_int8_t *) data);
	break;
    case WSI_Unsigned16:
	// Unsigned 16
	newTLV->setUnsigned16((u_int16_t *) data);
	break;
    case WSI_Unsigned32:
	// Unsigned 32
	newTLV->setUnsigned32((u_int32_t *) data);
	break;
    case WSI_Unsigned64:
	// Unsigned 64
	newTLV->setUnsigned64((u_int64_t *) data);
	break;
    case WSI_Unsigned8:
	// Unsigned 8
	newTLV->setUnsigned8((u_int8_t *)data);
	break;
    case WSI_UnsignedVariableLength: 
	// Unsigned Value Length
	if(len_ != 0) {
	    newTLV->setUnsignedVariableLength((u_int8_t *)data, len_);
	} else {
	    newTLV->setUnsignedVariableLength((u_int32_t *)data);
	}
	break;
    case WSI_UTF8String:
	newTLV->setUTF8String((u_int8_t *) data);
	// UTF8String
	break;
    default:
	DBG_ERR("Unknown Parameter Type");
	break;
    }	

    DBG_INFO2("Added tlv with type = %04x and lenght = %d",type, newTLV->length());
    header_.msgLength += (newTLV->length()+4);
    params_.insert(pair<u_int16_t, tlv*> (type, newTLV));

    DBG_OUT("WSI_packet::addTLV");
}


int WSI_packet::serialize(u_int8_t *msg_buf, int buf_len)
{
    int length_;
    tlv_iter_t iter;
    u_int8_t *position;
    struct wsiHeader auxHeader;

    DBG_IN("WSI_packet::serialize");
    
    // Some chanity checks (about Length) are required
    
    position = msg_buf;

    memcpy(&auxHeader, &header_, sizeof(struct wsiHeader));
    auxHeader.msgLength = htons(auxHeader.msgLength);
    auxHeader.nslpID = htons(auxHeader.nslpID);
    
    memset(position, 0, buf_len);
    memcpy(position, (void *) &auxHeader, sizeof(struct wsiHeader));

    length_ = sizeof(struct wsiHeader);
    position += length_;

    for(iter = params_.begin() ; iter != params_.end() ; iter++) {
	position += (iter->second)->serialize(position);
    }
    length_ = position - msg_buf;

    DBG_OUT("WSI_packet::serialize");
    return length_;
}


void WSI_packet::parseRxPacket(u_int8_t *msg_buf, int buf_len)
{
    DBG_IN("WSI_packet::parseRxPacket");

    memcpy(&header_, (struct wsiHeader *) msg_buf, sizeof(struct wsiHeader));
    header_.msgLength = ntohs(header_.msgLength);
    header_.nslpID = ntohs(header_.nslpID);

    DBG_INFO2("Received packet - Type = %x",header_.nslpID);
    
    processRxTLV(msg_buf+sizeof(struct wsiHeader), header_.msgLength);
    
    DBG_OUT("WSI_packet::parseRxPacket");
}

void WSI_packet::processRxTLV(u_int8_t *msg_buf, int len)
{
    struct tlvHead *aux;
    tlv *newTLV;
    char temp[250];
    int i;
    u_paraValue_t *auxData;
    
    DBG_IN("WSI_packet::processRxTLV");

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
	case WSI_Enumerated:
	    // Enumerated
	    newTLV->rxEnumerated((u_int32_t *)auxData);
	    break;
// 	case WSI_Grouped:
// 	    // Grouped
// 	    newTLV->rxGrouped((u_int8_t *) auxData);
// 	    processGroupedTLV(msg_buf+4, newTLV);
// 	    break;
	case WSI_Signed32:
	    // Integer32
	    newTLV->rxSigned32((s_int32_t *) auxData);
	    break;
	case WSI_OctecString:
	    // Octec String
	    newTLV->rxOctecString((u_int8_t *) auxData);
	    break;
	case WSI_Unsigned16:
	    // Unsigned 16
	    newTLV->rxUnsigned16((u_int16_t *) auxData);
	    break;
	case WSI_Unsigned32:
	    // Unsigned 32
	    newTLV->rxUnsigned32((u_int32_t *) auxData);
	    break;
	case WSI_Unsigned8:
	    // Unsigned 8
	    newTLV->rxUnsigned8((u_int8_t *)auxData);
	    break;
	case WSI_UnsignedVariableLength:
	    // Unsigned Value Length
	    newTLV->rxUnsignedVariableLength((u_int8_t *)auxData);
	    break;
	case WSI_UTF8String:
	    newTLV->rxUTF8String((u_int8_t *) auxData);
	    // UTF8String
	    break;
	default:
	    DBG_ERR("Unknown Parameter Type");
	    break;

	};


	len -= (4 + newTLV->length());
	msg_buf += (4 + newTLV->length());

	DBG_INFO2("processing tlv -- length = %d",len);

	params_.insert(pair<u_int16_t, tlv*> (newTLV->type(), newTLV));


    }

    DBG_OUT("WSI_packet::processRxTLV");
}



void WSI_packet::getDataFromTLV(u_int16_t type, void *data, int *len)
{
    tlv_iter_t iter;
    vector<tlv*>::iterator grouped_iter;
    tlv *current;

    DBG_IN("WSI_packet::getDataFromTLV");
    
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

    DBG_OUT("WSI_packet::getDataFromTLV");

}
