#include "cLuaBrain.h"
#include <iostream>

#include "../Other Graphics Stuff/cMesh.h"			// 
#include "../Other Graphics Stuff/cLightManager.h"  // For making changes to these
#include "../Physics/sPhysicsProperties.h"			//

#include "iCommand.h"
#include "cCommand_MoveTo.h"
#include "cCommand_MoveToBezier.h"
#include "cCommand_Orient.h"
#include "cCommand_Follow.h"
#include "cCommand_LookAt.h"
#include "cCommand_SetTransform.h"
#include "cCommandFactory.h"
#include "cCommandGroup.h"



// Global info on objects in the system
std::vector<cMesh*> g_vecMesh;
std::vector<sPhysicsProperties*> g_vecPhys;

// Command Vectors
std::vector<iCommand*> g_vecSerialCommands; // Run only the first every update
std::vector<iCommand*> g_vecParallelCommands; // Run all of these every update

// Command Groups
std::map<std::string, iCommand*> g_mapGroupCommands;


// Command Factory
cCommandFactory* g_CommandFactory = new cCommandFactory();


///////// INFO SETTING FOR LUA CALLS ////////
void cLuaBrain::setMeshVec(std::vector<cMesh*> newVec)
{
	::g_vecMesh = newVec;
	return;
}

void cLuaBrain::setPhysVec(std::vector<sPhysicsProperties*> newVec)
{
	::g_vecPhys = newVec;
	return;
}

/// The Update Loop
void cLuaBrain::UpdateActiveCommands(double deltaTime)
{
	// Start with serial commands
	if (!::g_vecSerialCommands.empty())
	{
		if (::g_vecSerialCommands[0]->Update(deltaTime))
		{
			::g_vecSerialCommands.erase(::g_vecSerialCommands.begin());
		}
		
	}

	// Now parallel commands
	for (std::vector<iCommand*>::iterator itParallel = ::g_vecParallelCommands.begin();
		itParallel != ::g_vecParallelCommands.end();
		itParallel++)
	{
		if ((*itParallel)->Update(deltaTime))
		{
			itParallel = ::g_vecParallelCommands.erase(itParallel);
			if (itParallel != ::g_vecParallelCommands.end())
				itParallel--; // This puts it 1 before the next one to update
		}
		if (itParallel == ::g_vecParallelCommands.end())
			break;
	}

	return;
}

///////// LUA CALLS ////////////


/// Mesh Finding ///
cMesh* findMesh(std::string friendlyName)
{
	for (std::vector<cMesh*>::iterator itMesh = ::g_vecMesh.begin(); itMesh != g_vecMesh.end(); itMesh++)
	{
		if ((*itMesh)->friendlyName == friendlyName)
			return *itMesh;
	}
	// Didn't find it
	return NULL;
}
cMesh* findMesh(int ID)
{
	for (std::vector<cMesh*>::iterator itMesh = ::g_vecMesh.begin(); itMesh != g_vecMesh.end(); itMesh++)
	{
		if ((*itMesh)->uniqueID == ID)
			return *itMesh;
	}
	// Didn't find it
	return NULL;
}

/// Physics Finding ///
sPhysicsProperties* findPhys(std::string friendlyName)
{
	for (std::vector<sPhysicsProperties*>::iterator itPhys = ::g_vecPhys.begin(); itPhys != g_vecPhys.end(); itPhys++)
	{
		if ((*itPhys)->friendlyName == friendlyName)
			return *itPhys;
	}
	// Didn't find it
	return NULL;
}
sPhysicsProperties* findPhys(int ID)
{
	for (std::vector<sPhysicsProperties*>::iterator itPhys = ::g_vecPhys.begin(); itPhys != g_vecPhys.end(); itPhys++)
	{
		if ((*itPhys)->getUniqueID() == ID)
			return *itPhys;
	}
	// Didn't find it
	return NULL;
}

