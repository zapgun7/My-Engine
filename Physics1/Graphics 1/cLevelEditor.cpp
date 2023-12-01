#include "cLevelEditor.h"

#include "Other Graphics Stuff/cMesh.h" // For the mesh vec
#include "Other Graphics Stuff/cLightManager.h" // For the lights (pointer, but for now will just get it every update)
//#include "cEngineController.h" // Callbacks to edit object parameters

#include <iostream>


// cLevelEditor* cLevelEditor::GetInstance(void)
// {
// 	if (cLevelEditor::m_pTheEditor == nullptr)
// 	{
// 		cLevelEditor::m_pTheEditor = new cLevelEditor();
// 	}
// 	return cLevelEditor::m_pTheEditor;
// }

cLevelEditor::~cLevelEditor()
{
	//delete m_pSceneManager;
}

void cLevelEditor::Update()
{
	// Start by getting all info from physics, graphics, audio, etc.
	// Use that info to display and be able to modify that info
	// Modify info with calls to the engine controller

	// Data Retrieval
	std::vector<cMesh*> MeshVec;
	cLightManager TheLights;
	m_pEngineController->getActiveMeshNLights(&MeshVec, &TheLights);

	std::vector<std::string> AvailableSaves;
	m_pEngineController->getAvailableSaves(&AvailableSaves);


	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	RootWindow(MeshVec);

	if (m_ShowMeshEditor)
		MeshEditor(MeshVec);
	if (m_ShowLightEditor)
		LightEditor(&TheLights);
	if (m_ShowSceneManager)
		SceneManager(AvailableSaves);



	// ImGui Last steps to render
	ImGui::Render();

	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // This is called in the graphics class I guess

}

// Base window to access the other windows
void cLevelEditor::RootWindow(std::vector<cMesh*> ActiveMeshVec)
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
			//addNewMesh(m_AvailableModels[available_obj_idx], buf1);
			m_pEngineController->addNewObject(m_AvailableModels[available_obj_idx], buf1);
			memset(buf1, 0, 32);
			m_mesh_obj_idx = ActiveMeshVec.size() - 1;
		}
	}

	if (ImGui::Button("Physics Toggle TODO"))
	{
// 		if (enablePhysics)
// 			enablePhysics = false;
// 		else
// 			enablePhysics = true;
		// ::g_pPhysics->setPhysicsRunningState(enablePhysics); //UNPHYS
	}

// 	ImGui::SameLine();
// 	if (enablePhysics)
// 		ImGui::Text("ON    ");
// 	else
// 		ImGui::Text("OFF   ");


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
		//m_FlyCamSpeed += 0.1f;
	}
	//ImGui::Text(std::to_string(m_FlyCamSpeed).c_str());
	ImGui::Text("TODO");
	if (ImGui::Button("Slower"))
	{
// 		if (m_FlyCamSpeed > 0.1f)
// 			m_FlyCamSpeed -= 0.1f;
	}



	ImGui::End();
}

