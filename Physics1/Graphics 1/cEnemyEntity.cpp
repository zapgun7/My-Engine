#include "cEnemyEntity.h"

#include "Physics/sPhysicsProperties.h"


cEnemyEntity::cEnemyEntity(sPhysicsProperties* entityObj)
{
	m_pEntityObject = entityObj;
}

cEnemyEntity::~cEnemyEntity()
{

}

// SEEK
// FLEE
// PURSUE
// EVADE
// APPROACH
void cEnemyEntity::Update(double dt)
{
	glm::vec3 vecToGoal;// = m_pPlayerEntity->position - m_pEntityObject->position;
	//glm::vec3 vecToGoalNorm;// = glm::normalize(vecToGoal);

	if ((m_eType == SEEK) || (m_eType == FLEE) || (m_eType == APPROACH)) // Make this 2x (!condition)'s? 
		vecToGoal = m_pPlayerEntity->position - m_pEntityObject->position;
	else
		vecToGoal = m_pPlayerEntity->position + glm::normalize(m_pPlayerEntity->velocity) * PREDICTIONOFFSET;

	glm::vec3 vecToGoalNorm = glm::normalize(vecToGoal);

	//float radDiff = abs(acos(glm::dot(getLookVector(), vecToGoal) / glm::length(m_NewDir) * glm::length(m_dir)));
	//float radDiff = acos(glm::dot(getLookVector(), glm::normalize(vecToGoal))); // As long as vecs are unit, we don't need to divide?

	// Negative y: turn right,   Positive y: turn left       FOR SEEKING
	glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(vecToGoal));
	float radDiff = asin(glm::length(crossResult));
	
	printf("Y: %.2f  |  %.2f\n", crossResult.y, radDiff);


	// Depending on behavior do something related to the goal
	switch (this->m_eType)
	{
	case SEEK:
		if (crossResult.y > 0) // Turn left
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
			//m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}
		else // Turn right
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
			//m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}
		break;
	}


	return;
}

glm::vec3 cEnemyEntity::getPosition(void)
{
	return m_pEntityObject->position;
}

glm::quat cEnemyEntity::getOrientation(void)
{
	return m_pEntityObject->get_qOrientation();
}

void cEnemyEntity::setTargetObject(sPhysicsProperties* goalObj)
{
	this->m_pPlayerEntity = goalObj;
	return;
}

glm::vec3 cEnemyEntity::getLookVector(void)
{
	glm::vec3 lookVec(0, 0, 1);
	lookVec = this->m_pEntityObject->get_qOrientation() * lookVec;
	lookVec.y = 0;


	return glm::normalize(lookVec);
}

