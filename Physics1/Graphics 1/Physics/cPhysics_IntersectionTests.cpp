#include "cPhysics.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 

#include <iostream>

bool cPhysics::m_Sphere_Sphere_IntersectionTest(sPhysicsProperties* pSphereA, sPhysicsProperties* pSphereB)
{
	sPhysicsProperties::sSphere* sphereA = (sPhysicsProperties::sSphere*)(pSphereA->pShape);
	sPhysicsProperties::sSphere* sphereB = (sPhysicsProperties::sSphere*)(pSphereB->pShape);

	if (m_CheckExistingCollision(pSphereA, pSphereB))
	{
		std::cout << "Already Collided!" << std::endl;
		return true;
	}

	if (glm::distance(pSphereA->position, pSphereB->position) < (sphereA->radius + sphereB->radius)) // TODO, compare if prevpos -> currpos capsule colldies with each other
	{
		std::cout << "Spheres touching!" << std::endl; 


		glm::vec3 sphereDirectionA = glm::normalize(pSphereA->velocity);
		glm::vec3 sphereDirectionB = glm::normalize(pSphereB->velocity);

		glm::vec3 sphNormA = glm::normalize(pSphereB->position - pSphereA->position); // Normal on sphereA
		float sphereSpeedA = glm::length(pSphereA->velocity);
		glm::vec3 sphNormB = glm::normalize(pSphereA->position - pSphereB->position); // Normal on sphereB
		float sphereSpeedB = glm::length(pSphereB->velocity);

		// Reflection vector calc
		glm::vec3 reflectionVecA = glm::reflect(sphereDirectionA, sphNormB);
		glm::vec3 reflectionVecB = glm::reflect(sphereDirectionB, sphNormA);

		//Compare Dir and Reflection to see how much velocity is passed and kept
		glm::vec3 sphAFinalVel = glm::vec3(0);
		glm::vec3 sphBFinalVel = glm::vec3(0);

		// Degrees between spheres dir and reflection dir
		float sphADeg = glm::degrees(abs(acos(glm::dot(reflectionVecA, sphereDirectionA) / glm::length(reflectionVecA) * glm::length(sphereDirectionA)))); // Me understand degrees, prob faster without conversions
		float sphBDeg = glm::degrees(abs(acos(glm::dot(reflectionVecB, sphereDirectionB) / glm::length(reflectionVecB) * glm::length(sphereDirectionB))));

		// 180 = total momentum passing, 0 = No momentum passing
		// Sphere A momentum distribution
		sphAFinalVel += pSphereA->velocity * (1 - (sphADeg / 180));
		sphBFinalVel += sphereSpeedA * sphNormA * (sphADeg / 180);

		// Sphere B momentum distribution
		sphBFinalVel += pSphereB->velocity * (1 - (sphBDeg / 180));
		sphAFinalVel += sphereSpeedB * sphNormB * (sphBDeg / 180);


		// Set positions to barely not touching
		float distToScooch = (sphereA->radius + sphereB->radius) - glm::distance(pSphereA->position, pSphereB->position);
		// Move both spheres half distToScooch by their reflect vect
		pSphereA->position += reflectionVecA * (distToScooch / 2);
		pSphereB->position += reflectionVecB * (distToScooch / 2);


		// How to transfer momentum..
		//glm::vec3 newVelocityA = reflectionVecA * sphereSpeedA;
		//glm::vec3 newVelocityB = reflectionVecB * sphereSpeedB;


		// Must modify these velocities
		//pSphereA->velocity = newVelocityA;
		//pSphereB->velocity = newVelocityB;
		pSphereA->velocity = sphAFinalVel;
		pSphereB->velocity = sphBFinalVel;


		// TODO might want to add collision in the opposite direction (b->a)
		sCollisionEvent theCollision;

		theCollision.pObjectA = pSphereA; // For now just log which objects have been dealt with
		theCollision.pObjectB = pSphereB;


		// 
		//theCollision.contactPoint = closestContactPoint;
		//theCollision.reflectionNormal = reflectionVec;
		//		theCollision.velocityAtCollision = reflectionVec;

				// TODO: We'll have a problem later: what deletes this?
		//sPhsyicsProperties* pTriangleWeHit = new sPhsyicsProperties();

// 		pTriangleWeHit->setShape(new sPhsyicsProperties::sTriangle(closestTriangleVertices[0],
// 			closestTriangleVertices[1],
// 			closestTriangleVertices[2]));

		

		this->m_vecCollisionsThisFrame.push_back(theCollision);



		std::cout << "Collided two spheres!" << std::endl;
		return true;
	}
	
	// TODO: Insert amazing code here 
	return false;	
}

