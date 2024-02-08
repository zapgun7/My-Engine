#include "cEntityManager.h"
#include "cEnemyEntity.h"



cEntityManager::cEntityManager()
{

}

cEntityManager::~cEntityManager()
{

}

void cEntityManager::Initialize(void)
{
	m_pEntityBuilder = new cEntityBuilder();
	
	// Make AI entity for the AI project
	glm::vec3 aiPos = glm::vec3(20, 0, 20);
	m_vecEntities.push_back(m_pEntityBuilder->MakeEntity(AIPROJ, aiPos));

	(static_cast<cEnemyEntity*>(m_vecEntities[0]))->setTargetObject(m_pPlayerObj);
}

void cEntityManager::Update(double dt)
{

	for (iEntity* currEnt : m_vecEntities)
	{
		currEnt->Update(dt);
	}
}

void cEntityManager::SetPlayer(sPhysicsProperties* playerObj)
{
	m_pPlayerObj = playerObj;
	return;
}

