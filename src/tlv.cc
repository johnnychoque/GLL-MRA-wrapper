/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: tlv.cc
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

#include "tlv.h"

#include <arpa/inet.h>

#define DBG_LEVEL 1
#include "debug.h"

tlv::tlv() 
{
    DBG_IN("tlv::tlv");
    

    DBG_OUT("tlv::tlv");
}


tlv::~tlv()
{
    DBG_IN("tlv::~tlv");

    DBG_OUT("tlv::~tlv");
}

void tlv::setEnumerated(u_int32_t *data)
{
    DBG_IN("tlv::setEnumerated");

    // Getting the length_
    if(*data & 0xFFFF0000) {
	length_ = 4;
    } else if(*data & 0xFFFFFF00) {
	length_ = 2;
    } else {
	length_ = 1;
    }
    u32_data_ = *data;
    DBG_INFO("Adding new enumerated TLV - data = %u",u32_data_);
    format_ = TYPE_Enumerated;

    DBG_OUT("tlv::setEnumerated");
}

void tlv::setEnumerated(u_int32_t *data, int len)
{
    DBG_IN("tlv::setEnumerated");

    length_ = len;
    u32_data_ = *data;
    DBG_INFO("Adding new enumerated TLV - data = %u",u32_data_);
    format_ = TYPE_Enumerated;

    DBG_OUT("tlv::setEnumerated");
}


void tlv::rxEnumerated(u_int32_t *data)
{
    DBG_IN("tlv::setEnumerated");

    // The lenght should have been already initialized
    switch(length_) {
    case 1:
	u8_data_ = ((u_int8_t) (*data & 0x000000FF));
	DBG_INFO("Received new enumerated TLV - data = %u",u8_data_);
	break;
    case 2:
        u16_data_ = ntohs((u_int16_t) (*data & 0x0000FFFF));
	DBG_INFO("Received new enumerated TLV - data = %u",u16_data_);
	break;
    case 4:
	u32_data_ = ntohl(*data);
	DBG_INFO("Received new enumerated TLV - data = %u",u32_data_);
	break;
    }

    format_ = TYPE_Enumerated;

    DBG_OUT("tlv::setEnumerated");
}

void tlv::setGrouped(void)
{
    DBG_IN("tlv::setGrouped");

    format_ = TYPE_Grouped;
    length_ = 0;

    DBG_OUT("tlv::setGrouped");

}

void tlv::addTLV()
{
    DBG_IN("tlv::addTLVToGrouped");

    DBG_OUT("tlv::addTLVToGrouped");
}

void tlv::rxGrouped(u_int8_t *data)
{
    DBG_IN("tlv::rxGrouped");
    
    format_ = TYPE_Grouped;

    DBG_OUT("tlv::rxGrouped");
}


void tlv::setSigned32(s_int32_t *data)
{
    DBG_IN("tlv::setSigned32");
    
    length_ = 4;
    format_ = TYPE_Integer32;
    s32_data_ = *data;

    DBG_OUT("tlv::setSigned32");
}


void tlv::rxSigned32(s_int32_t *data)
{
    DBG_IN("tlv::setSigned32");
    
    format_ = TYPE_Integer32;

    DBG_OUT("tlv::setSigned32");
}


void tlv::setUnsigned8(u_int8_t *data)
{
    DBG_IN("tlv::setUnsigned8");
    
    length_ = 1;
    u8_data_ = *data;
    format_ = TYPE_Unsigned8;

    DBG_OUT("tlv::setUnsigned8");
}

void tlv::rxUnsigned8(u_int8_t *data)
{
    DBG_IN("tlv::setUnsigned8");
    
    u8_data_ = *data;
    format_ = TYPE_Unsigned8;

    DBG_OUT("tlv::setUnsigned8");
}

void tlv::setUnsigned16(u_int16_t *data)
{
    DBG_IN("tlv::setUnsigned16");

    length_ = 2;
    u16_data_ = *data;
    format_ = TYPE_Unsigned16;
    
    DBG_OUT("tlv::setUnsigned16");
}

void tlv::rxUnsigned16(u_int16_t *data)
{
    DBG_IN("tlv::setUnsigned16");

    u16_data_ = ntohs(*data);
    format_ = TYPE_Unsigned16;
    
    DBG_OUT("tlv::setUnsigned16");
}

void tlv::setUnsigned32(u_int32_t *data)
{
    DBG_IN("tlv::setUnsigned32");

    length_ = 4;
    u32_data_ = *data;
    format_ = TYPE_Unsigned32;

    DBG_OUT("tlv::setUnsigned32");
}

