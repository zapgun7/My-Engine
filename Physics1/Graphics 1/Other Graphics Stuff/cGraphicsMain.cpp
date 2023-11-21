#include "cGraphicsMain.h"

#include<iostream>
#include <fstream>
#include <random>


#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "GLWF_CallBacks.h"

#include "../Physics/cPhysics.h"





cGraphicsMain* cGraphicsMain::m_pTheOnlyGraphicsMain = NULL;
extern cPhysics* g_pPhysics;


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}


cGraphicsMain* cGraphicsMain::getGraphicsMain(void) // Making graphics main a singleton
{
	if (cGraphicsMain::m_pTheOnlyGraphicsMain == NULL)
	{
		cGraphicsMain::m_pTheOnlyGraphicsMain = new cGraphicsMain();
		if (!cGraphicsMain::m_pTheOnlyGraphicsMain->Initialize())
		{
			cGraphicsMain::m_pTheOnlyGraphicsMain->Destroy();
		}
	}
	return cGraphicsMain::m_pTheOnlyGraphicsMain;
}

cGraphicsMain::cGraphicsMain()
{
	m_cameraEye = glm::vec3(10.0f, 5.0f, 10.0f);
	m_cameraTarget = glm::vec3(-1.0f, -0.2f, -1.0f);
	m_cameraRotation = glm::vec3(0.0, 0.0f, 0.0f);
	m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_ShowLightEditor = false;
	m_ShowMeshEditor = false;
	//m_player = new cPlayer();
	//m_io = ImGui::GetIO();
}

bool cGraphicsMain::Initialize()
{
	m_InputHandler = new cInputHandler();
	m_pSceneManager = new cSceneManagement();
	m_pSceneManager->Initialize();


	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	m_window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwSetKeyCallback(m_window, key_callback);

	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


	m_pShaderThing = new cShaderManager();
	m_pShaderThing->setBasePath("assets/shaders");

	cShaderManager::cShader vertexShader;
	vertexShader.fileName = "vertexShader01.glsl";

	cShaderManager::cShader fragmentShader;
	fragmentShader.fileName = "fragmentShader01.glsl";

	if (!m_pShaderThing->createProgramFromFile("shader01", vertexShader, fragmentShader))
	{
		std::cout << "Error: Couldn't compile or link:" << std::endl;
		std::cout << m_pShaderThing->getLastError();
		return -1;
	}


	//
	m_shaderProgramID = m_pShaderThing->getIDFromFriendlyName("shader01");

	m_pMeshManager = new cVAOManager();
	::g_pPhysics->setVAOManager(m_pMeshManager);

	m_pMeshManager->setBasePath("assets/models");

	m_pTheLights = new cLightManager();
	m_pTheLights->SetUniformLocations(m_shaderProgramID);


	// MODEL LOADING /////////////////

	LoadModels();


	// Initialize lights here if ya want em
	m_lastTime = glfwGetTime();

	//ImGui setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = ImGui::GetIO(); (void)m_io;
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	// Our state
	//bool show_credits_window = false;



	// Load in default player object
// 	cMesh* meshToAdd = new cMesh();
// 	meshToAdd->meshName = "Sphere_1_unit_Radius.ply"; // Set object type
// 	meshToAdd->friendlyName = "Player";
// 	meshToAdd->bDoNotLight = true;
// 
// 	m_vec_pMeshesToDraw.push_back(meshToAdd);
// 
// 	// Create the physics object
// 	sPhsyicsProperties* newShape = new sPhsyicsProperties();
// 	newShape->shapeType = sPhsyicsProperties::SPHERE;
// 	newShape->setShape(new sPhsyicsProperties::sSphere(1.0f)); // Since a unit sphere, radius of .5 
// 	newShape->pTheAssociatedMesh = meshToAdd;
// 	newShape->inverse_mass = 1.0f; // Idk what to set this
// 	newShape->friendlyName = "Sphere";
// 	newShape->acceleration.y = -20.0f;
// 	newShape->position = glm::vec3(0, 10, 0);
// 	::g_pPhysics->AddShape(newShape);
// 	m_player->setAssociatedPhysObj(newShape);
// 
// 	meshToAdd->uniqueID = newShape->getUniqueID();

	return 1;
}






bool cGraphicsMain::Update() // Main "loop" of the window. Not really a loop, just gets called every tick
{

	//srand(time(NULL)); // better random

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	static double lastTime = glfwGetTime();
	double deltaTime = glfwGetTime() - lastTime;
	lastTime = glfwGetTime();


	static bool enablePhysics = false; // Toggle physics update calls
	//static bool isPlayer = false; // Toggle between player control and freecam


	float const velRandInterval = 0.1f;
	static double timeTillVelRand = velRandInterval;

	timeTillVelRand -= deltaTime;


	// Scary Balls
	if (timeTillVelRand <= 0)
	{
		timeTillVelRand += velRandInterval;
		for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++)
		{
			//std::string test = m_vec_pMeshesToDraw[i]->friendlyName.substr(0, 4);
			if (m_vec_pMeshesToDraw[i]->friendlyName.substr(0, 5) == "Scary") // If the type to fling around
			{
				if (rand() % 30 == 0)
				{
					sPhsyicsProperties* obj = ::g_pPhysics->findShapeByUniqueID(m_vec_pMeshesToDraw[i]->uniqueID);
					obj->velocity.x = rand() % 50;
					obj->velocity.z = rand() % 50;
					//obj->velocity.y = rand() % 50;
					if (rand() % 2 == 0)
						obj->velocity.x *= -1;
					if (rand() % 2 == 0)
						obj->velocity.z *= -1;
				}
			}
		}
	}

	// Teleport obj back up to 0,0

	for (cMesh* mesh : m_vec_pMeshesToDraw)
	{
		sPhsyicsProperties* obj = ::g_pPhysics->findShapeByUniqueID(mesh->uniqueID);
		if (obj->position.y < -300)
		{
			sPhsyicsProperties* obj = ::g_pPhysics->findShapeByUniqueID(mesh->uniqueID);
			obj->velocity = glm::vec3(0);
			obj->position = glm::vec3(0, 20, 0);
			obj->oldPosition = glm::vec3(0, 20, 0);
		}
	}




	// Graphics update will find a better spot for it later TODO
	::g_pPhysics->Update(deltaTime); 
	//::g_pPhysics->Update(0.001f); // DEBUG



