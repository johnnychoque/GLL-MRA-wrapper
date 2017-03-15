#ifndef _ANTriggerData_h
#define _ANTriggerData_h

/**
* @brief ANTriggerID
*/
class ANTriggerData
{
public:

	/**
	* Constructor
	*/
	ANTriggerData();

	/**
	* Destructor
	*/
	virtual ~ANTriggerData();

	/**
	 * Copy constructor
	 */
	ANTriggerData(const ANTriggerData& a);

	/**
	 * Copy assignment
	 */
	ANTriggerData& ANTriggerData::operator=(const ANTriggerData& a);

	bool setData(char *data, int dataLen);

	int getLength();

	char * getData();

	/**
	* Clear values
	*/
	void clear();

//private:


	char * m_data;
	int m_dataLen;	
};

#endif


