
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

void cEngineController::getAvailableModels(std::vector<std::string>* ModelVec)
{
	m_pTheEngineController->getAvailableModels(ModelVec);
	return;
}

void cEngineController::getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights)
{
	m_pTheEngineController->getActiveMeshNLights(MeshVec, TheLights);
	return;
}

void cEngineController::getAvailableSaves(std::vector<std::string>* SaveVec)
{
	m_pTheEngineController->getAvailableSaves(SaveVec);
	return;
}







/*********************************| DATA SETTING |**************************************/

void cEngineController::setMeshData(int meshID, std::string newFriendlyName, std::string newTextureNames[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor)
{
	m_pTheEngineController->setMeshData(meshID, newFriendlyName, newTextureNames, newRatios, isVisible, isWireframe, doNotLight, useDebugColor, debugColor);
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