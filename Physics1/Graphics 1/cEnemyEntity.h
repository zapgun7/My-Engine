#pragma once

#include "Other Graphics Stuff/OpenGLCommon.h"
#include <glm/vec3.hpp>

#include "iEntity.h"

struct sPhysicsProperties;




enum eAIType
{
	SEEK,
	FLEE,
	PURSUE,
	EVADE,
	APPROACH
	
};


class cEnemyEntity : public iEntity
{
public:
	cEnemyEntity(sPhysicsProperties* entityObj);
	~cEnemyEntity();

	virtual void Update(double dt);


	virtual glm::vec3 getPosition(void);
	virtual glm::quat getOrientation(void);


	void setTargetObject(sPhysicsProperties* goalObj);

private:
	sPhysicsProperties* m_pEntityObject = nullptr;
	sPhysicsProperties* m_pPlayerEntity = nullptr; // Do it this way, or pass target object every update


	glm::vec3 getLookVector(void); // Look vector in XZ


	const float MAXMOVESPEED = 20.0f;
	const float MAXTURNSPEED = 5.0f;

	eAIType m_eType = SEEK;

};