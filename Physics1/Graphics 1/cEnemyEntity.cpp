#include "cEnemyEntity.h"

#include "Physics/sPhysicsProperties.h"


cEnemyEntity::cEnemyEntity(sPhysicsProperties* entityObj, eAIType type)
{
	m_pEntityObject = entityObj;

	m_eType = type;
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
	bool isMoving = false;

	glm::vec3 vecToGoal;// = m_pPlayerEntity->position - m_pEntityObject->position;
	glm::vec3 goalPos;
	//glm::vec3 vecToGoalNorm;// = glm::normalize(vecToGoal);

	if ((m_eType == SEEK) || (m_eType == FLEE) || (m_eType == APPROACH)) // Make this 2x (!condition)'s? 
	{
		vecToGoal = m_pPlayerEntity->position - m_pEntityObject->position;
		goalPos = m_pPlayerEntity->position;
	}
	else
	{
		goalPos = m_pPlayerEntity->position + m_pPlayerEntity->velocity * PREDICTIONOFFSET;
		vecToGoal = goalPos - m_pEntityObject->position;
	}

	glm::vec3 vecToGoalNorm = glm::normalize(vecToGoal);

	//float radDiff = abs(acos(glm::dot(getLookVector(), vecToGoal) / glm::length(m_NewDir) * glm::length(m_dir)));
	//float radDiff = acos(glm::dot(getLookVector(), glm::normalize(vecToGoal))); // As long as vecs are unit, we don't need to divide?

	// Negative y: turn right,   Positive y: turn left       FOR SEEKING
	glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(vecToGoal));
	float radDiff = asin(glm::length(crossResult));
	
	printf("Y: %.2f  |  %.2f\n", crossResult.y, radDiff);


	// Depending on behavior do something related to the goal
	if ((m_eType == SEEK) || (m_eType == PURSUE))
	{
		if (crossResult.y > 0) // Turn left
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}
		else // Turn right
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}
		if ((radDiff < 0.3f)
			&& (glm::distance(m_pEntityObject->position, m_pPlayerEntity->position) > (glm::distance(m_pEntityObject->position + getLookVector(), m_pPlayerEntity->position))))
		{
			m_pEntityObject->velocity += getLookVector() * MOVESPEED * static_cast<float>(dt);
			isMoving = true;
		}
	}
	else if ((m_eType == FLEE) || (m_eType == EVADE))
	{
		if (crossResult.y > 0) 
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}
		else 
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}
		if ((radDiff < 0.8f) // Wider range to start fleeing/evading
			&& (glm::distance(m_pEntityObject->position, m_pPlayerEntity->position) < (glm::distance(m_pEntityObject->position + getLookVector(), m_pPlayerEntity->position))))
		{
			m_pEntityObject->velocity += getLookVector() * MOVESPEED * static_cast<float>(dt);
			isMoving = true;
		}
	}
	else // Approach
	{
		if (crossResult.y > 0) // Turn left
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}
		else // Turn right
		{
			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
		}

		float entPlyrDist = glm::distance(m_pEntityObject->position, m_pPlayerEntity->position);
		if ((radDiff < 0.2f)
			&& (entPlyrDist > (glm::distance(m_pEntityObject->position + getLookVector(), m_pPlayerEntity->position))))
		{
			if (entPlyrDist > APPROACHDIST + APPROACHAMP) // If too far away
			{
				// moveSpdAdjust reduces how much then enemy rubber bands back and forth trying to keep the certain distance
				// It just reduces its acceleration the closer to the sweet spot it is
				float moveSpdAdjust = (entPlyrDist - (APPROACHDIST + APPROACHAMP)) * 0.05f; 
				moveSpdAdjust = (moveSpdAdjust > 1) ? 1 : moveSpdAdjust;
				
				m_pEntityObject->velocity += getLookVector() * MOVESPEED * moveSpdAdjust * static_cast<float>(dt);
				isMoving = true;
			}
			else if (entPlyrDist < APPROACHDIST - APPROACHAMP) // If too close
			{
				float moveSpdAdjust = ((APPROACHDIST + APPROACHAMP) - entPlyrDist) * 0.05f;
				moveSpdAdjust = (moveSpdAdjust > 1) ? 1 : moveSpdAdjust;

				m_pEntityObject->velocity += getLookVector() * (-MOVESPEED) * moveSpdAdjust * static_cast<float>(dt);
				isMoving = true;
			}
		}
	}



	// Limit overall speed
	if (glm::length(m_pEntityObject->velocity) > VELOCITYLIMIT)
	{
		m_pEntityObject->velocity = glm::normalize(m_pEntityObject->velocity) * VELOCITYLIMIT;
	}

	if (true)//(!isMoving)
	{
		// Regular speed reduction
		m_pEntityObject->velocity -= m_pEntityObject->velocity * SPEEDREDUCTION * static_cast<float>(dt);
	}


	// Keep in bounds so fleeing entites don't truly escape
	if (abs(m_pEntityObject->position.x) > BOUNDSX)
	{
		m_pEntityObject->position.x = BOUNDSX * (abs(m_pEntityObject->position.x) / m_pEntityObject->position.x);
	}
	if (abs(m_pEntityObject->position.z) > BOUNDSZ)
	{
		m_pEntityObject->position.z = BOUNDSX * (abs(m_pEntityObject->position.z) / m_pEntityObject->position.z);
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

