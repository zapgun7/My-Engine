#include "cPhysics.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 

#include <iostream>


void cPhysics::m_Sphere_Collision(sPhysicsProperties* pSphere, sPossibleCollision& collision)
{
	if (collision.q == 0) // In or already touching triangle at start of update
	{
		glm::vec3 oldDeltaMove = pSphere->position - pSphere->oldPosition;


		glm::vec3 projSubVec = glm::dot(oldDeltaMove, collision.hitNorm) * collision.hitNorm;
		pSphere->position = pSphere->oldPosition + (oldDeltaMove - projSubVec);


		projSubVec = glm::dot(pSphere->velocity, collision.hitNorm) * collision.hitNorm;
		pSphere->velocity -= projSubVec;

		// I think this is it???
	}
	else // Regular triangle hit beyond the start and before the end of the update window
	{
		glm::vec3 sphereDirection = glm::normalize(pSphere->velocity);
		glm::vec3 sphereStep = pSphere->position - pSphere->oldPosition;


		glm::vec3 reflectionVec = glm::reflect(sphereDirection, collision.hitNorm);

		float sphereSpeed = glm::length(pSphere->velocity);
		pSphere->velocity = reflectionVec * sphereSpeed;

		// TODO change this to radians 
		// Degree between sphere dir and hit norm: 180 is absolute restitution application, 90 is none (parallel to it)
		float degToNorm = glm::degrees(abs(acos(glm::dot(sphereDirection, collision.hitNorm) / glm::length(sphereDirection) * glm::length(collision.hitNorm))));

		float restAppDegree = degToNorm - 90;
		restAppDegree /= 90;

		glm::vec3 restitutionVelLoss = -collision.hitNorm * pSphere->velocity; // Calculate vector we want to reduce velocity on (negative normal of surface it's bouncing on)
		pSphere->velocity += (restitutionVelLoss * (1.0f - pSphere->restitution)) * restAppDegree; // Subtract said vector from newVelocity, scaled with its restitution (0 restitution = no bounce, 1 = full bounce)


		// Update oldPosition to point of collision
		pSphere->oldPosition += sphereStep * collision.q;

		// Update new current position
		float remainingLength = glm::length(sphereStep * (1.0f - collision.q));
		pSphere->position = pSphere->oldPosition
			+ reflectionVec			// Direction it bounces (unit vec)
			* remainingLength		// Length of remaining distance it had to cover past the collision point
			* pSphere->restitution; // Throttles distance by its restitution
		//// TODO^ this restitution doesn't make sense, should vary based on angle bouncing 

		// Done... I think
	}

	return;
}



// The same as sphere collision handling, for now
void cPhysics::m_Capsule_Collision(sPhysicsProperties* pCapsule, sPossibleCollision& collision)
{
	if (collision.q == 0) // In or already touching triangle at start of update
	{
		glm::vec3 oldDeltaMove = pCapsule->position - pCapsule->oldPosition;


		glm::vec3 projSubVec = glm::dot(oldDeltaMove, collision.hitNorm) * collision.hitNorm;
		pCapsule->position = pCapsule->oldPosition + (oldDeltaMove - projSubVec);


		projSubVec = glm::dot(pCapsule->velocity, collision.hitNorm) * collision.hitNorm;
		pCapsule->velocity -= projSubVec;

		// I think this is it???
	}
	else // Regular triangle hit beyond the start and before the end of the update window
	{
		glm::vec3 sphereDirection = glm::normalize(pCapsule->velocity);
		glm::vec3 sphereStep = pCapsule->position - pCapsule->oldPosition;


		glm::vec3 reflectionVec = glm::reflect(sphereDirection, collision.hitNorm);

		float sphereSpeed = glm::length(pCapsule->velocity);
		pCapsule->velocity = reflectionVec * sphereSpeed;

		// TODO change this to radians 
		// Degree between sphere dir and hit norm: 180 is absolute restitution application, 90 is none (parallel to it)
		float degToNorm = glm::degrees(abs(acos(glm::dot(sphereDirection, collision.hitNorm) / glm::length(sphereDirection) * glm::length(collision.hitNorm))));

		float restAppDegree = degToNorm - 90;
		restAppDegree /= 90;

		glm::vec3 restitutionVelLoss = -collision.hitNorm * pCapsule->velocity; // Calculate vector we want to reduce velocity on (negative normal of surface it's bouncing on)
		pCapsule->velocity += (restitutionVelLoss * (1.0f - pCapsule->restitution)) * restAppDegree; // Subtract said vector from newVelocity, scaled with its restitution (0 restitution = no bounce, 1 = full bounce)


		// Update oldPosition to point of collision
		pCapsule->oldPosition += sphereStep * collision.q;

		// Update new current position
		float remainingLength = glm::length(sphereStep * (1.0f - collision.q));
		pCapsule->position = pCapsule->oldPosition
			+ reflectionVec			// Direction it bounces (unit vec)
			* remainingLength		// Length of remaining distance it had to cover past the collision point
			* pCapsule->restitution; // Throttles distance by its restitution
		//// TODO^ this restitution doesn't make sense, should vary based on angle bouncing 

		// Done... I think
	}

	return;
}