#pragma once


#include "Other Graphics Stuff/cGraphicsMain.h"
#include "Physics/cPhysics.h"
#include "cSoundManager.h"
#include "cHiResTimer.h"
#include "cLevelEditor.h"

class cEngineController_IMPL
{
public:
	cEngineController_IMPL();
	~cEngineController_IMPL();
	void Destroy(void); // Cleanup everything

	void Run(void); // Starts up the engine
	bool Initialize(void); // Called from start of Run

	void getAvailableModels(std::vector<std::string>* ModelVec);
	void getActiveMeshes(std::vector<cMesh*>* MeshVec);

private:
	// All objects to represent various game components
	cGraphicsMain* m_pTheGraphics;
	cPhysics* m_pThePhysics;
	audio::cSoundManager* m_pTheSound;
	cHiResTimer* m_pTheTimer;
	cLevelEditor* m_pTheEditor;
	// Scene manager should be controlled by imgui or some game controller that loads levels
};