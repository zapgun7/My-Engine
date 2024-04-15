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

	//currGoalDir = GetRandomDirection();

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


// 	float playerDst = glm::distance(m_pEntityObject->position, m_pPlayerEntity->position);
// 
// 	if (playerDst <= CHASEDIST)
// 	{
// 		// Turn and chase player
// 		glm::vec3 vecToGoal = m_pPlayerEntity->position - m_pEntityObject->position;
// 		vecToGoal.y = 0;
// 		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(vecToGoal));
// 		float radDiff = asin(glm::length(crossResult));
// 
// 		if (crossResult.y > 0) // Turn left
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 		else // Turn right
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 		if (radDiff < 0.3f)
// 		{
// 			m_pEntityObject->position += getLookVector() * MOVESPEED * static_cast<float>(dt);
// 		}
// 	}
// 	else if (playerDst <= WATCHDIST)
// 	{
// 		// Turn and watch player
// 		glm::vec3 vecToGoal = m_pPlayerEntity->position - m_pEntityObject->position;
// 		vecToGoal.y = 0;
// 		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(vecToGoal));
// 		float radDiff = asin(glm::length(crossResult));
// 
// 		if (crossResult.y > 0) // Turn left
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 		else // Turn right
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 	}
// 	else
// 	{
// 		// Continue on with waypoints
// 		glm::vec3 vecToGoal = waypoints[currGoalIDX] - m_pEntityObject->position;
// 		vecToGoal.y = 0;
// 		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(vecToGoal));
// 		float radDiff = asin(glm::length(crossResult));
// 
// 		if (crossResult.y > 0) // Turn left
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 		else // Turn right
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 		if (radDiff < 0.3f)
// 		{
// 			m_pEntityObject->position += getLookVector() * MOVESPEED * static_cast<float>(dt);
// 		}
// 
// 		if (glm::distance(waypoints[currGoalIDX], m_pEntityObject->position) < 5.0f)
// 		{
// 			currGoalIDX = (currGoalIDX + 1) % 4;
// 		}
// 	}

	// Start by getting the current triangle they're on
	cNavMesh::sNavTri* currtri = m_pNavMesh->getClosestTri(m_pCurrNavTri, m_pEntityObject->position - glm::vec3(0.0f, 0.0f, 0.0f));
	if (m_pCurrNavTri->id != currtri->id)
	{
		m_pPrevNavTri = m_pCurrNavTri;
		m_pCurrNavTri = currtri;
		//m_pCurrNavTri = currtri;

		// Find new goal
		if (m_eType == NAVWANDER)
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
		else  // Find next closest triangle to player triangle
		{
			cNavMesh::sNavTri* targetTri = m_pNavMesh->findPathToTargetTri(m_pCurrNavTri, m_pPlayerTri);
			if (targetTri == nullptr)
			{
				// Lost player, go back to wandering
				m_eType = NAVWANDER;
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
		}
		
	}



	// Now where do we want to move?
	// For now, just go to a new triangle, unless following or watching the player

	// Get player distance
	float plyrDist = glm::distance(m_pEntityObject->position, m_pPlayerEntity->position);
	

	cNavMesh::sNavTri* targetTri = nullptr;
	if (m_eType != NAVCHASE)
		targetTri = m_pNavMesh->findPathToTargetTri(m_pCurrNavTri, m_pPlayerTri);

// 	if (plyrDist <= CHASEDIST)
// 	{
// 		m_eType = NAVCHASE;
// 	}
// 	else if (plyrDist <= WATCHDIST)
// 	{
// 		m_eType = NAVWATCH;
// 	}
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


	// For now, testing wander these will stay these values
	m_eType = NAVWANDER;

	

// 	if (m_eType == NAVWATCH) // Turn to look at player
// 	{
// 		glm::vec3 vecToGoal = m_pPlayerEntity->position - m_pEntityObject->position;
// 		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(vecToGoal));
// 		if (crossResult.y > 0) // Turn left
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 		else // Turn right
// 		{
// 			glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(dt), 0)));
// 			m_pEntityObject->setRotationFromQuat(m_pEntityObject->get_qOrientation() * (rotAdjust));
// 		}
// 	}
// 	else // Navigate to the next triangle
	{
		glm::vec3 deltaMove(0.0f);
		glm::vec3 vecToGoal = glm::vec3(m_pTargetNavTri->centreTri.x, 0.0f, m_pTargetNavTri->centreTri.z) 
							 - glm::vec3(m_pEntityObject->position.x, 0.0f, m_pEntityObject->position.z);
		//vecToGoal.y = 0;
		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(glm::vec3(vecToGoal.x, 0.0f, vecToGoal.z)));
		float radDiff = asin(glm::length(crossResult));

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
		if (radDiff < 0.2f)
		{
			//m_pEntityObject->velocity += getLookVector() * MOVESPEED * static_cast<float>(dt); // CAREFUL, CAN WALK OFF TRI AREA
			//deltaMove += getLookVector() * MOVESPEED * static_cast<float>(dt);
			deltaMove = glm::normalize(vecToGoal) * MOVESPEED * static_cast<float>(dt);
			//velToAdd += getLookVector() * MOVESPEED * static_cast<float>(dt);
		}

		// Project velocity to plane defined by triangle it's currently on
 		glm::vec3 projSubVec = glm::dot(deltaMove, m_pCurrNavTri->normal) * m_pCurrNavTri->normal;
 		deltaMove -= projSubVec;
		m_pEntityObject->position += deltaMove;


		// Update position above the triangle
// 		glm::vec3 currSpotOnTri(0.0f);
// 		cPhysics::m_IntersectSegmentPlane(m_pEntityObject->position, 
// 										  m_pEntityObject->position - glm::vec3(0.0f, 20.0f, 0.0f),
// 										  m_pCurrNavTri->normal,
// 										  m_pCurrNavTri->pd,
// 										  currSpotOnTri);
// 		m_pEntityObject->position = currSpotOnTri + glm::vec3(0.0f, 10.0f, 0.0f);
	}


