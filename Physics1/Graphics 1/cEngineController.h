#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp> // just so it recognizes vec

// This will be effectively what the main.cpp does now
// Will instantiate all required parts: audio, graphics, physics, (NEW) imgui class
// This will be the face of the real class (*_IMPL)
// 

//#include "cEngineController_IMPL.h"
class cEngineController_IMPL; // Don't want to include everything the impl will
class cMesh;
class cLightManager;

class cEngineController
{
public:
	~cEngineController();
	static cEngineController* GetEngineController();
	void Destroy(void);

	void Run(void); // When this returns, the program ends
	bool Initialize(void); // Initializes the implementation, returns true if all goes smoothly


	//-------------- Calls for the Level Editor -----------------//

	/// Data Retrieval ///
	void getAvailableModels(std::vector<std::string>* ModelVec, std::vector<std::string>* TexVec); // Sets the provided variable with a vector of available models
	void getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights); // Sets provided vector with pointers to all active meshes
	void getAvailableSaves(std::vector<std::string>* SaveVec);

	/// Data Setting ///
	void setMeshData(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor);


	/// Data Creation ///
	void addNewObject(std::string meshName, char* friendlyName);

private:
	cEngineController();
	static cEngineController* m_pEngineFace;
	cEngineController_IMPL* m_pTheEngineController;
};