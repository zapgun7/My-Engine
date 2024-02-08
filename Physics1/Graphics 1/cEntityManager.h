#pragma once

#include "iEntity.h"
#include "cEntityBuilder.h"

#include <vector>

class cEntityManager
{
public:
	cEntityManager();
	~cEntityManager();


	void Initialize(void);
	void Update(double dt);


	void SetPlayer(sPhysicsProperties* playerObj);


private:
	std::vector<iEntity*> m_vecEntities;

	cEntityBuilder* m_pEntityBuilder;

	sPhysicsProperties* m_pPlayerObj;
};