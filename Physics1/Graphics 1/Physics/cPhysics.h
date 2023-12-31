#ifndef _cPhysics_HG_
#define _cPhysics_HG_

// This manages all the physics stuff
#include "sPhysicsProperties.h"
#include "../cVAOManager/cVAOManager.h"		// For the mesh indirect
#include <vector>
#include "../Other Graphics Stuff/cGraphicsMain.h"
#include "cAABB.h"


class cPhysics
{
public:
	cPhysics();
	~cPhysics();

	void setVAOManager(cVAOManager* pTheMeshManager);
	void generateAABBs(std::vector<std::string> models); // Will generate an AABB oct-tree for all models

	// Once you pass this in, the cPhysics handles the lifetime
	// (i.e. IT will call delete)
	// When added, if a mesh is present, the drawPosition, etc. are set
	void AddShape(sPhysicsProperties* pNewShape);
	void DeleteShape(int shapeID); // Deletes the physics shape
	std::vector< sPhysicsProperties* > getPhysicsVec(void);
	//void setGraphics(void); // Lazy

	void Update(double deltaTime);

	/////////////// LEVEL EDITOR //////////////////////

	/// Data for the editor
	void getPhysObj(int objID, sPhysicsProperties* theObj); // Unused

	/// Updating values for the editor  //////  !!! These will only be called when the physics simulation is paused, and only from the editor!
	void setShapePos(glm::vec3 newPos, unsigned int ID);
	void setShapeOri(glm::vec3 newOri, unsigned int ID);
	void setShapePori(glm::vec3 newPos, glm::vec3 newOri, unsigned int ID);
	void setPhysicsRunningState(bool isRunning); // Update whether the physics simulation is paused

	void deleteAllObjects(void); // Used in loading new scenes
	void switchScenes(std::vector<sPhysicsProperties*> newPhysVec);

	////////////////////////////////////////

	// returns NULL if not found
	sPhysicsProperties* findShapeByUniqueID(unsigned int uniqueIDtoFind);
	sPhysicsProperties* findShapeByFriendlyName(std::string friendlyNameToFind);

	// Vector is empty if none found
	std::vector<sPhysicsProperties*> findShapesByType(sPhysicsProperties::eShape shapeType);
	// Returns false if none found
	bool findShapesByType(sPhysicsProperties::eShape shapeType, std::vector<sPhysicsProperties*> &vecShapes);

	// There's a ray cast code in chapter 5 of Ericson's book.
	// What does this return? 
	// It could return all the things that the ray hit.
	// You could add some methods that only ray cast through specific objects (like only meshes)
	// You could also do something where it returns the "closest" object (maybe form the startXYZ)
	bool rayCast(glm::vec3 startXYZ, glm::vec3 endXYZ, std::vector<sPhysicsProperties*>& vecObjectsHit);

private:
	std::vector< sPhysicsProperties* > m_vec_pPhysicalProps;
	
	bool m_Sphere_Sphere_IntersectionTest(sPhysicsProperties* pSphereA, sPhysicsProperties* pShpereB);
	bool m_Sphere_Plane_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pPlane);
	bool m_Sphere_Triangle_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriangle);
	bool m_Sphere_AABB_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pAABB);
	bool m_Sphere_Capsule_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pCapsule);
	bool m_Sphere_TriMeshIndirect_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriMesh);
	bool m_Sphere_TriMeshLocal_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriMesh);

	// Oct-Tree AABBs
	std::map<std::string, cAABB*> m_map_ModelAABBs; // Map of AABB oct-treeified models
	cAABB* findAABBByModelName(std::string modelName);

	// The basic tests
	bool m_TestSphereTriangle(float sphereRadius, glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 sphereCentre);
	glm::vec3 m_ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
	float m_ClosestPtSegmentSegment(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2, float& s, float& t, glm::vec3& c1, glm::vec3& c2);
	glm::vec3 m_ClosestPtLineSegTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b, glm::vec3 c);

	cVAOManager* m_pMeshManager = NULL;
	//cGraphicsMain* m_pGraphicsMain;

	bool m_IsRunning = false; // If physics updates should be calculated. Pausing good for moving stuff around in editor then resuming physics

	

	// this is a list of all the objects that collided this past frame
	struct sCollisionEvent
	{
		sPhysicsProperties* pObjectA;		// Sphere
		sPhysicsProperties* pObjectB;		// Triangle
		// .. And other information we might need
		glm::vec3 contactPoint;
		glm::vec3 velocityAtCollision;
		glm::vec3 reflectionNormal;

	};
	std::vector< sCollisionEvent > m_vecCollisionsThisFrame;

	bool m_CheckExistingCollision(sPhysicsProperties* objA, sPhysicsProperties* objB); // Returns sCollisionEvent object if the objects have collided before
};

#endif // _cPhysics_HG_