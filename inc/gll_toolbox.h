// -*- Mode: C++; c-basic-indent: 4 -*- //

#ifndef _gll_toolbox_h_
#define _gll_toolbox_h_

using namespace std;

#include "ifaceScan.h"

int getScanInfo(struct ifreq *ifr, map<string, struct candidateEntry *> &detectedSet_);
void printScanInfo(vector<struct apdata *>& ap);
void deleteScanInfo(vector<struct apdata *>& ap);
int getSNR(char *ifname);

#endif // _gll_toolbox_h_
