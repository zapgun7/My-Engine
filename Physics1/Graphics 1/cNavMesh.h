#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "Other Graphics Stuff/cMesh.h"
#include <unordered_map>

#include <random>

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

		                // id             // depth
		std::unordered_map<int, std::pair< int, sNavTri*>> map_targetIDtoNearestTri; // Given a target ID, will return the closest triangle (within predetermined distance)

		unsigned int id;
	};

	void Initialize(std::vector<cMesh*> meshes); // Load up the nav mesh given the meshes provided


	sNavTri* getClosestTri(glm::vec3 pos); // Costly, scans through all tris to find the on the entity's closest to

	sNavTri* getClosestTri(sNavTri* currTri, glm::vec3 pos); // Checks if pos is on the provided tri, then checks adjacent (out a certain depth)
	sNavTri* getClosestTriEnemy(sNavTri* currTri, glm::vec3 pos);

	sNavTri* findPathToTargetTri(sNavTri* currTri, sNavTri* targetTri);

	




private:

	// Loads provided vec3 array with transformed versions
	void MakeTransformedMesh(cMesh* mesh, sNavTri* newTri, sNavTri* newTri2);

	// Attempts to connect the new triangle with other existing triangles
	void MakeConnections(sNavTri* newTri);

	// Returns true of both points are within distance tolerance
	bool CompareVerts(glm::vec3 v1, glm::vec3 v2);

	void GenerateNearMap(void); // Generates a lookup table for each triangle to get to other ones (only within a certain distance)
	void MapGenRecursion(sNavTri* originTri, sNavTri* originDirTri, sNavTri* nextTri, int depth);




	std::vector<sNavTri*> m_vecFullNavMesh; // Vector of all sNavTri's

	// Going to hard code the vertices and stuff of the default mesh
	std::vector<glm::vec3> m_vecVertices;
	std::vector<unsigned int> m_vecIndices;

	float m_distTolerance;
	unsigned int m_LocalSearchDepth;
	unsigned int m_NextID;

	unsigned int m_MapDepth;
};