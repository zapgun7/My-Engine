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

//bool cPhysics::m_Sphere_TriMeshIndirect_IntersectionTest(sPhysicsProperties* pSphere_General, sPhysicsProperties* pTriMesh_General, sPossibleCollision& returnCollision)
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



	//sPhysicsProperties reverseTransformedSphere = *pSphere_General;
	this->m_pReversedObject->position = pSphere_General->position;
	this->m_pReversedObject->oldPosition = pSphere_General->oldPosition;
	this->m_pReversedObject->pShape = pSphere_General->pShape;

	// Transform current to fit default triangle mesh
	this->m_pReversedObject->position = (matRevModelT * glm::vec4(this->m_pReversedObject->position, 1.0f));
	this->m_pReversedObject->position = (matRevModelR * glm::vec4(this->m_pReversedObject->position, 1.0f));

	// Do it to old position too for continuous collision detection
	this->m_pReversedObject->oldPosition = (matRevModelT * glm::vec4(this->m_pReversedObject->oldPosition, 1.0f));
	this->m_pReversedObject->oldPosition = (matRevModelR * glm::vec4(this->m_pReversedObject->oldPosition, 1.0f));

	// Rotate velocity to match default tri mesh
	//reverseTransformedSphere.velocity = (matRevModelR * glm::vec4(reverseTransformedSphere.velocity, 1.0f)); // Don't need this anymore
	
	
	// Recursive AABB function to return near triangles
	//std::vector<sTriangle_A> trisToCheck = TriMeshAABB->sphereCollision(&reverseTransformedSphere); // TODO make this capsule detection for the sphere sweep
	std::vector<sTriangle_A> trisToCheck = TriMeshAABB->sweepingSphereCollision(this->m_pReversedObject);

	int triSize = static_cast<int>(trisToCheck.size());

	

	if (triSize == 0) return false; // No tris to check
	//std::cout << triSize << std::endl;

	//std::cout << pSphere_General->position.y << "    " << pSphere_General->oldPosition.y << std::endl;


	if (!this->m_bUseThreading)
	{
		float earliestTime = FLT_MAX; // Time (0-1) over current update the sphere collides
		glm::vec3 hitNorm; // Normal used to calculate reflection

		// The newest code for dealing with continuous collision detection
		for (std::vector<sTriangle_A>::iterator itTri = trisToCheck.begin();
			itTri != trisToCheck.end();
			itTri++)
		{
			float t; // Time (0-1) over current update the sphere collides
			glm::vec3 hn; // Normal used to calculate reflection

			if (!m_TestMovingSphereTriangle(this->m_pReversedObject, &*itTri, t, hn))
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


		} //for ( unsigned int index...


		if (earliestTime <= 1.0f) // If earliest collision detected is within the update range
		{
			this->m_pTheSoonestCollision->q = earliestTime;
			this->m_pTheSoonestCollision->collisionObject = pTriMesh_General;
			hitNorm = (matModelR * glm::vec4(hitNorm, 1.0f));
			this->m_pTheSoonestCollision->hitNorm = hitNorm;
			return true;

		}
		return false;
	}

	
	unsigned int currTri = 0;

	// Set all thread info's shape to the sphere
	//m_ThreadInfos[0].theShape = &m_pReversedObject; // Set the pointer to point to the address of the current shape; should update for all threads?

	// !!! Will have to set shape type when this threading is set up for more than just sphere-tri_mesh
	

	int perThreadTriCount = triSize / NUM_THREADS;
	int overflowCount = triSize % NUM_THREADS;

	int currTriIdxToGive = 0;


	for (unsigned int threadIDX = 0; threadIDX < NUM_THREADS; threadIDX++)
	{
		//	Give each thread an (almost) equal amount of work
		//m_ThreadInfos[threadIDX].theShape = &reverseTransformedSphere;
		this->m_ThreadInfos[threadIDX].theShape = this->m_pReversedObject; // Give thread the sphere
		this->m_ThreadInfos[threadIDX].theTriangles = &(trisToCheck[currTriIdxToGive]); // Give thread the triangle array


		int amntToAdd = overflowCount-- > 0 ? perThreadTriCount + 1 : perThreadTriCount;


		this->m_ThreadInfos[threadIDX].arraySize = amntToAdd; // Tell thread how far to go into array given
		this->m_ThreadInfos[threadIDX].hasWork = true; // Start the thread
		currTriIdxToGive += amntToAdd;
	}


	for (unsigned int threadIDX = 0; threadIDX < NUM_THREADS; threadIDX++)
	{
		while (this->m_ThreadInfos[threadIDX].hasWork)
		{
			Sleep(0);
		}
		// Thread is done, grab its processed info
// 		if (m_ThreadInfos[threadIDX].soonestHit < m_pTheSoonestCollision->q)
// 		{
// 			m_pTheSoonestCollision->q = m_ThreadInfos[threadIDX].soonestHit;
// 			m_pTheSoonestCollision->hitNorm = m_ThreadInfos[threadIDX].hn;
// 			m_ThreadInfos[threadIDX].soonestHit = FLT_MAX; // Set up for next time thread runs
// 		}
	}

	// All threads are done at this point
	if (this->m_pTheSoonestCollision->q <= 1.0f)
	{
		this->m_pTheSoonestCollision->hitNorm = (matModelR * glm::vec4(this->m_pTheSoonestCollision->hitNorm, 1.0f));
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


	if (t > 1.0f)
	{
		// Sphere will hit this plane sometime in the future (not this update), don't have to worry about it now
		return false;
	}

// 	if (glm::distance(pSphere->oldPosition, pSphere->oldPosition + triV * t) > glm::distance(pSphere->oldPosition, pSphere->position))
// 	{
// 		// Too far, does not collide
// 		return false;
// 	}


	// Sphere intersects plane made by triangle, now find out if intersection point is in the triangle
	// We do closest point to triangle check: if distance is in epsilon land, the point is in the triangle, else we check that point for even later collision

	//glm::vec3 closestTriPoint = m_ClosestPtPointTriangle(q, pTri->vertices[0], pTri->vertices[1], pTri->vertices[2]);
	glm::vec3 closestTriPoint = m_ClosestPtTriPlanePointTriangle(q, pTri->vertices[0], pTri->vertices[1], pTri->vertices[2]);

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
	glm::vec3 triV = pSphere->position - pSphere->oldPosition;

	float updateLen = glm::length(triV);
	triV = glm::normalize(triV);


	/////// IDEA TO SKIP BELOW RAY FUNCTION: tri-point and sphere onto plane orthogonal to sphere move dir, compare dist.
	/////// This will require more work if it does hit, but might make up for it by avoiding the sqrt enough times??

	// Decreases performance, still keeping it here though
// 	glm::vec3 triVNorm = glm::normalize(triV);
// 	float triVLen = glm::length(triV);
// 
// 	glm::vec3 projTriPoint = (glm::dot(closestTriPoint, triV)/triVLen) * triVNorm;
// 	projTriPoint = closestTriPoint - projTriPoint;
// 
// 	glm::vec3 projSpherePoint = (glm::dot(pSphere->oldPosition, triV) / triVLen) * triVNorm;
// 	projTriPoint = pSphere->oldPosition - projSpherePoint;
// 
// 
// 	if (glm::distance(projTriPoint, projSpherePoint) > pSphereShape->radius)
// 	{
// 		// Does not intersect, don't bother casting ray
// 		return 0;
// 	}



	if (!m_IntersectRaySphere(closestTriPoint, -triV, pSphere, t, rayHitOnSphere))
	{
		// Ray misses sphere
		return false;
	}

	// t is in terms of unit vectors, so we gotta translate it back to time over the update
	float rayLen = glm::length(t * triV); 

	t = rayLen / updateLen; // Shouldn't ever divide by 0, but if it does hello future f u :)


	// Ray hit sphere, check if within update range
	if (t > 1.0f)
	{
		// Ray hits in the future
		return false;
	}
	else if (t == 0.0f)
	{
		hitNorm = triNorm;
		return true;
	}

	// Ray hits this update

	// Set the hitNorm, t is already set by the function called!

	hitNorm = glm::normalize(pSphere->oldPosition - rayHitOnSphere);

	return true;
}