bool cPhysics::m_Sphere_Plane_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pPlane)
{
	// TODO: Insert amazing code here 
	return false;
}

bool cPhysics::m_Sphere_Triangle_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriangle)
{
	// TODO: Insert amazing code here 
	return false;
}

bool cPhysics::m_Sphere_AABB_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pAABB)
{
	// TODO: Insert amazing code here 
	return false;
}

bool cPhysics::m_Sphere_Capsule_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pCapsule)
{
	// TODO: Insert amazing code here 
	return false;
}

bool cPhysics::m_Sphere_TriMeshIndirect_IntersectionTest(sPhysicsProperties* pSphere_General, sPhysicsProperties* pTriMesh_General)
{

	// Do we have a mesh manager? 
	if ( ! this->m_pMeshManager )
	{
		return false;
	}

	// Does the physics object have a mesh object associated? 
	if (!pTriMesh_General->pTheAssociatedMesh)
	{
		return false;
	}

	// Get the info about this shape, specifically
	sPhysicsProperties::sMeshOfTriangles_Indirect* pTriangleMesh = 
						(sPhysicsProperties::sMeshOfTriangles_Indirect*)(pTriMesh_General->pShape);

	sPhysicsProperties::sSphere* pSphere = (sPhysicsProperties::sSphere*)(pSphere_General->pShape);


	cAABB* TriMeshAABB = findAABBByModelName(pTriangleMesh->meshName);

	if (TriMeshAABB == nullptr)
	{
		std::cout << "Couldn't find AABB" << std::endl;
		return false;
	}

	// TODO create another sPhysicsProperties for the sphere and reverse the TriMesh's transform on it before passing 

	sPhysicsProperties reverseTransformedSphere = *pSphere_General;

	glm::mat4 matRevModelT = glm::mat4(1.0f);
	glm::mat4 matRevModelR = glm::mat4(1.0f);
	glm::mat4 matRevTranslate = glm::translate(glm::mat4(1.0f),
											glm::vec3(-pTriMesh_General->position.x,
													  -pTriMesh_General->position.y,
													  -pTriMesh_General->position.z));
	glm::mat4 matModelRT = glm::mat4(1.0f);
	glm::mat4 matModelR = glm::mat4(1.0f);
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
											glm::vec3(pTriMesh_General->position.x,
													  pTriMesh_General->position.y,
													  pTriMesh_General->position.z));


	glm::mat4 matRevRotation = glm::mat4(glm::inverse(pTriMesh_General->get_qOrientation()));
	glm::mat4 matRotation = glm::mat4(pTriMesh_General->get_qOrientation());


	// For transforming relative to the base-triangles
	matRevModelT *= matRevTranslate;
	//matRevModelT *= matRevRotation;
	matRevModelR *= matRevRotation;

	// For converting back
	matModelRT *= matTranslate;
	matModelRT *= matRotation;
	matModelR *= matRotation;

	//reverseTransformedSphere.oldPosition = (matRevModelRT * glm::vec4(reverseTransformedSphere.oldPosition, 1.0f));


	// Transform current to fit default triangle mesh
	reverseTransformedSphere.position = (matRevModelT * glm::vec4(reverseTransformedSphere.position, 1.0f));
	reverseTransformedSphere.position = (matRevModelR * glm::vec4(reverseTransformedSphere.position, 1.0f));

	// Do it to old position too for continuous collision detection
	reverseTransformedSphere.oldPosition = (matRevModelT * glm::vec4(reverseTransformedSphere.oldPosition, 1.0f));
	reverseTransformedSphere.oldPosition = (matRevModelR * glm::vec4(reverseTransformedSphere.oldPosition, 1.0f));

	// Rotate velocity to match default tri mesh
	reverseTransformedSphere.velocity = (matRevModelR * glm::vec4(reverseTransformedSphere.velocity, 1.0f));
	


	// Recursive AABB function to return near triangles
	std::vector<sTriangle_A> trisToCheck = TriMeshAABB->sphereCollision(&reverseTransformedSphere); // TODO make this capsule detection for the sphere sweep


