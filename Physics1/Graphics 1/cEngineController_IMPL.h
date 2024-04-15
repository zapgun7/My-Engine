#pragma once


#include "Other Graphics Stuff/cGraphicsMain.h"
#include "Physics/cPhysics.h"
#include "Physics/sPhysicsProperties.h"
#include "cSoundManager.h"
#include "cHiResTimer.h"
#include "cLevelEditor.h"
#include "cSceneManagement.h"
#include "Lua/cLuaBrain.h"
#include "cPlayer.h"
#include "cAnimationManager.h"
#include "cEntityManager.h"
#include "cInputHandler.h"
#include "cDatabaseManager.h"
#include "cTimer.h"


#include "cNavMesh.h" // For testing here

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
	//void setMeshData(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor);
	void setPhysData(int objID, glm::vec3 newPos, glm::vec3 newOri);
	void setLightData(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2);
	void setPhysicsRunning(bool isRunning);
	void updateDebugMode(bool useDebug, int selcetedMesh, int selectedLight);
	// Animation Manager Settings Setting
	void setTimescale(float newTS);
	void toggleRunningState(void);
	// Verlet Objects
	//void UpdateVAO(std::string meshName, sModelDrawInfo modelInfo);


	// Data Creation ///
	void addNewObject(std::string meshName, char* friendlyName);
	void addCustomObject(cMesh* newMesh, sPhysicsProperties* newObj);
	void saveScene(char* fileName);
	void loadScene(std::string fileName);
	void resetScene(std::vector<cMesh*> newMeshVec, std::vector<cLight> newLights, std::vector<sPhysicsProperties*> newPhysVec);

	/// Data Deletion ///
	void deleteObject(int ID);


private:
	// All objects to represent various game components
	cGraphicsMain* m_pTheGraphics;
	cPhysics* m_pThePhysics;
	cSoundManager* m_pTheSound;
	cHiResTimer* m_pTheTimer;
	cLevelEditor* m_pTheEditor;
	cSceneManagement* m_pTheSceneManager;
	cLuaBrain* m_pLuaBrain;
	cAnimationManager* m_pAnimationsManager;
	cEntityManager* m_pEntityManager;
	cInputHandler* m_pInputHandler;
	cDatabaseManager* m_pDatabaseManager;
	cTimer* m_pTimerManager;
	// Scene manager should be controlled by imgui or some game controller that loads levels

	sPhysicsProperties* m_TheCamera; // Just gonna put this here for now
	cPlayer* m_ThePlayer;


	bool isUsingEntityManager = false; // For quickly swapping using entities


	cNavMesh* testNav;
};