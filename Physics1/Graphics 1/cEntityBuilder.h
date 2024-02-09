#pragma once

#include "cEngineController.h"

#include "iEntity.h"


enum eEntityType
{
	AIPROJ
};



class cEntityBuilder
{
public:
	cEntityBuilder();
	~cEntityBuilder();


	iEntity* MakeEntity(eEntityType type, glm::vec3& pos);



private:
	cEngineController* m_pEngineController = nullptr;


	int tempCounter4Project = 0;
};