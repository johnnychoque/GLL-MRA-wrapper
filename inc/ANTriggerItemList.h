#ifndef ANParsedItems_h
#define ANParsedItems_h

#include <list>
using namespace std;

#include "ANTriggerItem.h"

/**
* @brief List of triggers items 
*/
class ANTriggerItemList : public list<ANTriggerItem*>
{
public:

    /**
    * Default operator
    */
    ANTriggerItemList();

	/**
    * copy constructor
    */
	ANTriggerItemList( const ANTriggerItemList &X )  
	{ 
		(*this) = X;
	}

    virtual ~ANTriggerItemList();

    /**
    * Assignment operator
    */
    ANTriggerItemList&          operator = ( const ANTriggerItemList &X );

    void                        freeItems();
};


#endif
