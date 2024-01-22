#pragma once

#include "cEmitterBuilder.h"
#include "cParticleSystem.h"

 // This will manage all instances of cParticleSystem
// Should this be dynamic to some extent? Should we preload a bunch of general use emitters?



class cParticleManager
{
public:
	~cParticleManager();

	static cParticleManager* GetInstance(void);
	int Destroy(void);

	void Update(double deltaTime);


	void startEmitter(EmitterPreset preset, glm::vec3 pos); // Later add info about simple adjacent geometry? (plane)

	void getParticleList(std::vector< cParticleSystem::sParticleRender >& vecParticles); // For now just return the default list


private:
	cParticleManager();
	int Initialize(void);

	std::vector<cParticleSystem> m_vecParticleSystems; // Vector to hold all (for now) the particle systems at our disposal
	
	

	static cParticleManager* m_pTheParticleManager;
};