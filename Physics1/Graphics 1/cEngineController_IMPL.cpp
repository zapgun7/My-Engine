
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

void cEngineController_IMPL::getAvailableModels(std::vector<std::string>* ModelVec, std::vector<std::string>* TexVec)
{
	m_pTheGraphics->getAvailableModels(ModelVec, TexVec);
	return;
}

void cEngineController_IMPL::getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights, std::vector<sPhysicsProperties*>* PhysVec)
{
	m_pTheGraphics->getActiveMeshNLights(MeshVec, TheLights);
	std::vector<sPhysicsProperties*> tempVec;
	tempVec = m_pThePhysics->getPhysicsVec();

	for (unsigned int i = 0; i < tempVec.size(); i++)
	{
		PhysVec->push_back(tempVec[i]);
	}
	
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

void cEngineController_IMPL::setMeshData(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor)
{
	m_pTheGraphics->updateMesh(meshID, newFriendlyName, newTextureIdx, newRatios, isVisible, isWireframe, doNotLight, useDebugColor, debugColor);
	return;
}

void cEngineController_IMPL::setPhysData(int objID, glm::vec3 newPos, glm::vec3 newOri)
{
	m_pThePhysics->setShapePori(newPos, newOri, objID);
	return;
}

void cEngineController_IMPL::setLightData(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2)
{
	m_pTheGraphics->updateSelectedLight(lightIdx, friendlyName, newPos, newDiff, newSpec, newAtten, newDir, newParam1, newParam2);
	return;
}

void cEngineController_IMPL::addNewObject(std::string meshName, char* friendlyName)
{
	// Create cmesh and add to cGraphicsMain
	// Create generic physics object and add to cPhysics
	// Set physics generated id to mesh
	// Set associated mesh to physics objectcMesh* newMesh
	cMesh* newMesh = new cMesh();
	newMesh->meshName = meshName;
	newMesh->friendlyName = friendlyName;
	m_pTheGraphics->addNewMesh(newMesh);



	// Now physics
	sPhysicsProperties* newObject = new sPhysicsProperties();
	newObject->pTheAssociatedMesh = newMesh;
	newObject->friendlyName = friendlyName;
	m_pThePhysics->AddShape(newObject);


	// Set unique id's matching
	newMesh->uniqueID = newObject->getUniqueID();
	

	return;
}

