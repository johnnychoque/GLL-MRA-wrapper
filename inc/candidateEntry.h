/* -*- Mode: C++; c-basic-offset: 4; tab-width: 8; indent-tabs-mode: nil  -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: candidateEntry.h
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
#ifndef candidateEntry_h_
#define candidateEntry_h_

#include <sys/socket.h>

#if defined(__linux__)
#include <linux/if.h>
#define IFACEHWADDRLEN IFHWADDRLEN
#elif defined(__FreeBSD__)
#include <sys/param.h>
#include <net/if.h>
#include <net/ethernet.h>
#define IFACEHWADDRLEN ETHER_ADDR_LEN
#endif

#include <string>
using namespace std;

struct candidateEntry {
    string m_networkID;
    u_int8_t m_cellID[IFACEHWADDRLEN];
    u_int8_t m_channel;
    u_int8_t m_mode;
    u_int8_t m_rssi;
};    

#endif    // candidateEntry_h_
