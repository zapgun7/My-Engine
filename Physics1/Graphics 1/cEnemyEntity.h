#pragma once

#include "Other Graphics Stuff/OpenGLCommon.h"
#include <glm/vec3.hpp>

#include "cNavMesh.h" // Where can I go??

#include "iEntity.h"

#include "cSoundManager.h" // For setting heartbeat sounds

#include <random>

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
	FLOCK,
	NAVWANDER,
	NAVWATCH,
	NAVCHASE
};


class cEnemyEntity : public iEntity
{
public:
	cEnemyEntity(sPhysicsProperties* entityObj, eAIType type);
	~cEnemyEntity();

	virtual void Update(double dt);
	virtual void updatePlayerTri(void* tri);

	virtual glm::vec3 getPosition(void);
	virtual glm::quat getOrientation(void);


	void setNavMesh(cNavMesh* theNavMesh);
	void setTargetObject(sPhysicsProperties* goalObj);
	void setFlockTarget(glm::vec3* target);

private:
	sPhysicsProperties* m_pEntityObject = nullptr;
	sPhysicsProperties* m_pPlayerEntity = nullptr; // Do it this way, or pass target object every update

	// NavMesh
	cNavMesh* m_pNavMesh = nullptr;
	cNavMesh::sNavTri* m_pPrevNavTri = nullptr;
	cNavMesh::sNavTri* m_pCurrNavTri = nullptr;
	cNavMesh::sNavTri* m_pTargetNavTri = nullptr;

	cNavMesh::sNavTri* m_pPlayerTri = nullptr;

	cNavMesh::sNavTri* chooseNewDir(void);
	int distInMemory(unsigned int triID);
	void AddToMemory(int newID);

	cSoundManager* m_pSoundManager = nullptr;
	float m_HeartBeatVolume = 0.0f;
	float m_DroneVolume = 0.0f;
	float m_DroneVolTarget = 0.45f; // This is reduced when the heartbeat ramps up


	const unsigned int MEMORY_SIZE = 100;
	int* m_IntersectionMemory; // paths it took where there were >2 choices
	unsigned int nextMemIDX;


	// RANDOM ARRAY
	int getRandNum(void);
	int randArraySize;
	int nextRandIDX;
	int* m_RandArray;

	std::vector<cNavMesh::sNavTri*> contenders;





	// Basic nav waypoints
	unsigned int currGoalIDX;
	std::vector<glm::vec3> waypoints;
	


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


	const float MOVESPEED = 80.0f;
	const float ROTATIONSPEED = 200.0f;
	const float VELOCITYLIMIT = 15.0f;
	const float SPEEDREDUCTION = 0.99f;

	const float APPROACHDIST = 30.0f;
	const float APPROACHAMP = 2.0f;

	const float BOUNDSX = 150.0f;
	const float BOUNDSZ = 150.0f;


	const float PREDICTIONOFFSET = 1.5f; // Distance in direction the player's moving the AI will work around

	eAIType m_eType = SEEK;

};