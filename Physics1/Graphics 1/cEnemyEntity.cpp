#include "cEnemyEntity.h"

#include "Physics/sPhysicsProperties.h"
#include "Physics/cPhysics.h"

#include "cTimer.h"



glm::vec3 GetRandomDirection(void)
{
	glm::vec3 retVec;
	retVec.y = 0.0f;
	retVec.x = (rand() % 100) / 100.0f;
	retVec.z = (rand() % 100) / 100.0f;

	if (rand() % 2 == 0)
		retVec.x *= -1;
	if (rand() % 2 == 0)
		retVec.z *= -1;

	return glm::normalize(retVec);
}


cEnemyEntity::cEnemyEntity(sPhysicsProperties* entityObj, eAIType type)
{
	m_pEntityObject = entityObj;

	m_eType = type;

	m_IntersectionMemory = new int[MEMORY_SIZE] {-1}; // paths it tool where there were >2 choices
	nextMemIDX = 0;


	// Generate random array of ints to make decisions off of
	
	randArraySize = 200;
	nextRandIDX = 0;

	std::mt19937_64 generator;
	std::uniform_int_distribution<int> distribution;
	distribution = std::uniform_int_distribution<int>(1, 200);

	m_RandArray = new int[randArraySize];

	for (int i = 0; i < randArraySize; i++)
	{
		m_RandArray[i] = distribution(generator);
	}

	contenders.reserve(2); // Max novel paths from one triangle


	m_pSoundManager = cSoundManager::GetInstance();

}

cEnemyEntity::~cEnemyEntity()
{

}



