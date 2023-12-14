#include "cCommandFactory.h"

#include "cCommand_MoveTo.h"
#include "cCommand_MoveToBezier.h"
#include "cCommand_Orient.h"
#include "cCommand_Follow.h"
#include "cCommand_LookAt.h"

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
	case BezMove:
	{
		newCommand = new cCommand_MoveToBezier();
		newCommand->Initialize(pOldSchoolRulz);
		return newCommand;
	}
	case Orient:
	{
		newCommand = new cCommand_Orient();
		newCommand->Initialize(pOldSchoolRulz);
		return newCommand;
	}
	case Follow:
	{
		newCommand = new cCommand_Follow();
		newCommand->Initialize(pOldSchoolRulz);
		return newCommand;
	}
	case LookAt:
	{
		newCommand = new cCommand_LookAt();
		newCommand->Initialize(pOldSchoolRulz);
		return newCommand;
	}
	default:

		return nullptr;
	}
}
