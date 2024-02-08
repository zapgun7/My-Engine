#include "cEnemyEntity.h"

#include "Physics/sPhysicsProperties.h"


cEnemyEntity::cEnemyEntity(sPhysicsProperties* entityObj)
{
	m_pEntityObject = entityObj;
}

cEnemyEntity::~cEnemyEntity()
{

}

void cEnemyEntity::Update(double dt)
{
	glm::vec3 vecToGoal = m_pPlayerEntity->position - m_pEntityObject->position;
	glm::vec3 vecToGoalNorm = glm::normalize(vecToGoal);

	//float radDiff = abs(acos(glm::dot(getLookVector(), vecToGoal) / glm::length(m_NewDir) * glm::length(m_dir)));
	float radDiff = acos(glm::dot(getLookVector(), glm::normalize(vecToGoal))); // As long as vecs are unit, we don't need to divide?
	// !!!!!!!! Try something like this with cross product, something that will give negative values: So we can easily determine which way to rotate to face the player
	printf("%.2f\n", radDiff);


	// Depending on behavior do something related to the goal
	switch (this->m_eType)
	{
	case SEEK:
		break;
	}
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

