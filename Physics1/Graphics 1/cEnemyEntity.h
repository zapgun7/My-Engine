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
	cEnemyEntity(sPhysicsProperties* entityObj, eAIType type);
	~cEnemyEntity();

	virtual void Update(double dt);


	virtual glm::vec3 getPosition(void);
	virtual glm::quat getOrientation(void);


	void setTargetObject(sPhysicsProperties* goalObj);

private:
	sPhysicsProperties* m_pEntityObject = nullptr;
	sPhysicsProperties* m_pPlayerEntity = nullptr; // Do it this way, or pass target object every update


	glm::vec3 getLookVector(void); // Look vector in XZ


	const float MOVESPEED = 20.0f;
	const float ROTATIONSPEED = 30.0f;
	const float VELOCITYLIMIT = 15.0f;
	const float SPEEDREDUCTION = 0.99f;

	const float APPROACHDIST = 30.0f;
	const float APPROACHAMP = 2.0f;

	const float BOUNDSX = 150.0f;
	const float BOUNDSZ = 150.0f;


	const float PREDICTIONOFFSET = 1.5f; // Distance in direction the player's moving the AI will work around

	eAIType m_eType = SEEK;

};