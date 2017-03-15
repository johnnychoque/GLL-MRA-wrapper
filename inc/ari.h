/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: ari.h
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


#ifndef _ari_h
#define _ari_h

#include <map>
using namespace std;

#include "tlv.h"

#define ARI_FLAGS_REQUEST                     0x01
#define ARI_FLAGS_ANSWER                      0x02
#define ARI_FLAGS_INDICATION                  0x03

#define ARI_CURRENT_VERSION                   0x01

#define ARI_MAX_PKT_SIZE                      512

#define ARI_NEW_DEVICE                        0x0001
#define ARI_DELETE_DEVICE                     0x0005

#define ARI_CANDIDATE_REPORT                  0x0002
#define ARI_LINK_ATTACH                       0x0003
#define ARI_LINK_REPORT                       0x0004
#define ARI_LINK_DETACH                       0x0005

//#define ARI_RTSOCK_INFO                       0x0010

#define ARI_Enumerated                        0x1000
#define ARI_Grouped                           0x2000
#define ARI_Signed32                          0x3000
#define ARI_OctecString                       0x4000
#define ARI_Unsigned16                        0x5000
#define ARI_Unsigned32                        0x6000
#define ARI_Unsigned8                         0x7000
#define ARI_UnsignedVariableLength            0x8000
#define ARI_UTF8String                        0x9000

// Enumerated
#define ARI_ParaTAG_AccessTechType            0x001 | ARI_Enumerated

// OctecString
#define ARI_ParaTAG_DeviceID                  0x001 | ARI_OctecString
#define ARI_ParaTAG_NetworkID                 0x002 | ARI_OctecString

#define ARI_ParaTAG_MACaddr                   0x001 | ARI_UnsignedVariableLength
#define ARI_ParaTAG_CellID                    0x002 | ARI_UnsignedVariableLength
//#define ARI_ParaTAG_RTSockInfo                0x003 | ARI_UnsignedVariableLength

#define ARI_ParaTAG_LinkQual                  0x001 | ARI_Unsigned16




struct __attribute__((packed)) ARI_header {
    u_int8_t version;
#ifdef LITTLEENDIAN
    u_int8_t res:5, up:1, flags:2;
#else
    u_int8_t flags:2, up:1, res:5;
#endif
    u_int16_t type;
    u_int16_t length;
};

class ARI_message {
 public:
    ARI_message();
    ARI_message(u_int8_t *msg_buf, int buf_len);
    ~ARI_message();

    inline u_int8_t version(void) {return header_.version;}
    inline bool up(void) {return header_.up;}
    inline u_int8_t flags(void) {return header_.flags;}
    inline u_int16_t type(void) {return header_.type;}
    inline u_int16_t length(void) {return header_.length;}
    
    void addTLV(u_int16_t type, u_paraValue_t *data, int len = -1);
    int serialize(u_int8_t *msg_buf, int buf_len);
    
    void setHeader(u_int16_t type, u_int8_t flags);

    void getDataFromTLV(u_int16_t type, void *data, int *len);

 private:
    struct ARI_header header_;

    map<u_int16_t, tlv*> params_;
    typedef map<u_int16_t, tlv*>::iterator tlv_iter_t;

    void parseRxPacket(u_int8_t *msg_buf, int buf_len);
    void processRxTLV(u_int8_t *msg_buf, int len);
    void processGroupedTLV(u_int8_t *msg_buf, tlv *currentTLV);
};

#endif // _ari_h
