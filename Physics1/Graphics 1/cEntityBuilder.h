#pragma once

#include "cEngineController.h"

#include "iEntity.h"


enum eEntityType
{
	AIPROJ,
	AIPROJ2,
	AIPROJ3
};



class cEntityBuilder
{
public:
	cEntityBuilder();
	~cEntityBuilder();


	iEntity* MakeEntity(eEntityType type, glm::vec3& pos);



private:
	cEngineController* m_pEngineController = nullptr;

	glm::vec3* flockTarget;


	int tempCounter4Project = 0;
};