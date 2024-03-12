// This is a class for retrieving relevant triangles in the oct-tree for various object types

//#include "cAABB.h"
//#include "sPhysicsProperties.h"
#include "cPhysics.h" // For the sweeping physics tests
#include <algorithm>

std::vector<sTriangle_A> triangleVectorUnion(std::vector<sTriangle_A> vecA, std::vector<sTriangle_A> vecB);







cAABB* cAABB::getChild(unsigned int ID)
{
	std::map< unsigned int,
		cAABB* >::iterator
		itChild = this->mapChild_pAABBs.find(ID);

	// Find it? 
	if (itChild == this->mapChild_pAABBs.end())
	{
		// Nope
		return nullptr;
	}

	return itChild->second;
}

//////////////////// SPHERE COLLISION ///////////////////////////

// This will kick off the root check then go to a recursive method to tackle children (AABB ones)
std::vector<sTriangle_A> cAABB::sphereCollision(sPhysicsProperties* sphere)
{
	// Start by checking if the sphere is even in the root AABB
	float radius = ((sPhysicsProperties::sSphere*)sphere->pShape)->radius;
	glm::vec3 sphPos = sphere->position;
	std::vector<sTriangle_A> returnTris;

	if (sphPos.x - radius > this->xMax()) return returnTris;
	if (sphPos.x + radius < this->xMin()) return returnTris;

	if (sphPos.z - radius > this->zMax()) return returnTris;
	if (sphPos.z + radius < this->zMin()) return returnTris;

	if (sphPos.y - radius > this->yMax()) return returnTris; // Check y last cause I think the other two are more likely?
	if (sphPos.y + radius < this->yMin()) return returnTris;

	// Assuming it is in the root box
	// Now we kick off the recursion

	returnTris = this->sphereRecursion(radius, sphPos);

	return returnTris;

}

std::vector<sTriangle_A> cAABB::sphereRecursion(float sphRad, glm::vec3 sphPos)
{
	// First check if we are in a leaf AABB
	//if (this->mapChild_pAABBs.size() == 0)
	if (this->getChild(0) == nullptr)
	{
		// This is a leaf, return the triangles stored here
		return triangles;
	}
	// This is not a leaf


	// We start by deciding which child AABBs to traverse (we know we are within the parent's bounds)
	bool validChildren[8] = {true, true, true, true, true, true, true, true};

	// #: xyz
	// 0: ---
	// 1: -+-
	// 2: --+
	// 3: -++
	// 4: +-+
	// 5: +++
	// 6: +--
	// 7: ++-
	// Now we prune the trues
	if (sphPos.y - sphRad > this->Ycenter()) // If bottom of sphere is above the center (all -y children = false)
	{
		validChildren[0] = false;
		validChildren[2] = false;
		validChildren[4] = false;
		validChildren[6] = false;
	}
	else if (sphPos.y + sphRad < this->Ycenter()) // If top of sphere is below center
	{
		validChildren[1] = false;
		validChildren[3] = false;
		validChildren[5] = false;
		validChildren[7] = false;
	}

	if (sphPos.x - sphRad > this->Xcenter()) // If -x of sphere is +x of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[2] = false;
		validChildren[3] = false;
	}
	else if (sphPos.x + sphRad < this->Xcenter()) // If +x of sphere is -x of center
	{
		validChildren[4] = false;
		validChildren[5] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}

	if (sphPos.z - sphRad > this->Zcenter()) // If -z of sphere is +z of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}
	else if (sphPos.z + sphRad < this->Zcenter()) // if +z of sphere is -z of center
	{
		validChildren[2] = false;
		validChildren[3] = false;
		validChildren[4] = false;
		validChildren[5] = false;
	}


	// Now we know what children we need to traverse
	std::vector<sTriangle_A> vec0;
	std::vector<sTriangle_A> vec1;
	std::vector<sTriangle_A> vec2;
	std::vector<sTriangle_A> vec3;
	std::vector<sTriangle_A> vec4;
	std::vector<sTriangle_A> vec5;
	std::vector<sTriangle_A> vec6;
	std::vector<sTriangle_A> vec7;



	if (validChildren[0])
		vec0 = this->getChild(0)->sphereRecursion(sphRad, sphPos);
	if (validChildren[1])
		vec1 = this->getChild(1)->sphereRecursion(sphRad, sphPos);
	if (validChildren[2])
		vec2 = this->getChild(2)->sphereRecursion(sphRad, sphPos);
	if (validChildren[3])
		vec3 = this->getChild(3)->sphereRecursion(sphRad, sphPos);
	if (validChildren[4])
		vec4 = this->getChild(4)->sphereRecursion(sphRad, sphPos);
	if (validChildren[5])
		vec5 = this->getChild(5)->sphereRecursion(sphRad, sphPos);
	if (validChildren[6])
		vec6 = this->getChild(6)->sphereRecursion(sphRad, sphPos);
	if (validChildren[7])
		vec7 = this->getChild(7)->sphereRecursion(sphRad, sphPos);

	vec0 = triangleVectorUnion(vec0, vec1);
	vec2 = triangleVectorUnion(vec2, vec3);
	vec4 = triangleVectorUnion(vec4, vec5);
	vec6 = triangleVectorUnion(vec6, vec7);

	vec0 = triangleVectorUnion(vec0, vec2);
	vec4 = triangleVectorUnion(vec4, vec6);

	vec0 = triangleVectorUnion(vec0, vec4);
	
	


	// Cleanup
	//delete validChildren; 
	return vec0;
}


 ///////// SWEEPING SPHERE COLLISION ////////////

