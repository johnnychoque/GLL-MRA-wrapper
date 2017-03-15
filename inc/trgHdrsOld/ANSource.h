#ifndef _ANSource_h
#define _ANSource_h

#include "ANTriggerItem.h"

/**
* @brief ANTriggerID
*/
class ANSource : public ANTriggerItem
{
public:

	/**
	* Constructor
	*/
	ANSource();

	/**
	* Destructor
	*/
	virtual ~ANSource();

	bool				checkIfTriggering(ANTrigger *trigger);

    /**
    * Assignment operator
    */
    ANSource&           operator = ( ANSource &X );

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

	/**
	* Parse source values from string
	*
	* @param str string to be parsed from
	*
	* @return number of values parsed
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

	bool				hasANI();
	bool				hasASI();
	bool				hasARI();
	bool				hasACS();

private:

	/**
	* ANI 
	*/
	float				m_ANI;
	
	/**
	* ASI
	*/
	float				m_ASI;
	
	/**
	* ARI
	*/
	float				m_ARI;
	
	/**
	* ACS 
	*/
	float				m_ACS;
	
};

#endif