// The new and improved code

	float closestDistanceSoFar = FLT_MAX;
	glm::vec3 closestTriangleVertices[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };
	glm::vec3 closestContactPoint = glm::vec3(0.0f);
	unsigned int indexOfClosestTriangle = INT_MAX;


	// We now have the mesh object location and the detailed mesh information 
					// Which triangle is closest to this sphere (right now)
// 	for(std::vector<sTriangle_A>::iterator itTri = trisToCheck.begin(); 
// 		itTri != trisToCheck.end(); 
// 		itTri++)
// 	{
// 		// And make sure you multiply the normal by the inverse transpose
// 		// OR recalculate it right here! 
// 
// 		// ******************************************************
// 
// 		//glm::vec3 thisTriangleClosestPoint = this->m_ClosestPtPointTriangle(pSphere_General->position,
// 		//	itTri->vertices[0], itTri->vertices[1], itTri->vertices[2]);
// 		glm::vec3 thisTriangleClosestPoint = this->m_ClosestPtPointTriangle(reverseTransformedSphere.position,
// 				itTri->vertices[0], itTri->vertices[1], itTri->vertices[2]);
// 
// 		// Is this the closest so far
// 		//float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, pSphere_General->position);
// 		float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, reverseTransformedSphere.position);
// 
// 		if (distanceToThisTriangle < closestDistanceSoFar) // TODO Keep track of all triangles that are in/touching the sphere, figure out which one it hit first
// 		{
// 			// this one is closer
// 			closestDistanceSoFar = distanceToThisTriangle;
// 			// Make note of the triangle index
// 			//indexOfClosestTriangle = index;
// 			// 
// 			closestTriangleVertices[0] = itTri->vertices[0];
// 			closestTriangleVertices[1] = itTri->vertices[1];
// 			closestTriangleVertices[2] = itTri->vertices[2];
// 
// 			closestContactPoint = thisTriangleClosestPoint;
// 		}
// 
// 
// 	} //for ( unsigned int index...

	float earliestTime = FLT_MAX; // Time (0-1) over current update the sphere collides
	glm::vec3 hitNorm; // Normal used to calculate reflection

	// The newest code for dealing with continuous collision detection
	for (std::vector<sTriangle_A>::iterator itTri = trisToCheck.begin();
		itTri != trisToCheck.end();
		itTri++)
	{
		// And make sure you multiply the normal by the inverse transpose
		// OR recalculate it right here! 

		// ******************************************************

		//glm::vec3 thisTriangleClosestPoint = this->m_ClosestPtPointTriangle(pSphere_General->position,
		//	itTri->vertices[0], itTri->vertices[1], itTri->vertices[2]);
		//glm::vec3 thisTriangleClosestPoint = this->m_ClosestPtPointTriangle(reverseTransformedSphere.position,
		//	itTri->vertices[0], itTri->vertices[1], itTri->vertices[2]);

		// Is this the closest so far
		//float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, pSphere_General->position);
		//float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, reverseTransformedSphere.position);

		float t; // Time (0-1) over current update the sphere collides
		glm::vec3 hn; // Normal used to calculate reflection

		if (!m_TestMovingSphereTriangle(&reverseTransformedSphere, &*itTri, t, hn))
		{
			// No collision, go to next triangle
			continue;
		}


		// Collision confirmed within update range

		if (t < earliestTime) // If this collision happens sooner than the one stored
		{
			earliestTime = t;
			hitNorm = hn;
		}


// 		if (distanceToThisTriangle < closestDistanceSoFar) // TODO Keep track of all triangles that are in/touching the sphere, figure out which one it hit first
// 		{
// 			// this one is closer
// 			closestDistanceSoFar = distanceToThisTriangle;
// 			// Make note of the triangle index
// 			//indexOfClosestTriangle = index;
// 			// 
// 			closestTriangleVertices[0] = itTri->vertices[0];
// 			closestTriangleVertices[1] = itTri->vertices[1];
// 			closestTriangleVertices[2] = itTri->vertices[2];
// 
// 			closestContactPoint = thisTriangleClosestPoint;
// 		}


	} //for ( unsigned int index...

	//    ____  _     _                  _     _ _     _        _                   _     ___ 
	//   |  _ \(_) __| | __      _____  | |__ (_) |_  | |_ _ __(_) __ _ _ __   __ _| | __|__ \
	//   | | | | |/ _` | \ \ /\ / / _ \ | '_ \| | __| | __| '__| |/ _` | '_ \ / _` | |/ _ \/ /
	//   | |_| | | (_| |  \ V  V /  __/ | | | | | |_  | |_| |  | | (_| | | | | (_| | |  __/_| 
	//   |____/|_|\__,_|   \_/\_/ \___| |_| |_|_|\__|  \__|_|  |_|\__,_|_| |_|\__, |_|\___(_) 
	//                                                                        |___/           
 
	// Old code

