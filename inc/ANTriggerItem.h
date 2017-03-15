#ifndef _ANTriggerItem_h
#define _ANTriggerItem_h

#include <list>
#include <string>
#include <sstream>
using namespace std;

#include "ANTriggerItemPtr.h"

#include "ANTriggerItemList.h"

class ANTrigger;

/**
* @brief Base class for triggers
*/
class ANTriggerItem
{
public:

    enum EComparison { ENoComparison , ELessThan , ELessThanOrEqual , EGreaterThan , EGreaterThanOrEqual , EBetween };

	/**
	* Constructor
	*/
	ANTriggerItem();

	/**
	* Destructor
	*/
	virtual ~ANTriggerItem();

    /**
    * Assignment operator           
    */
    ANTriggerItem&                  operator = ( ANTriggerItem &X );

	/**
	* Check if this triggeritem triggers for given trigger
	*
	* @param trigger trigger to be checked
	*/
	virtual	bool					checkIfTriggering(ANTrigger *trigger) = 0;

	/**
	* Check policy validity of this triggeritem for given trigger
	*
	* @param trigger trigger to be checked
	*/
	virtual	bool					checkPolicy(ANTrigger *trigger) = 0;

	/**
	* Set trigger item values from string
	*
	* @param str string to be parsed from
	*
	* @return number of values parsed
	*/
	virtual int						setValues(const char *str) = 0;

	/**
	* Fill values for given trigger according to this trigger item
	*
	* @param trigger trigger to be filled
	*
	* @return number of members of trigger filled
	*/
	virtual int						fillTrigger(ANTrigger *trigger) = 0;

	/**
	* String representation of this class
	*/
	virtual string					toString() = 0;

	/**
	* Clear values
	*/
	virtual void					clear() = 0;

	/**
	* Comparison operator
	*/
	virtual bool					operator == (const ANTriggerItem &X) = 0;

    /**
    * Clone 
    */
    virtual ANTriggerItem*          clone() = 0;

	bool							isValid();

	/**
	* Parse ANTriggerItem derived classes from string line to 
	* list of pointers
	*
	* @param ptr string to be parsed from
    * @param parsedItems referentce to list of pointers of trigger items where parsing result will be written
	*
	* @return number of items parsed to parsedItems
	*/
	static ANTriggerItemList 	    parseItems(const char *ptr);

	/**
	* Check if string can be found from another string
	*
	* @param str string to be searched from
	* @param searchStr string to be searhed
	* @param value reference parameter to which real number value after searchStr 
	*	will be written if it exists, otherwise value will be 1
	*
	* @return index to string if found
	* @return -1 if searchStr not found
	*/
	static int						find(const char* str , const char* searchStr , float &value);

protected:

	/**
	* Has some value been set for this trigger item
	*/
	bool							m_isValid;
};


#endif


