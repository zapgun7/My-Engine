#ifndef _cPhysics_HG_
#define _cPhysics_HG_

// This manages all the physics stuff
#include "sPhysicsProperties.h"
#include "../cVAOManager/cVAOManager.h"		// For the mesh indirect
#include <vector>
#include "../Other Graphics Stuff/cGraphicsMain.h"
#include "cAABB.h"

//struct sPossibleCollision;


class cPhysics
{
private:
	struct sPossibleCollision
	{
		sPhysicsProperties* collisionObject; // For future detecting what properties apply to collision (surface restitution
		glm::vec3 hitNorm;
		float q = FLT_MAX; // Ratio of hit over update window (0 = start of window; 1 = end of window)
	};

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
	bool m_Sphere_TriMeshIndirect_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriMesh, sPossibleCollision& returnCollision);
	bool m_Sphere_TriMeshLocal_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriMesh);

	bool m_Capsule_TriMeshIndirect_IntersectionTest(sPhysicsProperties* pCapsule, sPhysicsProperties* pTriMesh, sPossibleCollision& returnCollision);


	// Collision Handling
	void m_Sphere_Collision(sPhysicsProperties* pSphere, sPossibleCollision& collision);
	void m_Capsule_Collision(sPhysicsProperties* pCapsule, sPossibleCollision& collision);

	// Oct-Tree AABBs
	std::map<std::string, cAABB*> m_map_ModelAABBs; // Map of AABB oct-treeified models
	cAABB* findAABBByModelName(std::string modelName);

public:
	// The basic tests
	static bool m_TestSphereTriangle(float sphereRadius, glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 sphereCentre);
	static int m_TestSphereAABB(sPhysicsProperties* pSphere, cAABB* b, glm::vec3& q);

	static glm::vec3 m_ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
	static glm::vec3 m_ClosestPtTriPlanePointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
	static float m_ClosestPtSegmentSegment(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2, float& s, float& t, glm::vec3& c1, glm::vec3& c2);
	static glm::vec3 m_ClosestPtLineSegTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b, glm::vec3 c);
	static void m_ClosestPtPointAABB(glm::vec3 p, cAABB* b, glm::vec3& q);
	static void m_ClosestPtSegmentAABB(glm::vec3& p1, glm::vec3& p2, cAABB& b, glm::vec3& q); // Closest point on segment

	static int m_IntersectMovingSpherePlane(sPhysicsProperties* pSphere, glm::vec3 pn, float pd, float& t, glm::vec3& q);
	static int m_IntersectRaySphere(glm::vec3 p, glm::vec3 d, sPhysicsProperties* pSphere, float& t, glm::vec3& q);
	static int m_IntersectRayAABB(glm::vec3 p, glm::vec3 d, cAABB* a, float& tmin, glm::vec3& q);
	static int m_IntersectMovingSphereAABB(sPhysicsProperties* pSphere, cAABB* b, float& t); // TODO !!!INCOMPLETE!!!      Needs finer check for vertex regions
	static int m_IntersectMovingCapsuleAABB(sPhysicsProperties* pCapsule, cAABB* b); // TODO !!!INCOMPLETE!!! misses corners of boxes swiping the capsule mid-section
	static int m_IntersectSegmentAABB(glm::vec3& p0, glm::vec3& p1, cAABB& b);

	// Less Basic Tests
	static bool m_TestMovingSphereTriangle(sPhysicsProperties* pSphere, sTriangle_A* pTri, float &t, glm::vec3 &hitNorm);
	static bool m_TestMovingCapsuleTriangle(sPhysicsProperties* pCapsule, sTriangle_A* pTri, float& t, glm::vec3& hitNorm);

private:

	cVAOManager* m_pMeshManager = NULL;
	//cGraphicsMain* m_pGraphicsMain;

	bool m_IsRunning = false; // If physics updates should be calculated. Pausing good for moving stuff around in editor then resuming physics
	glm::vec3 m_WorldGravity = glm::vec3(0, -20.0f, 0);
	

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