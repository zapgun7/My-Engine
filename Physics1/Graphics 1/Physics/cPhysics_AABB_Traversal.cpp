// This is a class for retrieving relevant triangles in the oct-tree for various object types

#include "cAABB.h"
#include "sPhysicsProperties.h"
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

// 	cAABB* temp[8];
// 	for (int i = 0; i < 8; i++)
// 	{
// 		temp[i] = this->getChild(i);
// 	}
// 	for (int i = 0; i < 8; i++)
// 	{
// 		if (sphPos.x - sphRad > temp[i]->xMax())
// 		{
// 			validChildren[i] = false;
// 			continue;
// 		}
// 		if (sphPos.x + sphRad < temp[i]->xMin())
// 		{
// 			validChildren[i] = false;
// 			continue;
// 		}
// 
// 		if (sphPos.z - sphRad > temp[i]->zMax()) 
// 		{
// 			validChildren[i] = false;
// 			continue;
// 		}
// 		if (sphPos.z + sphRad < temp[i]->zMin()) 
// 		{
// 			validChildren[i] = false;
// 			continue;
// 		}
// 
// 		if (sphPos.y - sphRad > temp[i]->yMax())
// 		{
// 			validChildren[i] = false;
// 			continue; // Check y last cause I think the other two are more likely?
// 		}
// 		if (sphPos.y + sphRad < temp[i]->yMin()) 
// 		{
// 			validChildren[i] = false;
// 			continue;
// 		}
// 	}

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

// Helper function to combine 2 vectors, removing duplicates via their ID
std::vector<sTriangle_A> triangleVectorUnion(std::vector<sTriangle_A> vecA, std::vector<sTriangle_A> vecB)
{
	std::vector<sTriangle_A>::iterator itA = vecA.begin();
	std::vector<sTriangle_A>::iterator itB = vecB.begin();

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