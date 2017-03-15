#ifndef ANTriggerPacket_h
#define ANTriggerPacket_h

#include <ctime> //<time.h>
#ifndef WIN32
#include <netinet/in.h>
#ifdef __FreeBSD__
#include <sys/endian.h>
#else
#define bswap64(x) __bswap_64(x)
#endif
#else
#include <winsock2.h>
#endif

#include <list>
#include <string>
#include <cstdlib>
#include "ANTrigger.h"
using namespace std;

#ifdef WIN32
typedef unsigned short int uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long int uint32_t;
typedef unsigned long int uint64_t;
#endif

typedef uint16_t trg_id_t;
#define htonid(s) htons((s))
#define ntohid(s) ntohs((s))

// the following might be GNU-specific
#ifndef ntohll
 #ifdef WIN32 
  #define ntohll(x) (x) // probably won't work but won't complain either :)
 #elif __CYGWIN32__
   #define ntohll(x) (x)
 #else
  #if __BYTE_ORDER == __LITTLE_ENDIAN
   /*
   #define ntohll(x) (((uint64_t)(ntohl((int)((x << 32) >> 32))) << 32) | \
                      (unsigned int) ntohl(((int)(x >> 32))))
   */
   #define ntohll(x) bswap64(x)
  #else
   #define ntohll(x) (x)
  #endif
 #endif // WIN32 - else
 #define htonll(x) ntohll(x)
#endif

enum TRGSource {
	SRC_NONUSED,
	SRC_ASI,
	SRC_ANI,
	SRC_ARI,
	SRC_ACS,
	SRC_UNKNOWN
};

struct ANTriggerHeader {

	trg_id_t triggerID;
	trg_id_t groupID;
	uint16_t source;	// trg source: ASI, ANI, ARI, ACS, unknown, non-used
	time_t timestamp;  	// in seconds since epoch
	uint32_t lifetime; 	// lifetime in seconds
	uint16_t dataLen;	// in bytes

#ifndef WIN32
} __attribute__((packed));
#else
};
#endif


class ANTriggerPacket {

	public:
	
	ANTriggerPacket() { m_dataLen = 0; m_hdr = NULL; m_data = NULL; m_allocated = 0; };
	~ANTriggerPacket();
	
	/**
	 * Allocate memory for a ANTriggerPacket with no data
	 */
	static inline ANTriggerPacket *allocPacket();

	/**
	 * Allocate memory for a ANTriggerPacket with given data size
	 */
	static inline ANTriggerPacket *allocPacket(size_t dataLen);

	/**
	 * Allocate a ANTriggerPacket from existing data
	 */
	static inline ANTriggerPacket *createFromData(void *data, size_t dataLen);

	/**
	 * Allocate and copy a ANTriggerPacket from data
	 */
	static inline ANTriggerPacket *copyFromData(void *data, size_t dataLen);

	struct ANTriggerHeader *m_hdr;
	size_t m_dataLen;
	uint8_t *m_data;

	// indicates whether the header&data was allocated by us or not
	uint8_t m_allocated;

};

inline ANTriggerPacket *ANTriggerPacket::allocPacket()
{
	void *voidPtr = NULL;
	ANTriggerPacket *pkt = NULL;
	voidPtr = malloc(sizeof(struct ANTriggerHeader));
	if (voidPtr != NULL) {
		memset(voidPtr, 0, sizeof(struct ANTriggerHeader));
		pkt = new ANTriggerPacket();
		pkt->m_dataLen = 0;
		pkt->m_hdr = (struct ANTriggerHeader *) voidPtr;
		pkt->m_data = NULL;
		pkt->m_allocated = 1;
	}
	return (pkt);
}

inline ANTriggerPacket *ANTriggerPacket::allocPacket(size_t dataLen)
{
	void *voidPtr = NULL;
	ANTriggerPacket *pkt = NULL;
	if (dataLen <= 0) {
		return allocPacket();
	}
	// alloc mem for header + data
	voidPtr = malloc(sizeof(struct ANTriggerHeader) + dataLen);
	if (voidPtr != NULL) {
		memset(voidPtr, 0, sizeof(struct ANTriggerHeader) + dataLen);
		pkt = new ANTriggerPacket();
		pkt->m_dataLen = dataLen;
		pkt->m_hdr = (struct ANTriggerHeader *) voidPtr;
		pkt->m_data = (uint8_t *) voidPtr + sizeof(struct ANTriggerHeader);
		pkt->m_allocated = 1;
	}
	return (pkt);
}

inline ANTriggerPacket *ANTriggerPacket::createFromData(void *data, size_t dataLen)
{
	ANTriggerPacket *pkt = NULL;

	if (dataLen < sizeof(struct ANTriggerHeader))
		return NULL;

	pkt = new ANTriggerPacket();
	if (NULL == pkt)
		return NULL;

	pkt->m_hdr = (struct ANTriggerHeader *) data;
	pkt->m_data = (uint8_t *) ((uint8_t *) data + sizeof(struct ANTriggerHeader));
	if ((size_t) ntohs(pkt->m_hdr->dataLen) > dataLen - sizeof(struct ANTriggerHeader)) {
		//printf("Warning: trigger data length had to be shortened\n");
		pkt->m_dataLen = dataLen - sizeof(struct ANTriggerHeader);
		pkt->m_hdr->dataLen = htons(pkt->m_dataLen);
		pkt->m_allocated = 0;
	}
	return pkt;
}

inline ANTriggerPacket *ANTriggerPacket::copyFromData(void *data, size_t dataLen)
{
	ANTriggerPacket *pkt = NULL;
	size_t allocLen = dataLen;

	if (dataLen < sizeof(struct ANTriggerHeader))
		return NULL;

	allocLen = dataLen - sizeof(struct ANTriggerHeader);

	pkt = ANTriggerPacket::allocPacket(allocLen);
	if (NULL == pkt)
		return NULL;

	memcpy(pkt->m_hdr, data, dataLen);

	pkt->m_data = (uint8_t *) ((uint8_t *) pkt->m_hdr + sizeof(struct ANTriggerHeader));
	if ((size_t)ntohs(pkt->m_hdr->dataLen) > allocLen) {
		//printf("Warning: trigger data length had to be shortened\n");
		pkt->m_dataLen = allocLen;
		pkt->m_hdr->dataLen = htons(pkt->m_dataLen);
		pkt->m_allocated = 1;
	}
	return pkt;
}

#endif
