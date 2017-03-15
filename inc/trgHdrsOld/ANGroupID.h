#ifndef ANGroupID_h
#define ANGroupID_h

#include "ANTriggerItem.h"

/**
* @brief ANTriggerID
*/
class ANGroupID : public ANTriggerItem
{
public:

	/**
	* Constructor
	*/
	ANGroupID();

	/**
	* Destructor
	*/
	virtual ~ANGroupID();

	void				setID(int aID);

	int					getID();

    /**
    * Assignment operator
    */
    ANGroupID&          operator = ( ANGroupID &X );

    /**
    * Clone 
    */
    ANTriggerItem*      clone();

	/**
	* Check policy validity of this triggeritem for given trigger
	*
	* @param trigger trigger to be checked
	*/
	bool				checkPolicy(ANTrigger *trigger);

	bool				checkIfTriggering(ANTrigger *trigger);

	/**
	* Parse groupID value from string
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
	* Group ID
	*/
	int					m_ID;
};

#endif


