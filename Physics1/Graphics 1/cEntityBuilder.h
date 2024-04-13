#pragma once

#include "cEngineController.h"

#include "iEntity.h"


class cNavMesh;

enum eEntityType
{
	AIPROJ,
	AIPROJ2,
	AIPROJ3,
	AIPROJ4
};



class cEntityBuilder
{
public:
	cEntityBuilder();
	~cEntityBuilder();


	iEntity* MakeEntity(eEntityType type, glm::vec3& pos, sPhysicsProperties* plyrObj, cNavMesh* navMesh);



private:
	cEngineController* m_pEngineController = nullptr;

	glm::vec3* flockTarget;


	int tempCounter4Project = 0;
};