#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
//#include "../Other Graphics Stuff/cMesh.h"
#include "../Physics/sPhysicsProperties.h"

#include "iCommand.h"

enum bezierState
{
	BEZRAMPUP,
	BEZSTEADY,
	BEZRAMPDOWN,
	BEZDONE
};

struct initBezMoveInfo
{
	initBezMoveInfo()
		: startPos(glm::vec3(0))
		, destPos(glm::vec3(0))
		, bezOffset(glm::vec2(0))
		, rampUpTime(0)
		, rampDownTime(0)
		, timeInSeconds(0)
		, theObj(nullptr)
	{};

	glm::vec3 startPos;
	glm::vec3 destPos;
	glm::vec2 bezOffset; // up/down & left/right curve direction from object's start->end direction
	float timeInSeconds, rampUpTime, rampDownTime;
	sPhysicsProperties* theObj;
};

class cCommand_MoveToBezier : public iCommand
{
public:
	//cCommand_MoveTo(sPhysicsProperties* pPhysToMove, 
	//				glm::vec3 startXYZ, glm::vec3 endXYZ, 
	//				float timeToMove);
	cCommand_MoveToBezier();
	virtual ~cCommand_MoveToBezier();

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

	// New to bezier!
	glm::vec2 bezOffset; // Will play into calculations of below variables in PreStart
	glm::vec3 bezierPoint;
	glm::vec3 AtoBez;
	glm::vec3 BeztoB;

	glm::vec3 internalFauxPos; // Old position we'll still keep track of


	bezierState m_state;

	double m_elapsedTime;
};