/// Group Finding ///
iCommand* findGroup(std::string friendlyName)
{
	for (std::map<std::string, iCommand*>::iterator itMap = ::g_mapGroupCommands.begin();
		itMap != ::g_mapGroupCommands.end();
		itMap++)
	{
		if (itMap->first == friendlyName)
			return itMap->second;
	}

	return nullptr;
}

/// Group Deletion ///
void removeGroup(std::string friendlyName)
{
	std::map<std::string, iCommand*>::iterator itMap = ::g_mapGroupCommands.find(friendlyName);

	if (itMap == g_mapGroupCommands.end())
	{
		std::cout << "No group found" << std::endl;
		return;
	}

	::g_mapGroupCommands.erase(itMap);
	return;
}


// Command Adding

/// MOVE ///
int lua_AddSerialMoveObjectCommand(lua_State* L)
{
	std::string objFriendlyName(lua_tostring(L, 1));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	initMoveInfo* info = new initMoveInfo();

	info->theObj = theObj;
	info->startPos = theObj->position;
	info->destPos.x = (float)lua_tonumber(L, 2);
	info->destPos.y = (float)lua_tonumber(L, 3);
	info->destPos.z = (float)lua_tonumber(L, 4);
	info->timeInSeconds = (float)lua_tonumber(L, 5);
	info->rampUpTime = (float)lua_tonumber(L, 6);
	info->rampDownTime = (float)lua_tonumber(L, 7);

	iCommand* newCommand = ::g_CommandFactory->makeCommand(Move, info);
	::g_vecSerialCommands.push_back(newCommand);
	delete info;

	return 0;
}

/// ORIENT ///
int lua_AddSerialOrientObjectCommand(lua_State* L)
{
	std::string objFriendlyName(lua_tostring(L, 1));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	initOrientInfo* info = new initOrientInfo();

	info->theObj = theObj;
	info->startOri = theObj->position;
	info->destOri.x = (float)lua_tonumber(L, 2);
	info->destOri.y = (float)lua_tonumber(L, 3);
	info->destOri.z = (float)lua_tonumber(L, 4);
	info->timeInSeconds = (float)lua_tonumber(L, 5);
	info->rampUpTime = (float)lua_tonumber(L, 6);
	info->rampDownTime = (float)lua_tonumber(L, 7);

	iCommand* newCommand = ::g_CommandFactory->makeCommand(Orient, info);
	::g_vecSerialCommands.push_back(newCommand);
	delete info;

	return 0;
}

///////// GROUP COMMANDS //////////

// First parameter is t/f for if it is parallel, second is group friendlyName, third is object friendly name
int lua_AddMoveToGroup(lua_State* L)
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		commandGroup = new cCommandGroup();
		::g_mapGroupCommands[groupFriendlyName] = commandGroup;
	}

	std::string objFriendlyName(lua_tostring(L, 3));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	initMoveInfo* info = new initMoveInfo();

	info->theObj = theObj;
	info->startPos = theObj->position;
	info->destPos.x = (float)lua_tonumber(L, 4);
	info->destPos.y = (float)lua_tonumber(L, 5);
	info->destPos.z = (float)lua_tonumber(L, 6);
	info->timeInSeconds = (float)lua_tonumber(L, 7);
	info->rampUpTime = (float)lua_tonumber(L, 8);
	info->rampDownTime = (float)lua_tonumber(L, 9);

	iCommand* newCommand = ::g_CommandFactory->makeCommand(Move, info);

	if (isParallel)
		((cCommandGroup*)commandGroup)->AddParallelCommand(newCommand);
	else
		((cCommandGroup*)commandGroup)->AddSerialCommand(newCommand);


	delete info;

	
	return 0;
}

