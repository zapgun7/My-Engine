
#include "cEngineController_IMPL.h"
#include <iostream>

cEngineController_IMPL::cEngineController_IMPL()
{

}

cEngineController_IMPL::~cEngineController_IMPL()
{

}

void cEngineController_IMPL::Destroy(void)
{
	delete m_pTheGraphics;
	delete m_pThePhysics;
	delete m_pTheSound;
	delete m_pTheTimer;
	this->~cEngineController_IMPL();
}

void cEngineController_IMPL::Run(void)
{
	// Initialize all the objects required to run (graphics, physics, sound, etc.)
	//if (!this->Initialize())
	//	return;

	bool shouldClose = false;
	while (!shouldClose)
	{
		double deltaTime = m_pTheTimer->getFrameTime();
		this->m_pInputHandler->Update();
		this->m_pTimerManager->Update(deltaTime);
		//if (this->m_pInputHandler->IsPressedEvent(GLFW_KEY_T)) m_pThePhysics->ToggleThreading();
		//double deltaTime = m_pTheTimer->getFrameTimeUncapped();

		// Messy, but will do this here
		glm::vec3 camPos = m_TheCamera->position;
		glm::quat camQuat = m_TheCamera->get_qOrientation();
		m_ThePlayer->Update(deltaTime, camPos, camQuat);
		m_TheCamera->position = camPos;
		m_TheCamera->setRotationFromQuat(camQuat);
		m_pTheGraphics->UpdateCamera(camPos, camQuat);
		// End of messy

		//m_pLuaBrain->UpdateActiveCommands(deltaTime);

		m_pAnimationsManager->Update(deltaTime);

		if (isUsingEntityManager)
		{
			m_pEntityManager->Update(deltaTime);
			m_pEntityManager->SetPlayerNavTri(m_ThePlayer->GetCurrTri());
		}

		m_pTheEditor->Update(deltaTime); //m_pTheEditor->Update(uncappedDT); 
		m_pThePhysics->Update(deltaTime);
		shouldClose = m_pTheGraphics->Update(deltaTime);
		m_pTheSound->Update(deltaTime);

		// Input For Animation Project
// 		if (m_pInputHandler->IsPressedEvent(GLFW_KEY_1))
// 		{
// 			m_pAnimationsManager->SetActiveAnimation(0);
// 		}
// 		else if (m_pInputHandler->IsPressedEvent(GLFW_KEY_2))
// 		{
// 			m_pAnimationsManager->SetActiveAnimation(1);
// 		}
// 		else if (m_pInputHandler->IsPressedEvent(GLFW_KEY_3))
// 		{
// 			m_pAnimationsManager->SetActiveAnimation(2);
// 		}
// 		else if (m_pInputHandler->IsPressedEvent(GLFW_KEY_4))
// 		{
// 			m_pAnimationsManager->SetActiveAnimation(3);
// 		}
// 		else if (m_pInputHandler->IsPressedEvent(GLFW_KEY_5))
// 		{
// 			m_pAnimationsManager->SetActiveAnimation(4);
// 		}
// 		else if (m_pInputHandler->IsPressedEvent(GLFW_KEY_6))
// 		{
// 			m_pAnimationsManager->SetActiveAnimation(5);
// 		}
// 		else if (m_pInputHandler->IsPressedEvent(GLFW_KEY_7))
// 		{
// 			m_pAnimationsManager->SetActiveAnimation(6);
// 		}
	}
}

