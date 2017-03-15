// -*- Mode: C++; c-basic-offset: 4 -*- //
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: ifaceTable.cc
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

#include <sys/socket.h>
#include <sys/ioctl.h>

#if defined(__linux__)
#include <linux/if.h>
#include <linux/wireless.h>
#elif defined(__FreeBSD__)
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h> //LLADDR
#include <net/ethernet.h>
#include <net/if_media.h>
#endif

#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include "scheduler.h"
#include "ifaceTable.h"
//#include "gll_toolbox.h"

#include <arpa/inet.h>

#define DBG_LEVEL 2
#include "debug.h"

InterfaceTable::InterfaceTable(void)
{
    DBG_IN("InterfaceTable::InterfaceTable");

    m_sockFD = socket(AF_INET, SOCK_DGRAM, 0);

    DBG_OUT("InterfaceTable::InterfaceTable");
}

void InterfaceTable::checkInterfaces(void)
{
    struct ifconf ifc;
    struct ifreq *ifr;
    char buff[1024];
    int remainingSize, currentSize;
    string devID_;
    NetInterface *currentIface_;
    netIface_iter_t iter_;

    DBG_IN("InterfaceTable::checkInterfaces");

    ifc.ifc_len = sizeof(buff);
    ifc.ifc_buf = buff;
  
    if(ioctl(m_sockFD, SIOCGIFCONF, &ifc) < 0) {
	DBG_ERR("Error with... ioctl (SIOCGIFCONF) - %s",strerror(errno));
    }

    ifr = ifc.ifc_req;
    remainingSize = ifc.ifc_len;
    
    for(iter_ = m_ifaceMap.begin() ; iter_ != m_ifaceMap.end() ; iter_++) {
	currentIface_ = iter_->second;
	currentIface_->state() = UNKNOWN;
    }

    while(remainingSize) {
	devID_ = ifr->ifr_name;
#if defined(__linux__)
	if(devID_ != "lo") {  // We're not interested in the Loopback device
#elif defined(__FreeBSD__)
	if(devID_ != "lo0") {  // We're not interested in the Loopback device
#endif
	    if(checkIfaceUP(ifr)) {  // Check whether the iface is UP
		if((iter_ = m_ifaceMap.find(devID_)) == m_ifaceMap.end()) {  // New device
		    addInterface(ifr);
		} else {
		    currentIface_=iter_->second;
		    currentIface_->state() = UP;
		}
		
	    }
	}
	
#ifdef __FreeBSD__
	currentSize = ifr->ifr_addr.sa_len + IFNAMSIZ;
#else
	currentSize = sizeof(struct ifreq);
#endif // __FreeBSD__
	
	ifr = (struct ifreq *) (((char *) ifr) + currentSize);
	remainingSize -= currentSize;
    }

	for(iter_ = m_ifaceMap.begin() ; iter_ != m_ifaceMap.end() ; iter_++) {
	    currentIface_ = iter_->second;
	    if(currentIface_->state() == UNKNOWN) {
		DBG_INFO("Dispatch event to erase INTERFACE");
		scheduler::instance()->dispatchEvent(EVT_DELETE_IFACE, (void *) currentIface_);
	    }
	}
	
    DBG_OUT("InterfaceTable::checkInterfaces");
}

bool InterfaceTable::checkIfaceUP(struct ifreq *ifr)
{
    struct ifreq ifaceAux;
    bool ret_ = false;

    DBG_IN("InterfaceTable::checkIfaceUP");

    memset(&ifaceAux, 0, sizeof(struct ifreq));
    memcpy(ifaceAux.ifr_name, ifr->ifr_name, IFNAMSIZ);

    ioctl(m_sockFD, SIOCGIFFLAGS, &ifaceAux);
    if(ifaceAux.ifr_flags & (IFF_UP | IFF_RUNNING)) {
	ret_ = true;
    }
    
    DBG_OUT("InterfaceTable::checkIfaceUP");
    return ret_;
}

 int InterfaceTable::setIfaceUpDown(struct ifreq *ifr, int state)
{
    struct ifreq ifaceAux;
    int flags, value = (state==UP)?IFF_UP:-IFF_UP;

    DBG_IN("InterfaceTable::setIfaceUpDown");

    memset(&ifaceAux, 0, sizeof(struct ifreq));
    memcpy(ifaceAux.ifr_name, ifr->ifr_name, IFNAMSIZ);

    ioctl(m_sockFD, SIOCGIFFLAGS, &ifaceAux);
    
    flags = (ifr->ifr_flags & 0xffff) | (ifr->ifr_flagshigh << 16);

    if (value < 0) {
	value = -value;
	flags &= ~value;
    } else {
	flags |= value;
    }
    ifr->ifr_flags = flags & 0xffff;
    ifr->ifr_flagshigh = flags >> 16;
    ioctl(m_sockFD, SIOCSIFFLAGS, &ifaceAux);
    
    DBG_OUT("InterfaceTable::setIfaceUpDown");
    return 0;
}

int InterfaceTable::addInterface(struct ifreq *ifr)
{
    NetInterface *newIface_;

    DBG_IN("InterfaceTable::addInterface");

    /*    succeeded = producer.Register(100);

    trigger.setID(100);
    trigger.setValue("SNR");
    trigger.setType(69);
    
    succeeded = producer.send( trigger );
    if ( !succeeded ) {
	std::cout << "\nCouldn't send trigger to Triggering framework\n";
	std::cout << producer.getError();
	}*/

    newIface_ = new NetInterface;

    newIface_->m_ifName = ifr->ifr_name;
    newIface_->state() = UP;

    DBG_INFO2("Iface name: %s", newIface_->m_ifName.c_str());
    getTechType(ifr,newIface_);

    if ( newIface_->m_techType == Tech_FIXED_Ethernet) {
	DBG_INFO("new FIXED interface --- now we don't discard it");
    }

    getIPaddr(ifr,newIface_);
    getHWaddr(ifr,newIface_);

    /*    system("/home/jchoque/AN/gll-v0.3/scripts/iwiset.sh down");
	  system("/home/jchoque/AN/gll-v0.3/scripts/iwiset.sh up"); */

    /*if (newIface_->m_techType != Tech_FIXED_Ethernet)
	do {
	    vector<struct apdata *> ap;
	    int ap_num;
	    
	    ap_num = getScanInfo(ifr, ap);
	    if (ap_num < 0) {
		DBG_ERR("Error getting scan info");
		return -1;
	    }
	    
	    printScanInfo(ap);
	    deleteScanInfo(ap);
	    } while (0);*/
    
    //system("/home/jchoque/AN/gll-v0.3/scripts/iwiset.sh connsec");
    //snr_val = getSNR(ifr->ifr_name);

    //if (newIface_->m_techType != Tech_FIXED_Ethernet) {
    DBG_INFO2("Dispatch event for new interface");
    m_ifaceMap.insert(pair<string,NetInterface*> (newIface_->m_ifName,newIface_));
    scheduler::instance()->dispatchEvent(EVT_NEW_IFACE, (void *) newIface_);
    //}
	/*    else {
	DBG_INFO("new FIXED interface discard");
	}*/

    DBG_OUT("InterfaceTable::addInterface");
    return 0;
}

/*
int InterfaceTable::getLinkQual(string& deviceID_)
{
    int snr_;
    netIface_iter_t iter_;

    DBG_IN("InterfaceTable::getLinkQual");

    iter_ = m_ifaceMap.find(deviceID_);
    if(iter_ != m_ifaceMap.end()) {
	//snr_ = getSNR((char *) deviceID_.c_str());
    } else {
	DBG_ERR("Device %s is not present",deviceID_.c_str());
	snr_ = -99;
    }
    
    DBG_OUT("InterfaceTable::getLinkQual");
    return snr_;
}
*/

int InterfaceTable::getIPaddr(struct ifreq *ifr, NetInterface *iface)
{
    struct ifreq ifaceAux;
    struct sockaddr_in *ipAddrAux;

    DBG_IN("InterfaceTable::getIPaddr");

    memset(&ifaceAux, 0, sizeof(struct ifreq));
    memcpy(ifaceAux.ifr_name, ifr->ifr_name, IFNAMSIZ);

    ioctl(m_sockFD, SIOCGIFADDR, &ifaceAux);
    ipAddrAux = (struct sockaddr_in *) &ifaceAux.ifr_addr;
    iface->m_ipAddr = ipAddrAux->sin_addr;

    DBG_INFO2("IP address: %s",inet_ntoa(iface->m_ipAddr));

    DBG_OUT("InterfaceTable::getIPaddr");
    return 0;
} 

int InterfaceTable::getDetectedSet(string &deviceID_, vector<struct candidateEntry *> &detectedSet_)
{
    int ret_;
    netIface_iter_t iter_;
    NetInterface *current_;
    
    DBG_IN("InterfaceTable::getDetectedSet");

    iter_ = m_ifaceMap.find(deviceID_);
    
    if(iter_ != m_ifaceMap.end()) {
	current_ = iter_->second;
	
	ret_ = current_->getCandidates(detectedSet_);
    } else {
	DBG_ERR("The device %s was not registered",deviceID_.c_str());
	ret_ = -1;
    }
	

    DBG_OUT("InterfaceTable::getDetectedSet");
    return ret_;
}

int InterfaceTable::getHWaddr(struct ifreq *ifr, NetInterface *iface)
{
#if defined(__FreeBSD__)
    int mib[6];
    struct if_msghdr *ifm;
    struct sockaddr_dl *sdl;
    int ifindex;
    char name[IFNAMSIZ];
    size_t needed;
    char *buf;
#elif defined(__linux__)
    struct ifreq ifaceAux;
#endif
    unsigned char *u;

    DBG_IN("InterfaceTable::getHWaddr");

#if defined(__FreeBSD__)
    strncpy(name, ifr->ifr_name, sizeof(name));
    ifindex = if_nametoindex(name);

    mib[0] = CTL_NET;
    mib[1] = PF_ROUTE;
    mib[2] = 0;
    mib[3] = 0;      	/* address family */
    mib[4] = NET_RT_IFLIST;
    mib[5] = ifindex;		/* interface index */

    if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
	DBG_ERR("iflist-sysctl-estimate");
    if ((buf = (char *)malloc(needed)) == NULL)
	DBG_ERR("malloc");
    if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
	DBG_ERR("actual retrieval of interface table");
    }

    ifm = (struct if_msghdr *)buf;
    sdl = (struct sockaddr_dl *)((char *)ifm + sizeof(struct if_msghdr) -
				 sizeof(struct if_data) + ifm->ifm_data.ifi_datalen);

    iface->m_hwAddr = new u_int8_t [ETHER_ADDR_LEN];
    memcpy(iface->m_hwAddr, (u_int8_t *)LLADDR(sdl), ETHER_ADDR_LEN);
    free(buf);
#elif defined(__linux__)
    memset(&ifaceAux, 0, sizeof(struct ifreq));
    memcpy(ifaceAux.ifr_name, ifr->ifr_name, IFNAMSIZ);

    ioctl(m_sockFD, SIOCGIFHWADDR, &ifaceAux);

    iface->m_hwAddr = new u_int8_t[IFHWADDRLEN];
    memcpy(iface->m_hwAddr, (u_int8_t *) ifaceAux.ifr_hwaddr.sa_data, IFHWADDRLEN);
#endif
    /*******************************/
    u = (unsigned char *) iface->m_hwAddr;

    if (u[0] + u[1] + u[2] + u[3] + u[4] + u[5]) {
      DBG_INFO2("HW Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
	       u[0], u[1], u[2], u[3], u[4], u[5]);
    }
    /************************************/

    DBG_OUT("InterfaceTable::getHWaddr");
    return 0;
    
}

