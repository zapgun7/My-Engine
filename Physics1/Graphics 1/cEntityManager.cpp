#include "cEntityManager.h"
#include "cEnemyEntity.h"



cEntityManager::cEntityManager()
{

}

cEntityManager::~cEntityManager()
{

}

glm::vec3 getRandomVec3(float y, int radius)
{
	glm::vec3 retVec;
	retVec.y = y;

	retVec.x = rand() % radius;
	retVec.z = rand() % radius;

	if (rand() % 2 == 0)
		retVec.x *= -1;
	if (rand() % 2 == 0)
		retVec.z *= -1;

	return retVec;
}

void cEntityManager::Initialize(void)
{
	m_pEntityBuilder = new cEntityBuilder();

	
	// Make AI entity for the AI project
	//glm::vec3 aiPos = glm::vec3(20, 0, 20);
// 	std::vector<glm::vec3> aiPoss;
// 	aiPoss.push_back(glm::vec3(50, 0, 0));
// 	aiPoss.push_back(glm::vec3(0, 0, 50));
// 	aiPoss.push_back(glm::vec3(-50, 0, 0));

// 	for (unsigned int i = 0; i < 100; i++)
// 	{
// 		//m_vecEntities.push_back(m_pEntityBuilder->MakeEntity(AIPROJ2, aiPoss[i]));
// 		glm::vec3 newPos = getRandomVec3(0.0f, 100.0f);
// 		m_vecEntities.push_back(m_pEntityBuilder->MakeEntity(AIPROJ3, newPos));
// 
// 		//(static_cast<cEnemyEntity*>(m_vecEntities[i]))->setTargetObject(m_pPlayerObj);
// 	}

	glm::vec3 newPos = glm::vec3(100, 20, -500);
	m_vecEntities.push_back(m_pEntityBuilder->MakeEntity(AIPROJ4, newPos, m_pPlayerObj, m_pTheNavMesh));
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

void cEntityManager::SetNavMesh(cNavMesh* mesh)
{
	m_pTheNavMesh = mesh;
	return;
}

void cEntityManager::SetPlayerNavTri(void* playerTri)
{
	m_vecEntities[0]->updatePlayerTri(playerTri);
	return;
}

glm::vec3 cEntityManager::getEntityPos(void)
{
	return m_vecEntities[0]->getPosition();
}

