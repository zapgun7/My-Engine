#include "cPhysics.h"


#include <iostream>

// HACK:
void g_DrawDebugSphere(glm::vec3 position, float scale, glm::vec4 colourRGBA);

void cPhysics::UpdatePlayerObj(double deltaTime)
{
// 	sPhysicsProperties tempPlyr = *m_pThePlayerObj;
// 	m_pThePlayerObj->velocity = tempPlyr.velocity; m_pThePlayerObj->position = tempPlyr.position; m_pThePlayerObj->oldPosition = tempPlyr.oldPosition;
	///////// GROUNDED DETECTION //////////
	if (!m_pThePlayerObj->playerInfo->jumpNormThisFrame)
	{
		m_pThePlayerObj->playerInfo->framesAirborne++;
	}
	else
	{
		m_pThePlayerObj->playerInfo->framesAirborne = 0;
	}
	// If more than 3 frames of not touching valid ground, considered airborne
	if (m_pThePlayerObj->playerInfo->framesAirborne > 3) m_pThePlayerObj->playerInfo->isGrounded = false;
	else m_pThePlayerObj->playerInfo->isGrounded = true;
	//////////// END OF GROUNDED DETECTION /////////////


	if /*(*/(!m_pThePlayerObj->playerInfo->isInputting)// && (!m_pThePlayerObj->playerInfo->isSprinting))// If not pressing WASD
	{
		if (m_pThePlayerObj->playerInfo->isGrounded) // Touching "valid" ground
		{
			//glm::vec3 velReduction = (glm::vec3(m_pThePlayerObj->velocity.x, 0, m_pThePlayerObj->velocity.z)) * m_pThePlayerObj->playerInfo->friction;
			//velReduction = (velReduction - glm::vec3(m_pThePlayerObj->velocity.x, 0, m_pThePlayerObj->velocity.z)) * static_cast<float>(deltaTime) * 10.0f;
			glm::vec3 velReduction = m_pThePlayerObj->velocity * m_pThePlayerObj->playerInfo->friction;
			velReduction = (velReduction - m_pThePlayerObj->velocity) * static_cast<float>(deltaTime) * 10.0f;
			m_pThePlayerObj->velocity += velReduction;
		}
		else // Midair
		{
			glm::vec3 velReduction = (glm::vec3(m_pThePlayerObj->velocity.x, 0, m_pThePlayerObj->velocity.z)) * m_pThePlayerObj->playerInfo->airDrag;
			velReduction = (velReduction - glm::vec3(m_pThePlayerObj->velocity.x, 0, m_pThePlayerObj->velocity.z)) * static_cast<float>(deltaTime) * 10.0f;
			m_pThePlayerObj->velocity += velReduction;
		}
	}
	else if (false) // Still inputting and/or sprinting
	{
		// Reduce speed if touching the ground
		if (m_pThePlayerObj->playerInfo->isGrounded)
		{
			//float currHSpd = glm::length(glm::vec3(m_pThePlayerObj->velocity.x, 0, m_pThePlayerObj->velocity.z));
			float currHSpd = glm::length(m_pThePlayerObj->velocity);
			float addSprntSpd = 0.0f;
			if (m_pThePlayerObj->playerInfo->isSprinting) addSprntSpd = m_pThePlayerObj->playerInfo->sprintSpeedIncrease;

			if (currHSpd > m_pThePlayerObj->playerInfo->maxGroundedSpeed + addSprntSpd)
			{
// 				glm::vec3 velReduction = (glm::vec3(m_pThePlayerObj->velocity.x, 0, m_pThePlayerObj->velocity.z)) * m_pThePlayerObj->playerInfo->friction;
// 				velReduction = (velReduction - glm::vec3(m_pThePlayerObj->velocity.x, 0, m_pThePlayerObj->velocity.z)) * static_cast<float>(deltaTime) * 10.0f;
				glm::vec3 velReduction = m_pThePlayerObj->velocity * m_pThePlayerObj->playerInfo->friction;
				velReduction = m_pThePlayerObj->velocity * static_cast<float>(deltaTime) * 10.0f;
				m_pThePlayerObj->velocity += velReduction;
			}

		}
	}

	glm::vec3 deltaVelocityThisFrame;
	if (m_pThePlayerObj->playerInfo->isGrounded)
		deltaVelocityThisFrame = (m_pThePlayerObj->acceleration - m_pThePlayerObj->playerInfo->groundNorm * 1.0f) * static_cast<float>(deltaTime);
	else
		deltaVelocityThisFrame = (m_pThePlayerObj->acceleration + m_WorldGravity) * static_cast<float>(deltaTime);

	m_pThePlayerObj->oldPosition = m_pThePlayerObj->position;


	// Update the velocity based on this delta velocity
	// Then this part: NewVelocity = LastVel + ...
	m_pThePlayerObj->velocity += deltaVelocityThisFrame;


	// Position change is based on the velocity over this time frame
	// This part: (Vel * DeltaTime)	
	glm::vec3 deltaPosition = m_pThePlayerObj->velocity * static_cast<float>(deltaTime);

	// ...then this part: NewPosition = LastPos + ...
	// Upatate the position based on this delta position
//		pCurrentMesh->pPhysProps->position += deltaPosition;
	m_pThePlayerObj->position.x += deltaPosition.x;
	m_pThePlayerObj->position.y += deltaPosition.y;
	m_pThePlayerObj->position.z += deltaPosition.z;

	return;
} // UpdatePlayerObj()

