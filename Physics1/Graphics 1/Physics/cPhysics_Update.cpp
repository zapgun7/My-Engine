#include "cPhysics.h"


#include <iostream>

// HACK:
void g_DrawDebugSphere(glm::vec3 position, float scale, glm::vec4 colourRGBA);


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


	if (!m_IsRunning) // If physics paused, update positions/orientations only and return; no collision detection
	{
		for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
		{
			if (pObject->pTheAssociatedMesh)
			{
				pObject->pTheAssociatedMesh->setDrawPosition(pObject->position);
				//			pObject->pTheAssociatedMesh->setDrawOrientation(pObject->orientation);
				pObject->pTheAssociatedMesh->setDrawOrientation(pObject->get_qOrientation());
				pObject->pTheAssociatedMesh->setDrawScale(pObject->scale);
			}
		}



		return;
	}

	// Perform the iteration loop
	for (sPhysicsProperties* pObject : this->m_vec_pPhysicalProps)
	{
		// Infinite mass? 
		if (pObject->inverse_mass >= 0.0f)
		{
			// Explicit forward Euler "integration step"
			//		NewVelocity = LastVel + (Accel * DeltaTime)
			//		NewPosition = LastPos + (Vel * DeltaTime)	

			// Update the "old" position
			pObject->oldPosition = pObject->position;

			// Velocity change is based on the acceleration over this time frame 
			// This part: (Accel * DeltaTime)
			glm::vec3 deltaVelocityThisFrame = pObject->acceleration * (float)deltaTime;

			// Update the velocity based on this delta velocity
			// Then this part: NewVelocity = LastVel + ...
			pObject->velocity += deltaVelocityThisFrame;


			// Position change is based on the velocity over this time frame
			// This part: (Vel * DeltaTime)	
			glm::vec3 deltaPosition = pObject->velocity * (float)deltaTime;

			// ...then this part: NewPosition = LastPos + ...
			// Upatate the position based on this delta position
	//		pCurrentMesh->pPhysProps->position += deltaPosition;
			pObject->position.x += deltaPosition.x;
			pObject->position.y += deltaPosition.y;
			pObject->position.z += deltaPosition.z;

		}//if (pObject->inverse_mass >= 0.0f)

	}//for (sPhsyicsProperties* pObject



	for (sPhysicsProperties* pObjectA : m_vec_pPhysicalProps)
	{
		bool isStillColliding = true;

		while (isStillColliding)
		{
			//std::vector<sPossibleCollision> possibleCollisions;
			sPossibleCollision theCollision;
			bool didCollide = false;

			for (sPhysicsProperties* pObjectB : m_vec_pPhysicalProps)
			{
				if (pObjectA == pObjectB) continue;

				sPossibleCollision newCollision;

				switch (pObjectA->shapeType)
				{
				case sPhysicsProperties::SPHERE:
					switch (pObjectB->shapeType)
					{
					case sPhysicsProperties::SPHERE:
						break;

					case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
						if (this->m_Sphere_TriMeshIndirect_IntersectionTest(pObjectA, pObjectB, newCollision))
						{
							if (newCollision.q < theCollision.q)
								theCollision = newCollision;
							didCollide = true;
						}
						break;
					}


					break;
				}
			}
			if (!didCollide) isStillColliding = false; // Soonest collision doesn't happen this frame
			else // Handle Collision
			{
				switch (pObjectA->shapeType)
				{
				case sPhysicsProperties::SPHERE:
					m_Sphere_Collision(pObjectA, theCollision);
				}
			}
		}
	}


	// See which object is colliding with which object...