// First parameter is t/f for if it is parallel, second is group friendlyName, third is object friendly name
// 456 destination, 78, bezier offset, 9 time, 10 rampup, 11 rampdown
int lua_AddBezierMoveToGroup(lua_State* L)
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		commandGroup = new cCommandGroup();
		::g_mapGroupCommands[groupFriendlyName] = commandGroup;
	}

	std::string objFriendlyName(lua_tostring(L, 3));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	initBezMoveInfo* info = new initBezMoveInfo();

	info->theObj = theObj;
	info->startPos = theObj->position;
	info->destPos.x = (float)lua_tonumber(L, 4);
	info->destPos.y = (float)lua_tonumber(L, 5);
	info->destPos.z = (float)lua_tonumber(L, 6);
	info->bezOffset.x = (float)lua_tonumber(L, 7);
	info->bezOffset.y = (float)lua_tonumber(L, 8);
	info->timeInSeconds = (float)lua_tonumber(L, 9);
	info->rampUpTime = (float)lua_tonumber(L, 10);
	info->rampDownTime = (float)lua_tonumber(L, 11);

	iCommand* newCommand = ::g_CommandFactory->makeCommand(BezMove, info);

	if (isParallel)
		((cCommandGroup*)commandGroup)->AddParallelCommand(newCommand);
	else
		((cCommandGroup*)commandGroup)->AddSerialCommand(newCommand);


	delete info;


	return 0;
}

int lua_AddOrientToGroup(lua_State* L)
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		commandGroup = new cCommandGroup();
		g_mapGroupCommands[groupFriendlyName] = commandGroup;
	}

	std::string objFriendlyName(lua_tostring(L, 3));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	initOrientInfo* info = new initOrientInfo();

	info->theObj = theObj;
	//info->startOri = theObj->get_eOrientation();
	info->startOri.x = (float)lua_tonumber(L, 4);
	info->startOri.y = (float)lua_tonumber(L, 5);
	info->startOri.z = (float)lua_tonumber(L, 6);
	info->destOri.x = (float)lua_tonumber(L, 7);
	info->destOri.y = (float)lua_tonumber(L, 8);
	info->destOri.z = (float)lua_tonumber(L, 9);
	info->timeInSeconds = (float)lua_tonumber(L, 10);
	info->rampUpTime = (float)lua_tonumber(L, 11);
	info->rampDownTime = (float)lua_tonumber(L, 12);

	iCommand* newCommand = ::g_CommandFactory->makeCommand(Orient, info);

	if (isParallel)
		((cCommandGroup*)commandGroup)->AddParallelCommand(newCommand);
	else
		((cCommandGroup*)commandGroup)->AddSerialCommand(newCommand);


	delete info;


	return 0;
}

// 1: parallel   2:group  3:obj1  4:obj2   5: duration   6: lerp t/f  789: offset
int lua_AddFollowToGroup(lua_State* L)
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		commandGroup = new cCommandGroup();
		g_mapGroupCommands[groupFriendlyName] = commandGroup;
	}

	std::string objFriendlyName(lua_tostring(L, 3));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	std::string objFriendlyName2(lua_tostring(L, 4));
	sPhysicsProperties* theObj2 = findPhys(objFriendlyName2);
	if (theObj2 == NULL)
	{
		std::cout << "Couldn't find other object by that name" << std::endl;
		return 0;
	}

	initFollowInfo* info = new initFollowInfo();

	info->theObj = theObj;
	info->theOtherObj = theObj2;
	info->timeInSeconds = (float)lua_tonumber(L, 5);
	info->isLERP = (bool)lua_toboolean(L, 6);
	info->offset.x = (float)lua_tonumber(L, 7);
	info->offset.y = (float)lua_tonumber(L, 8);
	info->offset.z = (float)lua_tonumber(L, 9);


	iCommand* newCommand = ::g_CommandFactory->makeCommand(Follow, info);

	if (isParallel)
		((cCommandGroup*)commandGroup)->AddParallelCommand(newCommand);
	else
		((cCommandGroup*)commandGroup)->AddSerialCommand(newCommand);


	delete info;


	return 0;
}