int InterfaceTable::getTechType(struct ifreq *ifr, NetInterface *iface) 
{

#if defined(__FreeBSD__)
    struct ifmediareq ifmr;
    struct ifmedia_description *desc;
    struct ifmedia_description ifm_type_descriptions[] = IFM_TYPE_DESCRIPTIONS;
#elif defined(__linux__)
    struct iwreq pwrq;
#endif
    int ret_ = 0;
  
    DBG_IN("InterfaceTable::getTechType");

#if defined(__FreeBSD__)
    (void) memset(&ifmr, 0, sizeof(ifmr));
    (void) strncpy(ifmr.ifm_name, ifr->ifr_name, sizeof(ifmr.ifm_name));
  
    if (ioctl(m_sockFD, SIOCGIFMEDIA, (caddr_t)&ifmr) < 0) {
	/* Interface doesn't support SIOC{G,S}IFMEDIA.*/
	ret_ = -1;
    } else {

	/* Find the top-level interface type. */
	for (desc = ifm_type_descriptions; desc->ifmt_string != NULL; desc++)
	    if (IFM_TYPE(ifmr.ifm_current) == desc->ifmt_word)
		break;
      
	if (strstr(desc->ifmt_string, "Wireless") == NULL) {
	    iface->m_techType = Tech_FIXED_Ethernet;
	} else {
	    iface->m_techType = Tech_WLAN_Generic;
	}
    }
#elif defined(__linux__)
    strncpy(pwrq.ifr_name, ifr->ifr_name, IFNAMSIZ);
    if (ioctl(m_sockFD, SIOCGIWNAME, &pwrq) < 0) {
	iface->m_techType = Tech_FIXED_Ethernet;
    }  else {
      if (!strcmp(pwrq.u.name,"IEEE 802.11a")) {
	iface->m_techType = Tech_WLAN_80211a;
      }
      else if (!strcmp(pwrq.u.name,"IEEE 802.11b")) {
	iface->m_techType = Tech_WLAN_80211b;
      }
      else if (!strcmp(pwrq.u.name,"IEEE 802.11g")) {
	iface->m_techType = Tech_WLAN_80211g;
      }
      else {
	iface->m_techType = Tech_WLAN_Generic;
      }
    }
#endif

    DBG_INFO2("m_techType = %X", iface->m_techType);
    DBG_OUT("InterfaceTable::getTechType");
    return ret_;
}

