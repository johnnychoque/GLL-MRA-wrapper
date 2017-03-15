/* WSwrapperStub.h
   Generated by gSOAP 2.7.9e from MRAwrapper.h
   Copyright(C) 2000-2006, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef WSwrapperStub_H
#define WSwrapperStub_H
#define WITH_NONAMESPACES
#ifndef WITH_NOGLOBAL
#define WITH_NOGLOBAL
#endif
#include "stdsoap2.h"

namespace WSwrapper {

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#ifndef SOAP_TYPE_WSwrapper_ns__getsnrResponse
#define SOAP_TYPE_WSwrapper_ns__getsnrResponse (10)
/* ns:getsnrResponse */
struct ns__getsnrResponse
{
public:
	double result;	/* RPC return element */	/* required element of type xsd:double */
};
#endif

#ifndef SOAP_TYPE_WSwrapper_ns__getsnr
#define SOAP_TYPE_WSwrapper_ns__getsnr (11)
/* ns:getsnr */
struct ns__getsnr
{
public:
	double ifname;	/* required element of type xsd:double */
};
#endif

#ifndef SOAP_TYPE_WSwrapper_SOAP_ENV__Header
#define SOAP_TYPE_WSwrapper_SOAP_ENV__Header (14)
/* SOAP Header: */
struct SOAP_ENV__Header
{
public:
	void *dummy;	/* transient */
};
#endif

#ifndef SOAP_TYPE_WSwrapper_SOAP_ENV__Code
#define SOAP_TYPE_WSwrapper_SOAP_ENV__Code (15)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#ifndef SOAP_TYPE_WSwrapper_SOAP_ENV__Detail
#define SOAP_TYPE_WSwrapper_SOAP_ENV__Detail (17)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
	char *__any;
};
#endif

#ifndef SOAP_TYPE_WSwrapper_SOAP_ENV__Reason
#define SOAP_TYPE_WSwrapper_SOAP_ENV__Reason (18)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_WSwrapper_SOAP_ENV__Fault
#define SOAP_TYPE_WSwrapper_SOAP_ENV__Fault (19)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};
#endif

/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE_WSwrapper__XML
#define SOAP_TYPE_WSwrapper__XML (4)
typedef char *_XML;
#endif

#ifndef SOAP_TYPE_WSwrapper__QName
#define SOAP_TYPE_WSwrapper__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE_WSwrapper_xsd__double
#define SOAP_TYPE_WSwrapper_xsd__double (7)
typedef double xsd__double;
#endif


/******************************************************************************\
 *                                                                            *
 * Typedef Synonyms                                                           *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Service Operations                                                         *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 ns__getsnr(struct soap*, double ifname, double &result);

/******************************************************************************\
 *                                                                            *
 * Skeletons                                                                  *
 *                                                                            *
\******************************************************************************/

SOAP_FMAC5 int SOAP_FMAC6 WSwrapper_serve(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 WSwrapper_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__getsnr(struct soap*);

} // namespace WSwrapper


#endif

/* End of WSwrapperStub.h */