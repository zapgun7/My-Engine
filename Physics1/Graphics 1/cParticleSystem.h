#pragma once

#include "Other Graphics Stuff/OpenGLCommon.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <vector>

// Possible Enums:
// - Particle Shape (sphere, diamond, imposter, etc.)
// - Particle move algorithm 
// - Particle Color?


// Algorithm Ideas:
// - Speed control (faster/slower the farther from spawn point) (speed reduction on certain axes: have something shoot out but stabilize on the horizontal axes and float up)
// - Pathing (hard)   (Spiral path or zigzag not too hard; reference straight line)     (Falling leaves, or blowing snow would be more difficult)
//     * 

struct sEmitterInfo
{
	unsigned int maxNumParticles = 0;

	glm::vec3 emitterPosition = glm::vec3(0.0f);

	glm::vec3 positionOffsetMin = glm::vec3(0.0f);
	glm::vec3 positionOffsetMax = glm::vec3(0.0f);

	// Min & max particles per update?
	unsigned int minNumParticlesPerUpdate = 0;
	unsigned int maxNumParticlesPerUpdate = 0;


	// Lifetime
	float minLifetime = 0.0f;
	float maxLifetime = 0.0f;

	// Velocity
	glm::vec3 initVelocityMin = glm::vec3(0.0f);
	glm::vec3 initVelocityMax = glm::vec3(0.0f);

	// Rotation
	glm::vec3 oriChangeMinRadians = glm::vec3(0.0f);
	glm::vec3 oriChangeMaxRadians = glm::vec3(0.0f);


	glm::vec3 constantForce = glm::vec3(0.0f);   // The "gravity"
	glm::vec3 terminalVelocity = glm::vec3(0.0f);
};

class cParticleSystem
{
public:
	cParticleSystem();
	~cParticleSystem();


	

	struct sParticleRender
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::quat orientation = glm::quat(glm::vec3(0.0f));
		glm::vec3 scale = glm::vec3(0.0f);
		// Color?
		// Texture?
		// UV-Offset??
	};

	void InitializeSpace(unsigned int size);
	void InitializeParticles(sEmitterInfo& initialState);

	void Enable(void);
	void Disable(void);

	void Show_MakeVisible(void);
	void Hide_MakeInvisible(void);

	void Update(double deltaTime);
	void UpdatePosition(glm::vec3 newPosition);
	void UpdateConstantForce(glm::vec3 newForce, bool bUpdateParticlesToo = false);

	glm::vec3 getPosition(void);


	void getParticleList(std::vector< sParticleRender >& vecParticles);

	sEmitterInfo getEmitterState(void);

private:

	class cParticle
	{
	public:
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 velocity = glm::vec3(0.0f);
		glm::vec3 acceleration = glm::vec3(0.0f);

		float lifetime = 0.0f;

		glm::quat orientation = glm::quat(glm::vec3(0.0f));
		glm::quat rotSpeed = glm::quat(glm::vec3(0.0f));
		glm::vec3 scaleXYZ = glm::vec3(0.0f);
	};

	std::vector< cParticle > m_particles;

	sEmitterInfo m_EmitterInitState;

	bool m_isEnabled = false;
	bool m_isVisible = false;

	double m_SecondElapsedCounter = 0.0;

	unsigned int m_LivingParticles = 0;

	void m_SecondElapsedEvent(void);

	float m_getRandFloat(float a, float b);
	glm::vec3 m_getRandVec3Float(glm::vec3 min, glm::vec3 max);

	unsigned int m_getRandomInt(unsigned int min, unsigned int max);


	const double MAX_DELTATIME = 1.0 / 60.0;
};