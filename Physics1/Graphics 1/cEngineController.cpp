
#include "cEngineController.h"
#include "cEngineController_IMPL.h"




cEngineController* cEngineController::m_pEngineFace = nullptr;


cEngineController::~cEngineController()
{
	delete this->m_pTheEngineController;
}

cEngineController* cEngineController::GetEngineController(void)
{
	if (cEngineController::m_pEngineFace == nullptr)
	{
		cEngineController::m_pEngineFace = new cEngineController();
	}
	return cEngineController::m_pEngineFace;
}

void cEngineController::Destroy(void)
{
	m_pTheEngineController->Destroy();
	this->~cEngineController();
}

void cEngineController::Run()
{
	m_pTheEngineController->Run();
}

bool cEngineController::Initialize()
{
	if (m_pTheEngineController->Initialize())
		return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////EDITOR CALLS/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////



		/*********************************| DATA RETRIEVAL |**************************************/

void cEngineController::getAvailableModels(std::vector<std::string>* ModelVec, std::vector<std::string>* TexVec)
{
	m_pTheEngineController->getAvailableModels(ModelVec, TexVec);
	return;
}

void cEngineController::getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights, std::vector<sPhysicsProperties*>* PhysVec)
{
	m_pTheEngineController->getActiveMeshNLights(MeshVec, TheLights, PhysVec);
	return;
}

void cEngineController::getAvailableSaves(std::vector<std::string>* SaveVec)
{
	m_pTheEngineController->getAvailableSaves(SaveVec);
	return;
}







/*********************************| DATA SETTING |**************************************/

void cEngineController::setMeshData(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor)
{
	m_pTheEngineController->setMeshData(meshID, newFriendlyName, newTextureIdx, newRatios, isVisible, isWireframe, doNotLight, useDebugColor, debugColor);
	return;
}

void cEngineController::setPhysData(int objID, glm::vec3 newPos, glm::vec3 newOri)
{
	m_pTheEngineController->setPhysData(objID, newPos, newOri);
}

void cEngineController::setLightData(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2)
{
	m_pTheEngineController->setLightData(lightIdx, friendlyName, newPos, newDiff, newSpec, newAtten, newDir, newParam1, newParam2);
	return;
}

void cEngineController::setPhysicsRunning(bool isRunning)
{
	m_pTheEngineController->setPhysicsRunning(isRunning);
	return;
}

void cEngineController::updateDebugMode(bool useDebug, int selcetedMesh, int selectedLight)
{
	m_pTheEngineController->updateDebugMode(useDebug, selcetedMesh, selectedLight);
	return;
}

void cEngineController::setTimescale(float newTS)
{
	m_pTheEngineController->setTimescale(newTS);
	return;
}

void cEngineController::toggleRunningState(void)
{
	m_pTheEngineController->toggleRunningState();
	return;
}

// void cEngineController::UpdateVAO(std::string meshName, sModelDrawInfo modelInfo)
// {
// 	m_pTheEngineController->UpdateVAO(meshName, modelInfo);
// }

void cEngineController::addNewObject(std::string meshName, char* friendlyName)
{
	m_pTheEngineController->addNewObject(meshName, friendlyName);
	return;
}

void cEngineController::addCustomObject(cMesh* newMesh, sPhysicsProperties* newObj)
{
	m_pTheEngineController->addCustomObject(newMesh, newObj);
	return;
}

void cEngineController::saveScene(char* fileName)
{
	m_pTheEngineController->saveScene(fileName);
	return;
}

void cEngineController::loadScene(std::string fileName)
{
	m_pTheEngineController->loadScene(fileName);
	return;
}

void cEngineController::resetScene(std::vector<cMesh*> newMeshVec, std::vector<cLight> newLights, std::vector<sPhysicsProperties*> newPhysVec)
{
	m_pTheEngineController->resetScene(newMeshVec, newLights, newPhysVec);
	return;
}

void cEngineController::deleteObject(int ID)
{
	m_pTheEngineController->deleteObject(ID);
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////EDITOR CALLS END/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




cEngineController::cEngineController()
{
	// Make implementation
	this->m_pTheEngineController = new cEngineController_IMPL();
}