// 	if (closestDistanceSoFar < pSphere->radius)
// 	{
// 		// Hit it!
// 		// Take the normal of that triangle and bounce the sphere along it
// 
// 		// How do we calculate the new direction after the "bounce"? 
// 		// 
// 		// Answer: it's based on the REFLECTION vector around the normal.
// 		// The sphere is travelling along its VELOCITY vector (at this moment)
// 		//	and will "bounce off" along an angle reflected by the normal
// 
// 		// The object HAS PENETRATED the triangle
// 		// Instead of using the CURRENT location, 
// 		//	calculate everything based on the LAST location of the object. 
// 		// i.e. JUST BEFORE the object collided.
// 		
// // Add whatever information we might need later to do something for the response.
// 
// 
// 		// Calculate the current "direction" vector 
// 		// We're using the velocity
// 		//glm::vec3 sphereDirection = pSphere_General->velocity;
// 		glm::vec3 sphereDirection = reverseTransformedSphere.velocity;
// 		// Normalize... 
// 		sphereDirection = glm::normalize(sphereDirection);
// 
// 		// Calcualte the current normal from the TRANSFORMED vertices
// 		glm::vec3 edgeA = closestTriangleVertices[1] - closestTriangleVertices[0];
// 		glm::vec3 edgeB = closestTriangleVertices[2] - closestTriangleVertices[0];
// 
// 		glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));
// 
// 		// Calculate the reflection vector from the normal	
// 		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
// 		// 1st parameter is the "incident" vector
// 		// 2nd parameter is the "normal" vector
// 		glm::vec3 reflectionVec = glm::reflect(sphereDirection, triNormal);
// 
// 		// Update the  velocity based on this reflection vector
// 		//float sphereSpeed = glm::length(pSphere_General->velocity);
// 		float sphereSpeed = glm::length(reverseTransformedSphere.velocity);
// 		glm::vec3 newVelocity = reflectionVec * sphereSpeed;
// 
// 		// RESTITUTION CALCULATION
// 		//newVelocity.y *= 0.5f;
// 
// 		// Degree between sphere dir and tri norm: 180 is absolute restitution application, 90 is none (parallel to it)
// 		float degToNorm = glm::degrees(abs(acos(glm::dot(sphereDirection, triNormal) / glm::length(sphereDirection) * glm::length(triNormal)))); 
// 
// 		//degToNorm -= 90; // Now we're working with 90: total restitution app.   0: no app
// 		float restAppDegree = degToNorm - 90; // Multiplier (1.0 - 0.0) to influence effects of the restitution application
// 		restAppDegree /= 90;
// 
// 		glm::vec3 restitutionVelLoss = -triNormal * newVelocity; // Calculate vector we want to reduce velocity on (negative normal of surface it's bouncing on)
// 		newVelocity += (restitutionVelLoss * (1.0f - pSphere_General->restitution)) * restAppDegree; // Subtract said vector from newVelocity, scaled with its restitution (0 restitution = no bounce, 1 = full bounce)
// 
// 
// 		// TODO will have to rotate the velocity vector back
// 		newVelocity = (matModelR * glm::vec4(newVelocity, 1.0f));
// 		pSphere_General->velocity = newVelocity;
// 
// 
// 		// Re-position the sphere where it would be if it perfectly bounced off the triangle
// 		float distToCorrect = (pSphere->radius - closestDistanceSoFar) * 2; // Get length we need to move the sphere by
// 		//glm::vec3 moveDir = glm::normalize((pSphere_General->position - closestContactPoint));
// 		//pSphere_General->position += (moveDir * distToCorrect);
// 		glm::vec3 moveDir = glm::normalize((reverseTransformedSphere.position - closestContactPoint));
// 
// 		//glm::vec3 oldPos = reverseTransformedSphere.position;
// 		reverseTransformedSphere.position += (moveDir * distToCorrect);
// 
// 		// TODO reverse the rotation of this vector
// 		glm::vec3 deltaPos = (moveDir * distToCorrect);
// 		deltaPos = (matModelR * glm::vec4(deltaPos, 1.0f));
// 
// 		////////// RE-REVERSING UPDATED POSITION /////////
// 		//pSphere_General->position = (matModel * glm::vec4(reverseTransformedSphere.position, 1.0f));
// 		pSphere_General->position += deltaPos;
// 
// 
// 		// We add this "collision event" to the list or queue of collisions
// 		sCollisionEvent theCollision;
// 		
// 		theCollision.pObjectA = pSphere_General;
// 		// 
// 		theCollision.contactPoint = closestContactPoint;
// 		theCollision.reflectionNormal = reflectionVec;
// //		theCollision.velocityAtCollision = reflectionVec;
// 		
// 		// TODO: We'll have a problem later: what deletes this?
// 		sPhysicsProperties* pTriangleWeHit = new sPhysicsProperties();
// 
// 		pTriangleWeHit->setShape(new sPhysicsProperties::sTriangle(closestTriangleVertices[0],
// 																   closestTriangleVertices[1],
// 																   closestTriangleVertices[2]));
// 		theCollision.pObjectB = pTriangleWeHit;
// 		
// 		this->m_vecCollisionsThisFrame.push_back(theCollision);
// 
// // Or we 
// //		pSphere_General->velocity = glm::vec3(0.0f);
// 
// 		return true;
// 	}

