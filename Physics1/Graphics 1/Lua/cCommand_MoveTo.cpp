#include "cCommand_MoveTo.h"

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
	initInfo* info = (initInfo*)initStruct;
	
	this->m_startXYZ = info->startPos;
	this->m_endXYZ = info->destPos;
	this->m_TimeToMove = info->timeInSeconds;
	this->m_pThePhysObj = info->theObj;

	// Now to calculate
	this->m_direction = glm::normalize(m_endXYZ - m_startXYZ);
	this->m_speed = glm::length(m_endXYZ - m_startXYZ) / m_TimeToMove;
	this->m_velocity = m_direction * m_speed;
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

	glm::vec3 currentXYZ = this->m_pThePhysObj->position;

	currentXYZ += (this->m_velocity * (float)deltaTime);

	this->m_pThePhysObj->position = currentXYZ;

	return this->isDone();
}

// Also called every frame. Returns true when done
bool cCommand_MoveTo::isDone(void)
{
	if (this->m_elapsedTime >= this->m_TimeToMove )
	{
		// Yes
		return true;
	}
	// Nope
	return false;
}

bool cCommand_MoveTo::PreStart(void)
{
	// Re-initialize variables
	m_startXYZ = m_pThePhysObj->position;
	this->m_direction = glm::normalize(m_endXYZ - m_startXYZ);
	this->m_speed = glm::length(m_endXYZ - m_startXYZ) / m_TimeToMove;
	this->m_velocity = m_direction * m_speed;
	return false;
}

bool cCommand_MoveTo::PostEnd(void)
{

	return false;
}