std::vector<sTriangle_A> cAABB::sweepingSphereCollision(sPhysicsProperties* sphere)
{
	// Start by checking if the sphere-swept area is even in the root AABB
	float radius = ((sPhysicsProperties::sSphere*)sphere->pShape)->radius;
	//glm::vec3 sphPos = sphere->position;
	std::vector<sTriangle_A> returnTris;

// 	if (sphPos.x - radius > this->xMax()) return returnTris;
// 	if (sphPos.x + radius < this->xMin()) return returnTris;
// 
// 	if (sphPos.z - radius > this->zMax()) return returnTris;
// 	if (sphPos.z + radius < this->zMin()) return returnTris;
// 
// 	if (sphPos.y - radius > this->yMax()) return returnTris; // Check y last cause I think the other two are more likely?
// 	if (sphPos.y + radius < this->yMin()) return returnTris;

	float t;
	if (!cPhysics::m_IntersectMovingSphereAABB(sphere, this, t))
	{
		// Not in root AABB, return empty vector
		return returnTris;
	}


	// Assuming it is in the root box
	// Now we kick off the recursion

	returnTris = this->sweepingSphereRecursion(radius, sphere->oldPosition, sphere->position);

	return returnTris;
}

std::vector<sTriangle_A> cAABB::sweepingSphereRecursion(float sphRad, glm::vec3 sphPos1, glm::vec3 sphPos2)
{
	// First check if we are in a leaf AABB
	//if (this->mapChild_pAABBs.size() == 0)
	if (this->getChild(0) == nullptr)
	{
		// This is a leaf, return the triangles stored here
		return triangles;
	}
	// This is not a leaf


	// We start by deciding which child AABBs to traverse (we know we are within the parent's bounds)
	bool validChildren[8] = { true, true, true, true, true, true, true, true };

	// #: xyz
	// 0: ---
	// 1: -+-
	// 2: --+
	// 3: -++
	// 4: +-+
	// 5: +++
	// 6: +--
	// 7: ++-
	// Now we prune the trues so we do less ray-AABB tests
	float tempMax, tempMin; // Min and max of each endpoint axis
	if (sphPos1.y > sphPos2.y)
	{
		tempMax = sphPos1.y + sphRad;
		tempMin = sphPos2.y - sphRad;
	}
	else
	{
		tempMax = sphPos2.y + sphRad;
		tempMin = sphPos1.y - sphRad;
	}
	
	if (tempMin > this->Ycenter()) // If bottom of capsule is above the center (all -y children = false)
	{
		validChildren[0] = false;
		validChildren[2] = false;
		validChildren[4] = false;
		validChildren[6] = false;
	}
	else if (tempMax < this->Ycenter()) // If top of sphere is below center
	{
		validChildren[1] = false;
		validChildren[3] = false;
		validChildren[5] = false;
		validChildren[7] = false;
	}


	if (sphPos1.x > sphPos2.x)
	{
		tempMax = sphPos1.x + sphRad;
		tempMin = sphPos2.x - sphRad;
	}
	else
	{
		tempMax = sphPos2.x + sphRad;
		tempMin = sphPos1.x - sphRad;
	}
	if (tempMin > this->Xcenter()) // If -x of sphere is +x of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[2] = false;
		validChildren[3] = false;
	}
	else if (tempMax < this->Xcenter()) // If +x of sphere is -x of center
	{
		validChildren[4] = false;
		validChildren[5] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}

	if (sphPos1.z > sphPos2.z)
	{
		tempMax = sphPos1.z + sphRad;
		tempMin = sphPos2.z - sphRad;
	}
	else
	{
		tempMax = sphPos2.z + sphRad;
		tempMin = sphPos1.z - sphRad;
	}
	if (tempMin > this->Zcenter()) // If -z of sphere is +z of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}
	else if (tempMax < this->Zcenter()) // if +z of sphere is -z of center
	{
		validChildren[2] = false;
		validChildren[3] = false;
		validChildren[4] = false;
		validChildren[5] = false;
	}


	// Now we know what children we need to traverse
	std::vector<sTriangle_A> vec0;
	std::vector<sTriangle_A> vec1;
	std::vector<sTriangle_A> vec2;
	std::vector<sTriangle_A> vec3;
	std::vector<sTriangle_A> vec4;
	std::vector<sTriangle_A> vec5;
	std::vector<sTriangle_A> vec6;
	std::vector<sTriangle_A> vec7;