// SEEK
// FLEE
// PURSUE
// EVADE
// APPROACH
// WANDER1
// WANDER2
// WANDER3
// FLOCK
void cEnemyEntity::Update(double dt)
{
	const float WATCHDIST = 50.0f;
	const float CHASEDIST = 30.0f;

	static bool hasPlayerEnteredFacility = false;

	if (m_pPlayerEntity->position.z < -110.0f) hasPlayerEnteredFacility = true;

	if (!hasPlayerEnteredFacility) return;



	// Update drone sound for player
	const float DRONERAMPSPEED = 0.05f;

	if (m_pPlayerEntity->position.z < -110.0f)
	{
		if (m_DroneVolume < m_DroneVolTarget)
		{
			m_DroneVolume += DRONERAMPSPEED * static_cast<float>(dt);
			m_DroneVolume = m_DroneVolume > m_DroneVolTarget ? m_DroneVolTarget : m_DroneVolume;
		}
		else if (m_DroneVolume > m_DroneVolTarget)
		{
			m_DroneVolume -= DRONERAMPSPEED * static_cast<float>(dt);
			m_DroneVolume = m_DroneVolume < m_DroneVolTarget ? m_DroneVolTarget : m_DroneVolume;
		}
	}
	else
	{
		m_DroneVolume -= DRONERAMPSPEED * static_cast<float>(dt);
		m_DroneVolume = m_DroneVolume < 0.0f ? 0.0f : m_DroneVolume;
	}
	m_pSoundManager->setDroneVolume(m_DroneVolume);





	static sTimer* initialGracePeriod = cTimer::MakeNewTimer(25.0f, sTimer::ONEOFF);
	static sTimer* deathGracePeriod = cTimer::MakeNewTimer(20.0f, sTimer::ONEOFF); // Keeps enemy in wander mode for 20s after "death"

	static bool binitialGracePeriod = true;
	static bool isGraceDeath = false;

	// Enemy is inactive for the first 30s
	if (!initialGracePeriod->CheckInterval() && binitialGracePeriod)
	{
		deathGracePeriod->CheckInterval();
		return;
	}
	else
		binitialGracePeriod = false;

	if ((isGraceDeath) && (deathGracePeriod->CheckInterval()))
	{
		isGraceDeath = false;
	}



	// Check if moved to different triangle
	cNavMesh::sNavTri* currtri = m_pNavMesh->getClosestTriEnemy(m_pCurrNavTri, m_pEntityObject->position);
	if (m_pCurrNavTri->id != currtri->id)
	{
		m_pPrevNavTri = m_pCurrNavTri;
		m_pCurrNavTri = currtri;
		//m_pCurrNavTri = currtri;

		// Find new goal
		if (m_eType == NAVWANDER)
		{
			m_pTargetNavTri = chooseNewDir();
			AddToMemory(m_pTargetNavTri->id);
		}

	}


	if (m_eType == NAVCHASE)  // Find next closest triangle to player triangle
	{
		cNavMesh::sNavTri* targetTri = m_pNavMesh->findPathToTargetTri(m_pCurrNavTri, m_pPlayerTri);
		if (targetTri == nullptr)
		{
			// Lost player, go back to wandering
			m_eType = NAVWANDER;


			if (m_pCurrNavTri->adjacentTris.size() <= 2)
			{
				for (unsigned int neighborIDX = 0; neighborIDX < m_pCurrNavTri->adjacentTris.size(); neighborIDX++)
				{
					if ((m_pCurrNavTri->adjacentTris[neighborIDX]->id != m_pPrevNavTri->id)  // Is new tri (not backtracking)
						&& (m_pCurrNavTri->adjacentTris[neighborIDX]->adjacentTris.size() > 1)) // Not a dead end
					{
						m_pTargetNavTri = m_pCurrNavTri->adjacentTris[neighborIDX];
						break;
					}
				}
			}
			else // Use "smart" memory to choose new paths
			{
				m_pTargetNavTri = chooseNewDir();
				AddToMemory(m_pTargetNavTri->id);
			}
		}
		else
		{
			m_pTargetNavTri = targetTri;
		}
	}



	// Now where do we want to move?
	// For now, just go to a new triangle, unless following or watching the player




	// Initiate a chase
	cNavMesh::sNavTri* targetTri = nullptr;
	if ((m_eType != NAVCHASE) && (!isGraceDeath))
	{
		targetTri = m_pNavMesh->findPathToTargetTri(m_pCurrNavTri, m_pPlayerTri);

		if (targetTri != nullptr)
		{
			// Close enough to player to start chasing
			m_pTargetNavTri = targetTri;
			m_eType = NAVCHASE;
		}
		else
		{
			m_eType = NAVWANDER;
		}
	}



	//////////////// HEART BEAT VOLUME CONTROL /////////////
	const float HEARTSOUNDINCREASESPD = 0.75f;
	if (m_eType == NAVCHASE)
	{
		float targetVol = glm::clamp(( glm::distance(m_pEntityObject->position, m_pPlayerEntity->position) / 200.0f), 0.0f, 1.0f);
		targetVol = abs(targetVol - 1.0f);

		m_HeartBeatVolume += targetVol * HEARTSOUNDINCREASESPD * static_cast<float>(dt);
		if (m_HeartBeatVolume > 1.0f) m_HeartBeatVolume = 1.0f;
	}
	else
	{
		// Reduce the heartbeat intensity
		m_HeartBeatVolume -= HEARTSOUNDINCREASESPD * static_cast<float>(dt);
		if (m_HeartBeatVolume < 0.0f) m_HeartBeatVolume = 0.0f;
	}
	if (m_HeartBeatVolume < 0.0f) m_HeartBeatVolume = 0.0f;
	m_DroneVolTarget = 0.45f - (0.25f * m_HeartBeatVolume);

	m_pSoundManager->setHeartBeatVol(m_HeartBeatVolume);
	//////////////// ///// //// ////// /////// /////////////


// 	sTimer* heartVol = cTimer::MakeNewTimer(0.1f, sTimer::REPEAT);
// 
// 	if (heartVol->CheckInterval())
// 	{
		//printf("%.3f\n", m_HeartBeatVolume);
	/*}*/


	

	glm::vec3 triOffset = glm::vec3(0.0f, 6.0f, 0.0f);//m_pTargetNavTri->normal * 10.0f;

	glm::vec3 deltaMove(0.0f);
	glm::vec3 vecToGoal(0.0f);
	if ((m_eType == NAVWANDER) || ((m_eType == NAVCHASE) && (m_pCurrNavTri->id != m_pPlayerTri->id)))
	{
		vecToGoal = glm::vec3(m_pTargetNavTri->centreTri.x, m_pTargetNavTri->centreTri.y, m_pTargetNavTri->centreTri.z)
			- glm::vec3(m_pEntityObject->position.x, m_pEntityObject->position.y, m_pEntityObject->position.z) + triOffset;
	}
	else // If on the same triangle, go right for the player
	{
		vecToGoal = m_pPlayerEntity->position - m_pEntityObject->position;
	}




	float spdRed = 1.0f;
	if (m_eType == NAVCHASE) spdRed = 0.3f;

	deltaMove = glm::normalize(vecToGoal) * MOVESPEED * spdRed * static_cast<float>(dt);

	m_pEntityObject->position += deltaMove;



	if ((m_eType == NAVCHASE) && (glm::distance(m_pPlayerEntity->position, m_pEntityObject->position) < 5.0f)) // Teleport player back to start if close enough
	{
		m_pPlayerEntity->position = glm::vec3(0, 10, -20);
		m_pPlayerEntity->oldPosition = m_pPlayerEntity->position;
		m_pPlayerEntity->velocity = glm::vec3(0.0f);
		m_pPlayerEntity->playerInfo->hasTeleported = true;

		// Reset to wander
		m_eType = NAVWANDER;
		m_pTargetNavTri = chooseNewDir();
		AddToMemory(m_pTargetNavTri->id);

		isGraceDeath = true;
		deathGracePeriod->Reset();
	}


// 	if ((glm::length(m_pEntityObject->velocity) != 0) && (glm::length(m_pEntityObject->velocity) > VELOCITYLIMIT))
// 	{
// 		glm::vec3 normVel = glm::normalize(m_pEntityObject->velocity);
// 		m_pEntityObject->velocity = normVel * VELOCITYLIMIT;
// 	}

	return;
	// Everything below are unused behaviors


	if ((m_eType == WANDER1) || (m_eType == WANDER2) || (m_eType == WANDER3))
	{
		if (remainingDist <= 0)
		{
			if (m_eType == WANDER1) // Move far, keep general direction
			{
				remainingDist = 12.0f;
				glm::vec3 adjustRot = glm::vec3(0, static_cast<float>(rand() % 15 + 5), 0);
				if (rand() % 2 == 0) adjustRot.y *= -1;

				currMoveDirGoal = glm::quat(glm::radians(adjustRot)) * currMoveDirGoal;
			}
			else if (m_eType == WANDER2) // Move medium keep general direction
			{
				remainingDist = 8.0f;
				glm::vec3 adjustRot = glm::vec3(0, static_cast<float>(rand() % 50 + 20), 0);
				if (rand() % 2 == 0) adjustRot.y *= -1;

				currMoveDirGoal = glm::quat(glm::radians(adjustRot)) * currMoveDirGoal;
			}
			else if (m_eType == WANDER3) // Move short, Have more varying direction
			{
				remainingDist = 4.0f;
				glm::vec3 adjustRot = glm::vec3(0, static_cast<float>(rand() % 90 + 45), 0);
				if (rand() % 2 == 0) adjustRot.y *= -1;

				currMoveDirGoal = glm::quat(glm::radians(adjustRot)) * currMoveDirGoal;
			}
		}

		// Start by turning towards the goal direction
		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(currMoveDirGoal));
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


		// Move entity in its look direction
		glm::vec3 deltaMove = getLookVector() * static_cast<float>(dt) * MOVESPEED / 4.0f;
		remainingDist -= glm::length(deltaMove);

		m_pEntityObject->position += deltaMove;
	}
	else if (m_eType == FLOCK)
	{
		float distToTarget = glm::distance(m_pEntityObject->position, *flockTarget);
		// Start by checking if the entity is too far away
		if ((distToTarget > flockRadius) && (!isOnCourseForCircle))
		{
			// Change direction to somewhere in the circle
			glm::vec3 newGoalOffset = GetRandomDirection();
			float distOffset = rand() % (int)(flockRadius * 0.8f);
			newGoalOffset = newGoalOffset * distOffset + *flockTarget;
			currGoal = newGoalOffset;

			// Calculate new direction to go towards
			currGoalDir = glm::normalize(currGoal - m_pEntityObject->position);
			isOnCourseForCircle = true; // !!! This will break if we let the player move the flock target around

			// Also want to rotate the entity to face the direction they're going
			//m_pEntityObject->setRotationFromQuat(glm::quat(glm::lookAt(glm::vec3(0.0f), glm::vec3(currGoalDir.x, 0.0f, -currGoalDir.z), glm::vec3(0.0f, 1.0f, 0.0f))));
		}
		else if ((isOnCourseForCircle) && (distToTarget <= flockRadius))
		{
			isOnCourseForCircle = false;
		}
		else if (isOnCourseForCircle)
		{
			currGoalDir = glm::normalize(currGoal - m_pEntityObject->position);
		}

		// Update in the movement direction
		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(currGoalDir));
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


		// Move entity in its look direction
		glm::vec3 deltaMove = getLookVector() * static_cast<float>(dt) * MOVESPEED;
		remainingDist -= glm::length(deltaMove);

		m_pEntityObject->position += deltaMove;
		//m_pEntityObject->position += currGoalDir * static_cast<float>(dt) * MOVESPEED;
	}
	else
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
	}


	return;
}

