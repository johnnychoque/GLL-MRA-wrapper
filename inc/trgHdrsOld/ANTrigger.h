#ifndef ANTrigger_h
#define ANTrigger_h

#include <ctime> //<time.h>
#ifndef WIN32
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif
#include "ANTriggerID.h"
#include "ANGroupID.h"
#include "ANType.h"
#include "ANSource.h"
#include "ANTriggerData.h"
#include "ANTriggerPacket.h"

#include <list>
#include <string>
using namespace std;

class ANTriggerPacket;

class ANTrigger
{
public:

    enum EValueComparison { ENoComparison , EValueSmaller , EValueSmallerOrEqual , EValueBigger , EValueBiggerOrEqual };

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
	void					setType( int type );

    /**
    * Get type
    */
	int						getType();

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
    void                    setValue( int value );

    /**
    * Set value
    */
    void                    setValue( string value );

    /**
    * Get value
    */
    string                  getValue();

    /**
    * Get integer value
    *
    * @return true if integer was succesfully found from value
    */
    bool                    getIntegerValue( int &value , EValueComparison *comparisonOperator = 0 );

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
	 * Serialize to a allocated packet
	 */
	void					serialize(ANTriggerPacket *pkt);

	/**
	 * Alloc and serialize to a packet
	 */
	ANTriggerPacket *		serialize();

	/**
	 * Unserialize from a packet
	 */
	void					unserialize(ANTriggerPacket *pkt);

	/**
	* Group of the trigger
	*/
	ANGroupID				m_groupID;

	/**
	* Source of the trigger
	*/
	ANSource				m_source;
	
	/**
	* How many seconds to hold this trigger in Triggering Events Repository
	*/
	int						m_repositoryTime;

	/**
	* Unique ID of the trigger (Triggering Events Collection gives this ID)
	* (unique within triggering FA)
	*/
	int						m_uniqueID;

	/**
	* Variable for internal use
	*/
	int						m_reserved;

	/**
	*
	*/
	ANTriggerData	        m_data;

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
    string                  m_value;

    /**
    * Has value been set
    */
    bool                    m_hasValue;

	/**
	* Time stamp (given by Triggering Events Collection)
	* timestamp is number of seconds elapsed since midnight (00:00:00), January 1, 1970
	*/
	time_t					m_time;

};

#endif
