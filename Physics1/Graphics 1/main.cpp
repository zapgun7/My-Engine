#include "Other Graphics Stuff/cGraphicsMain.h"
#include "Physics/cPhysics.h"
#include "cSoundManager.h"
#include <time.h>

cPhysics* g_pPhysics = NULL;

int main(int argc, char** argv)
{
	srand(time(NULL));

	::g_pPhysics = new cPhysics();
	cGraphicsMain* graphics = cGraphicsMain::getGraphicsMain();// This initializes it if not yet made 
	g_pPhysics->setGraphics();

	audio::cSoundManager* sounds = audio::cSoundManager::GetInstance();
	sounds->Initialize();
	sounds->LoadSound("demoman_laugh.mp3");
	sounds->LoadSound("laser1.wav");
	sounds->CreateChannelGroup("Default");


	// Create a test sphere with audio
		 //Load in default player object
 	cMesh* meshToAdd = new cMesh();
 	meshToAdd->meshName = "Sphere_1_unit_Radius.ply"; // Set object type
 	meshToAdd->friendlyName = "Test";
 	meshToAdd->bDoNotLight = true;
 
    graphics->m_vec_pMeshesToDraw.push_back(meshToAdd);
 
 	// Create the physics object
 	sPhsyicsProperties* newShape = new sPhsyicsProperties();
 	newShape->shapeType = sPhsyicsProperties::SPHERE;
 	newShape->setShape(new sPhsyicsProperties::sSphere(1.0f)); // Since a unit sphere, radius of .5 
 	newShape->pTheAssociatedMesh = meshToAdd;
 	newShape->inverse_mass = 1.0f; // Idk what to set this
 	newShape->friendlyName = "Sphere";
 	newShape->acceleration.y = -20.0f;
 	newShape->position = glm::vec3(0, 0, 0);
	newShape->restitution = 1.0f;
 	::g_pPhysics->AddShape(newShape);

 
 	meshToAdd->uniqueID = newShape->getUniqueID();

	int id = meshToAdd->uniqueID;

	sounds->AddNewObject(newShape);

	sounds->PlaySoundFromObject(id, "demoman_laugh.mp3", true);


	while (graphics->Update() == 0)
	{
		// Running...
		//sounds->PlaySoundFromObject(id, "laser1.wav", false);
		glm::vec3 forwardTest = glm::vec3(graphics->m_cameraTarget.x, 0, graphics->m_cameraTarget.z);
		forwardTest = glm::normalize(forwardTest);
		sounds->SetListenerAttribs(graphics->m_cameraEye, glm::vec3(0), glm::vec3(0, 1, 0), forwardTest);// Setting velocity to 0 for now
		sounds->Update();

	}
	graphics->Destroy();

	return 0;
}