// -*- Mode: C++; c-basic-indent: 4 -*- //

#ifndef _ifaceScan_h_
#define _ifaceScan_h_

#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <string.h>

#include <iostream>
#include <vector>
#include <map>
#include <new>
using namespace std;

#include "candidateEntry.h"

#define INFRA 0
#define ADHOC 1
#define UNKN  2

struct apdata {
    string ssid;
    string bssid;
    int channel;
    int mode;
    u_int8_t rssi;
};

#ifdef __linux__
#include <stdio.h>
#include <errno.h>
#include <stdlib.h> //malloc
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <net/ethernet.h>

/* Type of headers we know about (basically union iwreq_data) */
#define IW_HEADER_TYPE_NULL	0	/* Not available */
#define IW_HEADER_TYPE_CHAR	2	/* char [IFNAMSIZ] */
#define IW_HEADER_TYPE_UINT	4	/* __u32 */
#define IW_HEADER_TYPE_FREQ	5	/* struct iw_freq */
#define IW_HEADER_TYPE_ADDR	6	/* struct sockaddr */
#define IW_HEADER_TYPE_POINT	8	/* struct iw_point */
#define IW_HEADER_TYPE_PARAM	9	/* struct iw_param */
#define IW_HEADER_TYPE_QUAL	10	/* struct iw_quality */

/* Headers for the various requests */
static const char standard_ioctl_hdr[] = {
    IW_HEADER_TYPE_NULL,	/* SIOCSIWCOMMIT */
    IW_HEADER_TYPE_CHAR,	/* SIOCGIWNAME */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWNWID */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWNWID */
    IW_HEADER_TYPE_FREQ,	/* SIOCSIWFREQ */
    IW_HEADER_TYPE_FREQ,	/* SIOCGIWFREQ */
    IW_HEADER_TYPE_UINT,	/* SIOCSIWMODE */
    IW_HEADER_TYPE_UINT,	/* SIOCGIWMODE */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWSENS */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWSENS */
    IW_HEADER_TYPE_NULL,	/* SIOCSIWRANGE */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWRANGE */
    IW_HEADER_TYPE_NULL,	/* SIOCSIWPRIV */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWPRIV */
    IW_HEADER_TYPE_NULL,	/* SIOCSIWSTATS */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWSTATS */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWSPY */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWSPY */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWTHRSPY */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWTHRSPY */
    IW_HEADER_TYPE_ADDR,	/* SIOCSIWAP */
    IW_HEADER_TYPE_ADDR,	/* SIOCGIWAP */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWMLME */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWAPLIST */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWSCAN */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWSCAN */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWESSID */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWESSID */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWNICKN */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWNICKN */
    IW_HEADER_TYPE_NULL,	/* -- hole -- */
    IW_HEADER_TYPE_NULL,	/* -- hole -- */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWRATE */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWRATE */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWRTS */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWRTS */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWFRAG */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWFRAG */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWTXPOW */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWTXPOW */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWRETRY */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWRETRY */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWENCODE */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWENCODE */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWPOWER */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWPOWER */
    IW_HEADER_TYPE_NULL,	/* -- hole -- */
    IW_HEADER_TYPE_NULL,	/* -- hole -- */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWGENIE */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWGENIE */
    IW_HEADER_TYPE_PARAM,	/* SIOCSIWAUTH */
    IW_HEADER_TYPE_PARAM,	/* SIOCGIWAUTH */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWENCODEEXT */
    IW_HEADER_TYPE_POINT,	/* SIOCGIWENCODEEXT */
    IW_HEADER_TYPE_POINT,	/* SIOCSIWPMKSA */
    IW_HEADER_TYPE_NULL,	/* -- hole -- */
};

static const unsigned int standard_ioctl_num = sizeof(standard_ioctl_hdr);

static const char	standard_event_hdr[] = {
    IW_HEADER_TYPE_ADDR,	/* IWEVTXDROP */
    IW_HEADER_TYPE_QUAL,	/* IWEVQUAL */
    IW_HEADER_TYPE_POINT,	/* IWEVCUSTOM */
    IW_HEADER_TYPE_ADDR,	/* IWEVREGISTERED */
    IW_HEADER_TYPE_ADDR,	/* IWEVEXPIRED */
    IW_HEADER_TYPE_POINT,	/* IWEVGENIE */
    IW_HEADER_TYPE_POINT,	/* IWEVMICHAELMICFAILURE */
    IW_HEADER_TYPE_POINT,	/* IWEVASSOCREQIE */
    IW_HEADER_TYPE_POINT,	/* IWEVASSOCRESPIE */
    IW_HEADER_TYPE_POINT,	/* IWEVPMKIDCAND */
};
static const unsigned int standard_event_num = sizeof(standard_event_hdr);

/* Size (in bytes) of various events */
static const int event_type_size[] = {
    IW_EV_LCP_LEN,		/* IW_HEADER_TYPE_NULL */
    0,
    IW_EV_CHAR_LEN,		/* IW_HEADER_TYPE_CHAR */
    0,
    IW_EV_UINT_LEN,		/* IW_HEADER_TYPE_UINT */
    IW_EV_FREQ_LEN,		/* IW_HEADER_TYPE_FREQ */
    IW_EV_ADDR_LEN,		/* IW_HEADER_TYPE_ADDR */
    0,
    IW_EV_POINT_LEN,	/* Without variable payload */
    IW_EV_PARAM_LEN,	/* IW_HEADER_TYPE_PARAM */
    IW_EV_QUAL_LEN,		/* IW_HEADER_TYPE_QUAL */
};

typedef struct stream_descr
{
    char *        end;            /* End of the stream */
    char *        current;        /* Current event in stream of events */
    char *        value;          /* Current value in event */
} stream_descr;

double iw_freq2float(const struct iw_freq *);
int iw_extract_event_stream(struct stream_descr *, struct iw_even *);
int iface_scanning(int skfd, char *, vector<struct apdata *>&);

#endif /*__linux__*/

#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>
#include <net80211/ieee80211_ioctl.h>
#include <net80211/ieee80211_freebsd.h>

u_int ieee80211_mhz2ieee(u_int);
int copy_essid(char [], size_t, const u_int8_t *, size_t);
void scan_and_wait(int, char *);
int iface_scanning(int, char *, map<string,struct candidateEntry *>&);

#endif /*__FreeBSD__*/

#endif // _ifaceScan_h_
