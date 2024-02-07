#include "cEnemyEntity.h"

#include "Physics/sPhysicsProperties.h"
#include "Other Graphics Stuff/OpenGLCommon.h"

cEnemyEntity::cEnemyEntity()
{

}

cEnemyEntity::~cEnemyEntity()
{

}

void cEnemyEntity::Update(double deltaTime)
{
	// Depending on behavior do something related to the goal
	switch (this->m_eType)
	{

	}
}

void cEnemyEntity::setTargetObject(sPhysicsProperties* goalObj)
{
	this->m_pPlayerEntity = goalObj;
	return;
}

