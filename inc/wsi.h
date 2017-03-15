/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: wsi.h
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

#ifndef _wsi_h_
#define _wsi_h_

#include "config.h"

#include <string>
using namespace std;

#define WSI_Enumerated                        0x100
#define WSI_Grouped                           0x200
#define WSI_Signed32                          0x300
#define WSI_OctecString                       0x400
#define WSI_Unsigned16                        0x500
#define WSI_Unsigned32                        0x600
#define WSI_Unsigned8                         0x700
#define WSI_UnsignedVariableLength            0x800
#define WSI_UTF8String                        0x900
#define	WSI_Unsigned64			      0xA00

/* Wrapper messages command */
#define MSGTYPE_MHI                     0x0200
#define MSGTYPE_GHI                     0x0300
#define MSGTYPE_MSI                     0x0400
#define MSGTYPE_ACSF                    0x0500

#define MSGTYPE_ACSF_REGISTER_FE        0x0501
#define MSGTYPE_ACSF_UNREGISTER_FE      0x0502

#define MSGTYPE_WSI_LINK_ATTACH_GLL     0x0301
#define MSGTYPE_WSI_RTSOCKINFO_GLL      0x0302

// These are the types
#define	WSI_ParaTAG_TransactionID	0x001 | WSI_Unsigned64 
#define WSI_ParaTAG_RTSockInfo          0x0F0 | WSI_UnsignedVariableLength
#define WSI_ParaTAG_FE_ID               0x010 | WSI_Enumerated

#define MRRM_FE                         0x02
#define GLL_FE                          0x03

// 8.8.1 Result-code
#define ResultCode_UNKNOWN               0x00
#define ResultCode_SUCCESS               0x01
#define ResultCode_UNSUPPORTED           0x10
#define ResultCode_FORMATERROR           0x11
#define ResultCode_MANDATORYPARAMMISSING 0x12
#define ResultCode_INVALIDPARAMVALUE     0x13
#define ResultCode_PARAMTOOMANYTIMES     0x14
#define ResultCode_UNABLEDELIVER         0x15
#define ResultCode_TOOBUSY               0x16

#define MAX_PKT_SIZE                     512

#define CURRENT_VERSION                  0x01
#define WSI_GISTHOPS                     0x02

#define INDICATION                       0x00
#define REQUEST                          0x01
#define ANSWER                           0x02

/* Wrapper messages type */
#define QUERY_GIST_TYPE                  0x00
#define RESPONSE_GIST_TYPE               0x01    
#define DATA_GIST_TYPE                   0x03

struct __attribute__((packed)) wsiHeader {
    u_int8_t version;
    u_int8_t gistHops;
    u_int16_t msgLength;
    u_int16_t nslpID;
    u_int8_t type;
#ifdef LITTLEENDIAN
    u_int8_t res:5, e_:1, r_:1, s_:1;
#else
    u_int8_t s_:1, r_:1, e_:1, res:5;
#endif
};

struct trigger {
    int triggerId;
    int type;
    string value;
    //    type_t *timeStamp;
};


#endif // _wsi_h_