int InterfaceTable::connectToNetwork(string &deviceID_, string &networkID_, NetInterface **iface_)
{
    int ret_;
    netIface_iter_t iter_;
    NetInterface *current_;

    DBG_IN("InterfaceTable::connectToNetwork");

    iter_ = m_ifaceMap.find(deviceID_);

    if(iter_ != m_ifaceMap.end()) {
	current_ = iter_->second;
	ret_ = current_->connect(networkID_);
	*iface_ = current_;
	
    } else {
	DBG_ERR("The device %s was not registered at the GLL AL",deviceID_.c_str());
	ret_ = -1;
	*iface_ = NULL;
    }

    DBG_OUT("InterfaceTable::connectToNetwork");
    return ret_;
}

int InterfaceTable::disconnectFromNetwork(string &deviceID_, NetInterface **iface_)
{
    int ret_;
    netIface_iter_t iter_;
    NetInterface *current_;

    DBG_IN("InterfaceTable::disconnectFromNetwork");

    iter_ = m_ifaceMap.find(deviceID_);

    if(iter_ != m_ifaceMap.end()) {
	current_ = iter_->second;
	ret_ = current_->setIfaceUpDown(DOWN);
	*iface_ = current_;
	
    } else {
	DBG_ERR("The device %s was not registered at the GLL AL",deviceID_.c_str());
	ret_ = -1;
	*iface_ = NULL;
    }

    DBG_OUT("InterfaceTable::disconnectFromNetwork");
    return ret_;
}

NetInterface *InterfaceTable::getInterface(string &deviceID_)
{
    NetInterface *ret_;
    netIface_iter_t iter_;
   
    DBG_IN("InterfaceTable::getInterface");
    
    ret_ = NULL;

    iter_ = m_ifaceMap.find(deviceID_);

    if(iter_ != m_ifaceMap.end()) {
	ret_ = iter_->second;
    }

    DBG_OUT("InterfaceTable::getInterface");
    return ret_;
}

void InterfaceTable::deleteInterface(string &deviceID_)
{
    netIface_iter_t iter_;
    
    DBG_IN("InterfaceTable::deleteInterface");

    iter_ = m_ifaceMap.find(deviceID_);
    
    if(iter_ != m_ifaceMap.end()) {
	m_ifaceMap.erase(iter_);

    } else {
	DBG_ERR("There was not such interface detected");
    }

    DBG_OUT("InterfaceTable::deleteInterface");
}
