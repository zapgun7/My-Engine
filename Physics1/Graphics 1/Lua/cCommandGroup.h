#pragma once

#include <vector>
#include <string>

#include "iCommand.h"

class cCommandGroup : public iCommand
{
public:
	cCommandGroup();

	std::string friendlyName;
	unsigned int UniqueID;

	void AddSerialCommand(iCommand* pTheCommand);
	void AddParallelCommand(iCommand* pTheCommand);

	virtual bool Update(double deltaTime);
	virtual bool isDone(void);

	virtual void Initialize(void* initStruct);

	// Call every frame
	//virtual bool Update(double deltaTime);
	// Also called every frame. Returns true when done
	//virtual bool isDone(void);

	virtual bool PreStart(void);
	virtual bool PostEnd(void);

private:
	std::vector< iCommand* > m_vecSerialCommands;
	// Points to the next command we're going to run
	std::vector< iCommand* >::iterator m_itNextSerialCommand;
	bool m_UpdateSerial(double deltaTime);
	bool m_isDoneSerial(void);



	std::vector< iCommand* > m_vecParallelCommands;
	bool m_UpdateParallel(double deltaTime);
	bool m_isDoneParallel(void);

	std::vector< cCommandGroup* > m_vecCommandGroups;
	
};
