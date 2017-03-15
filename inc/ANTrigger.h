#ifndef ANTrigger_h
#define ANTrigger_h

#include <ctime> //<time.h>
#ifndef WIN32
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif
#include "ANTriggerID.h"
#include "ANType.h"
#include "ANValue.h"

#include <list>
#include <string>
using namespace std;

class ANTrigger
{
public:

	/**
	* Constructor
	*/ 
	ANTrigger();

	/**
	* Destructor
	*/
	virtual ~ANTrigger();

    /**
    * Assignment operator
    */
    ANTrigger&              operator = ( ANTrigger &X );

	/**
	* Is identical to another trigger comparing basic members
	*/
	bool					isIdentical( ANTrigger &X );

    /**
    * Set trigger ID
    */
	void					setID( int id );

    /**
    * Get trigger ID
    */
	int						getID();

    /**
    * Set type
    */
	void					setType( int type , ANTriggerItem::EComparison *comparison = 0 );

    /**
    * Set type between two values (for filtering)
    */
	void					setTypeBetween( int type1 , int type2 );

    /**
    * Get type
    */
	int						getType();

	int						getTypeBetweenUpperValue();

	/**
	* Copy constructor
	*/ 
	//ANTrigger::ANTrigger(const ANTrigger& a);

	/**
	* String representation of this class
	*/
	string					toString();

    /**
    * Set integer value
    */
    void                    setValue( string value , ANTriggerItem::EComparison *comparison = 0 );

    /**
    * Set value between two values (for filtering)
    */
	void					setValueBetween( string value1 , string value2 );

    /**
    * Get value
    */
    string                  getValue();

	string					getValueBetweenUpperValue();

    /**
    * Get integer value
    *
    * @return true if integer was succesfully found from value
    */
    bool                    getNumberValue( float &value );

    /**
    * Get timestamp
    */
	time_t					getTimeStamp();

    /**
    * Set timestamp
    */
	void					setTimeStamp( time_t time );

	/**
	* Check if trigger has been fired for given handover rule
	*/
	bool					hasBeenFiredFor(int ruleID);

	/**
	* Check if another trigger triggers 
	*/
	bool					checkIfTriggering(ANTrigger &trigger);

	/**
	* How many seconds to hold this trigger in Triggering Events Repository
	*/
	int						m_repositoryTime;

	/**
	* Variable for internal use
	*/
	int						m_reserved;

private:

	/**
	* ID of the trigger
	*/
	ANTriggerID				m_ID;

	/**
	* Type
	*/
	ANType					m_type;

    /**
    * Trigger value         
    */
    ANValue					m_value;

	/**
	* Time stamp (given by Triggering Events Collection)
	* timestamp is number of seconds elapsed since midnight (00:00:00), January 1, 1970
	*/
	time_t					m_time;

};

#endif
