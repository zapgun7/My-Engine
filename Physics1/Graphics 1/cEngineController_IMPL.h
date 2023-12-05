#pragma once


#include "Other Graphics Stuff/cGraphicsMain.h"
#include "Physics/cPhysics.h"
#include "Physics/sPhysicsProperties.h"
#include "cSoundManager.h"
#include "cHiResTimer.h"
#include "cLevelEditor.h"
#include "cSceneManagement.h"

class cEngineController_IMPL
{
public:
	cEngineController_IMPL();
	~cEngineController_IMPL();
	void Destroy(void); // Cleanup everything

	void Run(void); // Starts up the engine
	bool Initialize(void); // Called from start of Run

	//-------------- Calls for the Level Editor -----------------//

	/// Data Retrieval ///
	void getAvailableModels(std::vector<std::string>* ModelVec, std::vector<std::string>* TexVec);
	void getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights, std::vector<sPhysicsProperties*>* PhysVec);
	void getAvailableSaves(std::vector<std::string>* SaveVec);

	/// Data Setting ///
	void setMeshData(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor);
	void setPhysData(int objID, glm::vec3 newPos, glm::vec3 newOri);
	void setLightData(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2);

	// Data Creation ///
	void addNewObject(std::string meshName, char* friendlyName);

	/// Data Deletion ///
	void deleteObject(int ID);

private:
	// All objects to represent various game components
	cGraphicsMain* m_pTheGraphics;
	cPhysics* m_pThePhysics;
	audio::cSoundManager* m_pTheSound;
	cHiResTimer* m_pTheTimer;
	cLevelEditor* m_pTheEditor;
	cSceneManagement* m_pTheSceneManager;
	// Scene manager should be controlled by imgui or some game controller that loads levels
};