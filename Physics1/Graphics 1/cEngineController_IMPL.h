#pragma once


#include "Other Graphics Stuff/cGraphicsMain.h"
#include "Physics/cPhysics.h"
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
	void getAvailableModels(std::vector<std::string>* ModelVec);
	void getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights);
	void getAvailableSaves(std::vector<std::string>* SaveVec);

	/// Data Setting ///
	void setMeshData(int meshID, std::string newFriendlyName, std::string newTextureNames[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor);


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