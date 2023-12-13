#include "cCommand_MoveTo.h"
#include <iostream>

cCommand_MoveTo::cCommand_MoveTo()
{

}

cCommand_MoveTo::~cCommand_MoveTo()
{

}

// cCommand_MoveTo::cCommand_MoveTo(sPhysicsProperties* pThePhys,
// 								 glm::vec3 startXYZ,
// 								 glm::vec3 endXYZ, 
// 								 float timeToMove)
// {
// 	this->m_startXYZ = startXYZ;
// 	this->m_endXYZ = endXYZ;
// 	this->m_TimeToMove = timeToMove;
// 
// 	this->m_pThePhysObj = pThePhys;
// 
// 
// 	this->m_rayFromStartToEnd = this->m_endXYZ - this->m_startXYZ;
// 
// 	this->m_direction = glm::normalize(this->m_rayFromStartToEnd);
// 
// 	float totalDistance = glm::distance(this->m_endXYZ, this->m_startXYZ);
// 	this->m_speed = totalDistance / timeToMove;
// 
// 	this->m_velocity = this->m_direction * this->m_speed;
// 
// 	this->m_elapsedTime = 0.0;
// }

void cCommand_MoveTo::Initialize(void* initStruct)
{
	initMoveInfo* info = (initMoveInfo*)initStruct;
	
	this->m_startXYZ = info->startPos;
	this->m_endXYZ = info->destPos;
	this->m_TimeToMove = info->timeInSeconds;
	this->m_pThePhysObj = info->theObj;
	//this->rU = info->rampUp;
	//this->rD = info->rampDown;
	this->rUT = info->rampUpTime;
	this->rDT = info->rampDownTime;

	// Now to calculate
	this->m_direction = glm::normalize(m_endXYZ - m_startXYZ);
	//this->m_speed = glm::length(m_endXYZ - m_startXYZ) / m_TimeToMove;
	//this->m_velocity = m_direction * m_speed;

	this->m_state = RAMPUP;
}

bool cCommand_MoveTo::Update(double deltaTime)
{
	if (m_elapsedTime == 0)
	{
		PreStart();
	}
	// I'm deciding that when the time is up, I'm done.

	// Update the time
	this->m_elapsedTime += deltaTime;

	if (m_state == RAMPUP)
	{
		m_pThePhysObj->position = m_startXYZ + (float)(m_elapsedTime * m_elapsedTime) * m_direction * rUSpd;

		if (m_elapsedTime >= rUT)
		{
			// Set where it should be here
			m_pThePhysObj->position = steadyStartPos;//m_startXYZ + (float)(rUT * rUT) * m_direction * rUSpd; 

			// Make up overflowed time
			float overflowTime = m_elapsedTime - rUT;
			m_pThePhysObj->position = steadyStartPos + m_direction * m_MaxSpeed * overflowTime;
			m_state = STEADY;
		}
	}
	else if (m_state == STEADY)
	{
		m_pThePhysObj->position = steadyStartPos + (float)(m_elapsedTime - rUT) * m_direction * m_MaxSpeed;

		if (m_elapsedTime >= m_TimeToMove - rDT)
		{
			m_pThePhysObj->position = rampDownStartPos;

			// Make up for time
			float overflowTime = m_elapsedTime - (m_TimeToMove - rDT);
			m_pThePhysObj->position = rampDownStartPos + ((m_direction * rDSpd * overflowTime) + m_MaxSpeed) * overflowTime;
			m_state = RAMPDOWN;
		}
	}
	else if (m_state == RAMPDOWN)
	{
		float timePassed = m_elapsedTime - (m_TimeToMove - rDT);
		m_pThePhysObj->position = rampDownStartPos + m_direction * ((rDSpd * timePassed) + m_MaxSpeed) * timePassed;

		if (m_elapsedTime >= m_TimeToMove)
		{
			m_pThePhysObj->position = m_endXYZ;
			m_state = DONE;
		}
	}

	return this->isDone();
}

// Also called every frame. Returns true when done
bool cCommand_MoveTo::isDone(void)
{
// 	if (this->m_elapsedTime >= this->m_TimeToMove )
// 	{
// 		// Yes
// 		return true;
// 	}
// 	// Nope
// 	return false;
	return (this->m_state == DONE);
}

bool cCommand_MoveTo::PreStart(void)
{
	// Re-initialize variables
	m_startXYZ = m_pThePhysObj->position;

	m_distance = glm::length(m_endXYZ - m_startXYZ);
	this->m_direction = glm::normalize(m_endXYZ - m_startXYZ);
	
	m_MaxSpeed = m_distance / (m_TimeToMove - rUT * 0.5f - rDT * 0.5f);

	// Calculate constant accel/deccel 
	rUSpd = m_MaxSpeed / rUT;
	rDSpd = -m_MaxSpeed / rDT;

	rUSpd /= 2;
	rDSpd /= 2;
	

	steadyStartPos = m_startXYZ + rUT * rUT * m_direction * rUSpd;
	rampDownStartPos = steadyStartPos + m_MaxSpeed * m_direction * (m_TimeToMove - rUT - rDT);


	
	m_state = RAMPUP;

	return false;
}

bool cCommand_MoveTo::PostEnd(void)
{

	return false;
}

