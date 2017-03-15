// -*- Mode: C++; c-basic-offset: 4 -*- //

#include "ifaceScan.h"

#define DBG_LEVEL 2
#include "debug.h"

#ifdef __linux__
double iw_freq2float(const struct iw_freq *in)
{
    int           i;
    double        res = (double) in->m;

    DBG_IN("ifaceScan::iw_freq2float");
    for(i = 0; i < in->e; i++)
	res *= 10;
    return(res);
}

int iw_extract_event_stream(struct stream_descr *stream,  /* Stream of events */
			    struct iw_event *iwe)	/* Extracted event */
{
    int		event_type = 0;
    unsigned int  event_len = 1;		/* Invalid */
    char *	pointer;
    unsigned	cmd_index;		/* *MUST* be unsigned */

    /* Check for end of stream */
    if((stream->current + IW_EV_LCP_LEN) > stream->end)
	return(0);

    /* Extract the event header (to get the event id).
     * Note : the event may be unaligned, therefore copy... */
    memcpy((char *) iwe, stream->current, IW_EV_LCP_LEN);

    /* Check invalid events */
    if(iwe->len <= IW_EV_LCP_LEN)
	return(-1);

    /* Get the type and length of that event */
    if(iwe->cmd <= SIOCIWLAST)
	{
	    cmd_index = iwe->cmd - SIOCIWFIRST;
	    if(cmd_index < standard_ioctl_num)
		event_type = standard_ioctl_hdr[cmd_index];
	}
    else
	{
	    cmd_index = iwe->cmd - IWEVFIRST;
	    if(cmd_index < standard_event_num)
		event_type = standard_event_hdr[cmd_index];
	}

    /* Unknown events -> event_type=0 => IW_EV_LCP_LEN */
    event_len = event_type_size[event_type];

    /* Check if we know about this event */
    if(event_len <= IW_EV_LCP_LEN)
	{
	    /* Skip to next event */
	    stream->current += iwe->len;
	    return(2);
	}
    event_len -= IW_EV_LCP_LEN;

    /* Set pointer on data */
    if(stream->value != NULL)
	pointer = stream->value;			/* Next value in event */
    else
	pointer = stream->current + IW_EV_LCP_LEN;	/* First value in event */

    /* Copy the rest of the event (at least, fixed part) */
    if((pointer + event_len) > stream->end)
	{
	    /* Go to next event */
	    stream->current += iwe->len;
	    return(-2);
	}

    /* Fixup for WE-19 and later : pointer no longer in the stream */
    if((WIRELESS_EXT > 18) && (event_type == IW_HEADER_TYPE_POINT))
	memcpy((char *) iwe + IW_EV_LCP_LEN + IW_EV_POINT_OFF,
	       pointer, event_len);
    else
	memcpy((char *) iwe + IW_EV_LCP_LEN, pointer, event_len);

    /* Skip event in the stream */
    pointer += event_len;

    /* Special processing for iw_point events */
    if(event_type == IW_HEADER_TYPE_POINT)
	{
	    /* Check the length of the payload */
	    if((iwe->len - (event_len + IW_EV_LCP_LEN)) > 0)
		/* Set pointer on variable part (warning : non aligned) */
		iwe->u.data.pointer = pointer;
	    else
		/* No data */
		iwe->u.data.pointer = NULL;

	    /* Go to next event */
	    stream->current += iwe->len;
	}
    else
	{
	    /* Is there more value in the event ? */
	    if((pointer + event_len) <= (stream->current + iwe->len))
		/* Go to next value */
		stream->value = pointer;
	    else
		{
		    /* Go to next event */
		    stream->value = NULL;
		    stream->current += iwe->len;
		}
	}
    return(1);
}

