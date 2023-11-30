
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

	this->m_pTheSceneManager = new cSceneManagement();
	this->m_pTheSceneManager->Initialize();

	// TODO set physics VAOManager

	return true;
}

void cEngineController_IMPL::getAvailableModels(std::vector<std::string>* ModelVec)
{
	m_pTheGraphics->getAvailableModels(ModelVec);
	return;
}

void cEngineController_IMPL::getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights)
{
	m_pTheGraphics->getActiveMeshNLights(MeshVec, TheLights);
	return;
}

void cEngineController_IMPL::getAvailableSaves(std::vector<std::string>* SaveVec)
{
	std::vector<std::string> tempVec = m_pTheSceneManager->getAvailableSaves();
	for (unsigned int i = 0; i < tempVec.size(); i++)
	{
		SaveVec->push_back(tempVec[i]);
	}

	return;
}

void cEngineController_IMPL::setMeshData(int meshID, std::string newFriendlyName, std::string newTextureNames[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor)
{
	m_pTheGraphics->updateMesh(meshID, newFriendlyName, newTextureNames, newRatios, isVisible, isWireframe, doNotLight, useDebugColor, debugColor);
	return;
}