// 	if (validChildren[0])
// 		vec0 = this->getChild(0)->sphereRecursion(sphRad, sphPos);
// 	if (validChildren[1])
// 		vec1 = this->getChild(1)->sphereRecursion(sphRad, sphPos);
// 	if (validChildren[2])
// 		vec2 = this->getChild(2)->sphereRecursion(sphRad, sphPos);
// 	if (validChildren[3])
// 		vec3 = this->getChild(3)->sphereRecursion(sphRad, sphPos);
// 	if (validChildren[4])
// 		vec4 = this->getChild(4)->sphereRecursion(sphRad, sphPos);
// 	if (validChildren[5])
// 		vec5 = this->getChild(5)->sphereRecursion(sphRad, sphPos);
// 	if (validChildren[6])
// 		vec6 = this->getChild(6)->sphereRecursion(sphRad, sphPos);
// 	if (validChildren[7])
// 		vec7 = this->getChild(7)->sphereRecursion(sphRad, sphPos);
	if (validChildren[0])
		vec0 = this->getChild(0)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);
	if (validChildren[1])
		vec1 = this->getChild(1)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);
	if (validChildren[2])
		vec2 = this->getChild(2)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);
	if (validChildren[3])
		vec3 = this->getChild(3)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);
	if (validChildren[4])
		vec4 = this->getChild(4)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);
	if (validChildren[5])
		vec5 = this->getChild(5)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);
	if (validChildren[6])
		vec6 = this->getChild(6)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);
	if (validChildren[7])
		vec7 = this->getChild(7)->sweepingSphereRecursion(sphRad, sphPos1, sphPos2);

	vec0 = triangleVectorUnion(vec0, vec1);
	vec2 = triangleVectorUnion(vec2, vec3);
	vec4 = triangleVectorUnion(vec4, vec5);
	vec6 = triangleVectorUnion(vec6, vec7);

	vec0 = triangleVectorUnion(vec0, vec2);
	vec4 = triangleVectorUnion(vec4, vec6);

	vec0 = triangleVectorUnion(vec0, vec4);




	// Cleanup
	//delete validChildren; 
	return vec0;
}

