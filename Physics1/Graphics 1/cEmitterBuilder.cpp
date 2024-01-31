#include "cEmitterBuilder.h"

#include "cParticleSystem.h"

//struct cParticleSystem::sEmitterInfo;

void cEmitterBuilder::makeEmitter(EmitterPreset preset, sEmitterInfo* info)
{
	switch (preset)
	{
	case DEFAULT:
	{
		// Position is done elsewhere 
		// info->emitterPosition = glm::vec3(0.0f, 5.0f, 0.0f);

		info->initVelocityMin = glm::vec3(-3.0f, 10.0f, -3.0f);
		info->initVelocityMax = glm::vec3(3.0f, 15.0f, 3.0f);

		// Don't need rotation now
// 		info->orientationChangeMinRadians = glm::vec3(-0.1f, -0.1f, -0.1f);
// 		info->orientationChangeMaxRadians = glm::vec3(0.1f, 0.1f, 0.1f);

		info->constantForce = glm::vec3(0.0f, -4.0f, 0.0f);

		info->minLifetime = 5.0f;
		info->maxLifetime = 8.0f;

		info->minNumParticlesPerUpdate = 0; // TODO change these to particles per second
		info->maxNumParticlesPerUpdate = 2;

		break;
	}
	}

	return;
}