bool cEngineController_IMPL::Initialize(void)
{
	this->m_pTheGraphics = cGraphicsMain::getGraphicsMain();
	cInputHandler::Initialize(this->m_pTheGraphics->getWindow());
	this->m_pInputHandler = cInputHandler::GetInstance();

	this->m_pThePhysics = cPhysics::GetInstance();
	this->m_pTheSound = cSoundManager::GetInstance();
	this->m_pTheTimer = new cHiResTimer(30);


	this->m_pTheEditor = new cLevelEditor(m_pTheGraphics->getWindow());

	this->m_pTheSceneManager = new cSceneManagement();
	this->m_pTheSceneManager->Initialize();


	this->m_pThePhysics->setVAOManager(m_pTheGraphics->getVAOManager());
	std::vector<std::string> tempModelVec;
	this->m_pTheGraphics->getAvailableModels(&tempModelVec);
	this->m_pThePhysics->generateAABBs(tempModelVec);
	
	//this->m_pLuaBrain = new cLuaBrain();
	//this->m_pLuaBrain->RunScriptImmediately("TestThing()");

	this->m_pAnimationsManager = cAnimationManager::GetInstance();
	this->m_pAnimationsManager->SetVAOManager(m_pTheGraphics->getVAOManager());

	//this->m_pDatabaseManager = cDatabaseManager::GetInstance();

	this->m_pTimerManager->GetInstance();
	
	//cSoftBodyVerlet* theBlob = m_pThePhysics->CreateVerlet();

	


	// Make the camera object
	m_TheCamera = new sPhysicsProperties();
	m_TheCamera->friendlyName = "cam";
	m_TheCamera->position = glm::vec3(0, 20, -40);
	m_ThePlayer = new cPlayer(m_pTheGraphics->getWindow());

	//m_ThePlayer->setPlayerVerlet(theBlob);





	// TESTING NAV MESH GENERATION
	this->testNav = new cNavMesh();

	// First person player setup
	if (true) 
	{
		m_pTheSceneManager->loadScene("navPathsPhys");//("testNav4");
		
		sPhysicsProperties* playerObj = new sPhysicsProperties();
		playerObj->setShape(new sPhysicsProperties::sCapsule(1.5f, 0.5f));
		playerObj->shapeType = sPhysicsProperties::CAPSULE;
		playerObj->friendlyName = "plyr";

		playerObj->position = glm::vec3(0, 10, 0);
		playerObj->restitution = 0.0f;
		playerObj->inverse_mass = 1.0f;
		// Add PlayerInfo Struct
		sPlayerPhysics* plyrPhys = new sPlayerPhysics();
		playerObj->playerInfo = plyrPhys;

		plyrPhys->friction = 0.1f;
		plyrPhys->airDrag = 0.99f;
		playerObj->isPlayer = true;
		m_ThePlayer->setPlayerObject(playerObj);


		cMesh* playerMesh = new cMesh();
		playerMesh->meshName = "Sphere_1_unit_Radius.ply";
		playerMesh->friendlyName = "Player";
		//playerMesh->textureName[0] = "metal_s01.bmp";
		playerMesh->scale = glm::vec3(2.0f);
		playerMesh->uniqueID = playerObj->getUniqueID();

		playerObj->pTheAssociatedMesh = playerMesh;

		// Load test scene
		//m_pTheSceneManager->loadScene("MovementPGCorners+");
		//m_pTheSceneManager->loadScene("rampTest");
		//m_pTheSceneManager->loadScene("test4");
		//m_pThePhysics->AddShape(playerObj);
		addCustomObject(playerMesh, playerObj);
	}

	// Generate Enemies
	if (true) // EnemyEntity
	{
		//m_pTheSceneManager->loadScene("complexNav");
		m_pThePhysics->Update(0.0f);
		isUsingEntityManager = true;

		std::vector<cMesh*> meshVec;
		std::vector<cMesh*> trimmedMeshVec;
		m_pTheGraphics->getActiveMeshes(&meshVec);
		for (cMesh* currMesh : meshVec)
		{
// 			if (currMesh->meshName == "Flat_1x1_plane.ply")
// 				trimmedMeshVec.push_back(currMesh);
			if (currMesh->isNavMesh)
				trimmedMeshVec.push_back(currMesh);
		}
		testNav->Initialize(trimmedMeshVec);

		this->m_pEntityManager = new cEntityManager();
		this->m_pEntityManager->SetPlayer(m_TheCamera);
		this->m_pEntityManager->SetNavMesh(testNav);
		this->m_pEntityManager->Initialize();

		// So the player can keep track of their position, letting the AI know where it is
		this->m_ThePlayer->setPlayerNavMesh(testNav);
		this->m_pEntityManager->SetPlayerNavTri(this->m_ThePlayer->GetCurrTri());
	}



	if (false) // Boned animation character test
	{
		// Make boned character
		cMesh* bonedBoy = new cMesh();
		bonedBoy->meshName = "AnimatedHuman.fbx";
		bonedBoy->friendlyName = "a";

		//m_pTheGraphics->addNewMesh(bonedBoy);
		sPhysicsProperties* bonedBoyPhys = new sPhysicsProperties();
		//bonedBoyPhys->setRotationFromEuler(glm::vec3(-90, 0, 0));
		bonedBoyPhys->pTheAssociatedMesh = bonedBoy;
		addCustomObject(bonedBoy, bonedBoyPhys);
	}
	
	if (false) // Setup for soft-bodied example
	{
		//m_pTheSceneManager->loadScene("FacilityScreens3");

		cMesh* blobMesh = new cMesh();
		blobMesh->friendlyName = "blobPlayer";
		blobMesh->meshName = "sphere.ply";
		blobMesh->bUseCustomColors = true;
		blobMesh->customColorRGBA = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		blobMesh->isDoubleSided = true;
		//blobMesh->isSpooky = true; // Just gonna test the fbo thang on this thang
		blobMesh->transparencyAlpha = 0.6f;

		m_pTheGraphics->addNewMesh(blobMesh);
		
		cMesh* plat1Mesh = new cMesh();
		plat1Mesh->friendlyName = "plat1";
		plat1Mesh->meshName = "plat.ply";
		plat1Mesh->bUseCustomColors = true;
		plat1Mesh->customColorRGBA = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
		//plat1Mesh->isDoubleSided = true;

		m_pTheGraphics->addNewMesh(plat1Mesh);

		cMesh* ground = new cMesh();
		ground->friendlyName = "metalGround";
		ground->meshName = "Big_Flat_Mesh.ply";
		//ground->textureName[0] = "metal_s01.bmp";
		ground->drawPosition.y = -0.1f;

		m_pTheGraphics->addNewMesh(ground);
	}



	printf("Done Initializing!\n");

	return true;
}