std::vector<sTriangle_A> cAABB::sweepingCapsuleCollision(sPhysicsProperties* pCapsule)
{
	std::vector<sTriangle_A> returnTris;

	// Start by checking if the moving capsule intersects the root AABB
	if (!cPhysics::m_IntersectMovingCapsuleAABB(pCapsule, this))
	{
		// Not in root AABB, return empty vector
		return returnTris;
	}


	// Assuming it is in the root box
	// Now we kick off the recursion


	// Calculate box made by capsule swept area for quick discard of child AABBs
	sCapsuleInfo newInfo;
	glm::vec3 currUpVec = pCapsule->upVec * ((sPhysicsProperties::sCapsule*)pCapsule->pShape)->halfLength;

	glm::vec3 oldUp = pCapsule->oldPosition + currUpVec;
	glm::vec3 newUp = pCapsule->oldPosition + currUpVec;
	glm::vec3 oldDown = pCapsule->oldPosition - currUpVec;
	glm::vec3 newDown = pCapsule->oldPosition - currUpVec;



	// TODO make it so a max vector is not re-checked for being a min
	newInfo.max.x = glm::max<float>(glm::max<float>(oldUp.x, newUp.x), glm::max<float>(oldDown.x, newDown.x));
	newInfo.max.y = glm::max<float>(glm::max<float>(oldUp.y, newUp.y), glm::max<float>(oldDown.y, newDown.y));
	newInfo.max.z = glm::max<float>(glm::max<float>(oldUp.z, newUp.z), glm::max<float>(oldDown.z, newDown.z));
	newInfo.min.x = glm::min<float>(glm::min<float>(oldUp.x, newUp.x), glm::min<float>(oldDown.x, newDown.x));
	newInfo.min.y = glm::min<float>(glm::min<float>(oldUp.y, newUp.y), glm::min<float>(oldDown.y, newDown.y));
	newInfo.min.z = glm::min<float>(glm::min<float>(oldUp.z, newUp.z), glm::min<float>(oldDown.z, newDown.z));





	//returnTris = this->sweepingCapsuleRecursion(radius, capsule->upVec, capsule->oldPosition, capsule->position);
	returnTris = this->sweepingCapsuleRecursion(newInfo, pCapsule);

	return returnTris;
}