void tlv::rxUnsigned32(u_int32_t *data)
{
    DBG_IN("tlv::setUnsigned32");

    u32_data_ = ntohl(*data);
    format_ = TYPE_Unsigned32;

    DBG_OUT("tlv::setUnsigned32");
}

void tlv::setUnsigned64(u_int64_t *data)
{
    DBG_IN("tlv::setUnsigned64");

    //length_ = 8;
    length_ = 16;
    u64_data_ = *data;
    format_ = TYPE_Unsigned64;

    DBG_OUT("tlv::setUnsigned64");
}

void tlv::rxUnsigned64(u_int64_t *data)
{
    DBG_IN("tlv::rxUnsigned64");

    // How does the conversion between network and host order here?
    u64_data_ = *data;
    format_ = TYPE_Unsigned64;

    DBG_OUT("tlv::rxUnsigned64");
}


void tlv::setUnsignedVariableLength(u_int32_t *data)
{
    DBG_IN("tlv::setUnsignedVariableLength");

    // Here I have to assume that this is u_int32_t, or u_int16_t or u_int8_t
    format_ = TYPE_UnsignedVariableLength;
    u32_data_ = *data;
    if(*data & 0xFFFF0000) {
	length_ = 4;
    } else if(*data & 0xFFFFFF00) {
	length_ = 2;
    } else {
	length_ = 1;
    }

    DBG_OUT("tlv::setUnsignedVariableLength");
}

void tlv::setUnsignedVariableLength(u_int8_t *data, int len)
{
    DBG_IN("tlv::setUnsignedVariableLength");

    format_ = TYPE_UnsignedVariableLength;
    length_ = len;

    memset(uvl_data_, 0, MAX_UVALUE_LEN);
    memcpy(uvl_data_, data, len);

    DBG_OUT("tlv::setUnsignedVariableLength");
}


void tlv::rxUnsignedVariableLength(u_int8_t *data)
{
    u_int32_t *aux32data_;

    DBG_IN("tlv::rxUnsignedVariableLength");

    if(length_ > MAX_UVALUE_LEN) {
	DBG_ERR("This is bigger than the maximum lenght for the unsigned variable length value");
    } else {

	DBG_INFO("Received Unsigned Variable Lenght -- type = %04x, length = %d",type_, length_);
	memset(uvl_data_, 0, MAX_UVALUE_LEN);
	memcpy(uvl_data_, data, length_);
	format_ = TYPE_UnsignedVariableLength;

	if(length_ <= 4) {
	    aux32data_ = (u_int32_t *) data;
	    switch(length_) {
	    case 1:
		u8_data_ = ((u_int8_t) (*aux32data_ & 0x000000FF));
		DBG_INFO("Received new Unsigned Variable Length TLV - data = %u (%u)",u8_data_,*data);
		break;
	    case 2:
		u16_data_ = ntohs((u_int16_t) (*aux32data_ & 0x0000FFFF));
		DBG_INFO("Received new Unsigned Variable Length TLV - data = %u",u16_data_);
		break;
	    case 4:
		u32_data_ = ntohl(*aux32data_);
		DBG_INFO("Received new Unsigned Variable Length TLV - data = %u",u32_data_);
		break;
	    }
	    
	}
    }
	
    DBG_OUT("tlv::rxUnsignedVariableLength");
}


void tlv::setUTF8String(u_int8_t *data)
{
    int len_;

    DBG_IN("tlv::setUTF8String");

    len_ = 0;
    while(data[len_] != '\0') {
	len_++;
    }
    octecString_data = new u_int8_t[len_];

    length_ = len_;
    memcpy(octecString_data, data, len_);
    format_ = TYPE_UTF8String;

    DBG_OUT("tlv::setUTF8String");

}


void tlv::setUTF8String(u_int8_t *data,int len)
{
    DBG_IN("tlv::setUTF8String");

    format_ = TYPE_UTF8String;

    DBG_OUT("tlv::setUTF8String");

}


void tlv::rxUTF8String(u_int8_t *data)
{
    DBG_IN("tlv::rxUTF8String");
    
    octecString_data = new u_int8_t[length_];
    memcpy(octecString_data, data, length_);
    format_ = TYPE_UTF8String;

    DBG_OUT("tlv::rxUTF8String");

}


void tlv::setOctecString(u_int8_t *data)
{
    u_int16_t len_;
    
    DBG_IN("tlv::setOctecString");

    len_ = 0;
    while(data[len_] != '\0') {
	len_++;
    }
    octecString_data = new u_int8_t[len_];

    length_ = len_;
    memcpy(octecString_data, data, len_);
    format_ = TYPE_OctecString;
    
    DBG_OUT("tlv::setOctecString");
}

