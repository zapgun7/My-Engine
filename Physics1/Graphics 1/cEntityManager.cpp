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
	//glm::vec3 aiPos = glm::vec3(20, 0, 20);
	std::vector<glm::vec3> aiPoss;
	aiPoss.push_back(glm::vec3(50, 0, 0));
	aiPoss.push_back(glm::vec3(0, 0, 50));
	aiPoss.push_back(glm::vec3(-50, 0, 0));

	for (unsigned int i = 0; i < 3; i++)
	{
		m_vecEntities.push_back(m_pEntityBuilder->MakeEntity(AIPROJ2, aiPoss[i]));

		//(static_cast<cEnemyEntity*>(m_vecEntities[i]))->setTargetObject(m_pPlayerObj);
	}
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