//std::vector<sTriangle_A> cAABB::sweepingCapsuleRecursion(float capRad, glm::vec3 upVec, glm::vec3 capPos1, glm::vec3 capPos2)
std::vector<sTriangle_A> cAABB::sweepingCapsuleRecursion(sCapsuleInfo& capInfo, sPhysicsProperties* pCapsule)
{
	// Check if in leaf node
	if (this->getChild(0) == nullptr)
	{
		return triangles;
	}

	// Not a leaf

	
	bool validChildren[8] = { true, true, true, true, true, true, true, true };

	if (capInfo.min.y > this->Ycenter())
	{
		validChildren[0] = false;
		validChildren[2] = false;
		validChildren[4] = false;
		validChildren[6] = false;
	}
	else if (capInfo.max.y < this->Ycenter()) // If top of capsule is below center
	{
		validChildren[1] = false;
		validChildren[3] = false;
		validChildren[5] = false;
		validChildren[7] = false;
	}

	if (capInfo.min.x > this->Xcenter()) // If -x of capsule is +x of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[2] = false;
		validChildren[3] = false;
	}
	else if (capInfo.max.x < this->Xcenter()) // If +x of capsule is -x of center
	{
		validChildren[4] = false;
		validChildren[5] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}

	if (capInfo.min.z > this->Zcenter()) // If -z of capsule is +z of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}
	else if (capInfo.max.z < this->Zcenter()) // if +z of capsule is -z of center
	{
		validChildren[2] = false;
		validChildren[3] = false;
		validChildren[4] = false;
		validChildren[5] = false;
	}

	// Double check the remaining children with more scrutiny
	for (unsigned int i = 0; i < 8; i++)
	{
		if (validChildren[i])
		{
			if (!cPhysics::m_IntersectMovingCapsuleAABB(pCapsule, this)) validChildren[i] = false;
		}
	}


	// Now we know what children we need to traverse
	std::vector<sTriangle_A> vec0;
	std::vector<sTriangle_A> vec1;
	std::vector<sTriangle_A> vec2;
	std::vector<sTriangle_A> vec3;
	std::vector<sTriangle_A> vec4;
	std::vector<sTriangle_A> vec5;
	std::vector<sTriangle_A> vec6;
	std::vector<sTriangle_A> vec7;


	if (validChildren[0])
		vec0 = this->getChild(0)->sweepingCapsuleRecursion(capInfo, pCapsule);
	if (validChildren[1])
		vec1 = this->getChild(1)->sweepingCapsuleRecursion(capInfo, pCapsule);
	if (validChildren[2])
		vec2 = this->getChild(2)->sweepingCapsuleRecursion(capInfo, pCapsule);
	if (validChildren[3])
		vec3 = this->getChild(3)->sweepingCapsuleRecursion(capInfo, pCapsule);
	if (validChildren[4])
		vec4 = this->getChild(4)->sweepingCapsuleRecursion(capInfo, pCapsule);
	if (validChildren[5])
		vec5 = this->getChild(5)->sweepingCapsuleRecursion(capInfo, pCapsule);
	if (validChildren[6])
		vec6 = this->getChild(6)->sweepingCapsuleRecursion(capInfo, pCapsule);
	if (validChildren[7])
		vec7 = this->getChild(7)->sweepingCapsuleRecursion(capInfo, pCapsule);

	vec0 = triangleVectorUnion(vec0, vec1);
	vec2 = triangleVectorUnion(vec2, vec3);
	vec4 = triangleVectorUnion(vec4, vec5);
	vec6 = triangleVectorUnion(vec6, vec7);

	vec0 = triangleVectorUnion(vec0, vec2);
	vec4 = triangleVectorUnion(vec4, vec6);

	vec0 = triangleVectorUnion(vec0, vec4);



	return vec0;
}

// cPhysics::m_IntersectSegmentAABB()
std::vector<sTriangle_A> cAABB::lineSegmentCollision(glm::vec3& pos, glm::vec3& dir, float& len)
{
	std::vector<sTriangle_A> returnTris;

	glm::vec3 pos2 = pos + dir * len;

	if (!cPhysics::m_IntersectSegmentAABB(pos, pos2, *this))
	{
		return returnTris;
	}

	// Assuming in root box, commence recursion
	sLineSegmentInfo lineInfo;
	if (pos.x > pos2.x)
	{
		lineInfo.max.x = pos.x;
		lineInfo.min.x = pos2.x;
	}
	else
	{
		lineInfo.max.x = pos2.x;
		lineInfo.min.x = pos.x;
	}
	if (pos.y > pos2.y)
	{
		lineInfo.max.y = pos.y;
		lineInfo.min.y = pos2.y;
	}
	else
	{
		lineInfo.max.y = pos2.y;
		lineInfo.min.y = pos.y;
	}
	if (pos.z > pos2.z)
	{
		lineInfo.max.z = pos.z;
		lineInfo.min.z = pos2.z;
	}
	else
	{
		lineInfo.max.z = pos2.z;
		lineInfo.min.z = pos.z;
	}
	
	lineInfo.p1 = pos;
	lineInfo.p2 = pos2;


	returnTris = this->lineSegmentRecursion(lineInfo);

	return returnTris;
}

