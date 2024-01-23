#include "cParticleManager.h"

cParticleManager* cParticleManager::m_pTheParticleManager = nullptr;


cParticleManager::cParticleManager()
{

}

cParticleManager::~cParticleManager()
{

}


int cParticleManager::Initialize(void)
{
	// Reserves a bunch of space, initializes a set amount of emitters
	cParticleSystem newSystem;
	//newSystem.InitializeSpace(10'000);

	m_vecParticleSystems.push_back(newSystem);
	m_vecParticleSystems[0].InitializeSpace(1000);

	return 0;
}

int cParticleManager::Destroy(void)
{

	return 0;
}



// Updates all active emitters
void cParticleManager::Update(double deltaTime)
{
	m_vecParticleSystems[0].Update(deltaTime);

	return;
}


void cParticleManager::startEmitter(EmitterPreset preset, glm::vec3 pos)
{
	// Should start by finding particle system best suited or available
	// For now will just use the one default

	sEmitterInfo newInfo;

	cEmitterBuilder::makeEmitter(DEFAULT, &newInfo);

	newInfo.emitterPosition = pos;

	m_vecParticleSystems[0].InitializeParticles(newInfo);

	m_vecParticleSystems[0].Enable();
	m_vecParticleSystems[0].Show_MakeVisible();

	return;
}


void cParticleManager::getParticleList(std::vector< cParticleSystem::sParticleRender >& vecParticles)
{
	m_vecParticleSystems[0].getParticleList(vecParticles);
}

cParticleManager* cParticleManager::GetInstance(void) // Only need the one instance I think
{
	if (cParticleManager::m_pTheParticleManager == nullptr)
	{
		cParticleManager::m_pTheParticleManager = new cParticleManager();
		
		if (!cParticleManager::m_pTheParticleManager->Initialize())
		{
			// Initialization didn't work, destroy
			cParticleManager::m_pTheParticleManager->Destroy();
		}
	}
	return m_pTheParticleManager;
}

