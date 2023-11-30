#pragma once

#include <vector>
#include <string>

// This will be effectively what the main.cpp does now
// Will instantiate all required parts: audio, graphics, physics, (NEW) imgui class
// This will be the face of the real class (*_IMPL)
// 

//#include "cEngineController_IMPL.h"
class cEngineController_IMPL; // Don't want to include everything the impl will
class cMesh;

class cEngineController
{
public:
	~cEngineController();
	static cEngineController* GetEngineController();
	void Destroy(void);

	void Run(void); // When this returns, the program ends
	bool Initialize(void); // Initializes the implementation, returns true if all goes smoothly


	// Calls for the Level Editor
	void getAvailableModels(std::vector<std::string>* ModelVec); // Sets the provided variable with a vector of available models
	void getActiveMeshes(std::vector<cMesh*>* MeshVec); // Sets provided vector with pointers to all active meshes


private:
	cEngineController();
	static cEngineController* m_pEngineFace;
	cEngineController_IMPL* m_pTheEngineController;
};