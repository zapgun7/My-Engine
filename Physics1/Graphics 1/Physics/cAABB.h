#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <math.h>
#include <map>
#include <string>
#include <vector>
#include "sTriangle.h"

// These would also store triangle info, 
//	either the actual vertices of the triangle, or the index to the triangle...
//struct sTriangle_A;
class cVAOManager;
class sPhysicsProperties;


class cAABB
{
public:
	unsigned int uniqueID;

	// Storing Center and half lengths
	// I think this would make oct tree traversal easier? whatever
	glm::vec3 centerPosition;
	glm::vec3 halfLengths;


	// Extremity value getters
	float xMax(void)
	{
		return this->centerPosition.x + this->halfLengths.x;
	}
	float xMin(void)
	{
		return this->centerPosition.x - this->halfLengths.x;
	}
	float yMax(void)
	{
		return this->centerPosition.y + this->halfLengths.y;
	}
	float yMin(void)
	{
		return this->centerPosition.y - this->halfLengths.y;
	}
	float zMax(void)
	{
		return this->centerPosition.z + this->halfLengths.z;
	}
	float zMin(void)
	{
		return this->centerPosition.z - this->halfLengths.z;
	}
	glm::vec3 XYZMax(void)
	{
		return this->centerPosition + this->halfLengths;
	}
	glm::vec3 XYZMin(void)
	{
		return this->centerPosition - this->halfLengths;
	}

	// Other Getters
	glm::vec3 XYZcenter(void)
	{
		return this->centerPosition;
	}
	float Xcenter(void)
	{
		return this->centerPosition.x;
	}
	float Ycenter(void)
	{
		return this->centerPosition.y;
	}
	float Zcenter(void)
	{
		return this->centerPosition.z;
	}

	// Subdivide Functions
	bool StartMakeTree(std::string modelName, cVAOManager* borrowedManager, unsigned int maxTri); // This will determine the size of the root AABB and generate the vector of all triangles
	void MakeTree(std::vector<sTriangle_A>* parentTris, unsigned int maxTri, bool isRoot);
	std::vector<sTriangle_A> trisInBox(std::vector<sTriangle_A>* triangles, glm::vec3 minCorner, glm::vec3 maxCorner);

	// Traversal Functions
	std::vector<sTriangle_A> sphereCollision(sPhysicsProperties* sphere);// Takes a sphere with reverse transform matrix of the AABB object applied to it; returns vector of near triangles
	std::vector<sTriangle_A> sphereRecursion(float sphRad, glm::vec3 sphPos);
	cAABB* getChild(unsigned int ID);

	std::vector<sTriangle_A> triangles; // Triangles in this box; only leafs will have data here

	std::map< unsigned int, cAABB* > mapChild_pAABBs; // 8 Child AABB's


};