void tlv::setOctecString(u_int8_t *data, int len)
{
    DBG_IN("tlv::setOctecString");

    octecString_data = new u_int8_t[len];
    length_ = len;
    memcpy(octecString_data, data, len);
    format_ = TYPE_OctecString;
    
    DBG_OUT("tlv::setOctecString");
}


void tlv::rxOctecString(u_int8_t *data)
{
    DBG_IN("tlv::rxOctecString");

    octecString_data = new u_int8_t[length_];
    memcpy(octecString_data, data, length_);
    format_ = TYPE_OctecString;
    
    DBG_OUT("tlv::rxOctecString");
}



int tlv::serialize(u_int8_t *msg_buf)
{
    u_int16_t auxType;
    u_int16_t auxLength;
    u_paraValue_t aux;
    vector<tlv*>::iterator iter;
    int len_;

    DBG_IN("tlv::serialize");
    
    // Temporal solution --- MRRM Code has to be changed
    auxType = type_;  
    //auxType = (type_ & 0x0FFF ) ^ 0x4000;
    auxType = htons(auxType);

    //auxLength = length_;
    auxLength = length_ & 0x0FFF;
    auxLength = htons(auxLength);
    
    DBG_INFO("Serializing tlv with type %04x with length %d",type_,length_);

    switch(format_) {
    case TYPE_OctecString:
    case TYPE_UTF8String:
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	memcpy(msg_buf+4, octecString_data, length_);
	break;
    case TYPE_UnsignedVariableLength:
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	if(length_ > 4) {
	    memcpy(msg_buf+4, uvl_data_, length_);
	} else {
	    memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	    memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	    switch(length_) {
	    case 1:
		aux.u8_data = (0x000000FF & u32_data_);
		memcpy(msg_buf + 4, &(aux.u8_data), sizeof(u_int8_t));
		break;
	    case 2:
		aux.u16_data = htons((u_int16_t) (0x0000FFFF & u32_data_));
		memcpy(msg_buf + 4, &(aux.u16_data), sizeof(u_int16_t));
		break;
	    case 4:
		aux.u32_data = htonl(u32_data_);
		memcpy(msg_buf + 4, &(aux.u32_data), sizeof(u_int32_t));
		break;
	    }
	}
	break;
    case TYPE_Enumerated:
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	switch(length_) {
	case 1:
	    aux.u8_data = (0x000000FF & u32_data_);
	    memcpy(msg_buf + 4, &(aux.u8_data), sizeof(u_int8_t));
	    break;
	case 2:
	    aux.u16_data = htons((u_int16_t) (0x0000FFFF & u32_data_));
	    memcpy(msg_buf + 4, &(aux.u16_data), sizeof(u_int16_t));
	    break;
	case 4:
	    aux.u32_data = htonl(u32_data_);
	    memcpy(msg_buf + 4, &(aux.u32_data), sizeof(u_int32_t));
	    break;
	}
	break;
    case TYPE_Unsigned8:
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	memcpy(msg_buf+4, &u8_data_, sizeof(u_int8_t));
	break;
    case TYPE_Unsigned16:
	aux.u16_data = htons(u16_data_);
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	memcpy(msg_buf+4, &(aux.u16_data), sizeof(u_int16_t));
	break;
    case TYPE_Unsigned32:
	aux.u32_data = htonl(u32_data_);
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	memcpy(msg_buf+4, &(aux.u32_data), sizeof(u_int32_t));
	break;
    case TYPE_Unsigned64:
	aux.u64_data = htonl(u64_data_);
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	memcpy(msg_buf+4, &(aux.u64_data), sizeof(u_int64_t));
	break;
    case TYPE_Integer32:
	aux.s32_data = htonl(s32_data_);
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	memcpy(msg_buf+4, &(aux.s32_data), sizeof(s_int32_t));
	break;
    case TYPE_Grouped:
	len_ = 0;
	memcpy(msg_buf, &auxType, sizeof(u_int16_t));
	memcpy(msg_buf+2, &auxLength, sizeof(u_int16_t));
	for(iter = grouped_.begin() ; iter != grouped_.end() ; iter++) {
	    len_ += (*iter)->serialize(msg_buf + 4 + len_);
	}
	break;
    default:
	DBG_ERR("Unsupported format");
	break;
    }

    DBG_OUT("tlv::serialize");
    return length_+4;
}


int tlv::addGroupedTLV(tlv *avp)
{
    int ret_;

    DBG_IN("tlv::addGroupedTLV");

    if(format_ == TYPE_Grouped) {
	grouped_.push_back(avp);
	ret_ = 0;
    } else {
	DBG_ERR("TLV is not grouped... Can not add avps");
	ret_ = 1;
    }

    DBG_OUT("tlv::addGroupedTLV");
    return ret_;
}