int iface_scanning(int skfd, char *ifname, 
		   vector<struct apdata *>& ap)
{
    struct iwreq wrq;
    unsigned char *buffer;
    int buflen = IW_SCAN_MAX_DATA, ap_num=0, ap_tmp=0, ret;
    struct stream_descr stream;
    struct iw_event iwe;
    struct apdata *tmp;

    if (WIRELESS_EXT < 19) {
	printf("Current wireless extension %d, it is needed >= 19\n", WIRELESS_EXT);
	return -1;
    }
  
    /* Initiate Scan */
    wrq.u.data.pointer = NULL;
    wrq.u.data.flags = 0;
    wrq.u.data.length = 0;
    strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
    if(ioctl(skfd, SIOCSIWSCAN, &wrq) < 0) {
	printf("ioctl SIOCSIWSCAN error(%d): %s", errno, strerror(errno));
	return -1;
    }
  
    buffer = (unsigned char *)malloc(buflen);
    if (buffer == NULL) {
	printf("Memory allocation failed\n");
	return -1;
    }
  
    /* Try to read the results */
    wrq.u.data.pointer = buffer;
    wrq.u.data.flags = 0;
    wrq.u.data.length = buflen;
    strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
    if(ioctl(skfd, SIOCGIWSCAN, &wrq) < 0) {
	printf("ioctl SIOCGIWSCAN error(%d): %s", errno, strerror(errno));
	free(buffer);
	return -1;
    }
  
    if(wrq.u.data.length <=0) {
	printf("%s No scan results\n", ifname);
	return -1;
    }
  
    /*------------------------------------------------------------------
     *
     * Initialise the struct stream_descr so that we can extract
     * individual events from the event stream.
     */
  
    memset((char *) &stream, '\0', sizeof(struct stream_descr));
    stream.current = (char *)buffer;
    stream.end = (char *)buffer + wrq.u.data.length;

    tmp = new struct apdata;

    do {
	/* Extract an event and print it */
	ret = iw_extract_event_stream(&stream, &iwe);

	if(ret > 0) {
	    switch (iwe.cmd) {
	    case SIOCGIWAP:
		{
		    if (ap_tmp != ap_num) {
			ap.push_back(tmp);
			tmp = new struct apdata;
			ap_tmp = ap_num;
		    }
		    ap_num++;
		}
		break;
	    case SIOCGIWMODE:
		if (iwe.u.mode == 3) tmp->mode = INFRA;
		else if (iwe.u.mode == 1) tmp->mode = ADHOC;
		else tmp->mode = UNKN;
		break;
	    case SIOCGIWESSID: 
		{
		    char essid[IW_ESSID_MAX_SIZE+1];
	    
		    if((iwe.u.essid.pointer) && (iwe.u.essid.length))
			memcpy(essid, iwe.u.essid.pointer, iwe.u.essid.length);
		    essid[iwe.u.essid.length] = '\0';
		    tmp->ssid = essid;
		}
		break;
	    case SIOCGIWFREQ: 
		{
		    tmp->channel = (int) iw_freq2float(&(iwe.u.freq));
		}
		break;
	    }
	}
	else 
	    ap.push_back(tmp);
      
    } while(ret > 0);

    free(buffer);
    return ap_num;
}

#endif /*__linux__*/

#ifdef __FreeBSD__
u_int ieee80211_mhz2ieee(u_int freq)
{
    if (freq == 2484)
	return 14;
    if (freq < 2484)
	return (freq - 2407) / 5;
    if (freq < 5000)
	return 15 + ((freq - 2512) / 20);
    return (freq - 5000) / 5;
}

int copy_essid(char buf[], size_t bufsize, 
	       const u_int8_t *essid, size_t essid_len)
{
    const u_int8_t *p; 
    size_t maxlen;
    size_t i;

    if (essid_len > bufsize)
	maxlen = bufsize;
    else
	maxlen = essid_len;
    /* determine printable or not */
    for (i = 0, p = essid; i < maxlen; i++, p++) {
	if (*p < ' ' || *p > 0x7e)
	    break;
    }
    if (i != maxlen) {		/* not printable, print as hex */
	if (bufsize < 3)
	    return 0;
	strlcpy(buf, "0x", bufsize);
	bufsize -= 2;
	p = essid;
	for (i = 0; i < maxlen && bufsize >= 2; i++) {
	    sprintf(&buf[2+2*i], "%02x", p[i]);
	    bufsize -= 2;
	}
	if (i != essid_len)
	    memcpy(&buf[2+2*i-3], "...", 3);
    } else {			/* printable, truncate as needed */
	memcpy(buf, essid, maxlen);
	if (maxlen != essid_len)
	    memcpy(&buf[maxlen-3], "...", 3);
    }
    buf[maxlen]='\0';
    return maxlen;
}

