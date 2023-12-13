#include "cCommandFactory.h"

#include "cCommand_MoveTo.h"
#include "cCommand_Orient.h"

iCommand* cCommandFactory::makeCommand(CommandType type, void* pOldSchoolRulz)
{
	iCommand* newCommand = nullptr;

	switch (type)
	{

	case Move:
	{
		newCommand = new cCommand_MoveTo();
		newCommand->Initialize(pOldSchoolRulz);
		return newCommand;
	}
	case Orient:
	{
		newCommand = new cCommand_Orient();
		newCommand->Initialize(pOldSchoolRulz);
		return newCommand;
	}
	default:

		return nullptr;
	}
}
