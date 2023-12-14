#include "cCommand_Follow.h"
#include <iostream>

cCommand_Follow::cCommand_Follow()
{

}

cCommand_Follow::~cCommand_Follow()
{

}


void cCommand_Follow::Initialize(void* initStruct)
{
	initFollowInfo* info = (initFollowInfo*)initStruct;

	m_pThePhysObj = info->theObj;
	m_pTheOtherObj = info->theOtherObj;
	m_OffsetFollow = info->offset;
	m_IsLERP = info->isLERP; 
	m_TimeInSeconds = info->timeInSeconds;

	m_state = FOLLOWING;
}

bool cCommand_Follow::Update(double deltaTime)
{
	if ((this->m_pThePhysObj == nullptr) || (this->m_pTheOtherObj == nullptr))
	{
		m_state == FOLLOWDONE;
		return true;
	}

	if (m_elapsedTime == 0)
	{
		PreStart();
	}

	// Update the time
	this->m_elapsedTime += deltaTime;

	if (m_state == FOLLOWING)
	{
		if (m_IsLERP)
		{
			// LERP follow
			glm::vec3 objPos = m_pThePhysObj->position;
			objPos += ((m_pTheOtherObj->position + m_OffsetFollow) - objPos) * 0.8f * (float)deltaTime;
			m_pThePhysObj->position = objPos;
		}
		else
		{
			// Regular rigid follow
			m_pThePhysObj->position = m_pTheOtherObj->position + m_OffsetFollow;
		}

	}

	if (m_elapsedTime >= m_TimeInSeconds)
		m_state = FOLLOWDONE;

	

	return this->isDone();
}

// Also called every frame. Returns true when done
bool cCommand_Follow::isDone(void)
{
	return (this->m_state == FOLLOWDONE);
}

bool cCommand_Follow::PreStart(void)
{
	// Re-initialize variables



	m_state = FOLLOWING;

	return false;
}

bool cCommand_Follow::PostEnd(void)
{

	return false;
}

