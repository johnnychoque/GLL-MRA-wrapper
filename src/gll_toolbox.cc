/* -*- Mode:C++; c-basic-indent: 4 -*- */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdio.h>
#include <string.h>

#ifdef __FreeBSD__
#include <sys/ioctl.h>
#include <net/if.h>

#include <net80211/ieee80211.h>
#include <net80211/ieee80211_ioctl.h>
#include <dev/wi/if_wavelan_ieee.h>  // struct wi_req
#include <errno.h>
#include <err.h>
#endif

#ifdef __linux__
#include <linux/if.h>
#include <linux/wireless.h>
#include <error.h>
#endif

#include "gll_toolbox.h"

#define DBG_LEVEL 2
#include "debug.h"

int getScanInfo(struct ifreq *ifr, map<string, struct candidateEntry *> &detectedSet_)
{
    int skfd, ap_num;
    char ifname[IFNAMSIZ];
    
    DBG_IN("getScanInfo");

    strncpy(ifname, ifr->ifr_name, IFNAMSIZ);
    
    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	printf("error opening sockets\n");
	return -1;
    }
    

    ap_num = iface_scanning(skfd, ifname, detectedSet_);

    close(skfd);

    DBG_OUT("getScanInfo");
    return ap_num;
}

void printScanInfo(vector<struct apdata *>& ap)
{
    char temp[13];
    temp[12] = '\0';

    for (unsigned int i=0; i<ap.size(); i++) {
	cout << "AP " << i+1 << endl;
	cout << "  ssid: " << ap[i]->ssid << endl;
	cout << "  channel: " << ap[i]->channel << endl;
	cout << "  bssid: " << ap[i]->bssid << endl;
	cout << "  mode: ";
	if ((ap[i]->mode)==INFRA) cout << "Infraestructure";
	else if ((ap[i]->mode)==ADHOC) cout << "Ad-hoc";
	else cout << "Unknown";
	cout << endl;
	cout << endl;
    }

    cout << "Total APs: " << ap.size() << endl;
}

void deleteScanInfo(vector<struct apdata *>& ap)
{

    for (unsigned int i=0; i<ap.size(); i++) {
	delete ap[i];
    }

    ap.clear();
}

#ifdef __FreeBSD__
int getSNR_iwi(int skfd, char *ifname)
{
#define GET_RSSI_VALUE 65
    static u_int32_t stats[256];
    //    const struct statistic *stat;
    size_t len;
	
    len = sizeof stats;
    if (sysctlbyname("dev.iwi.0.stats", stats, &len, NULL, 0) == -1) {
	DBG_ERR("Can't retrieve statistics of iwi0");
	return -1;
    }
    /* IMPORTANTE: Al parecer el firmware FreeBSD para el 
       Intel PRO/Wireless 2200 solo ofrece este valor en RSSI.
       Para determinar el valor en dBm es necesario saber el 
       valor del RSSI-max el cual es desconocido */
    return stats[GET_RSSI_VALUE];
}

int getSNR_wi(int skfd, char *ifname)
{
    struct ifreq ifr;
    struct wi_req wreq;
    int signal, noise;

    bzero((char *)&wreq, sizeof(wreq));
    wreq.wi_len = WI_MAX_DATALEN;
    wreq.wi_type = WI_RID_COMMS_QUALITY;

    bzero((char *)&ifr, sizeof(ifr));
    strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
    ifr.ifr_data = (caddr_t)&wreq;

    if (ioctl(skfd, SIOCGWAVELAN, &ifr) == -1) {
	if (errno != EINPROGRESS && errno != EINVAL)
	    DBG_ERR("SIOCGWAVELAN %s", strerror(errno));
	return (-1);
    }
    
    signal = wreq.wi_val[1];
    noise = wreq.wi_val[2];

    return signal-noise;
}

int getSNR(char *ifname)
{
    int skfd, snr;

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	DBG_ERR("Error opening sockets");
	return -1;
    }

    if(!strcmp(ifname,"iwi0")) {
	// Solo para Intel PRO/Wireless 2200BG/2915ABG wireless devices
	snr = getSNR_iwi(skfd, ifname);
    } else {
	// Para Lucent, Intersil, and Atheros wireless devices
	snr = getSNR_wi(skfd, ifname);
    }

    close(skfd);
    return snr;
}

#endif // __FreeBSD__

#ifdef __linux__ 
int getSNR(char *ifname)
{
    struct iwreq wrq;
    struct iw_statistics stats;
    int signal, noise, skfd;

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	DBG_ERR("Error opening sockets");
	return -1;
    }

    wrq.u.data.pointer = (struct iw_statistics *) &stats;
    wrq.u.data.length = sizeof(struct iw_statistics);
    wrq.u.data.flags = 1;		/* Clear updated flag */

    strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
    if(ioctl(skfd, SIOCGIWSTATS, &wrq) < 0) {
	DBG_ERR("SIOCGIWSTATS %s", strerror(errno));
	return(-1);
    }
  
    if(stats.qual.updated & IW_QUAL_DBM) {
	/* Deal with signal level in dBm  (absolute power measurement) */
	if(!(stats.qual.updated & IW_QUAL_LEVEL_INVALID)) {
	    signal = stats.qual.level - 0x100;
	}

	/* Deal with noise level in dBm (absolute power measurement) */
	if(!(stats.qual.updated & IW_QUAL_NOISE_INVALID)) {
	    noise = stats.qual.noise - 0x100;
	}
    }
    else {
	DBG_ERR("Error: Valor disponible en RSSI y no en dbm.");
    }
  
    close(skfd);
    return signal-noise;
}

#endif // __linux__
