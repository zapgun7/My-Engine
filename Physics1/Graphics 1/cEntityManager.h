#pragma once

#include "iEntity.h"
#include "cEntityBuilder.h"

#include "cNavMesh.h"

#include <vector>

class cEntityManager
{
public:
	cEntityManager();
	~cEntityManager();


	void Initialize(void);
	void Update(double dt);



	void SetPlayer(sPhysicsProperties* playerObj);
	void SetNavMesh(cNavMesh* mesh);
	void SetPlayerNavTri(void* playerTri);
	glm::vec3 getEntityPos(void);

private:
	std::vector<iEntity*> m_vecEntities;

	cEntityBuilder* m_pEntityBuilder;

	sPhysicsProperties* m_pPlayerObj;
	cNavMesh* m_pTheNavMesh;
};