// 	int tempVal = glfwGetKey(m_window, GLFW_KEY_P);
// 	static bool isPressed = false;
// 	if ((tempVal == GLFW_PRESS) && (!isPressed))
// 	{
// 		::g_pPhysics->Update(0.5f);
// 		isPressed = true;
// 	}
// 	else if (tempVal != GLFW_PRESS)
// 		isPressed = false;

	// Check input for camera movement
	m_InputHandler->queryKeys(m_window);
	std::vector<std::string> availSaves;

	// ---------------------------IMGUI WINDOWS---------------------------------------//
	static int mesh_obj_idx = 0;
	{
		ImGui::Begin("Main Editor Window");

		static int available_obj_idx = 0;
		if (ImGui::BeginListBox("Available Objects"))
		{
			for (int n = 0; n < m_AvailableModels.size(); n++)
			{
				const bool is_selected = (available_obj_idx == n);
				if (ImGui::Selectable(m_AvailableModels[n].c_str(), is_selected))
					available_obj_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		static char buf1[32] = ""; ImGui::InputText("Object Name", buf1, 32);
		ImGui::SameLine();
		if (ImGui::Button("AddObject")) // Button to add new object to the scene
		{
			if (std::strlen(buf1) > 0)
			{
				addNewMesh(m_AvailableModels[available_obj_idx], buf1);
				memset(buf1, 0, 32);
				mesh_obj_idx = m_vec_pMeshesToDraw.size() - 1;
			}
		}

		if (ImGui::Button("Physics Toggle"))
		{
			if (enablePhysics)
				enablePhysics = false;
			else
				enablePhysics = true;
			::g_pPhysics->setPhysicsRunningState(enablePhysics);
		}

		ImGui::SameLine();
		if (enablePhysics)
			ImGui::Text("ON    ");
		else
			ImGui::Text("OFF   ");


// 		ImGui::SameLine();
// 		if (ImGui::Button("Player Toggle"))
// 		{
// 			if (isPlayer)
// 				isPlayer = false;
// 			else
// 				isPlayer = true;
// 		}
// 		ImGui::SameLine();
// 		if (isPlayer)
// 			ImGui::Text("Player");
// 		else
// 			ImGui::Text("FreeCam");

		if (ImGui::Button("Mesh Editor"))
		{
			if (m_ShowMeshEditor)
				m_ShowMeshEditor = false;
			else
				m_ShowMeshEditor = true;
		}
		if (ImGui::Button("Light Editor"))
		{
			if (m_ShowLightEditor)
				m_ShowLightEditor = false;
			else
				m_ShowLightEditor = true;
		}
		if (ImGui::Button("Scene Manager"))
		{
			if (m_ShowSceneManager)
				m_ShowSceneManager = false;
			else
				m_ShowSceneManager = true;
		}
		ImGui::Separator();
		if (ImGui::Button("Faster"))
		{
			m_FlyCamSpeed += 0.1f;
		}
		ImGui::Text(std::to_string(m_FlyCamSpeed).c_str());
		if (ImGui::Button("Slower"))
		{
			if (m_FlyCamSpeed > 0.1f)
			m_FlyCamSpeed -= 0.1f;
		}



		ImGui::End();
	}


	if (m_ShowMeshEditor)
	{
		ImGui::Begin("Mesh Editor");

		if (ImGui::BeginListBox("Available Objects")) // List of active meshes
		{
			for (int n = 0; n < m_vec_pMeshesToDraw.size(); n++)
			{
				const bool is_selected = (mesh_obj_idx == n);
				if (ImGui::Selectable(m_vec_pMeshesToDraw[n]->friendlyName.c_str(), is_selected))
					mesh_obj_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		if (mesh_obj_idx >= m_vec_pMeshesToDraw.size()) // When changing scenes, this makes sure it doesn't go outta bounds
			mesh_obj_idx = 0;
		bool isExistingMesh = false; // Assert we have at least one mesh
		if (m_vec_pMeshesToDraw.size() > 0)
			isExistingMesh = true;

		static bool doNotLight = false;
		if (isExistingMesh)
			doNotLight = m_vec_pMeshesToDraw[mesh_obj_idx]->bDoNotLight;

		/////////// DUPLICATE MESH /////////////
		static char dupeName[32] = ""; ImGui::InputText(":)", dupeName, 32);
		ImGui::SameLine();
		if (ImGui::Button("Duplicate"))
		{
			if ((isExistingMesh) && (std::strlen(dupeName) > 0))
			{
				duplicateMesh(mesh_obj_idx, dupeName);
				mesh_obj_idx++; // Have new duplicate selected
			}
		}
		ImGui::SameLine();
		ImGui::Checkbox("doNotLight", &doNotLight);

		static bool toggalAllLight = false;
		if (toggalAllLight)
		{
			if (ImGui::Button("AllMeshLighting: On"))
			{
				for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++)
					m_vec_pMeshesToDraw[i]->bDoNotLight = true;
				toggalAllLight = false;
			}
		}
		else
		{
			if (ImGui::Button("AllMeshLighting: Off"))
			{
				for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++)
					m_vec_pMeshesToDraw[i]->bDoNotLight = false;
				toggalAllLight = true;
			}
		}



		float xPos = 0;
		float yPos = 0;
		float zPos = 0;
		float xOri = 0;
		float yOri = 0;
		float zOri = 0;
		float scale = 0;
		glm::vec3 customColor(0.0f, 0.0f, 0.0f);
		bool useCustomColor;


		if (isExistingMesh)
		{
			// TODO   this is fine for now, but we should be getting the values from the physics object instead. They should always match at this stage but who the hell knows?
			xPos = m_vec_pMeshesToDraw[mesh_obj_idx]->drawPosition.x;
			yPos = m_vec_pMeshesToDraw[mesh_obj_idx]->drawPosition.y;
			zPos = m_vec_pMeshesToDraw[mesh_obj_idx]->drawPosition.z;
			glm::vec3 meshEulerOri = m_vec_pMeshesToDraw[mesh_obj_idx]->getEulerOrientation();
			xOri = meshEulerOri.x;
			yOri = meshEulerOri.y;
			zOri = meshEulerOri.z;
			scale = m_vec_pMeshesToDraw[mesh_obj_idx]->scale.x;
			customColor = glm::vec3(m_vec_pMeshesToDraw[mesh_obj_idx]->wholeObjectDebugColourRGBA);
			useCustomColor = m_vec_pMeshesToDraw[mesh_obj_idx]->bUseDebugColours;
		}

		ImGui::SeparatorText("Position");
		ImGui::DragFloat("X", &xPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Y", &yPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Z", &zPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");

		ImGui::SeparatorText("Orientation");
		ImGui::DragFloat("X-Rotation", &xOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Y-Rotation", &yOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Z-Rotation", &zOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");

		ImGui::SeparatorText("Scale");
		ImGui::DragFloat("Scale", &scale, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");

		ImGui::SeparatorText("Custom Colors");
		ImGui::DragFloat("Red", &customColor.x, 0.005f, 0.0f, 1.0f, "%.3f");
		ImGui::DragFloat("Green", &customColor.y, 0.005f, 0.0f, 1.0f, "%.3f");
		ImGui::DragFloat("Blue", &customColor.z, 0.005f, 0.0f, 1.0f, "%.3f");
		ImGui::Checkbox("Use Custom Color", &useCustomColor);

		ImGui::Separator();
		if (ImGui::Button("Delete"))
		{
			if (isExistingMesh)
			{
				deleteMesh(mesh_obj_idx);
				mesh_obj_idx--;
				isExistingMesh = false;
			}
		}
		ImGui::SameLine();
		ImGui::Text("  Deletes currently selected mesh");
		
		// List all object attributes the user is able to edit

		if (isExistingMesh)
		{
			glm::vec3 newPos = glm::vec3(xPos, yPos, zPos);
			glm::vec3 newOri = glm::vec3(xOri, yOri, zOri);
			updateSelectedMesh(mesh_obj_idx, "A NEW FRIENDLY NAME", newPos, newOri, customColor, scale, doNotLight, useCustomColor);
		}
		ImGui::End();
	}

	if (m_ShowLightEditor)
	{
		ImGui::Begin("Light Editor");

		static int light_obj_idx = 0;
		if (ImGui::BeginListBox("Available Objects"))
		{
			for (int n = 0; n < m_pTheLights->NUMBER_OF_LIGHTS_IM_USING; n++)
			{
				const bool is_selected = (light_obj_idx == n);
				if (ImGui::Selectable(m_pTheLights->theLights[n].friendlyName.c_str(), is_selected))
					light_obj_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
// 		bool isExistingLight = true;
// 		if (m_vec_pMeshesToDraw.size() > 0)
// 			isExistingLight = false;


		glm::vec4 lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 lightDir = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 lightDiff = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 lightSpec = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 lightAtten = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 lightParam1 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 lightParam2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
// 		if (!isExistingLight)
// 		{
			lightPos = m_pTheLights->theLights[light_obj_idx].position;
			lightDir = m_pTheLights->theLights[light_obj_idx].direction;
			lightDiff = m_pTheLights->theLights[light_obj_idx].diffuse;
			lightSpec = m_pTheLights->theLights[light_obj_idx].specular; // rgb = highlight colour, w = power
			lightAtten = m_pTheLights->theLights[light_obj_idx].atten; // x = constant, y = linear, z = quadratic, w = DistanceCutOff
			lightParam1 = m_pTheLights->theLights[light_obj_idx].param1; // x: light type    y: inner angle    z: outer angle
			lightParam2 = m_pTheLights->theLights[light_obj_idx].param2; // x: light on(1) or off(0)
		/*}*/
		static char lightname[32] = ""; 
		//strcpy_s(lightname, m_pTheLights->theLights[light_obj_idx].friendlyName.c_str()); // TODO too long a name will prob break this

		ImGui::InputText("Light Name", lightname, 32);
		if (ImGui::Button("Set New Name")) // Button to set new light friendlyname
		{
			if (strlen(lightname) > 0)
				m_pTheLights->theLights[light_obj_idx].friendlyName = lightname;
		}

		ImGui::SeparatorText("Position");
		ImGui::DragFloat("X-Pos", &lightPos.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Y-Pos", &lightPos.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Z-Pos", &lightPos.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::SeparatorText("Direction");
		ImGui::DragFloat("X-Dir", &lightDir.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Y-Dir", &lightDir.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Z-Dir", &lightDir.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::SeparatorText("Spotlight Cone");
		ImGui::DragFloat("Inner Angle", &lightParam1.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat("Outer Angle", &lightParam1.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::SeparatorText("Diffuse");
		ImGui::DragFloat("Red Diffuse", &lightDiff.x, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Green Diffuse", &lightDiff.y, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Blue Diffuse", &lightDiff.z, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::SeparatorText("Specular");
		ImGui::DragFloat("Red Specular", &lightSpec.x, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Green Specular", &lightSpec.y, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Blue Specular", &lightSpec.z, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Spec Power", &lightSpec.w, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::SeparatorText("Attenuation");
		ImGui::DragFloat("Constant", &lightAtten.x, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Linear", &lightAtten.y, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Quadratic", &lightAtten.z, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
		ImGui::DragFloat("Distance Cutoff", &lightAtten.w, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");

		ImGui::SeparatorText("Other Light Options");
		const char* lightTypes[] = { "Point Light", "Spot Light", "Directional Light"};
		static int ltype_current_idx = 0;
		ltype_current_idx = lightParam1.x; // Set selected light type to one stored in the light
		const char* combo_preview_value = lightTypes[ltype_current_idx];
		if (ImGui::BeginCombo("Light Types", combo_preview_value))
		{
			for (int n = 0; n < IM_ARRAYSIZE(lightTypes); n++)
			{
				const bool is_selected = (ltype_current_idx == n);
				if (ImGui::Selectable(lightTypes[n], is_selected))
					ltype_current_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		lightParam1.x = ltype_current_idx; // Set light type
		ImGui::SameLine();
		static bool lightOn = true;
		if (lightParam2.x == 0)
			lightOn = false;
		else
			lightOn = true;

		ImGui::Checkbox("Toggle Light", &lightOn);
		if (lightOn)
			lightParam2.x = 1;
		else
			lightParam2.x = 0;


// 		if (isExistingLight)
// 		{
			updateSelectedLight(light_obj_idx, lightPos, lightDiff, lightSpec, lightAtten, lightDir, lightParam1, lightParam2);
		/*}*/

		ImGui::End();
	}

	if (m_ShowSceneManager)
	{
		ImGui::Begin("Scene Manager");

		availSaves = m_pSceneManager->getAvailableSaves(); // Update availible saves upon opening scene manager window
		static int saves_idx = 0;
		if (ImGui::BeginListBox("Available Saves")) // List of availible saves to load from
		{
			for (int n = 0; n < availSaves.size(); n++)
			{
				const bool is_save_selected = (saves_idx == n);
				if (ImGui::Selectable(availSaves[n].c_str(), is_save_selected))
					saves_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_save_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Load Scene"))
		{
			if (availSaves.size() > 0)
				m_pSceneManager->loadScene(availSaves[saves_idx]); // Will load a selected item from a list in the future
		}


		static char saveNameBuf[32] = ""; ImGui::InputText("Save Name", saveNameBuf, 32);
		ImGui::SameLine();
		if (ImGui::Button("Save Current Scene"))
		{
			if (std::strlen(saveNameBuf) > 0)
			{
				m_pSceneManager->saveScene(saveNameBuf, m_vec_pMeshesToDraw, m_pTheLights);
				memset(saveNameBuf, 0, 32); // Reset buffer
			}
			
		}

		ImGui::End();
	}



	ImGui::Render();
	// ---------------------------IMGUI END-------------------------------------------//




	float ratio;
	int width, height;

	glUseProgram(m_shaderProgramID);

	glfwGetFramebufferSize(m_window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	glCullFace(GL_BACK);


	// *****************************************************************
	// if ya want lights
	m_pTheLights->UpdateUniformValues(m_shaderProgramID);


	// *****************************************************************
			//uniform vec4 eyeLocation;

// 	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Lazy way to exit player mode
// 	{
// 		isPlayer = false;
// 		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
// 	}


	//std::vector<glm::vec3> newCamInfo;

	//if (!isPlayer)
	flyCameraInput(width, height); // UPDATE CAMERA STATS
// 	else
// 	{
// 		newCamInfo = m_player->Update(deltaTime, m_window, m_cameraEye, m_cameraTarget, m_cameraRotation);
// 
// 		m_cameraEye = newCamInfo[0];
// 		m_cameraTarget = newCamInfo[1];
// 		m_cameraRotation = newCamInfo[2];
// 		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
// 	}




	GLint eyeLocation_UL = glGetUniformLocation(m_shaderProgramID, "eyeLocation");
	glUniform4f(eyeLocation_UL,
		m_cameraEye.x, m_cameraEye.y, m_cameraEye.z, 1.0f);



	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 matProjection = glm::perspective(0.7f,
		ratio,
		0.01f,
		1000.0f); // n/f plane



	//glm::quat povRotation = glm::quat(glm::vec3(m_cameraForwardRotation));


	//m_cameraTarget = m_cameraEye + m_cameraTarget;
	//std::cout << "X: " << m_cameraTarget.x << " Y: " << m_cameraTarget.y << " Z: " << m_cameraTarget.z << std::endl;
	
	glm::mat4 matView = glm::lookAt(m_cameraEye,
		m_cameraEye + m_cameraTarget,
		m_upVector);

	//glm::quat povRotation = glm::quat(m_cameraForwardRotation);
	//matView *= glm::mat4(povRotation);

	GLint matProjection_UL = glGetUniformLocation(m_shaderProgramID, "matProjection");
	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

	GLint matView_UL = glGetUniformLocation(m_shaderProgramID, "matView");
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

	// *********************************************************************
	// Draw all the objects
	for (unsigned int index = 0; index != m_vec_pMeshesToDraw.size(); index++) // Prob black or smthn
	{
		cMesh* pCurrentMesh = m_vec_pMeshesToDraw[index];

		if (pCurrentMesh->bIsVisible)
		{

			glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

			DrawObject(pCurrentMesh, matModel, m_shaderProgramID);
		}//if (pCurrentMesh->bIsVisible)

	}//for ( unsigned int index

	// Time per frame (more or less)
// 	double currentTime = glfwGetTime();
// 	double deltaTime = currentTime - m_lastTime;
// 	//        std::cout << deltaTime << std::endl;
// 	m_lastTime = currentTime;

	glfwPollEvents();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);

	if (glfwWindowShouldClose(m_window))
		return -1;
	else
		return 0;
}



void cGraphicsMain::Destroy()
{

	glfwDestroyWindow(m_window);
	glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	exit(EXIT_SUCCESS);
}

// Creates a new mesh to bind to an object; returns a pointer which the arena can associate to specific objects to animate them


// void cGraphicsMain::addToDrawMesh(cMesh* newMesh)
// {
// 	m_vec_pMeshesToDraw.push_back(newMesh);
// 	return;
// }

void cGraphicsMain::removeFromDrawMesh(int ID) // Shouldn't be used as it doesn't delete the physics obj
{
	for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++)
	{
		if (m_vec_pMeshesToDraw[i]->uniqueID == ID)
		{
			delete m_vec_pMeshesToDraw[i]; // Prob breaks it
			m_vec_pMeshesToDraw.erase(m_vec_pMeshesToDraw.begin() + i);
			return;
		}
	}
}

// Will replace all meshes and lights with the ones provided
void cGraphicsMain::switchScene(std::vector< cMesh* > newMeshVec, std::vector<cLight> newLights) // TODO have to load in physics side of objects
{
	for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++) // Delete all pointers to meshes
	{
		delete m_vec_pMeshesToDraw[i];
	}
	m_vec_pMeshesToDraw = newMeshVec; // Set new mesh vector

	// Delete all current physics objects
	::g_pPhysics->deleteAllObjects();


	for (cMesh* meshObj : m_vec_pMeshesToDraw) // Attach physics objects to all new objects
	{
		sPhsyicsProperties* newShape;
		if (meshObj->meshName == "Sphere_1_unit_Radius.ply")
		{
			newShape = new sPhsyicsProperties();
			newShape->shapeType = sPhsyicsProperties::SPHERE;
			newShape->setShape(new sPhsyicsProperties::sSphere(1.0f)); // Since a unit sphere, radius of .5 
			newShape->pTheAssociatedMesh = meshObj;
			newShape->inverse_mass = 1.0f; // Idk what to set this
			newShape->friendlyName = "Sphere";
			newShape->acceleration.y = -20.0f;
			newShape->position = meshObj->drawPosition;
			newShape->oldPosition = meshObj->drawPosition;
			newShape->restitution = 0.5f;
			::g_pPhysics->AddShape(newShape);
		}
		else // Just make it an indirect triangle mesh
		{
			newShape = new sPhsyicsProperties();
			newShape->shapeType = sPhsyicsProperties::MESH_OF_TRIANGLES_INDIRECT;
			newShape->setShape(new sPhsyicsProperties::sMeshOfTriangles_Indirect(meshObj->meshName));
			newShape->pTheAssociatedMesh = meshObj;
			newShape->inverse_mass = 0.0f; // Idk what to set this
			newShape->friendlyName = "IndirectMesh";
			newShape->setRotationFromEuler(meshObj->getEulerOrientation());
			newShape->position = meshObj->drawPosition;
			newShape->oldPosition = meshObj->drawPosition;
			::g_pPhysics->AddShape(newShape);
		}
		meshObj->uniqueID = newShape->getUniqueID(); // Set mesh ID to match associated physics object's ID
	}

	// Load in default player object
// 	cMesh* meshToAdd = new cMesh();
// 	meshToAdd->meshName = "Sphere_1_unit_Radius.ply"; // Set object type
// 	meshToAdd->friendlyName = "Player";
// 	meshToAdd->bDoNotLight = true;
// 
// 	m_vec_pMeshesToDraw.push_back(meshToAdd);
// 
// 	// Create the physics object
// 	sPhsyicsProperties* newShape = new sPhsyicsProperties();
// 	newShape->shapeType = sPhsyicsProperties::SPHERE;
// 	newShape->setShape(new sPhsyicsProperties::sSphere(1.0f)); // Since a unit sphere, radius of .5 
// 	newShape->pTheAssociatedMesh = meshToAdd;
// 	newShape->inverse_mass = 1.0f; // Idk what to set this
// 	newShape->friendlyName = "Player";
// 	newShape->acceleration.y = -20.0f;
// 	newShape->position = glm::vec3(0, 10, 0);
// 	::g_pPhysics->AddShape(newShape);
// 	m_player->setAssociatedPhysObj(newShape);
// 
// 	meshToAdd->uniqueID = newShape->getUniqueID();


	for (unsigned int i = 0; i < m_pTheLights->NUMBER_OF_LIGHTS_IM_USING; i++) // Iterate through all lights and replace them with the new ones. Just replace non UL values
	//for (unsigned int i = 0; i < newLights.size(); i++) // Use this for updating files that contain less (total possible) than what it currently is
	{
		m_pTheLights->theLights[i].friendlyName = newLights[i].friendlyName;
		m_pTheLights->theLights[i].position = newLights[i].position;
		m_pTheLights->theLights[i].diffuse = newLights[i].diffuse;
		m_pTheLights->theLights[i].specular = newLights[i].specular;
		m_pTheLights->theLights[i].atten = newLights[i].atten;
		m_pTheLights->theLights[i].direction = newLights[i].direction;
		m_pTheLights->theLights[i].param1 = newLights[i].param1;
		m_pTheLights->theLights[i].param2 = newLights[i].param2;
	}
}



void cGraphicsMain::setCameraParams(glm::vec3 camPos, glm::vec3 camDir)
{
	m_cameraEye = camPos;
	m_cameraTarget = camDir;
	return;
}

cMesh* cGraphicsMain::m_pFindMeshByFriendlyName(std::string friendlyNameToFind)
{
	for (unsigned int index = 0; index != m_vec_pMeshesToDraw.size(); index++)
	{
		if (m_vec_pMeshesToDraw[index]->friendlyName == friendlyNameToFind)
		{
			// Found it
			return m_vec_pMeshesToDraw[index];
		}
	}
	// Didn't find it
	return NULL;
}

void cGraphicsMain::DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID)
{

	//         mat4x4_identity(m);
	glm::mat4 matModel = matModelParent;



	// Translation
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
		glm::vec3(pCurrentMesh->drawPosition.x,
			pCurrentMesh->drawPosition.y,        
			pCurrentMesh->drawPosition.z));


	// Rotation matrix generation
// 	glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f),
// 		pCurrentMesh->orientation.x, // (float)glfwGetTime(),
// 		glm::vec3(1.0f, 0.0, 0.0f));
// 
// 
// 	glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
// 		pCurrentMesh->orientation.y, // (float)glfwGetTime(),
// 		glm::vec3(0.0f, 1.0, 0.0f));
// 
// 	glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
// 		pCurrentMesh->orientation.z, // (float)glfwGetTime(),
// 		glm::vec3(0.0f, 0.0, 1.0f));


	// Quaternion Rotation
	glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());


	// Scaling matrix
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		pCurrentMesh->scale);
	//--------------------------------------------------------------

	// Combine all these transformation
	matModel = matModel * matTranslate;

// 	matModel = matModel * matRotateX;
// 	matModel = matModel * matRotateY;
// 	matModel = matModel * matRotateZ;

	matModel = matModel * matRotation;

	matModel = matModel * matScale;

	//        m = m * rotateZ;
	//        m = m * rotateY;
	//        m = m * rotateZ;



	   //mat4x4_mul(mvp, p, m);
	//    glm::mat4 mvp = matProjection * matView * matModel;

	//    GLint mvp_location = glGetUniformLocation(shaderProgramID, "MVP");
	//    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
	//    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint matModel_UL = glGetUniformLocation(shaderProgramID, "matModel");
	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));


	// Also calculate and pass the "inverse transpose" for the model matrix
	glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

	// uniform mat4 matModel_IT;
	GLint matModel_IT_UL = glGetUniformLocation(shaderProgramID, "matModel_IT");
	glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));


	if (pCurrentMesh->bIsWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//        glPointSize(10.0f);


			// uniform bool bDoNotLight;
	GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

	if (pCurrentMesh->bDoNotLight)
	{
		// Set uniform to true
		glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
	}
	else
	{
		// Set uniform to false;
		glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
	}

	//uniform bool bUseDebugColour;	
	GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramID, "bUseDebugColour");
	if (pCurrentMesh->bUseDebugColours)
	{
		glUniform1f(bUseDebugColour_UL, (GLfloat)GL_TRUE);
		//uniform vec4 debugColourRGBA;
		GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramID, "debugColourRGBA");
		glUniform4f(debugColourRGBA_UL,
			pCurrentMesh->wholeObjectDebugColourRGBA.r,
			pCurrentMesh->wholeObjectDebugColourRGBA.g,
			pCurrentMesh->wholeObjectDebugColourRGBA.b,
			pCurrentMesh->wholeObjectDebugColourRGBA.a);
	}
	else
	{
		glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);
	}



	sModelDrawInfo modelInfo;
	if (m_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo))
	{
		// Found it!!!

		glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)
		glDrawElements(GL_TRIANGLES,
			modelInfo.numberOfIndices,
			GL_UNSIGNED_INT,
			0);
		glBindVertexArray(0); 			            // disable VAO (and everything else)

	}

	return;
}

// Loads in all models that are available to us into the VAO
bool cGraphicsMain::LoadModels(void)
{
	sModelDrawInfo modelDrawingInfo;
// 	m_pMeshManager->LoadModelIntoVAO("bathtub_xyz_n_rgba.ply",
// 		modelDrawingInfo, m_shaderProgramID);
// 	std::cout << "Loaded: " << modelDrawingInfo.numberOfVertices << " vertices" << std::endl;
// 	m_AvailableModels.push_back("bathtub_xyz_n_rgba.ply");

	std::ifstream modelsToLoad("assets/models/models.txt");
	if (!modelsToLoad.is_open())
	{
		// didn't open :(
		std::cout << "ERROR: Failed to open the model list file!" << std::endl;
		std::cout << modelsToLoad.is_open();
	}
	std::string line = "";

	while (std::getline(modelsToLoad, line))
	{
		m_pMeshManager->LoadModelIntoVAO(line.c_str(),
			modelDrawingInfo, m_shaderProgramID);
		std::cout << "Loaded: " << modelDrawingInfo.numberOfVertices << " vertices" << std::endl;
		m_AvailableModels.push_back(line.c_str());
	}

	modelsToLoad.close();


	return true;
}

// Adds new object to the meshestodraw
void cGraphicsMain::addNewMesh(std::string fileName, char* friendlyName) // This should create the physics object first, then the mesh, then set associated mesh
{

	// Create the mesh
	cMesh* meshToAdd = new cMesh();
	meshToAdd->meshName = fileName; // Set object type
	meshToAdd->friendlyName = friendlyName;
	meshToAdd->bDoNotLight = true;
	

	m_vec_pMeshesToDraw.push_back(meshToAdd);
	//return;
	
	// Create the physics object

	sPhsyicsProperties* newShape = new sPhsyicsProperties();
	if (fileName == "Sphere_1_unit_Radius.ply")
	{
		newShape->shapeType = sPhsyicsProperties::SPHERE;
		newShape->setShape(new sPhsyicsProperties::sSphere(1.0f)); // Since a unit sphere, radius of .5 
		newShape->pTheAssociatedMesh = meshToAdd;
		newShape->inverse_mass = 1.0f; // Idk what to set this
		newShape->friendlyName = "Sphere";
		newShape->acceleration.y = -20.0f;
		newShape->restitution = 0.5f;
		::g_pPhysics->AddShape(newShape);
	}
// 	else if (fileName == "Flat_1x1_plane.ply")
// 	{
// 		// Add matching physics object
// 		newShape->shapeType = sPhsyicsProperties::PLANE;
// 
// 		//    pGroundMeshShape->setShape( new sPhsyicsProperties::sMeshOfTriangles_Indirect("HilbertRamp_stl (rotated).ply") );
// 		//newShape->setShape(new sPhsyicsProperties::sMeshOfTriangles_Indirect(meshToAdd->meshName));
// 		newShape->setShape(new sPhsyicsProperties::sPlane(glm::vec3(0, 1, 0))); // TODO calculate the actual normal later
// 
// 		// Tie this phsyics object to the associated mesh
// 		newShape->pTheAssociatedMesh = meshToAdd;
// 		// If it's infinite, the physics intrator ignores it
// 		newShape->inverse_mass = 0.0f;  // Infinite, so won't move
// 
// 		//    pGroundMeshShape->acceleration.y = (-9.81f / 5.0f);
// 
// 		//    pGroundMeshShape->position.x = -10.0f;
// 		newShape->position.y = -50.0f;
// 		//    pGroundMeshShape->orientation.z = glm::radians(-45.0f);
// 		newShape->friendlyName = "Plane";
// 		::g_pPhysics->AddShape(newShape);
// 	}
	else // Just make it an indirect triangle mesh
	{
		newShape->shapeType = sPhsyicsProperties::MESH_OF_TRIANGLES_INDIRECT;
		newShape->setShape(new sPhsyicsProperties::sMeshOfTriangles_Indirect(meshToAdd->meshName));
		newShape->pTheAssociatedMesh = meshToAdd;
		newShape->inverse_mass = 0.0f; // Idk what to set this
		newShape->friendlyName = "Plane";
		newShape->setRotationFromEuler(glm::vec3(0.0f));
 		::g_pPhysics->AddShape(newShape);
	}
	meshToAdd->uniqueID = newShape->getUniqueID(); // Set mesh ID to match associated physics object's ID

	return;
}

// Updates values of selected object from the gui
void cGraphicsMain::updateSelectedMesh(int meshIdx, std::string friendlyName, glm::vec3 newPos, glm::vec3 newOri, glm::vec3 customColor, float newScale, bool doNotLight, bool useCustomColor) 
{
	// pos and ori need to update the physics object

	::g_pPhysics->setShapePos(newPos, m_vec_pMeshesToDraw[meshIdx]->uniqueID);
	::g_pPhysics->setShapeOri(newOri, m_vec_pMeshesToDraw[meshIdx]->uniqueID);

	//m_vec_pMeshesToDraw[meshIdx]->drawPosition = newPos;
	//m_vec_pMeshesToDraw[meshIdx]->eulerOrientation = newOri;
	//glm::vec3 oldOri = m_vec_pMeshesToDraw[meshIdx]->getEulerOrientation();
	//glm::vec3 deltaOri = newOri - oldOri;
	//m_vec_pMeshesToDraw[meshIdx]->adjustRotationAngleFromEuler(deltaOri);
	//m_vec_pMeshesToDraw[meshIdx]->adjustRotationAngleFromEuler(glm::vec3(0.0f, 0.0f, 0.01f));
	m_vec_pMeshesToDraw[meshIdx]->scale = glm::vec3(newScale, newScale, newScale);
	m_vec_pMeshesToDraw[meshIdx]->bDoNotLight = doNotLight;
	m_vec_pMeshesToDraw[meshIdx]->wholeObjectDebugColourRGBA = glm::vec4(customColor, 1);
	m_vec_pMeshesToDraw[meshIdx]->bUseDebugColours = useCustomColor;
}

void cGraphicsMain::addNewLight(char* friendlyName)
{
	int newLightIdx = m_pTheLights->nextLightIdx++;
	m_pTheLights->theLights[newLightIdx].param2.x = 1.0f; // Turn light on
	m_pTheLights->theLights[newLightIdx].friendlyName = friendlyName; // Name the light
}

void cGraphicsMain::updateSelectedLight(int lightIdx, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2)
{
	m_pTheLights->theLights[lightIdx].position = newPos;
	m_pTheLights->theLights[lightIdx].diffuse = newDiff;
	m_pTheLights->theLights[lightIdx].specular = newSpec;
	m_pTheLights->theLights[lightIdx].atten = newAtten;
	m_pTheLights->theLights[lightIdx].direction = newDir;
	m_pTheLights->theLights[lightIdx].param1 = newParam1;
	m_pTheLights->theLights[lightIdx].param2 = newParam2;

	return;
}

void cGraphicsMain::duplicateMesh(int meshIdx, char* newName) // TODO also duplicate physics properties
{
	cMesh* dupedMesh = new cMesh();
	cMesh* meshToCopy = m_vec_pMeshesToDraw[meshIdx];

	dupedMesh->meshName = meshToCopy->meshName;
	dupedMesh->friendlyName = newName;
	dupedMesh->drawPosition = meshToCopy->drawPosition;
	dupedMesh->eulerOrientation = meshToCopy->eulerOrientation;
	dupedMesh->wholeObjectDebugColourRGBA = meshToCopy->wholeObjectDebugColourRGBA;
	dupedMesh->scale = meshToCopy->scale;
	dupedMesh->bIsVisible = meshToCopy->bIsVisible;
	dupedMesh->bIsWireframe = meshToCopy->bIsWireframe;
	dupedMesh->bDoNotLight = meshToCopy->bDoNotLight;
	dupedMesh->bUseDebugColours = meshToCopy->bUseDebugColours;
	dupedMesh->setRotationFromEuler(dupedMesh->getEulerOrientation());
	m_vec_pMeshesToDraw.push_back(dupedMesh);
}

void cGraphicsMain::deleteMesh(int meshIDX)
{
	m_vec_pMeshesToDraw.erase(m_vec_pMeshesToDraw.begin() + meshIDX);

	return;
}

void cGraphicsMain::flyCameraInput(int width, int height)
{
	static bool isRightClicking = false;
	static double mouseXPos = 0;
	static double mouseYPos = 0;

	int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS) // Start tracking delta mouse position
	{
		if (!isRightClicking) // start tracking
		{
			mouseXPos = width / 2;
			mouseYPos = height / 2;
			glfwSetCursorPos(m_window, width / 2, height / 2);
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			//glfwGetCursorPos(m_window, &mouseXPos, &mouseYPos);
			isRightClicking = true;
		}
		else
		{
			double deltaMouseX, deltaMouseY;
			glfwGetCursorPos(m_window, &deltaMouseX, &deltaMouseY); // Get current mouse position
			deltaMouseX -= mouseXPos; // Set the delta mouse positions
			deltaMouseY -= mouseYPos; // for this tick

			// Camera rotation here :)
			m_cameraRotation.x -= deltaMouseX / 1000;

			m_cameraRotation.y -= deltaMouseY / 1000;
			m_cameraTarget.y = m_cameraRotation.y;     // This is pitch
			m_cameraTarget.x = sin(m_cameraRotation.x);         // This is just y-rotation
			m_cameraTarget.z = sin(m_cameraRotation.x + 1.57);  //
			m_cameraTarget = glm::normalize(m_cameraTarget);
			m_cameraTarget.y *= 2;
			glfwSetCursorPos(m_window, width / 2, height / 2);
			glfwGetCursorPos(m_window, &mouseXPos, &mouseYPos); // Update this for next loop
		}

	}
	else if (isRightClicking)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		isRightClicking = false;
	}

	if (isRightClicking) // Have movement tied to right-clicking too
	{
		state = glfwGetKey(m_window, GLFW_KEY_W);
		if (state == GLFW_PRESS) // Move forward
		{
			m_cameraEye += glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
		}
		state = glfwGetKey(m_window, GLFW_KEY_S);
		if (state == GLFW_PRESS) // Move backwards
		{
			m_cameraEye -= glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
		}
		state = glfwGetKey(m_window, GLFW_KEY_A);
		if (state == GLFW_PRESS) // Move left
		{
			m_cameraEye += glm::normalize(glm::cross(m_upVector, m_cameraTarget)) * m_FlyCamSpeed;
		}
		state = glfwGetKey(m_window, GLFW_KEY_D);
		if (state == GLFW_PRESS) // Move right
		{
			m_cameraEye -= glm::normalize(glm::cross(m_upVector, m_cameraTarget)) * m_FlyCamSpeed;
		}
	}
}
