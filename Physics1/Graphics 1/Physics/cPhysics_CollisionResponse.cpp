#include "cPhysics.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 

#include <iostream>


void cPhysics::m_Sphere_Collision(sPhysicsProperties* pSphere, sPossibleCollision& collision)
{
	//std::cout << pSphere->position.y << "      " << pSphere->oldPosition.y << std::endl;
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

		// !!!!! IF SOMETHING IS BROKEN IT MIGHT BE THE BELOW LINE (fixed something in capsule so I changed it here!)

		glm::vec3 restitutionVelLoss = -collision.hitNorm * glm::vec3(abs(pSphere->velocity.x), abs(pSphere->velocity.y), abs(pSphere->velocity.z));//pSphere->velocity; // Calculate vector we want to reduce velocity on (negative normal of surface it's bouncing on)
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
void cPhysics::m_Capsule_Collision(sPhysicsProperties* pCapsule, sPossibleCollision& collision, double dt)
{
	float degDiff = acos(glm::dot(collision.hitNorm, glm::vec3(0, 1, 0)));

	if (collision.q == 0) // In or already touching triangle at start of update
	{
		glm::vec3 oldDeltaMove = pCapsule->position - pCapsule->oldPosition;


		glm::vec3 projSubVec = glm::dot(oldDeltaMove, collision.hitNorm) * collision.hitNorm;
		pCapsule->position = pCapsule->oldPosition + (oldDeltaMove - projSubVec);


		projSubVec = glm::dot(pCapsule->velocity - std::numeric_limits<float>::epsilon() * collision.hitNorm, collision.hitNorm) * collision.hitNorm;
		pCapsule->velocity -= projSubVec;


// 		if (degDiff < 40) // TODO need dt on this one, not the lower one though
// 		{
// 			glm::vec3 hypoDeltaVel = pCapsule->velocity * pCapsule->friction;
// 			hypoDeltaVel = hypoDeltaVel - pCapsule->velocity;
// 
// 			pCapsule->velocity += hypoDeltaVel * static_cast<float>(dt);
// 		}
		// I think this is it???
	}
	else // Regular triangle hit beyond the start and before the end of the update window
	{
		glm::vec3 sphereDirection = glm::normalize(pCapsule->velocity);
		glm::vec3 sphereStep = pCapsule->position - pCapsule->oldPosition;


		//glm::vec3 reflectionVec = glm::reflect(sphereDirection, collision.hitNorm);

		// Lets make the reflection vec just the sphereDir projected onto plane made by hitNorm
		
		glm::vec3 projSubVec = glm::dot(sphereDirection, collision.hitNorm) * collision.hitNorm;
		glm::vec3 reflectionVec = (sphereDirection - projSubVec);

		// Get the angle of the current velocity to the hitNorm
		float velToSurface = glm::dot(-collision.hitNorm, sphereDirection); // The closer to 0, the more it should take speed away [0, 1/2pi] 
		// Make it so the more the velocity is going right for the plane, the more speed it loses
		velToSurface = velToSurface / glm::half_pi<float>();

		float sphereSpeed = glm::length(pCapsule->velocity);
		sphereSpeed *= velToSurface; // This does make the player slow a little when landing, but whatever for now
		pCapsule->velocity = reflectionVec * sphereSpeed;

		// TODO change this to radians 
		// Degree between sphere dir and hit norm: 180 is absolute restitution application, 90 is none (parallel to it)
		//float degToNorm = glm::degrees(abs(acos(glm::dot(sphereDirection, collision.hitNorm) / glm::length(sphereDirection) * glm::length(collision.hitNorm))));

		//float restAppDegree = degToNorm - 90;
		//restAppDegree /= 90;

		glm::vec3 restitutionVelLoss = -collision.hitNorm * glm::vec3(abs(pCapsule->velocity.x), abs(pCapsule->velocity.y), abs(pCapsule->velocity.z)); // Calculate vector we want to reduce velocity on (negative normal of surface it's bouncing on)
		//pCapsule->velocity += (restitutionVelLoss * (1.0f - pCapsule->restitution)) * restAppDegree; // Subtract said vector from newVelocity, scaled with its restitution (0 restitution = no bounce, 1 = full bounce)
		pCapsule->velocity += restitutionVelLoss * (1.0f - pCapsule->restitution);

		// Friction??
// 		if (degDiff < 50)
// 			pCapsule->velocity *= pCapsule->friction;

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