void cEngineController_IMPL::getAvailableModels(std::vector<std::string>* ModelVec, std::vector<std::string>* TexVec)
{
	m_pTheGraphics->getAvailableModels(ModelVec);
	m_pTheGraphics->getAvailableTextures(TexVec);
	return;
}

void cEngineController_IMPL::getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights, std::vector<sPhysicsProperties*>* PhysVec)
{
	m_pTheGraphics->getActiveMeshes(MeshVec);
	m_pTheGraphics->getActiveLights(TheLights);
	std::vector<sPhysicsProperties*> tempVec;
	tempVec = m_pThePhysics->getPhysicsVec();

	for (unsigned int i = 0; i < tempVec.size(); i++)
	{
		PhysVec->push_back(tempVec[i]);
	}
	
	return;
}

void cEngineController_IMPL::getAvailableSaves(std::vector<std::string>* SaveVec)
{
	std::vector<std::string> tempVec = m_pTheSceneManager->getAvailableSaves();
	for (unsigned int i = 0; i < tempVec.size(); i++)
	{
		SaveVec->push_back(tempVec[i]);
	}

	return;
}

// void cEngineController_IMPL::setMeshData(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor)
// {
// 	m_pTheGraphics->updateMesh(meshID, newFriendlyName, newTextureIdx, newRatios, isVisible, isWireframe, doNotLight, useDebugColor, debugColor);
// 	return;
// }

void cEngineController_IMPL::setPhysData(int objID, glm::vec3 newPos, glm::vec3 newOri)
{
	m_pThePhysics->setShapePori(newPos, newOri, objID);
	return;
}

void cEngineController_IMPL::setLightData(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2)
{
	m_pTheGraphics->updateSelectedLight(lightIdx, friendlyName, newPos, newDiff, newSpec, newAtten, newDir, newParam1, newParam2);
	return;
}

