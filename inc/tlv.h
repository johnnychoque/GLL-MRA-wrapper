#ifndef _tlv_h_
#define _tlv_h_

#include <vector>
#include <string>
using namespace std;

#include "config.h"

#define MAX_OCTEC_STRING 32
#define MAX_CHUNK_SIZE 1024
#define MAX_UVALUE_LEN 1024

enum {
    TYPE_OctecString,
    TYPE_UnsignedVariableLength,
    TYPE_Unsigned8,
    TYPE_Unsigned16,
    TYPE_Unsigned32,
    TYPE_Unsigned64,
    TYPE_Integer32,
    TYPE_Integer64,
    TYPE_Float32,
    TYPE_Float64,
    TYPE_Grouped,
    TYPE_Address,
    TYPE_Time,
    TYPE_UTF8String,
    TYPE_Enumerated,
};
    


/* struct tlvHead { */
/* #ifdef LITTLEENDIAN */
/*     u_int16_t type:12, res1:2, B:1, A:1; */
/*     u_int16_t length: 12, res2:4; */
/* #else */
/*     u_int16_t A:1, B:1, res1:2, type:12; */
/*     u_int16_t res2:4, length:12; */
/* #endif */
/* }; */

struct tlvHead {
    u_int16_t type;
    u_int16_t length;
};

typedef union paraValue {
    u_int8_t os_data[MAX_OCTEC_STRING];
    u_int8_t u8_data;
    u_int16_t u16_data;
    u_int32_t u32_data;
    u_int64_t u64_data;
    s_int32_t s32_data;
    s_int64_t s64_data;
} u_paraValue_t;
    
class MGI_packet;
class ARI_message;
class WSI_packet;

class tlv {
    friend class MGI_packet;
    friend class WSI_packet;
    friend class ARI_message;
 public:
    tlv();
    ~tlv();
    
    inline u_int16_t &type(void) {return type_;}
    inline u_int16_t &length(void) {return length_;}
    inline u_int16_t &format(void) {return format_;}
    
    int serialize(u_int8_t *msg_buf);

    void setEnumerated(u_int32_t *data);
    void setEnumerated(u_int32_t *data, int len);
    void rxEnumerated(u_int32_t *data);

    void setGrouped(void);
    void addTLV(void);
    void rxGrouped(u_int8_t *data);

    void setSigned32(s_int32_t *data);
    void rxSigned32(s_int32_t *data);

    void setOctecString(u_int8_t *data);
    void setOctecString(u_int8_t *data, int len);
    void rxOctecString(u_int8_t *data);

    void setUnsigned8(u_int8_t *data);
    void rxUnsigned8(u_int8_t *data);
    
    void setUnsigned16(u_int16_t *data);
    void rxUnsigned16(u_int16_t *data);

    void setUnsigned32(u_int32_t *data);
    void rxUnsigned32(u_int32_t *data);

    void setUnsigned64(u_int64_t *data);
    void rxUnsigned64(u_int64_t *data);

    void setUnsignedVariableLength(u_int32_t *data);
    void setUnsignedVariableLength(u_int8_t *data, int len);
    void rxUnsignedVariableLength(u_int8_t *data);

    void setUTF8String(u_int8_t *data);
    void setUTF8String(u_int8_t *data, int len);
    void rxUTF8String(u_int8_t *data);

    int addGroupedTLV(tlv *avp);

 private:
    u_int16_t type_;
    u_int16_t length_;

    u_int16_t format_;

    u_int8_t u8_data_;
    u_int16_t u16_data_;
    u_int32_t u32_data_;
    u_int64_t u64_data_;

    s_int32_t s32_data_;

    u_int8_t uvl_data_[MAX_UVALUE_LEN];
    u_int8_t *octecString_data;

    vector<tlv*> grouped_;
};


#endif  // _tlv_h_
