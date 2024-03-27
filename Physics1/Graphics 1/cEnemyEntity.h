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
	APPROACH,
	WANDER1,
	WANDER2,
	WANDER3,
	FLOCK
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
	void setFlockTarget(glm::vec3* target);

private:
	sPhysicsProperties* m_pEntityObject = nullptr;
	sPhysicsProperties* m_pPlayerEntity = nullptr; // Do it this way, or pass target object every update

	// Flocking Vars
	glm::vec3* flockTarget;
	float flockRadius = 40.0f;
	glm::vec3 currGoal = glm::vec3(0.0f);
	glm::vec3 currGoalDir;// = glm::vec3(0.0f, 0.0f, 0.0f);
	bool isOnCourseForCircle = false;


	// Wandering Vars
	glm::vec3 currMoveDirGoal = glm::vec3(0.0f, 0.0f, 1.0f);
	float remainingDist = 0.0f;

	//End of Wandering Vars

	glm::vec3 getLookVector(void); // Look vector in XZ


	const float MOVESPEED = 20.0f;
	const float ROTATIONSPEED = 100.0f;
	const float VELOCITYLIMIT = 15.0f;
	const float SPEEDREDUCTION = 0.99f;

	const float APPROACHDIST = 30.0f;
	const float APPROACHAMP = 2.0f;

	const float BOUNDSX = 150.0f;
	const float BOUNDSZ = 150.0f;


	const float PREDICTIONOFFSET = 1.5f; // Distance in direction the player's moving the AI will work around

	eAIType m_eType = SEEK;

};