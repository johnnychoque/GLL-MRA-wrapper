#ifndef ANTriggerItemPtr_h
#define ANTriggerItemPtr_h

class ANTriggerItem;

/**
* @brief Class for sorting pointers of ANTriggerItem
*/
class ANTriggerItemPtr
{
public:

	/**
	* Constructor
	*/
	ANTriggerItemPtr(ANTriggerItem *ptr)
	{
		m_ptr = ptr;
	}

	/**
	* Desctructor
	*/
	virtual ~ANTriggerItemPtr()
	{}

	ANTriggerItem*		getPtr()
	{
		return m_ptr;
	}

	int					m_index;



	bool operator < (const ANTriggerItemPtr &X)
	{
		if(m_index < X.m_index)
			return true;
		else
			return false;
	}

private:
	ANTriggerItem*		m_ptr;
};

#endif