void cEnemyEntity::updatePlayerTri(void* tri)
{
	m_pPlayerTri = (cNavMesh::sNavTri*)tri;
}

glm::vec3 cEnemyEntity::getPosition(void)
{
	return m_pEntityObject->position;
}

glm::quat cEnemyEntity::getOrientation(void)
{
	return m_pEntityObject->get_qOrientation();
}

void cEnemyEntity::setNavMesh(cNavMesh* theNavMesh)
{
	m_pNavMesh = theNavMesh;
	m_pCurrNavTri = m_pNavMesh->getClosestTri(m_pEntityObject->position); // Initialize triangle they "spawn" on
	m_pPrevNavTri = m_pCurrNavTri;

	// Set target tri to start
	m_pTargetNavTri = m_pCurrNavTri->adjacentTris[0];
	return;
}

void cEnemyEntity::setTargetObject(sPhysicsProperties* goalObj)
{
	this->m_pPlayerEntity = goalObj;
	return;
}

void cEnemyEntity::setFlockTarget(glm::vec3* target)
{
	this->flockTarget = target;
	return;
}


// This should be called for every new wandering triangle we want
cNavMesh::sNavTri* cEnemyEntity::chooseNewDir(void)
{
	//if (m_pCurrNavTri->adjacentTris.size() == 1) return m_pCurrNavTri->adjacentTris[0];


	if (m_pCurrNavTri->adjacentTris.size() == 2)
	{
		for (cNavMesh::sNavTri* currTri : m_pCurrNavTri->adjacentTris)
		{
			if ((currTri->id != m_pPrevNavTri->id)
				&& (currTri->adjacentTris.size() > 1))
			{
				return currTri;

			}
		}
		// If we make it here, no good options (dead end) so we turn back
		return m_pPrevNavTri;
	}

	// At this point, we choose the most novel direction from the currTri we're at
	int noveltyRating = -1;
	cNavMesh::sNavTri* novelestTri = nullptr;

	for (cNavMesh::sNavTri* currTri : m_pCurrNavTri->adjacentTris)
	{
		int tempNov = distInMemory(currTri->id);
		if (tempNov > noveltyRating)
		{
			noveltyRating = tempNov;
			novelestTri = currTri;
			contenders.clear(); // Newest best found, clear contenders
		}
		else if (tempNov == noveltyRating)
		{
			if (contenders.size() == 0)
				contenders.push_back(novelestTri);
			contenders.push_back(currTri); // Equally viable path found
		}
	}

	if (contenders.size() > 0)
	{
		// Assume this has two values
		if (getRandNum() % 2 == 0)
			novelestTri = contenders[0];
		else
			novelestTri = contenders[1];
	}

	return novelestTri;
}

int cEnemyEntity::distInMemory(unsigned int triID)
{
	int distCounter = -1;
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		if (m_IntersectionMemory[(-i) % MEMORY_SIZE] == triID)
		{
			distCounter = i;
			break;
		}
	}

	if (distCounter == -1) distCounter = MEMORY_SIZE + 1; // Higher than any value something in actual memory could give

	return distCounter;
}

void cEnemyEntity::AddToMemory(int newID)
{
	m_IntersectionMemory[nextMemIDX] = newID;
	nextMemIDX = (nextMemIDX + 1) % MEMORY_SIZE;
	return;
}

int cEnemyEntity::getRandNum(void)
{
	int retVal = m_RandArray[nextRandIDX++];
	nextRandIDX = nextRandIDX % randArraySize;
	return retVal;
}

glm::vec3 cEnemyEntity::getLookVector(void)
{
	glm::vec3 lookVec(0, 0, 1);
	lookVec = this->m_pEntityObject->get_qOrientation() * lookVec;
	lookVec.y = 0;


	return glm::normalize(lookVec);
}