void cEngineController_IMPL::setPhysicsRunning(bool isRunning)
{
	m_pThePhysics->setPhysicsRunningState(isRunning);
	return;
}

void cEngineController_IMPL::updateDebugMode(bool useDebug, int selcetedMesh, int selectedLight)
{
	m_pTheGraphics->updateDebugStates(useDebug, selcetedMesh, selectedLight);
	return;
}

void cEngineController_IMPL::setTimescale(float newTS)
{
	m_pAnimationsManager->setTimescale(newTS);
	return;
}

void cEngineController_IMPL::toggleRunningState(void)
{
	m_pAnimationsManager->toggleRunning();
	return;
}

// void cEngineController_IMPL::UpdateVAO(std::string meshName, sModelDrawInfo modelInfo)
// {
// 	m_pTheGraphics->UpdateVAO(meshName, modelInfo);
// }

void cEngineController_IMPL::addNewObject(std::string meshName, char* friendlyName)
{
	// Create cmesh and add to cGraphicsMain
	// Create generic physics object and add to cPhysics
	// Set physics generated id to mesh
	// Set associated mesh to physics objectcMesh* newMesh
	cMesh* newMesh = new cMesh();
	newMesh->meshName = meshName;
	newMesh->friendlyName = friendlyName;
	m_pTheGraphics->addNewMesh(newMesh);



	// Now physics
	sPhysicsProperties* newObject = new sPhysicsProperties();
	newObject->pTheAssociatedMesh = newMesh;
	newObject->friendlyName = friendlyName;
	m_pThePhysics->AddShape(newObject);


	// Set unique id's matching
	newMesh->uniqueID = newObject->getUniqueID();
	

	///////// ANIMTAION TESTING /////////
// 	m_pAnimationsManager->AddAnimationObj(newObject);
// 	newMesh->bDoNotLight = false;
// 	newMesh->bUseDebugColours = true;
// 	newMesh->wholeObjectDebugColourRGBA = glm::vec4(.5, .5, .5, 1);


	return;
}

void cEngineController_IMPL::addCustomObject(cMesh* newMesh, sPhysicsProperties* newObj)
{
	m_pTheGraphics->addNewMesh(newMesh);
	if (newObj->isPlayer)
		m_pThePhysics->SetPlayerObj(newObj);
	else
		m_pThePhysics->AddShape(newObj);

	return;
}

void cEngineController_IMPL::saveScene(char* fileName)
{
	std::vector<cMesh*> meshVec; 
	m_pTheGraphics->getActiveMeshes(&meshVec);
	cLightManager lights;
	m_pTheGraphics->getActiveLights(&lights);
	std::vector<sPhysicsProperties*> physVec = m_pThePhysics->getPhysicsVec();

	m_pTheSceneManager->saveScene(fileName, meshVec, &lights, physVec);

	return;
}

void cEngineController_IMPL::loadScene(std::string fileName)
{
	m_pTheSceneManager->loadScene(fileName);

	// Lazy way to toss on animation when scene is loaded
//  	std::vector<sPhysicsProperties*> physVec = m_pThePhysics->getPhysicsVec();
// 	for (sPhysicsProperties* currObj : physVec)
// 	{
// 		m_pAnimationsManager->AddAnimationObj(currObj);
// 		if (currObj->getUniqueID() == 2)
// 			currObj->scale = glm::vec3(2); // Lazy way to make bigger
// 	}

	//physVec.push_back(m_TheCamera); // Add camera
 	//m_pLuaBrain->setPhysVec(physVec);

	return;
}

void cEngineController_IMPL::resetScene(std::vector<cMesh*> newMeshVec, std::vector<cLight> newLights, std::vector<sPhysicsProperties*> newPhysVec)
{
	m_pTheGraphics->switchScene(newMeshVec, newLights);
	m_pThePhysics->switchScenes(newPhysVec);

	// Will have to reset audio too whenever that gets used

	return;
}

void cEngineController_IMPL::deleteObject(int ID)
{
	m_pThePhysics->DeleteShape(ID);
	m_pTheGraphics->removeFromDrawMesh(ID);
	return;
}

