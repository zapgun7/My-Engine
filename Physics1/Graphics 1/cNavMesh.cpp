#include "cNavMesh.h"

#include "Physics/cPhysics.h" // For static functions

cNavMesh::cNavMesh()
{
	// Generate the default ground mesh data
	m_vecVertices.reserve(4);
	m_vecVertices.push_back(glm::vec3(50, 0, 50));
	m_vecVertices.push_back(glm::vec3(-50, 0, 50));
	m_vecVertices.push_back(glm::vec3(50, 0, -50));
	m_vecVertices.push_back(glm::vec3(-50, 0, -50));

	m_vecIndices.reserve(6);
	m_vecIndices.push_back(3);
	m_vecIndices.push_back(1);
	m_vecIndices.push_back(0);
	m_vecIndices.push_back(0);
	m_vecIndices.push_back(2);
	m_vecIndices.push_back(3);


	// Max dist for tris to connect
	m_distTolerance = 0.5f;
	m_LocalSearchDepth = 2; // Searches max 2 triangles away

	m_NextID = 0;
}


// Helper Funcitons
bool cNavMesh::CompareVerts(glm::vec3 v1, glm::vec3 v2)
{
	if (glm::distance(v1, v2) <= m_distTolerance)
		return true;
	return false;
}


// builds the mesh from the transformations provided by the cMesh vector
void cNavMesh::Initialize(std::vector<cMesh*> meshes) // These should all be Flat_1x1_plane.ply's
{
	m_vecFullNavMesh.reserve(meshes.size() * 2);

	for (cMesh* currMesh : meshes)
	{
		sNavTri* newTri1 = new sNavTri();
		sNavTri* newTri2 = new sNavTri();
		MakeTransformedMesh(currMesh, newTri1, newTri2);

		MakeConnections(newTri1);
		m_vecFullNavMesh.push_back(newTri1);
		MakeConnections(newTri2);
		m_vecFullNavMesh.push_back(newTri2);
	}
	printf("Done generating NavMesh\n");
}

cNavMesh::sNavTri* cNavMesh::getClosestTri(glm::vec3 pos)
{
	float nearestTriDist = FLT_MAX;
	sNavTri* nearestTri = nullptr;

	for (sNavTri* currTri : m_vecFullNavMesh)
	{
		// Start by making sure the position is on the correct (above) side of the plane, only want to check tris below the pos
		float planeSide = glm::dot(pos, currTri->normal) + currTri->pd; // Will be positive if on side normal is pointing to

		// Skip tri if under 
		if (planeSide < 0) continue;
		
		// Get closest point on triangle, and get distance to it
		glm::vec3 closestPoint = cPhysics::m_ClosestPtPointTriangle(pos, currTri->vertices[0], currTri->vertices[1], currTri->vertices[2]);

		float currTriDist = glm::distance(closestPoint, pos);

		if (currTriDist < nearestTriDist)
		{
			nearestTriDist = currTriDist;
			nearestTri = currTri;
		}
	}


	return nearestTri;
}

// Grabs closest tri with reference of a tri (usually the one the entity was last confirmed on)
// Way quicker than above, as it only searches a certain depth from the provided tri
cNavMesh::sNavTri* cNavMesh::getClosestTri(sNavTri* prevTri, glm::vec3 pos)
{
	// Start by adding base "currTri" as a statistic
	sNavTri* nearestTri = prevTri;
	float nearestTriDist = glm::distance(pos, cPhysics::m_ClosestPtPointTriangle(pos, prevTri->vertices[0], prevTri->vertices[1], prevTri->vertices[2]));

	// TODO project pos onto combined normals of two neighboring tris and then call a dist to each from that
	for (sNavTri* currTri : prevTri->adjacentTris)
	{
		float planeSide = glm::dot(pos, currTri->normal) + currTri->pd; // Will be positive if on side normal is pointing to

		// Skip tri if under 
		if (planeSide < 0) continue;

		// Get closest point on triangle, and get distance to it
		glm::vec3 closestPoint = cPhysics::m_ClosestPtPointTriangle(pos, currTri->vertices[0], currTri->vertices[1], currTri->vertices[2]);
		float tridist = glm::distance(closestPoint, pos);
		if (tridist < nearestTriDist)
		{
			nearestTriDist = tridist;
			nearestTri = currTri;
		}
	}

	return nearestTri;
}



cNavMesh::sNavTri* cNavMesh::getClosestTriToTri(sNavTri* currTri, sNavTri* targetTri)
{
	return nullptr;
}

void cNavMesh::MakeTransformedMesh(cMesh* mesh, sNavTri* newTri1, sNavTri* newTri2)
{
	glm::mat4 matModel(1.0f);
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
									mesh->scale);
	glm::mat4 matRotation = glm::mat4(mesh->get_qOrientation());
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
											glm::vec3(mesh->drawPosition.x,
											mesh->drawPosition.y,
											mesh->drawPosition.z));
	matModel *= matTranslate;
	matModel *= matRotation;// matModel is what we convert out vertices with
	matModel *= matScale;

	// Generate the transformed vertices
	for (unsigned int i = 0; i < 3; i++) // Tri #1
	{
		newTri1->vertices[i] = glm::vec3(matModel * glm::vec4(m_vecVertices[m_vecIndices[i]], 1.0f));
	}
	for (unsigned int i = 3; i < 6; i++) // Tri #2
	{
		newTri2->vertices[i - 3] = glm::vec3(matModel * glm::vec4(m_vecVertices[m_vecIndices[i]], 1.0f));
	}


	// Set new id's
	newTri1->id = m_NextID++;
	newTri2->id = m_NextID++;


	// Calculate normals for each
	glm::vec3 ab = newTri1->vertices[1] - newTri1->vertices[0];
	glm::vec3 ac = newTri1->vertices[2] - newTri1->vertices[0];
	newTri1->normal = glm::normalize(glm::cross(ab, ac));

	ab = newTri2->vertices[1] - newTri2->vertices[0];
	ac = newTri2->vertices[2] - newTri2->vertices[0];
	newTri2->normal = glm::normalize(glm::cross(ab, ac));

	// Calculate centres for both
	newTri1->centreTri = (newTri1->vertices[0] + newTri1->vertices[1] + newTri1->vertices[2]) / 3.0f;
	newTri2->centreTri = (newTri2->vertices[0] + newTri2->vertices[1] + newTri2->vertices[2]) / 3.0f;


	// Calculate pd's for both
	newTri1->pd = glm::dot(newTri1->normal, newTri1->vertices[0]);
	newTri2->pd = glm::dot(newTri2->normal, newTri2->vertices[0]);

	return;
}

void cNavMesh::MakeConnections(sNavTri* newTri)
{
	for (sNavTri* currTri : m_vecFullNavMesh) // Go through all meshes
	{
		int connectionCount = 0;
		for (unsigned int triAVertIDX = 0; triAVertIDX < 3; triAVertIDX++)
		{
			for (unsigned int triBVertIDX = 0; triBVertIDX < 3; triBVertIDX++)
			{
				if (CompareVerts(newTri->vertices[triAVertIDX], currTri->vertices[triBVertIDX]))
				{
					connectionCount++;
				}
				if (connectionCount > 1) break;
			}
			if (connectionCount > 1) break;
		}


		/// CONNECT TRIANGLES ///
		if (connectionCount > 1)
		{
			// Connect triangles!
			newTri->adjacentTris.push_back(currTri);
			currTri->adjacentTris.push_back(newTri);
		}


	}
}



