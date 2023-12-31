#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
//#include "../Other Graphics Stuff/cMesh.h"
#include "../Physics/sPhysicsProperties.h"

#include "iCommand.h"

enum moveState
{
	RAMPUP,
	STEADY,
	RAMPDOWN,
	DONE
};

struct initMoveInfo
{
	initMoveInfo()
		: startPos(glm::vec3(0))
		, destPos(glm::vec3(0))
		, rampUpTime(0)
		, rampDownTime(0)
		, timeInSeconds(0)
		, theObj(nullptr)
	{};

	glm::vec3 startPos;
	glm::vec3 destPos;
	float timeInSeconds, rampUpTime, rampDownTime;
	sPhysicsProperties* theObj;
	// Can add other stuff to specify easing functions l8r
};

class cCommand_MoveTo : public iCommand
{
public:
	//cCommand_MoveTo(sPhysicsProperties* pPhysToMove, 
	//				glm::vec3 startXYZ, glm::vec3 endXYZ, 
	//				float timeToMove);
	cCommand_MoveTo();
	virtual ~cCommand_MoveTo();

	virtual void Initialize(void* initStruct);

	// Call every frame
	virtual bool Update(double deltaTime);
	// Also called every frame. Returns true when done
	virtual bool isDone(void);

	virtual bool PreStart(void);
	virtual bool PostEnd(void);

private:
	// Can't call default constructor
	

	glm::vec3 m_startXYZ, m_endXYZ;
	float m_TimeToMove;
	sPhysicsProperties* m_pThePhysObj; 
	float rUT, rDT;
	float rUSpd, rDSpd;

	glm::vec3 steadyStartPos, rampDownStartPos;

	// These are calculated
	float m_distance;
	float m_MaxSpeed;
	glm::vec3 rampAcceleration;
	glm::vec3 rampDecelleration;
	glm::vec3 m_direction;


	moveState m_state;

	double m_elapsedTime;
};
