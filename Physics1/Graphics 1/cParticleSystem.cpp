#include "cParticleSystem.h"

cParticleSystem::cParticleSystem()
{

}

cParticleSystem::~cParticleSystem()
{

}

void cParticleSystem::InitializeSpace(unsigned int size)
{
	// Allocate all particles now, not when they're needed
	// "pool" of particles to draw from
	this->m_particles.clear();
	this->m_particles.reserve(size);
	this->m_EmitterInitState.maxNumParticles = size;
}

void cParticleSystem::InitializeParticles(sEmitterInfo& initialState)
{
	initialState.maxNumParticles = m_EmitterInitState.maxNumParticles;

	this->m_EmitterInitState = initialState;
	

	
	// Assume the particles start invisible and dead
	for (unsigned int count = 0; count != this->m_EmitterInitState.maxNumParticles; count++)
	{
		cParticle newParticle;
		// Set particle attributes here

		this->m_particles.push_back(newParticle);
	}

	return;
}


void cParticleSystem::Update(double deltaTime)
{
	// Enabled??
	if (!this->m_isEnabled)
	{
		return;
	}

	if (deltaTime > MAX_DELTATIME)
	{
		deltaTime = MAX_DELTATIME;
	}

	// Update the seconds elapsed
	this->m_SecondElapsedCounter += deltaTime;
	if (this->m_SecondElapsedCounter > 1.0)
	{
		// A second has passed
		this->m_SecondElapsedEvent();
		this->m_SecondElapsedCounter -= 1.0;
	}

	// 'create' some particles this frame
	unsigned int numParticlesToCreate = this->m_getRandomInt(this->m_EmitterInitState.minNumParticlesPerUpdate,
															 this->m_EmitterInitState.maxNumParticlesPerUpdate);
	unsigned int numParticlesCreatedSoFar = 0;
	for (cParticle& curParticle : this->m_particles)
	{
		// Created enough?
		if (numParticlesCreatedSoFar >= numParticlesToCreate)
		{
			break;
		}

		if (curParticle.lifetime <= 0.0)
		{
			// It's dead, revive it

			curParticle.lifetime = this->m_getRandFloat(this->m_EmitterInitState.minLifetime,
														this->m_EmitterInitState.maxLifetime);

			// TODO: Set particle state
			curParticle.position = this->m_EmitterInitState.emitterPosition;
			curParticle.position += this->m_getRandVec3Float(this->m_EmitterInitState.positionOffsetMin,
															 this->m_EmitterInitState.positionOffsetMax);

			curParticle.velocity = this->m_getRandVec3Float(this->m_EmitterInitState.initVelocityMin,
															this->m_EmitterInitState.initVelocityMax);

			curParticle.rotSpeed = glm::quat(this->m_getRandVec3Float(this->m_EmitterInitState.oriChangeMinRadians,
																	  this->m_EmitterInitState.oriChangeMaxRadians));

			// TODO maybe don't want this here??
			curParticle.acceleration = this->m_EmitterInitState.constantForce;

			numParticlesCreatedSoFar++;
		}
	}
	m_LivingParticles += numParticlesToCreate; // Add to internal count of total living particles

	// Basic Euler integration step on 'alive' particles

	for (cParticle& curParticle : this->m_particles)
	{
		if (curParticle.lifetime > 0.0f)
		{
			// Alive particle found

			curParticle.velocity += curParticle.acceleration * (float)deltaTime;

			curParticle.position += curParticle.velocity * (float)deltaTime;

			// Orientation too!

			curParticle.orientation *= curParticle.rotSpeed;

			curParticle.lifetime -= (float)deltaTime;

			m_LivingParticles -= (curParticle.lifetime <= 0) ? 1 : 0; // Could check the sign bit instead (faster? idk)
		}
	}

	return;
}



void cParticleSystem::getParticleList(std::vector< sParticleRender >& vecParticles)
{
	vecParticles.clear();
	vecParticles.reserve(m_LivingParticles);

	// Visible
	if (!this->m_isVisible)
	{
		return;
	}

	for (cParticle& curParticle : this->m_particles) // TODO add break condition when pushed back items = m_LivingParticles
	{
		if (curParticle.lifetime > 0.0)
		{
			sParticleRender drawnParticle;
			drawnParticle.position = curParticle.position;

			vecParticles.push_back(drawnParticle);
		}
	}
	return;
}


sEmitterInfo cParticleSystem::getEmitterState(void)
{
	return m_EmitterInitState;
}



void cParticleSystem::UpdatePosition(glm::vec3 newPosition)
{
	this->m_EmitterInitState.emitterPosition = newPosition;
	return;
}

void cParticleSystem::UpdateConstantForce(glm::vec3 newForce, bool bUpdateParticlesToo)
{
	this->m_EmitterInitState.constantForce = newForce;

	if (bUpdateParticlesToo)
	{
		for (cParticle& curParticle : this->m_particles)
		{
			curParticle.acceleration = this->m_EmitterInitState.constantForce;
		}
	}

	return;
}


void cParticleSystem::m_SecondElapsedEvent(void)
{
	// TODO something?

	return;
}

glm::vec3 cParticleSystem::getPosition(void)
{
	return this->m_EmitterInitState.emitterPosition;
}


void cParticleSystem::Enable(void)
{
	this->m_isEnabled = true;
	return;
}

void cParticleSystem::Disable(void)
{
	this->m_isEnabled = false;
	return;
}

void cParticleSystem::Show_MakeVisible(void)
{
	this->m_isVisible = true;
	return;
}

void cParticleSystem::Hide_MakeInvisible(void)
{
	this->m_isVisible = false;
	return;
}


unsigned int cParticleSystem::m_getRandomInt(unsigned int min, unsigned int max)
{
	unsigned int output = min + (rand() % static_cast<unsigned int>(max - min + 1));

	return output;
}

float cParticleSystem::m_getRandFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}


glm::vec3 cParticleSystem::m_getRandVec3Float(glm::vec3 min, glm::vec3 max)
{
	glm::vec3 randPoint;
	randPoint.x = this->m_getRandFloat(min.x, max.x);
	randPoint.y = this->m_getRandFloat(min.y, max.y);
	randPoint.z = this->m_getRandFloat(min.z, max.z);

	return randPoint;
}