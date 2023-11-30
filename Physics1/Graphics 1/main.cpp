//#include "Other Graphics Stuff/cGraphicsMain.h"
//#include "Physics/cPhysics.h"
//#include "cSoundManager.h"
#include "cEngineController.h"
#include <iostream>
#include <time.h>

// cPhysics* g_pPhysics = NULL;

int main(int argc, char** argv)
{
	srand(time(NULL));

	cEngineController* engineFace = cEngineController::GetEngineController();
	if (!engineFace->Initialize())
	{
		std::cout << "Something went wrong in the initialization";
		return 0;
	}
	engineFace->Run();
	engineFace->Destroy();

	//::g_pPhysics = new cPhysics();
	//cGraphicsMain* graphics = cGraphicsMain::getGraphicsMain();// This initializes it if not yet made 
	//g_pPhysics->setGraphics();

// 	audio::cSoundManager* sounds = audio::cSoundManager::GetInstance();
// 	sounds->Initialize();
// 	sounds->LoadSound("demoman_laugh.mp3");
// 	sounds->LoadSound("laser1.wav");
// 	sounds->LoadSound("baby.wav");
// 	sounds->CreateChannelGroup("Default");


	

	//while (graphics->Update() == 0)
	//{
		// Running...

		// Get xz forward vector to pass to listener attributes
		//glm::vec3 forwardTest = glm::vec3(graphics->m_cameraTarget.x, 0, graphics->m_cameraTarget.z);
		//forwardTest = glm::normalize(forwardTest);
		// Set listener attributes
		//sounds->SetListenerAttribs(graphics->m_cameraEye, glm::vec3(0), glm::vec3(0, 1, 0), forwardTest);// Setting velocity to 0 for now
		// General audio update, also updates all 3d sound objects
		//sounds->Update();
		

	//}
	//graphics->Destroy();



	return 0;
}