bool cPhysics::m_TestMovingCapsuleTriangle(sPhysicsProperties* pCapsule, sTriangle_A* pTri, float& t, glm::vec3& hitNorm)
{
	// Can decide what parts of the capsule can hit the triangle based on the triangle norm
	// Imagine an up-right capsule: if the norm has any y < 0 component, the top of the cap will hit a face or and edge, 
	// the lower cap could only hit an edge, and the midsection can only hit an edge


	// Another certainty:
	// The caps can hit an edge or a face
	// The midsection can only hit edges
	// If the tri is big enough, the caps will instead hit the faces

	// !!! Keep in mind the capsule is locked to its orientation


	// Hypothetical Algorithm:
	// More often than not, the lower cap will be the one to collide (the feet of the character)
	// 0. Could start by projecting capsule spine and triangle onto move-dir plane; test if line-tri distance is <= radius
	//		- Can even before that compare tri-norm to move direction to see if it will even bounce off the tri if it does "collide"
	// 1. Make sphere at the lower cap; plane test; closest point
	//		- If on face, make sure normal is facing the right way to do so; can't hit the top of the bottom cap sphere (that's in the midsection)
	//			* We can return this value if the normal is within the proper parameters
	//		- If on edge, try to put off the raycast (depends on tri-norm)
	//			* If normal is ideal for the top cap, do a quick plane and closest point check to see if it hits the face
	//            If it doesn't hit face, run raycast for lower and upper cap (for can't think of a case where one will not necessitate the other)
	//            If either of these raycasts return an unideal norm, we know that it hits the midsection
	//			  This unideal spot can be projected on the upvec to get the orthogonal direction to the capsule spine, to get the rough spot on the capsule for the hitnorm
	//            Can prob do some voodoo to get the q along the update path (brain not working 100% in class rn)
	//      - If the triangle edge will hit the midsection first and either hit a cap later or not at all: how to check?
	//			* 


	// Start by comparing the tri-norm to the move direction
	glm::vec3 ab = pTri->vertices[1] - pTri->vertices[0];
	glm::vec3 ac = pTri->vertices[2] - pTri->vertices[0];

	glm::vec3 triNorm = glm::normalize(glm::cross(ab, ac));

	//glm::vec3 moveDir = pCapsule->position - pCapsule->oldPosition;
	//glm::vec3 moveDirNorm = glm::normalize(moveDir);

	// Commenting this out since the spherePlane test does it already
// 	float degDiff = acos(glm::dot(triNorm, moveDir));
// 
// 	if (abs(degDiff) <= 90.0f) return 0; // Will not hit the front of the triangle

	// TODO try projecting capsule spine and triangle on moveDir plane; distance between segment and triangle (or 3x segment-segment), make sure it's <= radius


	// Get the degree between our upVec and the triNorm

	float degDiff = acos(glm::dot(triNorm, pCapsule->upVec)); // 0 - 90: favors lower cap    90-180: favors upper cap
	bool favorDown = false;
	bool favorUp = false;
	if (degDiff >= 90) favorUp = true;  // These both can be true
	if (degDiff <= 90) favorDown = true;

	sPhysicsProperties::sCapsule* capsule = (sPhysicsProperties::sCapsule*)pCapsule->pShape;

	// Now we start testing on the "lower" cap

	sPhysicsProperties::sSphere capSphere(capsule->radius); // TODO put these caps in some struct with the other stuff so we don't create these for each triangle
	sPhysicsProperties lowerCap;
	lowerCap.pShape = &capSphere;
	lowerCap.position = pCapsule->position - pCapsule->upVec * capsule->halfLength;
	lowerCap.oldPosition = pCapsule->oldPosition - pCapsule->upVec * capsule->halfLength;

	float pd = glm::dot(triNorm, pTri->vertices[0]);
	glm::vec3 q1 = glm::vec3(0); glm::vec3 q2 = glm::vec3(0);
	float t1 = 0.0f;
	float t2 = 0.0f;

	if (!m_IntersectMovingSpherePlane(&lowerCap, triNorm, pd, t1, q1))
	{
		return 0;
	}


	bool raycastLower = false;
	bool raycastUpper = false;

	
	glm::vec3 closestTriPointLower;
	if (t1 > 1.0f)
	{
		// Not returning, we have other things to check
	}
	else
	{
		closestTriPointLower = m_ClosestPtTriPlanePointTriangle(q1, pTri->vertices[0], pTri->vertices[1], pTri->vertices[2]);
		// Do the rest of the checks for this 
		if (favorDown)
		{
			// Favorable conditions for bouncing off the bottom of the sphere
			if (glm::distance(closestTriPointLower, q1) <= std::numeric_limits<float>::epsilon())
			{
				// Bottom cap hits triangle face; best case for collision
				hitNorm = triNorm;
				t = t1;
				return true;
			}
			else
			{
				// Can still hit the edge; put off raycast to later
				raycastLower = true;
			}
		}
		else
		{
			// Unfavorable; at best hits the edge. The bottom cap sphere hitting a face will hit the midsection or top cap first
			// Put off doing the raycast until we confirm the top cap doesn't cleanly hit a face
			raycastLower = true;
		}
	}

	// Now do the same for the top cap
	sPhysicsProperties upperCap;
	upperCap.pShape = &capSphere;
	upperCap.position = pCapsule->position + pCapsule->upVec * capsule->halfLength;
	upperCap.oldPosition = pCapsule->oldPosition + pCapsule->upVec * capsule->halfLength;

	//float pd = glm::dot(triNorm, pTri->vertices[0]);
	//glm::vec3 q = glm::vec3(0);
	m_IntersectMovingSpherePlane(&upperCap, triNorm, pd, t2, q2); // TODO can we easily get this by working off the answer to the bottom cap calculation?


	glm::vec3 closestTriPointUpper;
	if ((t2 > 1.0f) && (t1 > 1.0f))
	{
		// Neither sphere cap makes it to the plane this update
		return 0;
	}
	else
	{
		closestTriPointUpper = m_ClosestPtTriPlanePointTriangle(q2, pTri->vertices[0], pTri->vertices[1], pTri->vertices[2]);
		// Do the rest of the checks for this 
		if (favorUp)
		{
			// Favorable conditions for bouncing off the bottom of the sphere
			if (glm::distance(closestTriPointUpper, q2) <= std::numeric_limits<float>::epsilon())
			{
				// Bottom cap hits triangle face; best case for collision
				hitNorm = triNorm;
				t = t2;
				return true;
			}
			else
			{
				raycastUpper = true;
			}
		}
		else
		{
			// Unfavorable; at best hits the edge. This will hit the midsection if at all
			// Put off doing the raycast until we fully confirm bottom cap missing
			raycastUpper = true;
		}
	}


	// Now we're left to raycasting whatever spheres were flagged

	glm::vec3 rayHitOnSphere1;
	glm::vec3 triV = pCapsule->position - pCapsule->oldPosition;

	float updateLen = glm::length(triV);
	triV = glm::normalize(triV);


	// If we flagged the lowercap for a raycast
	if (raycastLower)
	{
		if (!m_IntersectRaySphere(closestTriPointLower, -triV, &lowerCap, t1, rayHitOnSphere1))
		{
			// Ray misses sphere, cool
		}
		else // Ray hits sphere
		{
			t1 = t1 / updateLen;
			if (t1 > 1.0f)
			{
				// Intersects some time after the update
			}
			else if (t1 == 0.0f)
			{
				hitNorm = triNorm; // Works for the sphere algorithm, might not for this capsule
				//hitNorm = glm::normalize(lowerCap.oldPosition - rayHitOnSphere1);
				t = t1;
				return 1;
			}
			// Ray hits the sphere; now we check whether it hits on the top or bottom half of the sphere
			degDiff = acos(glm::dot(capsule->upVec, rayHitOnSphere1 - lowerCap.oldPosition));
			if (degDiff < 90)
			{
				// Hits top of sphere, will hit midsection first
				if (favorDown) raycastUpper = false; // We know this will not hit the top one; it will hit the midsection however
			}
			else
			{
				hitNorm = glm::normalize(lowerCap.oldPosition - rayHitOnSphere1);
				t = t1;
				return 1;
				// Hits bottom of sphere
// 				if (favorDown)
// 				{
// 					// Guaranteed hit here, we can return true
// 					hitNorm = glm::normalize(lowerCap.oldPosition - rayHitOnSphere1);
// 					t = t1;
// 					return 1;
// 				}
// 				else
// 				{
// 					// Not guaranteed to hit this sphere
// 					//t1 = t;
// 				}
			}
		}
	}

	glm::vec3 rayHitOnSphere2;
	//glm::vec3 triV = pCapsule->position - pCapsule->oldPosition;

	//float updateLen = glm::length(triV);
	//triV = glm::normalize(triV);


	// If we flagged the lowercap for a raycast
	if (raycastUpper)
	{
		if (!m_IntersectRaySphere(closestTriPointUpper, -triV, &upperCap, t2, rayHitOnSphere2))
		{
			// Ray misses sphere, cool
		}
		else // Ray hits sphere
		{
			t2 = t2 / updateLen;
			if (t > 1.0f)
			{
				// Intersects some time after the update
			}
			else if (t2 == 0.0f)
			{
				hitNorm = triNorm; // Works for the sphere algorithm, might not for this capsule
				t = t2;
				return 1;
			}
			// Ray hits the sphere; now we check whether it hits on the top or bottom half of the sphere
			degDiff = acos(glm::dot(capsule->upVec, rayHitOnSphere2 - upperCap.oldPosition));
			if (degDiff > 90)
			{
				// Hits bottom of sphere, will hit midsection first
				//if (favorDown) raycastUpper = false; 
			}
			else
			{
				hitNorm = glm::normalize(upperCap.oldPosition - rayHitOnSphere2);
				t = t2;
				return 1;
				// Hits top of sphere
// 				if (favorUp)
// 				{
// 					// Guaranteed hit here, we can return true
// 					hitNorm = glm::normalize(upperCap.oldPosition - rayHitOnSphere2);
// 					t = t2;
// 					return 1;
// 				}
// 				else
// 				{
// 					// Not guaranteed to hit this sphere
// 					//t2 = t;
// 				}
			}
		}
	}


	// At this point I'm PRETTY SURE we just have to calculate the midsection collision (not guaranteed collision)


	//////// STRATEGY ////////
	// - Flat swept capsule (midsection) creates a quad; can be split into 2x tris and simpler tri intersection code can be used
	// - Idk gl future me

	return 0;
}