void scan_and_wait(int s, char *name)
{
    struct ieee80211req ireq;
    int sroute;

    sroute = socket(PF_ROUTE, SOCK_RAW, 0);
    if (sroute < 0) {
	perror("socket(PF_ROUTE,SOCK_RAW)");
	return;
    }
    (void) memset(&ireq, 0, sizeof(ireq));
    (void) strncpy(ireq.i_name, name, sizeof(ireq.i_name));
    ireq.i_type = IEEE80211_IOC_SCAN_REQ;
    /* NB: only root can trigger a scan so ignore errors */
    if (ioctl(s, SIOCS80211, &ireq) >= 0) {
	char buf[2048];
	struct if_announcemsghdr *ifan;
	struct rt_msghdr *rtm;

	do {
	    if (read(sroute, buf, sizeof(buf)) < 0) {
		perror("read(PF_ROUTE)");
		break;
	    }
	    rtm = (struct rt_msghdr *) buf;
	    if (rtm->rtm_version != RTM_VERSION)
		break;
	    ifan = (struct if_announcemsghdr *) rtm;
	} while (rtm->rtm_type != RTM_IEEE80211 ||
		 ifan->ifan_what != RTM_IEEE80211_SCAN);
    }
    close(sroute);
}

int iface_scanning(int s, char *name, map<string, struct candidateEntry *>& detectedSet_)
{
    uint8_t buf[24*1024];
    struct ieee80211req ireq;
    char ssid[IEEE80211_NWID_LEN+1];
    uint8_t *cp;
    size_t len, ssidmax=14;
    char bssid[IEEE80211_ADDR_LEN*2+1];
    int j;
    struct candidateEntry *current_;
    string currentBSSID_;
    map<string,struct candidateEntry *>::iterator iter;

    scan_and_wait(s, name);

    (void) memset(&ireq, 0, sizeof(ireq));
    (void) strncpy(ireq.i_name, name, sizeof(ireq.i_name));
    ireq.i_type = IEEE80211_IOC_SCAN_RESULTS;
    ireq.i_data = buf;
    ireq.i_len = sizeof(buf);
    if (ioctl(s, SIOCG80211, &ireq) < 0)
	printf("unable to get scan results\n");
    len = ireq.i_len;
    if (len < sizeof(struct ieee80211req_scan_result))
	return -1;
    
    cp = buf;
    do {
	struct ieee80211req_scan_result *sr;
	uint8_t *vp;
	
	sr = (struct ieee80211req_scan_result *) cp;
	vp = (u_int8_t *)(sr+1);

	bssid[IEEE80211_ADDR_LEN*2] = '\0';
	for(j = 0; j < IEEE80211_ADDR_LEN ; j++) {
	    sprintf(bssid + j*2, "%02x",sr->isr_bssid[j]);
	}
	currentBSSID_ = bssid;

	iter = detectedSet_.find(currentBSSID_);

	if(iter == detectedSet_.end()) {
	    // This is a new detected access!!!
	    
	    current_ = new struct candidateEntry;
	    detectedSet_.insert(pair<string, struct candidateEntry*> (currentBSSID_, current_));
	    
	    copy_essid(ssid, ssidmax, vp, sr->isr_ssid_len);
	    current_->m_networkID = ssid;
	    memcpy(current_->m_cellID, sr->isr_bssid, IEEE80211_ADDR_LEN);

	    current_->m_channel = ieee80211_mhz2ieee(sr->isr_freq);
	    current_->m_rssi = (u_int16_t) sr->isr_rssi;

	    if ((sr->isr_capinfo) & IEEE80211_CAPINFO_ESS) {
		current_->m_mode = INFRA;
	    } else if ((sr->isr_capinfo) & IEEE80211_CAPINFO_IBSS) {
		current_->m_mode = ADHOC;
	    } else {
		current_->m_mode = UNKN;
	    }
	} else {
	    // The detected set was already detected
	    current_ = iter->second;
	    current_->m_rssi = sr->isr_rssi;
	}

	cp += sr->isr_len, len -= sr->isr_len;

    } while (len >= sizeof(struct ieee80211req_scan_result));

    DBG_OUT("ifaceScanning_FreeBSD");
    return ((int) detectedSet_.size());
}
#endif /*__FreeBSD__*/