// Window for changing graphical properties of objects
void cLevelEditor::MeshEditor(std::vector<cMesh*> ActiveMeshVec)
{
	ImGui::Begin("Mesh Editor");
	 
	 		if (ImGui::BeginListBox("Available Objects")) // List of active meshes
	 		{
	 			for (int n = 0; n < ActiveMeshVec.size(); n++)
	 			{
	 				const bool is_selected = (m_mesh_obj_idx == n);
	 				if (ImGui::Selectable(ActiveMeshVec[n]->friendlyName.c_str(), is_selected))
						m_mesh_obj_idx = n;
	 
	 				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
	 				if (is_selected)
	 					ImGui::SetItemDefaultFocus();
	 			}
	 			ImGui::EndListBox();
	 		}
	 		if (m_mesh_obj_idx >= ActiveMeshVec.size()) // When changing scenes, this makes sure it doesn't go outta bounds
				m_mesh_obj_idx = 0;
	 		bool isExistingMesh = false; // Assert we have at least one mesh
	 		if (ActiveMeshVec.size() > 0)
	 			isExistingMesh = true;
	 
	 		static bool doNotLight = false;
	 		if (isExistingMesh)
	 			doNotLight = ActiveMeshVec[m_mesh_obj_idx]->bDoNotLight;
	 
	 		/////////// DUPLICATE MESH /////////////
	 		static char dupeName[32] = ""; ImGui::InputText(":)", dupeName, 32);
	 		ImGui::SameLine();
	 		if (ImGui::Button("Duplicate"))
	 		{
	 			if ((isExistingMesh) && (std::strlen(dupeName) > 0))
	 			{
	 				//duplicateMesh(m_mesh_obj_idx, dupeName);
					m_mesh_obj_idx++; // Have new duplicate selected
	 			}
	 		}
	 		ImGui::SameLine();
	 		ImGui::Checkbox("doNotLight", &doNotLight);
	 
	 		static bool toggalAllLight = false;
	 		if (toggalAllLight)
	 		{
	 			if (ImGui::Button("AllMeshLighting: On"))
	 			{
	 				for (unsigned int i = 0; i < ActiveMeshVec.size(); i++)
						ActiveMeshVec[i]->bDoNotLight = true;
	 				toggalAllLight = false;
	 			}
	 		}
	 		else
	 		{
	 			if (ImGui::Button("AllMeshLighting: Off"))
	 			{
	 				for (unsigned int i = 0; i < ActiveMeshVec.size(); i++)
						ActiveMeshVec[i]->bDoNotLight = false;
	 				toggalAllLight = true;
	 			}
	 		}
	 
	 
			int meshID = -1;
			std::string friendlyName = "";
			std::string* textureNames = nullptr;
			float* textureRatios = nullptr; 
			int* textureIdx = nullptr;
			bool isVisible = false;
			bool isWireframe = false;

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
	 			xPos = ActiveMeshVec[m_mesh_obj_idx]->drawPosition.x;
	 			yPos = ActiveMeshVec[m_mesh_obj_idx]->drawPosition.y;
	 			zPos = ActiveMeshVec[m_mesh_obj_idx]->drawPosition.z;
	 			glm::vec3 meshEulerOri = ActiveMeshVec[m_mesh_obj_idx]->getEulerOrientation();
	 			xOri = meshEulerOri.x;
	 			yOri = meshEulerOri.y;
	 			zOri = meshEulerOri.z;
	 			scale = ActiveMeshVec[m_mesh_obj_idx]->scale.x;
	 			customColor = glm::vec3(ActiveMeshVec[m_mesh_obj_idx]->wholeObjectDebugColourRGBA);
	 			useCustomColor = ActiveMeshVec[m_mesh_obj_idx]->bUseDebugColours;

				meshID = ActiveMeshVec[m_mesh_obj_idx]->uniqueID;
				friendlyName = ActiveMeshVec[m_mesh_obj_idx]->friendlyName;
				textureNames = ActiveMeshVec[m_mesh_obj_idx]->textureName;
				textureRatios = ActiveMeshVec[m_mesh_obj_idx]->textureRatios;
				isVisible = ActiveMeshVec[m_mesh_obj_idx]->bIsVisible;
				isWireframe = ActiveMeshVec[m_mesh_obj_idx]->bIsWireframe;

				textureIdx = ActiveMeshVec[m_mesh_obj_idx]->textureIdx;
	 		}
			// Position
	 		ImGui::SeparatorText("Position");
	 		ImGui::DragFloat("X", &xPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 		ImGui::DragFloat("Y", &yPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 		ImGui::DragFloat("Z", &zPos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
			
			// Orientation
	 		ImGui::SeparatorText("Orientation");
	 		ImGui::DragFloat("X-Rotation", &xOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 		ImGui::DragFloat("Y-Rotation", &yOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 		ImGui::DragFloat("Z-Rotation", &zOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
			
			// Scale TODO separate into x y z
	 		ImGui::SeparatorText("Scale");
	 		ImGui::DragFloat("Scale", &scale, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 
			// Custom Color
	 		ImGui::SeparatorText("Custom Colors");
	 		ImGui::DragFloat("Red", &customColor.x, 0.005f, 0.0f, 1.0f, "%.3f");
	 		ImGui::DragFloat("Green", &customColor.y, 0.005f, 0.0f, 1.0f, "%.3f");
	 		ImGui::DragFloat("Blue", &customColor.z, 0.005f, 0.0f, 1.0f, "%.3f");
	 		ImGui::Checkbox("Use Custom Color", &useCustomColor);
	 
			

			//////// TEXTURES //////////

			//char* textureBase[m_AvailableTextures.size()]
			//const char* textures[] = { "Point Light", "Spot Light", "Directional Light" };

			//const char* textures[];// = { m_AvailableTextures[0].c_str() , "Spot Light", "Directional Light" };
			//textures[0] = m_AvailableTextures[0].c_str();

			if (isExistingMesh)
			{
				static int textype_current_idx[8];

				int textureCount = m_AvailableTextures.size();

				ImGui::SeparatorText("Textures");
				for (unsigned int i = 0; i < cMesh::NUM_TEXTURES; i++)
				{
					ImGui::Text("Tex%d: ", i);
					ImGui::SameLine();
					// Texture combo box here

					textype_current_idx[i] = textureIdx[i]; // Set id of texture in the vector
					const char* combo_preview_value = m_AvailableTextures[textype_current_idx[i]].c_str();
					ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f); //ImGui::PushItemWidth(300);
					if (ImGui::BeginCombo(("##", std::to_string(i).c_str()), combo_preview_value))
					{
						for (int n = 0; n < textureCount; n++)
						{
							const bool is_selected = (textype_current_idx[i] == n);
							if (ImGui::Selectable(m_AvailableTextures[n].c_str(), is_selected))
								textype_current_idx[i] = n;

							// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
					ImGui::SameLine();
					//Texture ratio here 0-1.0f slider   textureRatios
					std::string sliderLabel = "Ratio" + std::to_string(i);
					ImGui::PushItemWidth(100);
					ImGui::DragFloat(sliderLabel.c_str(), &textureRatios[i], 0.01f, 0.0f, 1.0f, "%.2f");
					ImGui::PopItemWidth();
					

				}
				textureIdx = textype_current_idx; // Update new values to pass to update
			}
			


			


			// Keep delete button at the bottom
	 		ImGui::Separator();
	 		if (ImGui::Button("Delete"))
	 		{
	 			if (isExistingMesh)
	 			{
	 				//deleteMesh(mesh_obj_idx);
					m_mesh_obj_idx--;
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
				// This will call 2 functions: graphics and physics

				m_pEngineController->setMeshData(meshID, friendlyName, textureIdx, textureRatios, isVisible, isWireframe, doNotLight, useCustomColor, glm::vec4(customColor, 1));
	 			//updateSelectedMesh(mesh_obj_idx, "A NEW FRIENDLY NAME", newPos, newOri, customColor, scale, doNotLight, useCustomColor);
	 		}
	 		ImGui::End();
}

// Window for changing light properties
void cLevelEditor::LightEditor(cLightManager* TheLights)
{
 	ImGui::Begin("Light Editor");
 
 	static int light_obj_idx = 0;
 	if (ImGui::BeginListBox("Available Objects"))
 	{
 		for (int n = 0; n < TheLights->NUMBER_OF_LIGHTS_IM_USING; n++)
 		{
 			const bool is_selected = (light_obj_idx == n);
 			if (ImGui::Selectable(TheLights->theLights[n].friendlyName.c_str(), is_selected))
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
 		lightPos = TheLights->theLights[light_obj_idx].position;
 		lightDir = TheLights->theLights[light_obj_idx].direction;
 		lightDiff = TheLights->theLights[light_obj_idx].diffuse;
 		lightSpec = TheLights->theLights[light_obj_idx].specular; // rgb = highlight colour, w = power
 		lightAtten = TheLights->theLights[light_obj_idx].atten; // x = constant, y = linear, z = quadratic, w = DistanceCutOff
 		lightParam1 = TheLights->theLights[light_obj_idx].param1; // x: light type    y: inner angle    z: outer angle
 		lightParam2 = TheLights->theLights[light_obj_idx].param2; // x: light on(1) or off(0)
 	/*}*/
 	static char lightname[32] = ""; 
 	//strcpy_s(lightname, TheLights.theLights[light_obj_idx].friendlyName.c_str()); // TODO too long a name will prob break this
 
 	ImGui::InputText("Light Name", lightname, 32);
 	if (ImGui::Button("Set New Name")) // Button to set new light friendlyname
 	{
 		if (strlen(lightname) > 0)
			TheLights->theLights[light_obj_idx].friendlyName = lightname;
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
 		//updateSelectedLight(light_obj_idx, lightPos, lightDiff, lightSpec, lightAtten, lightDir, lightParam1, lightParam2); // TODO light update
 	/*}*/
 
 	ImGui::End();
}

// Window for scene management
void cLevelEditor::SceneManager(std::vector<std::string> AvailableSaves)
{
	ImGui::Begin("Scene Manager");
	 
	//availSaves = m_pSceneManager->getAvailableSaves(); // Update availible saves upon opening scene manager window
	static int saves_idx = 0;
	if (ImGui::BeginListBox("Available Saves")) // List of availible saves to load from
	{
	 	for (int n = 0; n < AvailableSaves.size(); n++)
	 	{
	 		const bool is_save_selected = (saves_idx == n);
	 		if (ImGui::Selectable(AvailableSaves[n].c_str(), is_save_selected))
	 			saves_idx = n;
	 
	 		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
	 		if (is_save_selected)
	 			ImGui::SetItemDefaultFocus();
	 	}
	 	ImGui::EndListBox();
	}
	 
	if (ImGui::Button("Load Scene"))
	{
	 	//if (AvailableSaves.size() > 0)
	 		//m_pSceneManager->loadScene(availSaves[saves_idx]); // Will load a selected item from a list in the future    // TODO load scene
	}
	 
	 
	static char saveNameBuf[32] = ""; ImGui::InputText("Save Name", saveNameBuf, 32);
	ImGui::SameLine();
	if (ImGui::Button("Save Current Scene"))
	{
	 	if (std::strlen(saveNameBuf) > 0)
	 	{
	 		//m_pSceneManager->saveScene(saveNameBuf, m_vec_pMeshesToDraw, m_pTheLights);  // TODO scene save
	 		memset(saveNameBuf, 0, 32); // Reset buffer
	 	}
	 			
	}
	 
	ImGui::End();
}

cLevelEditor::cLevelEditor(GLFWwindow* window)
	: m_ShowLightEditor(false)
	, m_ShowMeshEditor(false)
	, m_ShowSceneManager(false)
	, m_mesh_obj_idx(0)
{
	//m_pSceneManager = new cSceneManagement();
	//m_pSceneManager->Initialize();

	m_pEngineController = cEngineController::GetEngineController();

	m_pEngineController->getAvailableModels(&m_AvailableModels, &m_AvailableTextures);

	m_window = window;

	// Initialize ImGui
	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = ImGui::GetIO(); (void)m_io;
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}
