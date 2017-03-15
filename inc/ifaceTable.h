// -*- Mode: C++; c-basic-offset: 4 -*- //
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: interfaceTable.h
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


#ifndef _ifaceTable_h_
#define _ifaceTable_h_

#include <string>
#include <map>
using namespace std;

#include "netInterface.h"

#define EVT_NEW_IFACE 250
#define EVT_DELETE_IFACE 251

class gllAL;

class InterfaceTable {
    friend class gllAL;
 public:
    InterfaceTable();
    ~InterfaceTable() {}

    //int getLinkQual(string& deviceID_);
    int getDetectedSet(string& deviceID_, vector<struct candidateEntry *> &detectedSet_);
  
    int connectToNetwork(string &deviceID_, string &networkID_, NetInterface **iface_);
    int disconnectFromNetwork(string &deviceID_, NetInterface **iface_);

    NetInterface *getInterface(string &deviceID_);

 private:
    int m_sockFD;

    void checkInterfaces(void);
    bool checkIfaceUP(struct ifreq *ifr);
    int setIfaceUpDown(struct ifreq *ifr, int state);

    int addInterface(struct ifreq *ifr);
    void deleteInterface(string &ifaceName_);

    int getIPaddr(struct ifreq *ifr, NetInterface *iface);
    int getHWaddr(struct ifreq *ifr, NetInterface *iface);
    int getTechType(struct ifreq *ifr, NetInterface *iface);

    map<string, NetInterface*> m_ifaceMap;
    typedef map<string, NetInterface *>::iterator netIface_iter_t;
};

#endif // _ifaceTable_h_
