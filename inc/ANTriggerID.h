#ifndef _ANTriggerID_h
#define _ANTriggerID_h

#include "ANTriggerItem.h"

#define TRGID_NB_STABLE 91
#define TRGID_NB_UNSTABLE 92
#define TRGID_RG_FORMED		93

#define TRGID_RG_SENDRA 	95
#define TRGID_RG_SENDRC		96

#define TRGID_RG_GWDOWN		97
#define TRGID_RG_GWUP		98

#ifdef PIMRC_HACK
#define TRGID_SENSOR_MODE	5678
#endif

/**
* @brief ANTriggerID
*/
class ANTriggerID : public ANTriggerItem
{
public:

	/**
	* Constructor
	*/
	ANTriggerID();

	/**
	* Destructor
	*/
	virtual ~ANTriggerID();

	void				setID(int aID);

	int					getID();

    /**
    * Clone 
    */
    ANTriggerItem*      clone();

    /**
    * Assignment operator
    */
    ANTriggerID&        operator = ( ANTriggerID &X );

	/**
	* Check policy validity of this triggeritem for given trigger
	*
	* @param trigger trigger to be checked
	*/
	bool				checkPolicy(ANTrigger *trigger);

	bool				checkIfTriggering(ANTrigger *trigger);

	/**
	* Parse triggerID value from string
	*
	* @param str string to be parsed from
	*
	* @return 1 if value was parsed
	*/
	int					setValues(const char *str);

	/**
	* Fill values for given trigger according to this trigger item
	*
	* @param trigger trigger to be filled
	*
	* @return number of members of trigger filled
	*/
	int					fillTrigger(ANTrigger *trigger);

	/**
	* String representation of this class
	*/
	string				toString();

	/**
	* Clear values
	*/
	void				clear();

	/**
	* Comparison operator
	*/
	bool				operator == (const ANTriggerItem &X);

private:

	/**
	* Trigger ID
	*/
	int					m_ID;
};

#endif