// New code, doesn't care about triangle info, just the hit norm
	if (earliestTime <= 1.0f) // If earliest collision detected is within the update range
	{
		///// Sphere Direction Vector /////
		glm::vec3 sphereDirection = reverseTransformedSphere.velocity;
		// Normalize... 
		sphereDirection = glm::normalize(sphereDirection);

		glm::vec3 sphereStep = reverseTransformedSphere.position - reverseTransformedSphere.oldPosition; // The movement of the sphere over the update


	// 	Calcualte the current normal from the TRANSFORMED vertices
	// 		glm::vec3 edgeA = closestTriangleVertices[1] - closestTriangleVertices[0];
	// 		glm::vec3 edgeB = closestTriangleVertices[2] - closestTriangleVertices[0];
	// 	
	// 		glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));


		///// Reflection Vector calculated with hitNorm /////
		glm::vec3 reflectionVec = glm::reflect(sphereDirection, hitNorm); // I think this is normalized???

		// Update the  velocity based on this reflection vector
		float sphereSpeed = glm::length(reverseTransformedSphere.velocity);
		glm::vec3 newVelocity = reflectionVec * sphereSpeed;



		// Degree between sphere dir and hit norm: 180 is absolute restitution application, 90 is none (parallel to it)
		float degToNorm = glm::degrees(abs(acos(glm::dot(sphereDirection, hitNorm) / glm::length(sphereDirection) * glm::length(hitNorm))));

		//degToNorm -= 90; // Now we're working with 90: total restitution app.   0: no app
		float restAppDegree = degToNorm - 90; // Multiplier (1.0 - 0.0) to influence effects of the restitution application
		restAppDegree /= 90;

		glm::vec3 restitutionVelLoss = -hitNorm * newVelocity; // Calculate vector we want to reduce velocity on (negative normal of surface it's bouncing on)
		newVelocity += (restitutionVelLoss * (1.0f - pSphere_General->restitution)) * restAppDegree; // Subtract said vector from newVelocity, scaled with its restitution (0 restitution = no bounce, 1 = full bounce)

		// At this point we're done with changing velocity based on collision, now to transform all values back


		// TODO will have to rotate the velocity vector back
		newVelocity = (matModelR * glm::vec4(newVelocity, 1.0f));
		pSphere_General->velocity = newVelocity;


		// Re-position the sphere where it would be if it perfectly bounced off the triangle
		//float distToCorrect = (pSphere->radius - closestDistanceSoFar) * 2; // Get length we need to move the sphere by
		//glm::vec3 moveDir = glm::normalize((pSphere_General->position - closestContactPoint));
		//pSphere_General->position += (moveDir * distToCorrect);
		//glm::vec3 moveDir = glm::normalize((reverseTransformedSphere.position - closestContactPoint));

		//glm::vec3 oldPos = reverseTransformedSphere.position;
		//reverseTransformedSphere.position += (moveDir * distToCorrect);



		// Here we must set the old position to the collision point, and calculate the new current position based on the reflection and time left in the update

		// Start by calculating new old position with t

		glm::vec3 oldPos = reverseTransformedSphere.oldPosition;
		reverseTransformedSphere.oldPosition += sphereStep * earliestTime; // This is the point of collision

		// Now to add remainder of the update time of the reflected vector 
		float remainingLength = glm::length(sphereStep * (1.0f - earliestTime));
		reverseTransformedSphere.position = reverseTransformedSphere.oldPosition + reflectionVec * remainingLength;


		// TODO reverse the rotation of this vector
		//glm::vec3 deltaPos = (moveDir * distToCorrect);
		//deltaPos = (matModelR * glm::vec4(deltaPos, 1.0f));

		///// Reverse delta pos changes rotation to apply to OG sphere data //////  (Cheaper than full rotation + translation of old and new pos?)
		glm::vec3 deltaOld = reverseTransformedSphere.oldPosition - oldPos;
		glm::vec3 deltaNew = reverseTransformedSphere.position - reverseTransformedSphere.oldPosition;

		deltaOld = (matModelR * glm::vec4(deltaOld, 1.0f));
		deltaNew = (matModelR * glm::vec4(deltaNew, 1.0f));


		////////// RE-REVERSING UPDATED POSITION /////////
		//pSphere_General->position = (matModel * glm::vec4(reverseTransformedSphere.position, 1.0f));
		//pSphere_General->position += deltaPos;

		///// Apply Delta Positions /////
		pSphere_General->oldPosition += deltaOld;
		pSphere_General->position += deltaNew;

		/////////// TOOOOODOOOOOOO: Recursive step to restart this collision detection with updated old and new positions

		// We add this "collision event" to the list or queue of collisions
		sCollisionEvent theCollision;

		theCollision.pObjectA = pSphere_General;
		// 
		theCollision.contactPoint = closestContactPoint;
		theCollision.reflectionNormal = reflectionVec;
		//		theCollision.velocityAtCollision = reflectionVec;

				// TODO: We'll have a problem later: what deletes this?
		sPhysicsProperties* pTriangleWeHit = new sPhysicsProperties();

		pTriangleWeHit->setShape(new sPhysicsProperties::sTriangle(closestTriangleVertices[0],
			closestTriangleVertices[1],
			closestTriangleVertices[2]));
		theCollision.pObjectB = pTriangleWeHit;

		this->m_vecCollisionsThisFrame.push_back(theCollision);

		// Or we 
		//		pSphere_General->velocity = glm::vec3(0.0f);

		return true;
	}

	// Didn't hit
	return false;
}

