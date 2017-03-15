// -*- Mode: C++; c-basic-offset: 4 -*- //

#ifndef _netInterface_h_
#define _netInterface_h_

#include <netinet/in.h>

#include <string>
#include <vector>
#include <map>
using namespace std;

#include "candidateEntry.h"

// Technology Type (as specified in the MGI document)
#define Tech_UNKNOWN                          0x0000

#define Tech_FIXED_Generic                    0x0100
#define Tech_FIXED_Ethernet                   0x0101
#define Tech_FIXED_DSL                        0x0102

#define Tech_GSM_Generic                      0x0200

#define Tech_UMTS_Generic                     0x0300
#define Tech_UMTS_3GPP_UTRAN_FDD              0x0301
#define Tech_UMTS_3GPP_UTRAN_TDD              0x0302
#define Tech_UMTS_3GPP_General_HSDPA          0x0304
#define Tech_UMTS_3GPP_UTRAN_FDD_HSDPA        0x0305
#define Tech_UMTS_3GPP_UTRAN_TDD_HSDPA        0x0306
#define Tech_UMTS_3GPP_General_HSUPA          0x0308
#define Tech_UMTS_3GPP_UTRAN_FDD_HSDPA_HSUPA  0x030D
#define Tech_UMTS_3GPP_UTRAN_TDD_HSDPA_HSUPA  0x030D

#define Tech_WLAN_Generic                     0x0400
#define Tech_WLAN_80211a                      0x0401
#define Tech_WLAN_80211b                      0x0402
#define Tech_WLAN_80211ab                     0x0403
#define Tech_WLAN_80211g                      0x0404
#define Tech_WLAN_80211abg                    0x0407

#define Tech_WIMAX_Generic                    0x0500

// States of an interface
#define UP                                    0x01
#define DOWN                                  0x02
#define UNKNOWN                               0x03

class InterfaceTable;

class NetInterface {
    friend class InterfaceTable;
public:
    NetInterface();
    ~NetInterface();
    
    inline int &state(void) { return m_state; }

    string &getIFname(void) { return m_ifName; }
    u_int8_t *getHWaddr(void) { return m_hwAddr; }
    u_int16_t getTechType(void) { return m_techType; }

    struct candidateEntry *searchCandidate(string cellID_);
    struct candidateEntry *addCandidate(string cellID_);

    int getCandidates(vector<struct candidateEntry *> &detectedSet_);
    int connect(string &networkID_);
    int getLinkQual(void);

    int checkAssociated(void);
    int getCurrentConnection(u_int8_t *bssid_, u_int8_t *ssid_);

    int checkIfaceUP(void); // No usado? ya existe en ifaceTable.h
    int setIfaceUpDown(int state);

    //    void assignLocator();

private:
    string m_ifName;
    struct in_addr m_ipAddr;

    u_int8_t *m_hwAddr;
    u_int16_t m_techType;

    map<string, struct candidateEntry *> m_detectedSet;
    typedef map<string, struct candidateEntry *>::iterator ds_iter_t;

    candidateEntry *searchDetectedAccess(string cellID_);
    candidateEntry *addDetectedAccess(string cellID_);

    int m_state;
};


#endif // _netInterface_h_
