#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
//#include "../Other Graphics Stuff/cMesh.h"
#include "../Physics/sPhysicsProperties.h"

#include "iCommand.h"

enum oriState
{
	ORIRAMPUP,
	ORISTEADY,
	ORIRAMPDOWN,
	ORIDONE
};


struct initOrientInfo
{
	initOrientInfo()
		: startOri(glm::vec3(0))
		, destOri(glm::vec3(0))
		, timeInSeconds(0)
		, rampUpTime(0)
		, rampDownTime(0)
		, theObj(nullptr)
	{};

	glm::vec3 startOri;
	glm::vec3 destOri;
	float timeInSeconds, rampUpTime, rampDownTime;
	sPhysicsProperties* theObj;
	// Can add other stuff to specify easing functions l8r
};

class cCommand_Orient : public iCommand
{
public:
	//cCommand_MoveTo(sPhysicsProperties* pPhysToMove, 
	//				glm::vec3 startXYZ, glm::vec3 endXYZ, 
	//				float timeToMove);
	cCommand_Orient();
	virtual ~cCommand_Orient();

	virtual void Initialize(void* initStruct);

	// Call every frame
	virtual bool Update(double deltaTime);
	// Also called every frame. Returns true when done
	virtual bool isDone(void);

	virtual bool PreStart(void);
	virtual bool PostEnd(void);

private:
	// Can't call default constructor


	glm::vec3 m_startOriEuler, m_endOriEuler;
	float m_TimeToMove;
	sPhysicsProperties* m_pThePhysObj;
	float rUT, rDT; // Time it takes to ramp up and down
	float rUSpd, rDSpd;

	glm::vec3 steadyStartOri, rampDownStartOri;

	// These are calculated
	float m_distance;
	float m_MaxSpeed;
	glm::vec3 rampAcceleration;
	glm::vec3 rampDecelleration;

	glm::vec3 m_direction;
	//glm::vec3 m_RotateVelocity;
	//glm::quat m_qRotationalVelocity;
	//float m_speed;

	oriState m_state;

	double m_elapsedTime;
};
