#pragma once

#include "iCommand.h"
#include <string>


enum CommandType {
	Move,
	BezMove,
	Orient,
	Follow,
	LookAt,
	Transform

};

class cCommandFactory
{
public:
	cCommandFactory() {};

	// Passing whatever generic type you need to the 
	//	Abstract Factory...
	//iCommand* makeCommand(std::string parameters);
	//iCommand* makeCommand(std::string JSON);
	iCommand* makeCommand(CommandType type, void* pOldSchoolRulz);

};
