#include "cLuaBrain.h"
#include <iostream>

#include "../Other Graphics Stuff/cMesh.h"			// 
#include "../Other Graphics Stuff/cLightManager.h"  // For making changes to these
#include "../Physics/sPhysicsProperties.h"			//

#include "iCommand.h"
#include "cCommand_MoveTo.h"
#include "cCommand_Orient.h"
#include "cCommandFactory.h"



// Global info on objects in the system
std::vector<cMesh*> g_vecMesh;
std::vector<sPhysicsProperties*> g_vecPhys;

// Command Vectors
std::vector<iCommand*> g_vecSerialCommands; // Run only the first every update
std::vector<iCommand*> g_vecParallelCommands; // Run all of these every update


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
			itParallel--; // This puts it 1 before the next one to update
		}
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

/// Transform Setting

int lua_SetObjPositionByFriendlyName(lua_State* L)
{
	std::string ObjFriendlyName(lua_tostring(L, 1));
	sPhysicsProperties* theObj = findPhys(ObjFriendlyName);

	if (theObj == NULL)
		return 0;

	glm::vec3 newLocationXYZ;
	newLocationXYZ.x = (float)lua_tonumber(L, 2);
	newLocationXYZ.y = (float)lua_tonumber(L, 3);
	newLocationXYZ.z = (float)lua_tonumber(L, 4);

	theObj->position = newLocationXYZ;

	return 0;
}