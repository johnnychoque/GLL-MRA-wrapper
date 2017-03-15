#ifndef ANType_h
#define ANType_h

#include "ANTriggerItem.h"

/**
* @brief ANTriggerID
*/
class ANType : public ANTriggerItem
{
public:

	/**
	* Constructor
	*/
	ANType();

	/**
	* Destructor
	*/
	virtual ~ANType();

    void                setValue( int value , EComparison *comparison = 0 );

    /**
    * Set type between two values (for filtering)
    */
	void				setBetween( int type1 , int type2 );

    int                 getValue();

	int					getBetweenUpperValue();

    /**
    * Assignment operator
    */
    ANType&             operator = ( ANType &X );

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

private:

    int                 m_value;

	int					m_betweenUpperValue;

	EComparison			m_comparison;
};

#endif