int lua_AddLookAtToGroup(lua_State* L)
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		commandGroup = new cCommandGroup();
		g_mapGroupCommands[groupFriendlyName] = commandGroup;
	}

	std::string objFriendlyName(lua_tostring(L, 3));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	std::string objFriendlyName2(lua_tostring(L, 4));
	sPhysicsProperties* theObj2 = findPhys(objFriendlyName2);
	if (theObj2 == NULL)
	{
		std::cout << "Couldn't find other object by that name" << std::endl;
		return 0;
	}

	initLookAtInfo* info = new initLookAtInfo();

	info->theObj = theObj;
	info->theOtherObj = theObj2;
	info->timeInSeconds = (float)lua_tonumber(L, 5);
	info->isLERP = (bool)lua_toboolean(L, 6);
	info->offset.x = (float)lua_tonumber(L, 7);
	info->offset.y = (float)lua_tonumber(L, 8);
	info->offset.z = (float)lua_tonumber(L, 9);


	iCommand* newCommand = ::g_CommandFactory->makeCommand(LookAt, info);

	if (isParallel)
		((cCommandGroup*)commandGroup)->AddParallelCommand(newCommand);
	else
		((cCommandGroup*)commandGroup)->AddSerialCommand(newCommand);


	delete info;


	return 0;
}

//1: isParallel  2: Group name   3: Object Name   4: bool isTranslate    567: xyz
int lua_AddSetTransformToGroup(lua_State* L)
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		commandGroup = new cCommandGroup();
		g_mapGroupCommands[groupFriendlyName] = commandGroup;
	}

	std::string objFriendlyName(lua_tostring(L, 3));
	sPhysicsProperties* theObj = findPhys(objFriendlyName);
	if (theObj == NULL)
	{
		std::cout << "Couldn't find object by that name" << std::endl;
		return 0;
	}

	initTransformInfo* info = new initTransformInfo();

	info->theObj = theObj;
	info->isTranslate = (bool)lua_toboolean(L, 4);
	info->transformData.x = (float)lua_tonumber(L, 5);
	info->transformData.y = (float)lua_tonumber(L, 6);
	info->transformData.z = (float)lua_tonumber(L, 7);

	iCommand* newCommand = ::g_CommandFactory->makeCommand(Transform, info);

	if (isParallel)
		((cCommandGroup*)commandGroup)->AddParallelCommand(newCommand);
	else
		((cCommandGroup*)commandGroup)->AddSerialCommand(newCommand);


	delete info;


	return 0;
}

// Pushes group id'd by group name to serial/parallel vector
int lua_PushGroup(lua_State* L) // 1 = parallel/serial  2 = group name
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		std::cout << "Cannot find group with that name" << std::endl;
		return 0;
	}

	if (isParallel)
		::g_vecParallelCommands.push_back(commandGroup);
	else
		::g_vecSerialCommands.push_back(commandGroup);

	// Now we need to remove the group from the map

	removeGroup(groupFriendlyName);
}


// 1: isParallel   2: Group to push   3: Group pushing into
int lua_PushGroupIntoGroup(lua_State* L)
{
	bool isParallel(lua_toboolean(L, 1));
	std::string groupFriendlyName(lua_tostring(L, 2));

	iCommand* commandGroup = findGroup(groupFriendlyName);
	if (commandGroup == nullptr)
	{
		std::cout << "Cannot find group with that name" << std::endl;
		return 0;
	}

	std::string groupFriendlyName2(lua_tostring(L, 3));

	iCommand* commandGroup2 = findGroup(groupFriendlyName2);
	if (commandGroup2 == nullptr)
	{
		std::cout << "Cannot find group with that name" << std::endl;
		return 0;
	}

	if (isParallel)
		//::g_vecParallelCommands.push_back(commandGroup);
		((cCommandGroup*)commandGroup2)->AddParallelCommand(commandGroup);
	else
		//::g_vecSerialCommands.push_back(commandGroup);
		((cCommandGroup*)commandGroup2)->AddSerialCommand(commandGroup);

	// Now we need to remove the group from the map
	removeGroup(groupFriendlyName);
}

///////// END OF GROUPS /////////////



/// Transform Setting


