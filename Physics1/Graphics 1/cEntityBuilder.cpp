#include "cEntityBuilder.h"
#include "cEnemyEntity.h"
#include "Other Graphics Stuff/cMesh.h"


#include "Physics/sPhysicsProperties.h"

cEntityBuilder::cEntityBuilder()
{
	m_pEngineController = cEngineController::GetEngineController();
}

cEntityBuilder::~cEntityBuilder()
{

}

iEntity* cEntityBuilder::MakeEntity(eEntityType type, glm::vec3& pos)
{
	iEntity* newEntity = nullptr;

	switch (type)
	{
	case AIPROJ:
		sPhysicsProperties* newEntPhys = new sPhysicsProperties();
		newEntPhys->position = pos;
		newEntPhys->friendlyName = "enemy";

		cMesh* newEntMesh = new cMesh();
		newEntMesh->meshName = "rusty_ship.ply";
		newEntMesh->friendlyName = "enemy";
		newEntMesh->bDoNotLight = false;
		//newEntMesh->textureName[0] = "water.bmp";
		newEntMesh->bUseDebugColours = true;
		newEntMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0, 0, 1.0f);
		newEntMesh->uniqueID = newEntPhys->getUniqueID();

		newEntPhys->pTheAssociatedMesh = newEntMesh;
		
		m_pEngineController->addCustomObject(newEntMesh, newEntPhys);


		newEntity = new cEnemyEntity(newEntPhys);
		break;
	}



	return newEntity;
}

