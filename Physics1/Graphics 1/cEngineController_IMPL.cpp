
#include "cEngineController_IMPL.h"


cEngineController_IMPL::cEngineController_IMPL()
{

}

cEngineController_IMPL::~cEngineController_IMPL()
{

}

void cEngineController_IMPL::Destroy(void)
{
	delete m_pTheGraphics;
	delete m_pThePhysics;
	delete m_pTheSound;
	delete m_pTheTimer;
	this->~cEngineController_IMPL();
}

void cEngineController_IMPL::Run(void)
{
	// Initialize all the objects required to run (graphics, physics, sound, etc.)
	//if (!this->Initialize())
	//	return;

	bool shouldClose = false;
	while (!shouldClose)
	{
		double deltaTime = m_pTheTimer->getFrameTime();
		m_pTheEditor->Update();
		m_pThePhysics->Update(deltaTime);
		shouldClose = m_pTheGraphics->Update(deltaTime);
		m_pTheSound->Update();
	}
}

bool cEngineController_IMPL::Initialize(void)
{
	this->m_pTheGraphics = cGraphicsMain::getGraphicsMain();

	this->m_pThePhysics = new cPhysics();
	this->m_pTheSound = audio::cSoundManager::GetInstance();
	this->m_pTheTimer = new cHiResTimer(30);


	this->m_pTheEditor = new cLevelEditor(m_pTheGraphics->getWindow());

	// TODO set physics VAOManager

	return true;
}

void cEngineController_IMPL::getAvailableModels(std::vector<std::string>* ModelVec)
{
	m_pTheGraphics->getAvailableModels(ModelVec);
	return;
}

void cEngineController_IMPL::getActiveMeshes(std::vector<cMesh*>* MeshVec)
{
	m_pTheGraphics->getActiveMeshes(MeshVec);
	return;
}

