/*--------------------------------------------
 * project........: Ambient Networks
 * file...........: trg.h
 * authors........: vesa.kyllonen@vtt.fi
 * organization...: VTT Technical Research Centre of Finland
 * description....: Triggering producer/consumer definitions
 * copyright......: VTT, all rights reserved
 * History........: v0.0.1 08.08.2006 Comment: Created by Vesa Kyllönen
 *                  v0.0.2 22.08.2006          Added TRGPrivilegedConsumer
 *
 *
 */
#ifndef _trg_h_
#define _trg_h_

// System includes
#include <string>
#include <list>

#include "ANTrigger.h"

// Type definitions
typedef int ANIDType;

// Trigger IDs
const int GTriggerSimple 					= 50;

class TRGUserImplementation;
class TRGProducerImplementation;
class TRGConsumerImplementation;
class TRGProducer;
class TRGConsumer;

/**
* Class for subscribe-method of TRGConsumer
*/
class SubscribeSourceFilter
{
public:

	/**
	* Constructor
	*/
	SubscribeSourceFilter();

	/**
	* Constructor
	*/
	SubscribeSourceFilter( ANIDType id , std::string filter );

	/**
	* Destructor
	*/
	virtual ~SubscribeSourceFilter();

	ANIDType			getId();
	std::string			getFilter();

	void				setId( ANIDType id);
	void				setFilter( std::string filter );

private:

	ANIDType			m_id;
	std::string			m_filter;
}; 

/**
* Base class for Triggering objects
*/
class TRGUser
{
public:

	/**
	* Initialise to Triggering framework if is it is not at default address (localhost )
	*/
	bool					init( std::string TRGHost );

	/**
	* Initialise to Triggering framework if is it is not at default address
	* ( localhost , 8020 for producer, 8021 for consumer )
	*/
	bool					init( std::string TRGHost , int TRGPort );

	/**
	* Get latest error as string representation
	*/
	std::string				getError();

protected:

	/**
	* Constructor
	*/
	TRGUser();

	/**
	* Destructor
	*/
	virtual ~TRGUser();

	virtual TRGUserImplementation*	getInstance() = 0;

	TRGUserImplementation*	m_impl;

};

/**
* Base class for Triggering producers
*/
class TRGProducer : public TRGUser
{
public:

	/**
	* Constructor
	*/
	TRGProducer();

	/**
	* Destructor
	*/
	virtual ~TRGProducer();

	/**
	* Register to Triggering framework
	*
	* @param id identifier what kind of triggers producer is providing to Triggering framework
	*/
	bool					Register( ANIDType id );

	/**
	* UnRegister everything that has been registered
	*
	* This function is called also automatically when 
	* TRGProducer exits
	*/
	bool					unRegister();


	/**
	* Send trigger to Triggering framework
	*
	* @param trigger trigger to send to Triggering framework
	*/
	bool					send( ANTrigger trigger );

private:

	virtual TRGUserImplementation*	getInstance();

};

class TRGPrivilegedConsumer;

/**
* Base class for Triggering consumers
* This class cannot be instantiated because of abstract virtual function, 
* derive new class from this class and implement abstract virtual function.
*/
class TRGConsumer : public TRGUser
{
public:

	/**
	* TRGUser init
	*/
	bool					init( std::string TRGHost );

	/**
	* TRGUser init
	*/
	bool					init( std::string TRGHost , int TRGPort );

	/**
	* Initialise to Triggering framework by specifiying local port for incoming triggers
	*
	* @param localPort to which port Triggering framework should send triggers
	*/
	bool					init( std::string TRGHost , int TRGPort , int localPort );

	/**
	* Subscribe single trigger from Triggering framework
	*
	* @param id identifier to subscribe
	* @param filter subscription filter string 
	*/
	bool					subscribe( ANIDType id , std::string filter );

	/**
	* Subscribe multiple triggers from Triggering framework
	*
	* @param ids list of triggers to subscribe
	*/
	bool					subscribe( std::list<SubscribeSourceFilter> ids );

	/**
	* Unsubscribe everything that has been subscribed
	*
	* This function is called also automatically when 
	* TRGCosumer/TRGPrivilegedConsumer exits
	*/
	bool					unSubscribe();


	/**
	* Triggering framework sends trigger after registration
	*
	* @param trigger trigger received from Triggering framework
	*/
	virtual void			onTrigger( ANTrigger trigger ) = 0;

protected:

	/**
	* Constructor
	*/
	TRGConsumer();

	/**
	* Constructor
	*/
	TRGConsumer(int a);

	/**
	* Destructor
	*/
	virtual ~TRGConsumer();

private:

	virtual TRGUserImplementation*	getInstance();

};

/**
* Base class for Privileged Triggering consumers, privileged consumers get timestamp filled from Triggering Framework
* This class cannot be instantiated because of abstract virtual function, 
* derive new class from this class and implement abstract virtual function.
*/
class TRGPrivilegedConsumer : public TRGConsumer
{
public:

	/**
	* Subscribe to Triggering framework for all triggers
	*
	*/
	bool					subscribe();

protected:

	/**
	* Constructor
	*/
	TRGPrivilegedConsumer();

	/**
	* Destructor
	*/
	virtual ~TRGPrivilegedConsumer();

private:

	virtual TRGUserImplementation*	getInstance();
};

#endif
