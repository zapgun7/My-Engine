#include "cLevelEditor.h"

#include "Other Graphics Stuff/cMesh.h" // For the mesh vec



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
	delete m_pSceneManager;
}

void cLevelEditor::Update()
{
	// Start by getting all info from physics, graphics, audio, etc.
	// Use that info to display and be able to modify that info
	// Modify info with calls to the engine controller

	// Data Retrieval
	std::vector<cMesh*> MeshVec;
	m_pEngineController->getActiveMeshes(&MeshVec);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	

	RootWindow(MeshVec);

	if (m_ShowMeshEditor)
		MeshEditor(MeshVec);
	if (m_ShowLightEditor)
		LightEditor();
	if (m_ShowSceneManager)
		SceneManager();




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
	 			//updateSelectedMesh(mesh_obj_idx, "A NEW FRIENDLY NAME", newPos, newOri, customColor, scale, doNotLight, useCustomColor);
	 		}
	 		ImGui::End();
}

// Window for changing light properties
void cLevelEditor::LightEditor()
{

}

// Window for scene management
void cLevelEditor::SceneManager()
{

}

cLevelEditor::cLevelEditor(GLFWwindow* window)
	: m_ShowLightEditor(false)
	, m_ShowMeshEditor(false)
	, m_ShowSceneManager(false)
	, m_mesh_obj_idx(0)
{
	m_pSceneManager = new cSceneManagement();
	m_pSceneManager->Initialize();

	m_pEngineController = cEngineController::GetEngineController();

	m_pEngineController->getAvailableModels(&m_AvailableModels);

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
