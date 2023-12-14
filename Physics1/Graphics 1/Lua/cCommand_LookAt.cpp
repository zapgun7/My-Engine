#include "cCommand_LookAt.h"
#include <iostream>

cCommand_LookAt::cCommand_LookAt()
{

}

cCommand_LookAt::~cCommand_LookAt()
{

}


void cCommand_LookAt::Initialize(void* initStruct)
{
	initLookAtInfo* info = (initLookAtInfo*)initStruct;

	m_pThePhysObj = info->theObj;
	m_pTheOtherObj = info->theOtherObj;
	m_OffsetFollow = info->offset;
	m_IsLERP = info->isLERP;
	m_TimeInSeconds = info->timeInSeconds;

	m_state = LOOKING;
}

bool cCommand_LookAt::Update(double deltaTime)
{
	if ((this->m_pThePhysObj == nullptr) || (this->m_pTheOtherObj == nullptr))
	{
		m_state == LOOKINGDONE;
		return true;
	}

	if (m_elapsedTime == 0)
	{
		PreStart();
	}

	// Update the time
	this->m_elapsedTime += deltaTime;

	glm::vec3 direction = glm::normalize(m_pTheOtherObj->position - m_pThePhysObj->position + m_OffsetFollow);

	if (m_state == LOOKING)
	{
		if (m_IsLERP)
		{
			// LERP lookat
			glm::vec3 oldEuler = m_pThePhysObj->get_eOrientation();
			glm::mat4 tempMat = glm::lookAtLH(m_pThePhysObj->position, m_pThePhysObj->position + direction, glm::vec3(0, 1, 0));
			glm::vec4 newEuler = tempMat * glm::vec4(0, 0, 1,0);
			glm::vec3 finalEul(newEuler.x, newEuler.y, newEuler.z);
			//m_pThePhysObj->setRotationFromQuat(newQuat);
			m_pThePhysObj->setRotationFromEuler(finalEul);
		}
		else
		{
			// Regular rigid lookat
			//m_pThePhysObj->position = m_pTheOtherObj->position + m_OffsetFollow;
			//m_pThePhysObj->setRotationFromQuat(glm::quatLookAtRH(m_pThePhysObj->position + direction, glm::vec3(0, 1, 0)));
			//m_pThePhysObj->setRotationFromQuat(glm::quatLookAt(m_pTheOtherObj->position, glm::vec3(0, 1, 0)));
			//m_pThePhysObj->setRotationFromQuat(glm::quatLookAt(m_pThePhysObj->position + glm::vec3(1,0,0), glm::vec3(0, 1, 0)));

			m_pThePhysObj->setRotationFromQuat(glm::quat(glm::lookAtLH(m_pThePhysObj->position, m_pThePhysObj->position + direction, glm::vec3(0, 1, 0))));
			
			//std::cout << "test" << std::endl;
		}

	}

	if (m_elapsedTime >= m_TimeInSeconds)
		m_state = LOOKINGDONE;



	return this->isDone();
}

// Also called every frame. Returns true when done
bool cCommand_LookAt::isDone(void)
{
	return (this->m_state == LOOKINGDONE);
}

bool cCommand_LookAt::PreStart(void)
{
	// Re-initialize variables



	m_state = LOOKING;

	return false;
}

bool cCommand_LookAt::PostEnd(void)
{

	return false;
}