bool cPhysics::m_Capsule_TriMeshIndirect_IntersectionTest(sPhysicsProperties* pCapsule_General, sPhysicsProperties* pTriMesh_General, sPossibleCollision& returnCollision)
{
	// Do we have a mesh manager? 
	if (!this->m_pMeshManager)
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

	sPhysicsProperties::sCapsule* pCapsule = (sPhysicsProperties::sCapsule*)(pCapsule_General->pShape);


	cAABB* TriMeshAABB = findAABBByModelName(pTriangleMesh->meshName);

	if (TriMeshAABB == nullptr)
	{
		std::cout << "Couldn't find AABB" << std::endl;
		return false;
	}


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



	sPhysicsProperties reverseTransformedCapsule = *pCapsule_General;

	//sPhysicsProperties::sCapsule* reverseCapsule = (sPhysicsProperties::sCapsule*)(pCapsule_General->pShape);
	// Transform current to fit default triangle mesh
	reverseTransformedCapsule.position = (matRevModelT * glm::vec4(reverseTransformedCapsule.position, 1.0f));
	reverseTransformedCapsule.position = (matRevModelR * glm::vec4(reverseTransformedCapsule.position, 1.0f));

	// Do it to old position too for continuous collision detection
	reverseTransformedCapsule.oldPosition = (matRevModelT * glm::vec4(reverseTransformedCapsule.oldPosition, 1.0f));
	reverseTransformedCapsule.oldPosition = (matRevModelR * glm::vec4(reverseTransformedCapsule.oldPosition, 1.0f));


	reverseTransformedCapsule.upVec = (matRevModelR * glm::vec4(reverseTransformedCapsule.upVec, 1.0f));

	// Rotate velocity to match default tri mesh
	//reverseTransformedCapsule.velocity = (matRevModelR * glm::vec4(reverseTransformedCapsule.velocity, 1.0f)); // Don't need


	std::vector<sTriangle_A> trisToCheck = TriMeshAABB->sweepingCapsuleCollision(&reverseTransformedCapsule);

	// And then check each tri in the vec, same as the sphere stuff. gl future me <3

	float earliestTime = FLT_MAX; // Time (0-1) over current update the sphere collides
	glm::vec3 hitNorm; // Normal used to calculate reflection

	// The newest code for dealing with continuous collision detection
	for (std::vector<sTriangle_A>::iterator itTri = trisToCheck.begin();
		itTri != trisToCheck.end();
		itTri++)
	{

		float t;
		glm::vec3 hn;

		if (!m_TestMovingCapsuleTriangle(&reverseTransformedCapsule, &*itTri, t, hn))
		{
			// No collision
			continue;
		}

		if (t < earliestTime)
		{
			earliestTime = t;
			hitNorm = hn;
		}

	}

	if (earliestTime <= 1.0f)
	{
// 		returnCollision.q = earliestTime;
// 		returnCollision.collisionObject = pTriMesh_General;
// 		hitNorm = (matModelR * glm::vec4(hitNorm, 1.0f));
// 		returnCollision.hitNorm = hitNorm;
// 		return true;
		this->m_pTheSoonestCollision->q = earliestTime;
		this->m_pTheSoonestCollision->collisionObject = pTriMesh_General;
		hitNorm = (matModelR * glm::vec4(hitNorm, 1.0f));
		this->m_pTheSoonestCollision->hitNorm = hitNorm;
		return true;
	}

	return false;
}

