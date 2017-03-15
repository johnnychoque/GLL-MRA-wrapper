#ifndef ANValue_h
#define ANValue_h

#include "ANTriggerItem.h"

/**
* @brief ANTriggerID
*/
class ANValue : public ANTriggerItem
{
public:

	/**
	* Constructor
	*/
	ANValue();

	/**
	* Destructor
	*/
	virtual ~ANValue();

	void				setValue( int value );

	void				setValue( float value );

    void                setValue( string value , EComparison *comparison = 0 );

    /**
    * Set value between two values (for filtering)
    */
	void				setBetween( string value1 , string value2 );

    string				getValue();

	string				getBetweenUpperValue();

	/**
	* Get number value of m_value
	*/
    bool				getNumberValue( float &value );

	/**
	* Get number value of string value
	*/
    bool				getNumberValue( string strValue , float &value );

    /**
    * Assignment operator
    */
    ANValue&            operator = ( ANValue &X );

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

    string				m_value;

	string				m_betweenUpperValue;

	EComparison			m_comparison;
};

#endif


