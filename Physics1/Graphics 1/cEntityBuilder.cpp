#include "cEntityBuilder.h"
#include "cEnemyEntity.h"
#include "Other Graphics Stuff/cMesh.h"


#include "Physics/sPhysicsProperties.h"

cEntityBuilder::cEntityBuilder()
{
	m_pEngineController = cEngineController::GetEngineController();
	flockTarget = new glm::vec3(0.0f);
}

cEntityBuilder::~cEntityBuilder()
{

}

glm::vec4 GetRandomColor(void)
{
	glm::vec3 retVec;// = glm::vec4(1.0f);
	retVec.x = (rand() % 100 + 150);
	retVec.y = (rand() % 100 + 150);
	retVec.z = (rand() % 100 + 150);
	
	retVec = glm::normalize(retVec);
	retVec *= 1.5f;

	for (int i = 0; i < 3; i++)
	{
		if (retVec[i] > 1.0f)
		{
			float remvAmt = retVec[i] - 1.0f;
			retVec[i] -= remvAmt;
			retVec[(i + 1) % 3] -= remvAmt;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		if (retVec[i] < 0.0f)
		{
			retVec[i] = 0.0f;
		}
	}


	return glm::vec4(retVec, 1.0f);
}

iEntity* cEntityBuilder::MakeEntity(eEntityType type, glm::vec3& pos)
{
	iEntity* newEntity = nullptr;

	sPhysicsProperties* newEntPhys = new sPhysicsProperties();
	newEntPhys->position = pos;
	newEntPhys->friendlyName = "enemy";

	cMesh* newEntMesh = new cMesh();
	newEntMesh->meshName = "rusty_ship.ply";
	newEntMesh->friendlyName = "enemy";
	newEntMesh->bDoNotLight = false;
	//newEntMesh->textureName[0] = "water.bmp";
	newEntMesh->bUseCustomColors = true;
	newEntMesh->customColorRGBA = GetRandomColor();//glm::vec4(1.0f, 0, 0, 1.0f);
	newEntMesh->uniqueID = newEntPhys->getUniqueID();

	newEntPhys->pTheAssociatedMesh = newEntMesh;

	m_pEngineController->addCustomObject(newEntMesh, newEntPhys);


	switch (type)
	{
	case AIPROJ:

		// Setup just for spawning all required for the project
		if (tempCounter4Project == 0)
		{
			newEntMesh->customColorRGBA = glm::vec4(1.0f, 0, 0, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, SEEK);
			tempCounter4Project++;
		}
		else if (tempCounter4Project == 1)
		{
			newEntMesh->customColorRGBA = glm::vec4(0, 0, 1.0f, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, FLEE);
			tempCounter4Project++;
		}
		else if (tempCounter4Project == 2)
		{
			newEntMesh->customColorRGBA = glm::vec4(1.0f, 0, 1.0f, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, PURSUE);
			tempCounter4Project++;
		}
		else if (tempCounter4Project == 3)
		{
			newEntMesh->customColorRGBA = glm::vec4(0.0f, 0.3f, 0.7f, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, EVADE);
			tempCounter4Project++;
		}
		else if (tempCounter4Project == 4)
		{
			newEntMesh->customColorRGBA = glm::vec4(0.7f, 0.7f, 0, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, APPROACH);
			tempCounter4Project++;
		}
		break;
	case AIPROJ2:

		// Setup just for spawning all required for the project
		if (tempCounter4Project == 0)
		{
			newEntMesh->customColorRGBA = glm::vec4(1.0f, 0, 0, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, WANDER1);
			tempCounter4Project++;
		}
		else if (tempCounter4Project == 1)
		{
			newEntMesh->customColorRGBA = glm::vec4(0, 0, 1.0f, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, WANDER2);
			tempCounter4Project++;
		}
		else if (tempCounter4Project == 2)
		{
			newEntMesh->customColorRGBA = glm::vec4(1.0f, 0, 1.0f, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, WANDER3);
			tempCounter4Project++;
		}
		else if (tempCounter4Project == 3)
		{
			newEntMesh->customColorRGBA = glm::vec4(0.0f, 0.3f, 0.7f, 1.0f);
			newEntity = new cEnemyEntity(newEntPhys, EVADE);
			tempCounter4Project++;
		}
		break;
	case AIPROJ3:
		newEntity = new cEnemyEntity(newEntPhys, FLOCK);
		((cEnemyEntity*)newEntity)->setFlockTarget(flockTarget);
	}



	return newEntity;
}