std::vector<sTriangle_A> cAABB::lineSegmentRecursion(sLineSegmentInfo& lineInfo)
{
	// Check if in leaf node
	if (this->getChild(0) == nullptr)
	{
		return triangles;
	}

	bool validChildren[8] = { true, true, true, true, true, true, true, true };

	if (lineInfo.min.y > this->Ycenter())
	{
		validChildren[0] = false;
		validChildren[2] = false;
		validChildren[4] = false;
		validChildren[6] = false;
	}
	else if (lineInfo.max.y < this->Ycenter()) // If top of capsule is below center
	{
		validChildren[1] = false;
		validChildren[3] = false;
		validChildren[5] = false;
		validChildren[7] = false;
	}

	if (lineInfo.min.x > this->Xcenter()) // If -x of capsule is +x of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[2] = false;
		validChildren[3] = false;
	}
	else if (lineInfo.max.x < this->Xcenter()) // If +x of capsule is -x of center
	{
		validChildren[4] = false;
		validChildren[5] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}

	if (lineInfo.min.z > this->Zcenter()) // If -z of capsule is +z of center
	{
		validChildren[0] = false;
		validChildren[1] = false;
		validChildren[6] = false;
		validChildren[7] = false;
	}
	else if (lineInfo.max.z < this->Zcenter()) // if +z of capsule is -z of center
	{
		validChildren[2] = false;
		validChildren[3] = false;
		validChildren[4] = false;
		validChildren[5] = false;
	}


	for (unsigned int i = 0; i < 8; i++)
	{
		if (validChildren[i])
		{
			//if (!cPhysics::m_IntersectMovingCapsuleAABB(pCapsule, this)) validChildren[i] = false;
			if (!cPhysics::m_IntersectSegmentAABB(lineInfo.p1, lineInfo.p2, *this)) validChildren[i] = false;
		}
	}


	// Now we know what children we need to traverse
	std::vector<sTriangle_A> vec0;
	std::vector<sTriangle_A> vec1;
	std::vector<sTriangle_A> vec2;
	std::vector<sTriangle_A> vec3;
	std::vector<sTriangle_A> vec4;
	std::vector<sTriangle_A> vec5;
	std::vector<sTriangle_A> vec6;
	std::vector<sTriangle_A> vec7;


	if (validChildren[0])
		vec0 = this->getChild(0)->lineSegmentRecursion(lineInfo);
	if (validChildren[1])
		vec1 = this->getChild(1)->lineSegmentRecursion(lineInfo);
	if (validChildren[2])
		vec2 = this->getChild(2)->lineSegmentRecursion(lineInfo);
	if (validChildren[3])
		vec3 = this->getChild(3)->lineSegmentRecursion(lineInfo);
	if (validChildren[4])
		vec4 = this->getChild(4)->lineSegmentRecursion(lineInfo);
	if (validChildren[5])
		vec5 = this->getChild(5)->lineSegmentRecursion(lineInfo);
	if (validChildren[6])
		vec6 = this->getChild(6)->lineSegmentRecursion(lineInfo);
	if (validChildren[7])
		vec7 = this->getChild(7)->lineSegmentRecursion(lineInfo);

	vec0 = triangleVectorUnion(vec0, vec1);
	vec2 = triangleVectorUnion(vec2, vec3);
	vec4 = triangleVectorUnion(vec4, vec5);
	vec6 = triangleVectorUnion(vec6, vec7);

	vec0 = triangleVectorUnion(vec0, vec2);
	vec4 = triangleVectorUnion(vec4, vec6);

	vec0 = triangleVectorUnion(vec0, vec4);



	return vec0;

}



// Helper function to combine 2 vectors, removing duplicates via their ID
std::vector<sTriangle_A> triangleVectorUnion(std::vector<sTriangle_A> vecA, std::vector<sTriangle_A> vecB)
{
	std::vector<sTriangle_A>::iterator itA = vecA.begin();
	std::vector<sTriangle_A>::iterator itB = vecB.begin();

	// TODO: Should check if one is empty, then immediately return the other. Saves on copying over an entire vector

	std::vector<sTriangle_A> returnVec;

	while ((itA != vecA.end()) && (itB != vecB.end()))
	{
		if (itA->ID < itB->ID)
		{
			returnVec.push_back(*itA);
			itA++;
		}
		else if (itA->ID > itB->ID)
		{
			returnVec.push_back(*itB);
			itB++;
		}
		else // Duplicate found
		{
			returnVec.push_back(*itA);
			itA++;
			itB++;
		}
	}

	// Add the rest of the remaining vector
	while (itA != vecA.end())
	{
		returnVec.push_back(*itA);
		itA++;
	}
	while (itB != vecB.end())
	{
		returnVec.push_back(*itB);
		itB++;
	}

	return returnVec;
}