bool cPhysics::m_LineSegment_TriMeshIndirect_IntersectionTest(glm::vec3 pos, glm::vec3 dir, float len, sPhysicsProperties* pTriMesh_General, sPossibleCollision& returnCollision)
{
	// Do we have a mesh manager? 
	if (!this->m_pMeshManager)
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

	//sPhysicsProperties::sCapsule* pCapsule = (sPhysicsProperties::sCapsule*)(pCapsule_General->pShape);


	cAABB* TriMeshAABB = findAABBByModelName(pTriangleMesh->meshName);

	if (TriMeshAABB == nullptr)
	{
		std::cout << "Couldn't find AABB" << std::endl;
		return false;
	}


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
	matModelR *= matRotation; // Just rotation



	//sPhysicsProperties reverseTransformedCapsule = *pCapsule_General;

	//sPhysicsProperties::sCapsule* reverseCapsule = (sPhysicsProperties::sCapsule*)(pCapsule_General->pShape);
	// Transform current to fit default triangle mesh
	//reverseTransformedCapsule.position = (matRevModelT * glm::vec4(reverseTransformedCapsule.position, 1.0f));
	//reverseTransformedCapsule.position = (matRevModelR * glm::vec4(reverseTransformedCapsule.position, 1.0f));
	pos = (matRevModelT * glm::vec4(pos, 1.0f));
	pos = (matRevModelR * glm::vec4(pos, 1.0f));

	dir = (matRevModelR * glm::vec4(dir, 1.0f));

	// Do it to old position too for continuous collision detection
	//reverseTransformedCapsule.oldPosition = (matRevModelT * glm::vec4(reverseTransformedCapsule.oldPosition, 1.0f));
	//reverseTransformedCapsule.oldPosition = (matRevModelR * glm::vec4(reverseTransformedCapsule.oldPosition, 1.0f));


	std::vector<sTriangle_A> trisToCheck = TriMeshAABB->lineSegmentCollision(pos, dir, len);

	float earliestTime = FLT_MAX;
	glm::vec3 hitNorm;

	glm::vec3 pos2 = pos + dir * len;

	for (std::vector<sTriangle_A>::iterator itTri = trisToCheck.begin();
		itTri != trisToCheck.end();
		itTri++)
	{
		float t, u, v, w;
		glm::vec3 hn;

		if (!m_IntersectSegmentTriangle(pos, pos2, &*itTri, u, v, w, hn, t))
		{
			// No collision
			continue;
		}

		// Collison!
		if (t < earliestTime)
		{
			earliestTime = t;
			hitNorm = hn;
		}
	}

	if (earliestTime <= 1.0f)
	{
		returnCollision.q = earliestTime;
		returnCollision.collisionObject = pTriMesh_General;
		hitNorm = (matModelR * glm::vec4(hitNorm, 1.0f));
		returnCollision.hitNorm = hitNorm;
		return true;
	}

	return false;



}


