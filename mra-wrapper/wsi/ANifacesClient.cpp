/* ANifacesClient.cpp
   Generated by gSOAP 2.7.9e from ANifaces.h
   Copyright(C) 2000-2006, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#include "ANifacesH.h"

namespace ANifaces {

SOAP_SOURCE_STAMP("@(#) ANifacesClient.cpp ver 2.7.9e 2007-09-24 16:43:36 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__registerFE(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string _feName, std::string _feDescription, ArrayOf_USCOREsoapenc_USCOREstring *_sapId, ArrayOf_USCOREsoapenc_USCOREstring *_strLocation, ArrayOf_USCOREsoapenc_USCOREstring *_sapDescription, struct ns1__registerFEResponse &_param_1)
{	struct ns1__registerFE soap_tmp_ns1__registerFE;
	if (!soap_endpoint)
		soap_endpoint = "http://localhost:8180/ACSFC/services/RegistrationComponent";
	if (!soap_action)
		soap_action = "";
	soap->encodingStyle = "http://schemas.xmlsoap.org/soap/encoding/";
	soap_tmp_ns1__registerFE._feName = _feName;
	soap_tmp_ns1__registerFE._feDescription = _feDescription;
	soap_tmp_ns1__registerFE._sapId = _sapId;
	soap_tmp_ns1__registerFE._strLocation = _strLocation;
	soap_tmp_ns1__registerFE._sapDescription = _sapDescription;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_ns1__registerFE(soap, &soap_tmp_ns1__registerFE);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns1__registerFE(soap, &soap_tmp_ns1__registerFE, "ns1:registerFE", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns1__registerFE(soap, &soap_tmp_ns1__registerFE, "ns1:registerFE", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_ns1__registerFEResponse(soap, &_param_1);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_ns1__registerFEResponse(soap, &_param_1, "ns1:registerFEResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__unRegisterFE(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string _feName, struct ns1__unRegisterFEResponse &_param_2)
{	struct ns1__unRegisterFE soap_tmp_ns1__unRegisterFE;
	if (!soap_endpoint)
		soap_endpoint = "http://localhost:8180/ACSFC/services/RegistrationComponent";
	if (!soap_action)
		soap_action = "";
	soap->encodingStyle = "http://schemas.xmlsoap.org/soap/encoding/";
	soap_tmp_ns1__unRegisterFE._feName = _feName;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_ns1__unRegisterFE(soap, &soap_tmp_ns1__unRegisterFE);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns1__unRegisterFE(soap, &soap_tmp_ns1__unRegisterFE, "ns1:unRegisterFE", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns1__unRegisterFE(soap, &soap_tmp_ns1__unRegisterFE, "ns1:unRegisterFE", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_ns1__unRegisterFEResponse(soap, &_param_2);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_ns1__unRegisterFEResponse(soap, &_param_2, "ns1:unRegisterFEResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns2__sendTrigger(struct soap *soap, const char *soap_endpoint, const char *soap_action, ns2__WSANTrigger *trigger, std::string &result)
{	struct ns2__sendTrigger soap_tmp_ns2__sendTrigger;
	struct ns2__sendTriggerResponse *soap_tmp_ns2__sendTriggerResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://localhost:8020";
	if (!soap_action)
		soap_action = "";
	soap->encodingStyle = "http://schemas.xmlsoap.org/soap/encoding/";
	soap_tmp_ns2__sendTrigger.trigger = trigger;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_ns2__sendTrigger(soap, &soap_tmp_ns2__sendTrigger);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns2__sendTrigger(soap, &soap_tmp_ns2__sendTrigger, "ns2:sendTrigger", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns2__sendTrigger(soap, &soap_tmp_ns2__sendTrigger, "ns2:sendTrigger", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_std__string(soap, &result);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_ns2__sendTriggerResponse = soap_get_ns2__sendTriggerResponse(soap, NULL, "ns2:sendTriggerResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	result = soap_tmp_ns2__sendTriggerResponse->result;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns2__register(struct soap *soap, const char *soap_endpoint, const char *soap_action, int id, std::string &result)
{	struct ns2__register soap_tmp_ns2__register;
	struct ns2__registerResponse *soap_tmp_ns2__registerResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://localhost:8020";
	if (!soap_action)
		soap_action = "";
	soap->encodingStyle = "http://schemas.xmlsoap.org/soap/encoding/";
	soap_tmp_ns2__register.id = id;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_ns2__register(soap, &soap_tmp_ns2__register);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns2__register(soap, &soap_tmp_ns2__register, "ns2:register", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns2__register(soap, &soap_tmp_ns2__register, "ns2:register", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_std__string(soap, &result);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_ns2__registerResponse = soap_get_ns2__registerResponse(soap, NULL, "ns2:registerResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	result = soap_tmp_ns2__registerResponse->result;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns2__unRegister(struct soap *soap, const char *soap_endpoint, const char *soap_action, int id, std::string &result)
{	struct ns2__unRegister soap_tmp_ns2__unRegister;
	struct ns2__unRegisterResponse *soap_tmp_ns2__unRegisterResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://localhost:8020";
	if (!soap_action)
		soap_action = "";
	soap->encodingStyle = "http://schemas.xmlsoap.org/soap/encoding/";
	soap_tmp_ns2__unRegister.id = id;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_ns2__unRegister(soap, &soap_tmp_ns2__unRegister);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns2__unRegister(soap, &soap_tmp_ns2__unRegister, "ns2:unRegister", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns2__unRegister(soap, &soap_tmp_ns2__unRegister, "ns2:unRegister", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_std__string(soap, &result);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_ns2__unRegisterResponse = soap_get_ns2__unRegisterResponse(soap, NULL, "ns2:unRegisterResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	result = soap_tmp_ns2__unRegisterResponse->result;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns7__lookupURI(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string _nameToResolve, std::string &_lookupURIReturn)
{	struct ns7__lookupURI soap_tmp_ns7__lookupURI;
	struct ns7__lookupURIResponse *soap_tmp_ns7__lookupURIResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://localhost:8180/ACSFC/services/LookupComponent";
	if (!soap_action)
		soap_action = "";
	soap->encodingStyle = "http://schemas.xmlsoap.org/soap/encoding/";
	soap_tmp_ns7__lookupURI._nameToResolve = _nameToResolve;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_ns7__lookupURI(soap, &soap_tmp_ns7__lookupURI);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns7__lookupURI(soap, &soap_tmp_ns7__lookupURI, "ns7:lookupURI", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns7__lookupURI(soap, &soap_tmp_ns7__lookupURI, "ns7:lookupURI", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_std__string(soap, &_lookupURIReturn);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_ns7__lookupURIResponse = soap_get_ns7__lookupURIResponse(soap, NULL, "ns7:lookupURIResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	_lookupURIReturn = soap_tmp_ns7__lookupURIResponse->_lookupURIReturn;
	return soap_closesock(soap);
}

} // namespace ANifaces


/* End of ANifacesClient.cpp */