bool cPhysics::m_Sphere_TriMeshLocal_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriMesh)
{
	// TODO: Insert amazing code here 
	return false;
}


// Tests a moving sphere with a triangle
// Sets t as the "time" along the update path when it intersects
// Sets the hitNorm as the unit vector of the hit location on the sphere
bool cPhysics::m_TestMovingSphereTriangle(sPhysicsProperties* pSphere, sTriangle_A* pTri, float& t, glm::vec3& hitNorm)
{
	sPhysicsProperties::sSphere* pSphereShape = (sPhysicsProperties::sSphere*)(pSphere->pShape); // To access the radius
	

	// Start by getting the plane created by the triangle

	glm::vec3 ab = pTri->vertices[1] - pTri->vertices[0];
	glm::vec3 ac = pTri->vertices[2] - pTri->vertices[0];

	glm::vec3 triNorm = glm::normalize(glm::cross(ab, ac));


	// Now get the point on the sphere that will first hit that plane
	// Reverse the norm, and add that multiplied by the radius to the sphere's center

	//glm::vec3 spherePoint = pSphere->oldPosition - triNorm * pSphereShape->radius; // Use old position since at the start of this update, we set the new position (which might collide)

	// Now get the position this point will hit on the plane

	float pd = glm::dot(triNorm, pTri->vertices[0]);


	//int cPhysics::m_IntersectMovingSpherePlane(sPhysicsProperties* pSphere, glm::vec3 pn, float pd, float& t, glm::vec3& q)

	glm::vec3 q;

	if (!m_IntersectMovingSpherePlane(pSphere, triNorm, pd, t, q))
	{
		// Sphere does not intersect plane
		return false;
	}

	// Run a quick check to make sure the sphere makes it to the plane in this update
	glm::vec3 triV = pSphere->position - pSphere->oldPosition;

	if (t > 1.0f)
	{
		// Sphere will hit this plane sometime in the future, don't have to worry about it now
		return false;
	}

// 	if (glm::distance(pSphere->oldPosition, pSphere->oldPosition + triV * t) > glm::distance(pSphere->oldPosition, pSphere->position))
// 	{
// 		// Too far, does not collide
// 		return false;
// 	}


	// Sphere intersects plane made by triangle, now find out if intersection point is in the triangle
	// We do closest point to triangle check: if distance is in epsilon land, the point is in the triangle, else we check that point for even later collision

	glm::vec3 closestTriPoint = m_ClosestPtPointTriangle(q, pTri->vertices[0], pTri->vertices[1], pTri->vertices[2]);

	if (glm::distance(closestTriPoint, q) <= std::numeric_limits<float>::epsilon()) // If the distance is "0"
	{
		// We reflect off the triangle face! nice n' easy

		// Set the normal pointing to the impact position
		//hitNorm = glm::normalize(q - (pSphere->oldPosition + triV * t));
		hitNorm = triNorm;

		return true;
	}
	
	// We now have to check if the sphere later intersects with this point (past the plane cast by the triangle)

	// Could start by projecting all onto plane orthogonal to the movement vector and see if point q is within radius distance of sphere center
	// Will still have to figure out where on the sphere it collides with if it close enough

	glm::vec3 rayHitOnSphere; // Relative to sphere's old position

	if (!m_IntersectRaySphere(closestTriPoint, -triV, pSphere, t, rayHitOnSphere))
	{
		// Ray misses sphere
		return false;
	}

	// Ray hit sphere, check if within update range
	if (t > 1.0f)
	{
		// Ray hits in the future
		return false;
	}

	// Ray hits this update

	// Set the hitNorm, t is already set by the function called!

	hitNorm = glm::normalize(pSphere->oldPosition - rayHitOnSphere);

	return true;
}





