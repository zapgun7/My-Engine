#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
//#include "../Other Graphics Stuff/cMesh.h"
#include "../Physics/sPhysicsProperties.h"

#include "iCommand.h"

enum lookingState
{
	LOOKING,
	LOOKINGDONE
};

struct initLookAtInfo
{
	initLookAtInfo()
		:timeInSeconds(0)
		, theObj(nullptr)
		, theOtherObj(nullptr)
		, isLERP(false)
		, offset(glm::vec3(0))
	{};

	float timeInSeconds;
	sPhysicsProperties* theObj;
	sPhysicsProperties* theOtherObj;
	bool isLERP;
	glm::vec3 offset;
};

class cCommand_LookAt : public iCommand
{
public:
	cCommand_LookAt();
	virtual ~cCommand_LookAt();

	virtual void Initialize(void* initStruct);

	// Call every frame
	virtual bool Update(double deltaTime);
	// Also called every frame. Returns true when done
	virtual bool isDone(void);

	virtual bool PreStart(void);
	virtual bool PostEnd(void);

private:
	// Can't call default constructor


	//glm::vec3 m_startXYZ, m_endXYZ;
	//float m_TimeToMove;
	sPhysicsProperties* m_pThePhysObj;
	sPhysicsProperties* m_pTheOtherObj;

	glm::vec3 m_OffsetFollow;
	bool m_IsLERP;
	float m_TimeInSeconds;




	lookingState m_state;

	double m_elapsedTime;
};