// 	for ( sPhysicsProperties* pObjectA : this->m_vec_pPhysicalProps )
// 	{
// 		
// 		for (sPhysicsProperties* pObjectB : this->m_vec_pPhysicalProps )
// 		{
// 			// Are "A" and "B" the same object
// 			if (pObjectA == pObjectB)
// 			{
// 				// Yup, so skip this
// 				continue;
// 			}
// 
// 
// 			// What's the collision? 
// 			switch (pObjectA->shapeType)
// 			{
// 			case sPhysicsProperties::SPHERE:
// 				switch (pObjectB->shapeType)
// 				{
// 				case sPhysicsProperties::SPHERE:
// 					// Sphere - Sphere
// 					if (this->m_Sphere_Sphere_IntersectionTest(pObjectA, pObjectB))
// 					{
// 						//std::cout << "Hazzah!" << std::endl;
// 						// Explode asteroid here
// 					}
// 					break;
// 				case sPhysicsProperties::PLANE:
// 					// Sphere - Plane
// 					break;
// 				case sPhysicsProperties::TRIANGLE:
// 					// Sphere - Triangle
// 					break;
// 				case sPhysicsProperties::AABB:
// 					// Sphere - AABB
// 					break;
// 				case sPhysicsProperties::CAPSULE:
// 					// Sphere - Capsule
// 					break;
// 				case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
// 					// Sphere - Mesh triangle (indirect)
// 					if ( this->m_Sphere_TriMeshIndirect_IntersectionTest( pObjectA, pObjectB))
// 					{
// 						//std::cout << "Hazzah!" << std::endl;
// 						// Explode asteroid here
// 					}
// 					break;
// 				case sPhysicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
// 					// Sphere - Mesh (local vertices)
// 					break;
// 				}//switch (pObjectB->shapeType)
// 				break;
// 
// //			case sPhsyicsProperties::PLANE:
// //				break;
// //			case sPhsyicsProperties::TRIANGLE:
// //				break;
// // ??			case sPhsyicsProperties::AABB:
// // ??			break;
// 
// 			case sPhysicsProperties::CAPSULE:
// 				switch (pObjectB->shapeType)
// 				{
// 				case sPhysicsProperties::SPHERE:
// 					// Capsule - Sphere
// 					break;
// 				case sPhysicsProperties::PLANE:
// 					// Capsule - Plane
// 					break;
// 				case sPhysicsProperties::TRIANGLE:
// 					// Capsule - Triangle
// 					break;
// 				case sPhysicsProperties::AABB:
// 					// Capsule - AABB
// 					break;
// 				case sPhysicsProperties::CAPSULE:
// 					// Capsule - Capsule
// 					break;
// 				case sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT:
// 					// Capsule - Mesh triangle (indirect)
// 					break;
// 				case sPhysicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
// 					// Capsule - Mesh (local vertices)
// 					break;
// 				}//switch (pObjectB->shapeType)
// 				break;
// 
// //			case sPhsyicsProperties::MESH_OF_TRIANGLES_INDIRECT:
// //				switch (pObjectB->shapeType)
// //				{
// //				case sPhsyicsProperties::SPHERE:
// //					break;
// //				case sPhsyicsProperties::PLANE:
// //					break;
// //				case sPhsyicsProperties::TRIANGLE:
// //					break;
// //				case sPhsyicsProperties::AABB:
// //					break;
// //				case sPhsyicsProperties::CAPSULE:
// //					break;
// //				case sPhsyicsProperties::MESH_OF_TRIANGLES_INDIRECT:
// //					break;
// //				case sPhsyicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
// //					break;
// //				}//switch (pObjectB->shapeType)
// //				break;
// //
// //			case sPhsyicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
// //				switch (pObjectB->shapeType)
// //				{
// //				case sPhsyicsProperties::SPHERE:
// //					break;
// //				case sPhsyicsProperties::PLANE:
// //					break;
// //				case sPhsyicsProperties::TRIANGLE:
// //					break;
// //				case sPhsyicsProperties::AABB:
// //					break;
// //				case sPhsyicsProperties::CAPSULE:
// //					break;
// //				case sPhsyicsProperties::MESH_OF_TRIANGLES_INDIRECT:
// //					break;
// //				case sPhsyicsProperties::MESH_OF_TRIANGLES_LOCAL_VERTICES:
// //					break;
// //				}//switch (pObjectB->shapeType)
// //				break;
// // 
// 			//UNKNOWN_OR_UNDEFINED
// 
// 			}//switch (pObjectA->shapeType)
// 
// 		}//for (sPhsyicsProperties* pObjectB
// 
// 	}//for (sPhsyicsProperties* pObjectA...


	// Upadate the draw locations (and orientations) for all assocuated meshes
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
