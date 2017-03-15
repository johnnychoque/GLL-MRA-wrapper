/* WSwrapperMRAservicesObject.h
   Generated by gSOAP 2.7.9e from MRAwrapper.h
   Copyright(C) 2000-2006, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef WSwrapperMRAservices_H
#define WSwrapperMRAservices_H
#include "WSwrapperH.h"

/******************************************************************************\
 *                                                                            *
 * Service Object                                                             *
 *                                                                            *
\******************************************************************************/

extern SOAP_NMAC struct Namespace WSwrapper_namespaces[];

namespace WSwrapper {
class MRAservices : public soap
{    public:
	MRAservices()
	{ soap_init(this); this->namespaces = WSwrapper_namespaces; };
	virtual ~MRAservices() { soap_destroy(this); soap_end(this); soap_done(this); };
	virtual	int bind(const char *host, int port, int backlog) { return soap_bind(this, host, port, backlog); };
	virtual	int accept() { return soap_accept(this); };
	virtual	int serve() { return WSwrapper_serve(this); };
};

/******************************************************************************\
 *                                                                            *
 * Service Operations (you should define these)                               *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 ns__getsnr(struct soap*, double ifname, double &result);

} // namespace WSwrapper


#endif