#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp> // just so it recognizes vec

// This will be effectively what the main.cpp does now
// Will instantiate all required parts: audio, graphics, physics, (NEW) imgui class
// This will be the face of the real class (*_IMPL)
// 
//#include "Physics/sPhysicsProperties.h"

//#include "cEngineController_IMPL.h"
class cEngineController_IMPL; // Don't want to include everything the impl will
class cMesh;
class cLightManager;
class cLight;
struct sPhysicsProperties;

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
	void getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights, std::vector<sPhysicsProperties*>* ThePhys); // Sets provided vector with pointers to all active meshes
	void getAvailableSaves(std::vector<std::string>* SaveVec);

	/// Data Setting ///
	void setMeshData(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor);
	void setPhysData(int objID, glm::vec3 newPos, glm::vec3 newOri);
	void setLightData(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2);
	void setPhysicsRunning(bool isRunning);
	void updateDebugMode(bool useDebug, int selcetedMesh, int selectedLight);
	// Animation Manager Settings Setting
	void setTimescale(float newTS);
	void toggleRunningState(void);

	/// Data Creation ///
	void addNewObject(std::string meshName, char* friendlyName);
	void addCustomObject(cMesh* newMesh, sPhysicsProperties* newObj);
	void saveScene(char* fileName);
	void loadScene(std::string fileName);
	void resetScene(std::vector<cMesh*> newMeshVec, std::vector<cLight> newLights, std::vector<sPhysicsProperties*> newPhysVec);

	/// Data Deletion ///
	void deleteObject(int ID);

private:
	cEngineController();
	static cEngineController* m_pEngineFace;
	cEngineController_IMPL* m_pTheEngineController;
};