// 	if ((glm::length(m_pEntityObject->velocity) != 0) && (glm::length(m_pEntityObject->velocity) > VELOCITYLIMIT))
// 	{
// 		glm::vec3 normVel = glm::normalize(m_pEntityObject->velocity);
// 		m_pEntityObject->velocity = normVel * VELOCITYLIMIT;
// 	}

	return;
	if (plyrDist <= WATCHDIST) 
	{
		//Data for both conditions below
		glm::vec3 vecToGoal = /*m_pPlayerEntity->position*/m_pTargetNavTri->centreTri - m_pEntityObject->position;
		glm::vec3 crossResult = glm::cross(getLookVector(), glm::normalize(vecToGoal));
		float radDiff = asin(glm::length(crossResult));

		if (plyrDist <= CHASEDIST) // Chase player
		{
			m_eType = NAVCHASE;
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
				m_pEntityObject->velocity += getLookVector() * MOVESPEED * static_cast<float>(dt); // CAREFUL, CAN WALK OFF TRI AREA
				//velToAdd += getLookVector() * MOVESPEED * static_cast<float>(dt);
			}

			// Project velocity to plane defined by triangle it's currently on
			glm::vec3 projSubVec = glm::dot(m_pEntityObject->velocity, m_pCurrNavTri->normal) * m_pCurrNavTri->normal;
			m_pEntityObject->velocity -= projSubVec;

		}
		else // Watch player
		{
			m_eType = NAVWATCH;
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
		}
	}
	else // Wander to new tris
	{

	}



	// Update position based on the current triangle


	return;
	// Everything below are just unused behaviors


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

glm::vec3 cEnemyEntity::getLookVector(void)
{
	glm::vec3 lookVec(0, 0, 1);
	lookVec = this->m_pEntityObject->get_qOrientation() * lookVec;
	lookVec.y = 0;


	return glm::normalize(lookVec);
}

