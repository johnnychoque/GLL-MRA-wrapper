#ifndef _mgi_packet_h
#define _mgi_packet_h

#include <map>
using namespace std;

#include "tlv.h"
#include "gll_common.h"
#include "mgi.h"

class gllIM;

class MGI_packet {
    friend class gllIM;
 public:
    MGI_packet();
    MGI_packet(u_int8_t *msg_buf, int buf_len);
    ~MGI_packet();

    inline u_int8_t version(void) {return header_.version;}
    inline u_int8_t gistHops(void) {return header_.gistHops;}
    inline u_int16_t msgLength(void) {return header_.msgLength;}
    inline u_int16_t nslpID(void) {return header_.nslpID;}
    inline u_int8_t type(void) {return header_.type;}

    inline bool getSflag(void) {return header_.s_;}
    inline bool getRflag(void) {return header_.r_;}
    inline bool getEflag(void) {return header_.e_;}

    void setHeader(u_int16_t nslpID, u_int8_t flags);

    void addTLV(u_int16_t type, u_paraValue_t *data = NULL, int len_ = 0);
    int addTLVToGrouped(u_int16_t mainType, u_int16_t type, u_paraValue_t *data);
    
    int getMeasConfiguration(struct measConfiguration *measConfig);
 
    int serialize(u_int8_t *msg_buf, int buf_len);

    void getDataFromTLV(u_int16_t type, void *data, int *len);

 private:
    struct mgiHeader header_;
    map<u_int16_t, tlv*> params_;
    typedef map<u_int16_t, tlv*>::iterator tlv_iter_t;

    void parseRxPacket(u_int8_t *msg_buf, int buf_len);
    void processRxTLV(u_int8_t *msg_buf, int len);
    void processGroupedTLV(u_int8_t *msg_buf, tlv *currentTLV);
};

#endif // _mgi_packet_h
