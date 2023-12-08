#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <math.h>
#include <map>
#include <string>
#include <vector>

// These would also store triangle info, 
//	either the actual vertices of the triangle, or the index to the triangle...
struct sTriangle_A;


class cAABB
{
public:
	unsigned int uniqueID;

	// Storing Center and half lengths
	// I think this would make oct tree traversal easier? whatever
	glm::vec3 centerPosition;
	glm::vec3 halfLengths;


	// Extremity value getters
	float xMax();
	float xMin();
	float yMax();
	float yMin();
	float zMax();
	float zMin();
	glm::vec3 XYZMax();
	glm::vec3 XYZMin();

	// Other Getters
	glm::vec3 XYZcenter();
	float Xcenter();
	float Ycenter();
	float Zcenter();

	// Subdivide Functions
	void MakeTree(std::vector<sTriangle_A>* parentTris, unsigned int maxTri);
	std::vector<sTriangle_A> trisInBox(std::vector<sTriangle_A>* triangles, glm::vec3 minCorner, glm::vec3 maxCorner);


	std::map< unsigned int, cAABB* > vecChild_pAABBs; // 8 Child AABB's


};


