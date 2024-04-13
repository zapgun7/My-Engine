#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "Other Graphics Stuff/cMesh.h"


class cNavMesh
{
public:
	cNavMesh();

	struct sNavTri
	{
		sNavTri()
		{
			vertices = new glm::vec3[3];
		}
		glm::vec3 centreTri;
		glm::vec3* vertices;
		glm::vec3 normal;
		float pd;
		std::vector<sNavTri*> adjacentTris;

		unsigned int id;
	};

	void Initialize(std::vector<cMesh*> meshes); // Load up the nav mesh given the meshes provided


	sNavTri* getClosestTri(glm::vec3 pos); // Costly, scans through all tris to find the on the entity's closest to

	sNavTri* getClosestTri(sNavTri* currTri, glm::vec3 pos); // Checks if pos is on the provided tri, then checks adjacent (out a certain depth)

	sNavTri* getClosestTriToTri(sNavTri* currTri, sNavTri* targetTri);



private:

	// Loads provided vec3 array with transformed versions
	void MakeTransformedMesh(cMesh* mesh, sNavTri* newTri, sNavTri* newTri2);

	// Attempts to connect the new triangle with other existing triangles
	void MakeConnections(sNavTri* newTri);

	// Returns true of both points are within distance tolerance
	bool CompareVerts(glm::vec3 v1, glm::vec3 v2);





	std::vector<sNavTri*> m_vecFullNavMesh; // Vector of all sNavTri's

	// Going to hard code the vertices and stuff of the default mesh
	std::vector<glm::vec3> m_vecVertices;
	std::vector<unsigned int> m_vecIndices;

	float m_distTolerance;
	unsigned int m_LocalSearchDepth;
	unsigned int m_NextID;
};