void cPhysics::Update(double deltaTime)
{
//	for ( unsigned int index = 0; index !=  this->m_vec_pPhysicalProps.size(); index++ )
//	{
//		sPhsyicsProperties* pObject = this->m_vec_pPhysicalProps[index];
//		
//		// ...and so on...
//
//	}

	//deltaTime *= 10.0f;


	// Start by updating verlet objects!

	const double UPDATEINTERVAL = 0.005;
	static double timeSinceLastUpdate = 0.0;

	timeSinceLastUpdate += deltaTime;


	if (timeSinceLastUpdate >= UPDATEINTERVAL)
	{
		for (cSoftBodyVerlet* currSoftBod : m_VerletObjs)
		{
			currSoftBod->VerletUpdate(UPDATEINTERVAL);
			currSoftBod->SatisfyConstraints(UPDATEINTERVAL);
			currSoftBod->ApplyCollision(UPDATEINTERVAL);
			currSoftBod->UpdateVertexPositions();
			currSoftBod->UpdateNormals();

			m_pMeshManager->UpdateVAOBuffers(currSoftBod->m_ModelVertexInfo.meshName, currSoftBod->m_ModelVertexInfo, (GLuint)3); // Lazy, but this is the only shader program we're using now

		}

		timeSinceLastUpdate -= UPDATEINTERVAL;
	}


	








	if (!m_IsRunning) // If physics paused, update positions/orientations only and return; no collision detection
	{
		for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
		{
			if (pObject->pTheAssociatedMesh)
			{
				pObject->pTheAssociatedMesh->setDrawPosition(pObject->position);
				//			pObject->pTheAssociatedMesh->setDrawOrientation(pObject->orientation);
				pObject->pTheAssociatedMesh->setDrawOrientation(pObject->get_qOrientation());
				//pObject->pTheAssociatedMesh->setDrawScale(pObject->scale);
			}
		}



		return;
	}
	
	
	// Update the player first
	UpdatePlayerObj(deltaTime);

	// Perform the iteration loop
	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
	{
		// Infinite mass? 
		if ((pObject->inverse_mass >= 0.0f) && (!pObject->isPlayer))
		{

			// Velocity change is based on the acceleration over this time frame 
			// This part: (Accel * DeltaTime)
			glm::vec3 deltaVelocityThisFrame;
				
			
			///////////////////// END OF PLAYER CHARACTER UPDATES /////////////////

// 			if ((pObject->isPlayer) && (pObject->playerInfo->jumpNormThisFrame)) // If touching the ground
// 				deltaVelocityThisFrame = (pObject->acceleration) * static_cast<float>(deltaTime);
// 			else
				deltaVelocityThisFrame = (pObject->acceleration + m_WorldGravity) * static_cast<float>(deltaTime);

			// Explicit forward Euler "integration step"
			//		NewVelocity = LastVel + (Accel * DeltaTime)
			//		NewPosition = LastPos + (Vel * DeltaTime)	

			// Update the "old" position
			pObject->oldPosition = pObject->position;


			// Update the velocity based on this delta velocity
			// Then this part: NewVelocity = LastVel + ...
			pObject->velocity += deltaVelocityThisFrame;


			// Position change is based on the velocity over this time frame
			// This part: (Vel * DeltaTime)	
			glm::vec3 deltaPosition = pObject->velocity * static_cast<float>(deltaTime);

			// ...then this part: NewPosition = LastPos + ...
			// Upatate the position based on this delta position
	//		pCurrentMesh->pPhysProps->position += deltaPosition;
			pObject->position.x += deltaPosition.x;
			pObject->position.y += deltaPosition.y;
			pObject->position.z += deltaPosition.z;

		}//if (pObject->inverse_mass >= 0.0f)

	}//for (sPhsyicsProperties* pObject


// 	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
// 	{
// 		if (pObject->shapeType == sPhysicsProperties::CAPSULE)
// 			pObject->playerInfo->jumpNormThisFrame = 0;
// 	}
	m_pThePlayerObj->playerInfo->jumpNormThisFrame = 0;
	m_pThePlayerObj->playerInfo->groundNorm = glm::vec3(0.0f);
	int eh = 0;
	if (!m_pThePlayerObj->playerInfo->isGrounded)
		eh = 1;
	for (sPhysicsProperties* pObjectA : m_vec_pPhysicalProps)
	{
		bool isStillColliding = true;
		while (isStillColliding) // TODO add an escape condition when object gets stuck in geometry; have loop counter, when reaching a certain number, "teleport" to surface of colliding object surface
		{						 // Will have to add additional info to collision event, like pd so we can get a spot to put the object right off the surface
			//std::vector<sPossibleCollision> possibleCollisions;
			sPossibleCollision theCollision;
			bool didCollide = false;

			for (sPhysicsProperties* pObjectB : m_vec_pPhysicalProps)
			{
				if (pObjectA == pObjectB) continue;

				m_pTheSoonestCollision->q = FLT_MAX; // Reset this
				sPossibleCollision newCollision;

				switch (pObjectA->shapeType)
				{
				case sPhysicsProperties::SPHERE:
					switch (pObjectB->shapeType)
					{
					case sPhysicsProperties::SPHERE:
						break;

					case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
						//if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB, newCollision))
						if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB))
						{
							if (m_pTheSoonestCollision->q < theCollision.q)
							{
								theCollision = *m_pTheSoonestCollision;
								didCollide = true;
							}
						}
						break;
					}


					break;
				case sPhysicsProperties::CAPSULE:
					switch (pObjectB->shapeType)
					{
					case sPhysicsProperties::SPHERE:
						break;

					case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
						if (this->m_Capsule_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB, newCollision))
						{
// 							if (newCollision.q < theCollision.q)
// 								theCollision = newCollision;
// 							didCollide = true;
							if (m_pTheSoonestCollision->q < theCollision.q)
							{
								theCollision = *m_pTheSoonestCollision;
								didCollide = true;
							}
						}
						break;
					}
				}
			}
			if (!didCollide) isStillColliding = false; // Soonest collision doesn't happen this frame
			else // Handle Collision
			{
				switch (pObjectA->shapeType)
				{
				case sPhysicsProperties::SPHERE:
					m_Sphere_Collision(pObjectA, theCollision);
					break;
				case sPhysicsProperties::CAPSULE:
					
					m_Capsule_Collision(pObjectA, theCollision, deltaTime);

					// Set if the player can jump
					if (theCollision.hitNorm.y > 0)
					{
						if (acos(glm::dot(glm::normalize(theCollision.hitNorm), glm::vec3(0, 1, 0))) < 45)
						{
							pObjectA->playerInfo->jumpNormThisFrame = 1;
							pObjectA->playerInfo->groundNorm += theCollision.hitNorm; 
						}
					}



					break;
				}
			}
		}
	}
	if (m_pThePlayerObj->playerInfo->jumpNormThisFrame) // This normalizes the norms of all triangles the player is standing on
		m_pThePlayerObj->playerInfo->groundNorm = glm::normalize(m_pThePlayerObj->playerInfo->groundNorm); 

	// Update the draw locations (and orientations) for all associated meshes
	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
	{
		if (pObject->pTheAssociatedMesh)
		{
			pObject->pTheAssociatedMesh->setDrawPosition(pObject->position);
//			pObject->pTheAssociatedMesh->setDrawOrientation(pObject->orientation);
			pObject->pTheAssociatedMesh->setDrawOrientation(pObject->get_qOrientation());
		}
	}//for (sPhsyicsProperties* pObjectA


	// HACK:
	if ( ! this->m_vecCollisionsThisFrame.empty())
	{
// 		std::cout << "BREAK ME!" << std::endl;
// 		
// 		std::cout << this->m_vecCollisionsThisFrame.size() << std::endl;
// 		for (sCollisionEvent col:  this->m_vecCollisionsThisFrame)
// 		{
// 			std::cout
// 				<< col.pObjectA->getShapeTypeAsString()
// 				<< " hit "
// 				<< col.pObjectB->getShapeTypeAsString()
// 				<< std::endl;
// 		}
// 		std::cout << "******************************************" << std::endl;
// 		// HACK:
		this->m_vecCollisionsThisFrame.clear();
	}

	return;
}
