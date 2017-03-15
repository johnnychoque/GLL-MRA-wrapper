// -*- Mode: C++; c-basic-offset: 4 -*- //

#include <vector>
using namespace std;

#include <net/if_media.h>
#include <dev/wi/if_wavelan_ieee.h>  // struct wi_req

#include "gll_toolbox.h"
#include "netInterface.h"

#define DBG_LEVEL 2
#include "debug.h"

NetInterface::NetInterface(void)
{
    DBG_IN("NetInterface::NetInterface");

    DBG_OUT("NetInterface::NetInterface");
}

NetInterface::~NetInterface(void)
{
    ds_iter_t iter_;
    
    DBG_IN("~NetInterface::NetInterface");
    
    for(iter_ = m_detectedSet.begin() ; iter_ != m_detectedSet.end() ; iter_++) {
	delete (iter_->second);
    }
    
    delete m_hwAddr;

    DBG_OUT("~NetInterface::NetInterface");
}

struct candidateEntry *NetInterface::searchCandidate(string cellID_)
{
    ds_iter_t iter_;
    struct candidateEntry *candidate_;
    
    DBG_IN("NetInterface::searchCandidate");
    
    iter_ = m_detectedSet.find(cellID_);
    if(iter_ != m_detectedSet.end()) {
	candidate_ = iter_->second;
    } else {
	DBG_ERR("Candidate not present");
	candidate_ = NULL;
    }
    
    DBG_OUT("NetInterface::searchCandidate");
    return candidate_;
}

struct candidateEntry *NetInterface::addCandidate(string cellID_)
{
    ds_iter_t iter_;
    struct candidateEntry *candidate_;

    DBG_IN("NetInterface::addCandidate");
    
    candidate_ = searchCandidate(cellID_);
    
    if(!candidate_) {
	candidate_ = new struct candidateEntry;
	m_detectedSet.insert(pair<string, struct candidateEntry *> (cellID_, candidate_));
    } else {
	DBG_INFO("The candidate was already at the detected set");
    }
    
    DBG_OUT("NetInterface::addCandidate");
    return candidate_;
}

int NetInterface::getCandidates(vector<struct candidateEntry *> &detectedSet_)
{
    struct ifreq ifaceAux;
    int ret_;
    int numberCandidates_;
    string cellID_;
    ds_iter_t iter_;
    
    DBG_IN("NetInterface::getCandidates");

    memset(&ifaceAux, 0, sizeof(struct ifreq));
    memcpy(ifaceAux.ifr_name, (char *) m_ifName.c_str(), IFNAMSIZ);	

    numberCandidates_ = getScanInfo(&ifaceAux, m_detectedSet);

    for(iter_ = m_detectedSet.begin() ; iter_ != m_detectedSet.end() ; iter_++) {
	detectedSet_.push_back(iter_->second);
    }

    DBG_OUT("NetInterface::getCandidates");
    return ret_;
}

int NetInterface::connect(string &networkID_)
{
    struct ieee80211req ireq_;
    int ret_;
    int sock_;
    char ssid_[IEEE80211_NWID_LEN];

    DBG_IN("NetInterface::connect");
  
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock_ < 0) {
	DBG_ERR("Could not open the socket");
	ret_ = -1;
	goto connectEND;
    } 

    
    (void) memset(&ireq_, 0, sizeof(struct ifreq));
    (void) memcpy(ireq_.i_name, (char *) m_ifName.c_str(), IFNAMSIZ);
    ireq_.i_type = IEEE80211_IOC_SSID;
    
    memset(ssid_,'\0', IEEE80211_NWID_LEN);
    memcpy(ssid_, (char *) networkID_.c_str(), (int) networkID_.size());
    ireq_.i_data = (void *) ssid_;
    ireq_.i_val = 0; /* ssid?*/
    ireq_.i_len = (int) networkID_.size();
    
    if(ioctl(sock_, SIOCS80211, &ireq_) < 0) {
	DBG_ERR("Error with the ioctl");
	ret_ = -2;
	goto connectEND;
    }

    ret_ = 0;
    DBG_INFO("Connecting to %s",networkID_.c_str());
  
 connectEND:
    DBG_OUT("NetInterface::connect");
    return ret_;
}

int NetInterface::checkAssociated(void)
{
    struct ifmediareq ifmr;
    int ret_;
    int sock_;
    
    DBG_IN("NetInterface::checkAssociated");
    
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(sock_ < 0) {
	DBG_ERR("Could not open the socket");
	ret_ = -1;
	goto checkAssociatedEND;
    }
    
    (void) memset(&ifmr, 0, sizeof(ifmr));
    (void) strncpy(ifmr.ifm_name,  (char *) m_ifName.c_str(), IFNAMSIZ);
    
    if (ioctl(sock_, SIOCGIFMEDIA, (caddr_t)&ifmr) < 0) {
	DBG_ERR("Interface doesn't support SIOC{G,S}IFMEDIA.");
	ret_ = -2;
	goto checkAssociatedEND;
    }

    if (ifmr.ifm_status & IFM_AVALID && ifmr.ifm_status & IFM_ACTIVE) {
	ret_ = 1;
	DBG_INFO("Interface %s is associated",m_ifName.c_str());
    } else {
	ret_ = 0;
    }

 checkAssociatedEND:
    DBG_OUT("NetInterface::checkAssociated");
    return ret_;
    
}

