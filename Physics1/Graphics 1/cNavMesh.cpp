#include "cNavMesh.h"


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
	m_distTolerance = 3.0f;

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
}

cNavMesh::sNavTri* cNavMesh::getClosestTri(glm::vec3 pos)
{
	for (sNavTri* currTri : m_vecFullNavMesh)
	{
		if (!isAboveTri(currTri, pos.y)) // Check if position is above at least one vertex
			continue;

		// Project pos onto tri-plane


		// Then call a get closest point on tri

	}
	return nullptr;
}
// Helper function for above
bool cNavMesh::isAboveTri(sNavTri* tri, float yPos)
{
	for (int i = 0; i < 3; i++)
	{
		if (tri->vertices[i].y < yPos) return true;
	}
	return false;
}

cNavMesh::sNavTri* cNavMesh::getClosestTri(sNavTri currTri, glm::vec3 pos)
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
	newTri1->centreTri = (newTri1->vertices[0] + newTri1->vertices[1] + newTri1->vertices[2]) / 2.0f;
	newTri2->centreTri = (newTri2->vertices[0] + newTri2->vertices[1] + newTri2->vertices[2]) / 2.0f;

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



