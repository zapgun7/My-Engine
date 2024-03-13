#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <vector>

#include "../../cVAOManager/sModelDrawInfo.h"

class cSoftBodyVerlet
{
public:
	cSoftBodyVerlet();
	~cSoftBodyVerlet();

	struct sParticle
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 old_position = glm::vec3(0.0f);
		// Pointer back to the model vertex info
		sVertex* pModelVertex = NULL;
		std::vector<sParticle*> neighbours;
		bool isStatic = false; // true = un-moving anchor
		bool isPartModel = true; // If we need to update vertex position
	};

	struct sConstraint
	{
		enum eRelationType
		{
			REGULAR, // Pushes and pulls to achieve rest length
			PULL, // Only satisfies constraint when dist farther than rest length
			PUSH // Only satisfies when dist is shorter than rest length
		};

		sConstraint() {}
		sParticle* pParticleA = NULL;
		sParticle* pParticleB = NULL;

		// How far away they are supposed to be 'at rest'
		float restLength = 0.0f;

		unsigned int numIterations = 1;
		float tightFact = 1.0f;

		eRelationType relationType = REGULAR;

		// if false, this isnt' checked
		// Like if the constraint is 'broken'
		bool bIsActive = true;
		// This is interesting, too
		// From: I Spent a Week Making an AI's Video Game Idea - YouTube
		// https://www.youtube.com/watch?v=PGk0rnyTa1U&ab_channel=SebastianLague
		// He's using it as to fix the end of the constraint
		bool bIsLocked = false;


	};


	// This is for loading the original model
	// The 2nd param is the identy matrix 
	bool CreateSoftBody(sModelDrawInfo ModelDrawInfo, glm::mat4 matInitalTransform = glm::mat4(1.0f));

	// Will update the vertex information to match the particles
	void UpdateVertexPositions(void);

	void UpdateNormals(void);


	// Create random constraints within the object to 'brace' the shape
	// These are invisible, though
	void CreateRandomBracing(unsigned int numberOfBraces,
							float minDistanceBetweenVertices);
	void CreateRandomBracing(unsigned int numberOfBraces,
							 float minDistanceBetweenVertices,
							float constraintTightness,
							float addRestDist);

	void BuildPlatform(void); // Adds invisible ropes to the 4 highest points of the model   !!! Make sure to CreateSoftBody() first
	void AddRopeAttachment(sParticle* partToRope, float tightness);

	void Jump(double& deltaTime);

	void Move(glm::vec3& dir, double& deltaTime);

	// This will update the draw info vertex information from the 
	//	soft body simulation state (at the current moment)
	bool UpdateDrawInfo(void);

	void VerletUpdate(double deltaTime);
	// This is for the ground, hitting something, etc.
	void ApplyCollision(double deltaTime);

	void SatisfyConstraints(void);

	

	void cleanZeros(glm::vec3& value);


	float calcDistanceBetween(sParticle* pPartA, sParticle* pPartB);

	// These are the "rods" or "springs" between vertices
	// We get these from the mesh, BUT we might need additional
	//	constraints to make sure this stays in the desired shape
	

	// Force, like gravity or whatever
	glm::vec3 acceleration = glm::vec3(0.0f);

	float tightnessFactor = 1.0f;
	unsigned int iterations = 1;

	// For the player
	bool canJump = false;


	std::vector< sParticle* > vec_pParticles;
	std::vector< sConstraint* > vec_pConstraints;

	// Make this private...
	sModelDrawInfo m_ModelVertexInfo;

	// This method is public and has THE SAME critical section
	//	we have in the Update vertex and normal code
	sModelDrawInfo* pGetModelDrawInfo(void);

	// Max update time is 60Hz
	const double MAX_DELTATIME = 1.0 / 60.0;



	// HACK:
	// Some functions that we likely should figure out before hand...
	glm::vec3 getCentrePoint(void);

	// For rotating the wheel, the 'Wheel_15' wheel is around the x-axis
	// So we are looking for two vertices that have Z and Y ~ 0.0f
	// This will return a vector of vertex indices that are along this axis
	void findCentreVerticesOfWheel(std::vector< unsigned int >& particleIndices);

	// Creates constraints from the other vertices to the edges of the wheel
	void CreateWheelBracing(void);

};

