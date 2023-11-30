
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
void cEngineController::getAvailableModels(std::vector<std::string>* ModelVec)
{
	m_pTheEngineController->getAvailableModels(ModelVec);
	return;
}

void cEngineController::getActiveMeshes(std::vector<cMesh*>* MeshVec)
{
	m_pTheEngineController->getActiveMeshes(MeshVec);
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