int NetInterface::getCurrentConnection(u_int8_t *bssid_, u_int8_t *ssid_)
{
    struct ieee80211req ireq;
    u_int8_t data[32];
    int ret_;
    int sock_;
    
    DBG_IN("NetInterface::getCurrentConnection");    

    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(sock_ < 0) {
	DBG_ERR("Could not open the socket");
	ret_ = -1;
	goto getCurrentConnectionEND;
    }
    
    // First I gather the SSID
    memset(data, '\0',32);
    (void) memset(&ireq, 0, sizeof(ireq));
    (void) strncpy(ireq.i_name, (char *) m_ifName.c_str(), m_ifName.size());
    ireq.i_data = (void *) data;

    ireq.i_type  = IEEE80211_IOC_SSID;
    if (ioctl(sock_, SIOCG80211, &ireq) < 0) {
	DBG_ERR("Something went bad with the ioctl");
	ret_ = -2;
	goto getCurrentConnectionEND;
    }
    memcpy(ssid_, ireq.i_data, ireq.i_len);
    
    // Then the BSSID
    memset(data, '\0',32);
    (void) memset(&ireq, 0, sizeof(ireq));
    (void) strncpy(ireq.i_name, (char *) m_ifName.c_str(), m_ifName.size());
    ireq.i_data = (void *) data;
    
    ireq.i_type = IEEE80211_IOC_BSSID;
    ireq.i_len = IEEE80211_ADDR_LEN;
    if (ioctl(sock_, SIOCG80211, &ireq) < 0) {
	DBG_ERR("Something went bad with the ioctl");
	ret_ = -2;
	goto getCurrentConnectionEND;
    }
    memcpy(bssid_, ireq.i_data, ireq.i_len);
    DBG_DUMP("BSSID",bssid_,ireq.i_len);
    ret_ = 0;


 getCurrentConnectionEND:
    DBG_OUT("NetInterface::getCurrentConnection");
    return ret_;
}

candidateEntry *NetInterface::searchDetectedAccess(string cellID_)
{
    ds_iter_t iter_;
    candidateEntry *current_;

    DBG_IN("NetInterface::searchDetectedAccess");

    iter_ = m_detectedSet.find(cellID_);
    
    if(iter_ != m_detectedSet.end()) {
	current_ = iter_->second;
    } else {
	current_ = NULL;
    }
    
    DBG_OUT("NetInterface::searchDetectedAccess");
    return current_;
}

candidateEntry *NetInterface::addDetectedAccess(string cellID_)
{
    ds_iter_t iter_;
    candidateEntry *current_;

    DBG_IN("NetInterface::addDetectedAccess");

    current_ = searchDetectedAccess(cellID_);
    
    if(!current_) {
	current_ = new struct candidateEntry;
	m_detectedSet.insert(pair<string, struct candidateEntry *> (cellID_, current_));
    } else {
	DBG_INFO2("Access was already available");
    }

    DBG_OUT("NetInterface::addDetectedAccess");
    return current_;
}


int NetInterface::getLinkQual(void)
{
    struct ifreq ifr;
    struct wi_req wreq;
    int signal, noise;
    int sock_;
    int ret_;

    DBG_IN("NetInterface::getLinkQual");

    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(sock_ < 0) {
	DBG_ERR("Could not open the socket");
	ret_ = -1;
	goto getLinkQualEND;
    }

    bzero((char *)&wreq, sizeof(wreq));
    wreq.wi_len = WI_MAX_DATALEN;
    wreq.wi_type = WI_RID_COMMS_QUALITY;

    bzero((char *)&ifr, sizeof(ifr));
    strlcpy(ifr.ifr_name, (char *) m_ifName.c_str(), IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&wreq;

    if (ioctl(sock_, SIOCGWAVELAN, &ifr) == -1) {
	DBG_ERR("Could not execute ioctl");
	ret_ = -2;
	goto getLinkQualEND;
    }
    
    signal = wreq.wi_val[1];
    noise = wreq.wi_val[2];
    ret_ = signal - noise;
    
 getLinkQualEND:
    DBG_OUT("NetInterface::getLinkQual");
    return ret_;
}

int NetInterface::checkIfaceUP(void)
{
    int sock_;
    int ret_;
    struct ifreq ifr;
    
    DBG_IN("NetInterface::checkIfaceUP");

    sock_ = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock_ < 0) {
	DBG_ERR("Could not open the socket");
	ret_ = -1;
	goto checkIfaceUPEND;
    }
    
    memset(&ifr, 0, sizeof(struct ifreq));
    memcpy(ifr.ifr_name, (char *) m_ifName.c_str(), IFNAMSIZ);
    
    if (ioctl(sock_, SIOCGIFFLAGS, &ifr) == -1) {
	DBG_ERR("Could not execute ioctl");
	ret_ = -1;
	goto checkIfaceUPEND;
    }

    if(ifr.ifr_flags & (IFF_UP | IFF_RUNNING)) {
	ret_ = 1;
    } else {
	ret_ = 0;
    }
    

 checkIfaceUPEND:    
    DBG_OUT("NetInterface::checkIfaceUP");
    return ret_;
}

int NetInterface::setIfaceUpDown(int state)
{
    int m_sockFD;
    struct ifreq ifr;
    int flags, value = (state==UP)?IFF_UP:-IFF_UP;

    DBG_IN("NetInterface::setIfaceUpDown");

    memset(&ifr, 0, sizeof(struct ifreq));
    memcpy(ifr.ifr_name, (char *)m_ifName.c_str(), IFNAMSIZ);

    ioctl(m_sockFD, SIOCGIFFLAGS, &ifr);
    
    flags = (ifr.ifr_flags & 0xffff) | (ifr.ifr_flagshigh << 16);

    if (value < 0) {
	value = -value;
	flags &= ~value;
    } else {
	flags |= value;
    }
    ifr.ifr_flags = flags & 0xffff;
    ifr.ifr_flagshigh = flags >> 16;
    ioctl(m_sockFD, SIOCSIFFLAGS, &ifr);
    
    DBG_OUT("NetInterface::setIfaceUpDown");
    return 0;
}
