#include "cLevelEditor.h"

#include "Other Graphics Stuff/cMesh.h" // For the mesh vec
#include "Other Graphics Stuff/cLightManager.h" // For the lights (pointer, but for now will just get it every update)
#include "Physics/sPhysicsProperties.h"
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

void cLevelEditor::Update(double deltaTime)
{
	// Start by getting all info from physics, graphics, audio, etc.
	// Use that info to display and be able to modify that info
	// Modify info with calls to the engine controller

	// Data Retrieval
	std::vector<cMesh*> MeshVec;
	cLightManager TheLights;
	std::vector<sPhysicsProperties*> PhysVec;
	
	
	m_pEngineController->getActiveMeshNLights(&MeshVec, &TheLights, &PhysVec);

	std::vector<std::string> AvailableSaves;
	m_pEngineController->getAvailableSaves(&AvailableSaves);



	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	RootWindow(MeshVec, deltaTime);

	if (m_ShowMeshEditor)
	{
		MeshEditor(MeshVec, PhysVec);
		if ((m_ShowMaterialEditor) && (!m_JustDeleted))
		{
			if ((m_mesh_obj_idx < MeshVec.size()) && (MeshVec[m_mesh_obj_idx] != nullptr))
				MaterialEditor(MeshVec[m_mesh_obj_idx]);
			else
				MaterialEditor(nullptr);
		}
	}
	if ((m_ShowPhysicsEditor) && (!m_JustDeleted)) // If we delete physics obj in above mesh editor, wrong vec will be passed to this, so just skip this frame :)
		PhysEditor(PhysVec);
	if (m_ShowLightEditor)
		LightEditor(&TheLights);
	if (m_ShowSceneManager)
		SceneManager(AvailableSaves);



	// Debug Render
	int selectedMesh = m_mesh_obj_idx;
	int selectedLight = m_light_obj_idx;


	if ((!m_ShowMeshEditor) || (MeshVec.empty()) || (m_JustDeleted))
		selectedMesh = -1;
	if (!m_ShowLightEditor)
		selectedLight = -1;

	m_pEngineController->updateDebugMode(m_RenderDebug, selectedMesh, selectedLight);
	m_JustDeleted = false;


	// ImGui Last steps to render
	ImGui::Render();

	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // This is called in the graphics class I guess

}

// Base window to access the other windows
void cLevelEditor::RootWindow(std::vector<cMesh*> ActiveMeshVec, double dt)
{
	ImGui::Begin("Main Editor Window");

	ImGui::Text("FPS: %.2f", 1.0f / dt);

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

	static bool enablePhysics = false;

	if (ImGui::Button("Physics Toggle"))
	{
		if (enablePhysics)
			enablePhysics = false;
		else
			enablePhysics = true;
		m_pEngineController->setPhysicsRunning(enablePhysics);
	}

	ImGui::SameLine();
	if (enablePhysics)
		ImGui::Text("ON    ");
	else
		ImGui::Text("OFF   ");

if (ImGui::Button("Debug Render"))
	{
		if (m_RenderDebug)
			m_RenderDebug = false;
		else
			m_RenderDebug = true;
	}

	ImGui::SameLine();
	if (m_RenderDebug)
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
	if (ImGui::Button("Physics Editor"))
	{
		if (m_ShowPhysicsEditor)
			m_ShowPhysicsEditor = false;
		else
			m_ShowPhysicsEditor = true;
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

	static float camSpd = 20.0f;

	ImGui::DragFloat("CamSpd: ", &camSpd, 0.001f, FLT_MIN, FLT_MAX, "%.5f");
	m_pEngineController->setCamSpeed(camSpd);
// 	if (ImGui::Button("Faster"))
// 	{
// 		m_pEngineController->setCamSpeed(0.1f);
// 	}
// 	//ImGui::Text(std::to_string(m_FlyCamSpeed).c_str());
// 	if (ImGui::Button("Slower"))
// 	{
// 		m_pEngineController->setCamSpeed(-0.1f);
// 	}


	


	ImGui::End();
}

// Window for changing graphical properties of objects
void cLevelEditor::MeshEditor(std::vector<cMesh*> ActiveMeshVec, std::vector<sPhysicsProperties*> PhysVec)
{
	ImGui::Begin("Mesh Editor");
	 
	 		if (ImGui::BeginListBox("Available Objects")) // List of active meshes         // TODO should list physics objects as options
	 		{
	 			for (int n = 0; n < ActiveMeshVec.size(); n++)
	 			{
	 				const bool is_selected = (m_mesh_obj_idx == n);
	 				if (ImGui::Selectable((std::to_string(n) + " - " + ActiveMeshVec[n]->friendlyName).c_str(), is_selected))
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
					cMesh* meshToCopy = ActiveMeshVec[m_mesh_obj_idx];
					//cMesh* newDupe = new cMesh();

					DuplicateObj(meshToCopy, dupeName, PhysVec);


					m_mesh_obj_idx = ActiveMeshVec.size(); // Have new duplicate selected
					ImGui::End();
					return;
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

			if (ImGui::Button("Material Editor"))
			{
				m_ShowMaterialEditor = m_ShowMaterialEditor ? false : true;
			}

			if (isExistingMesh)
			{
				// REFLECT/REFRACT BUTTONS
				ImGui::SameLine();

				if (ActiveMeshVec[m_mesh_obj_idx]->bUseReflect)
				{
					if (ImGui::Button("Reflect: On"))
					{
						ActiveMeshVec[m_mesh_obj_idx]->bUseReflect = false;
					}
				}
				else
				{
					if (ImGui::Button("Reflect: Off"))
					{
						ActiveMeshVec[m_mesh_obj_idx]->bUseReflect = true;
					}
				}

				ImGui::SameLine();
				if (ActiveMeshVec[m_mesh_obj_idx]->bUseRefract)
				{
					if (ImGui::Button("Refract: On"))
					{
						ActiveMeshVec[m_mesh_obj_idx]->bUseRefract = false;
					}
				}
				else
				{
					if (ImGui::Button("Refract: Off"))
					{
						ActiveMeshVec[m_mesh_obj_idx]->bUseRefract = true;
					}
				}
			}


	 
			int objID = -1;
			std::string friendlyName = "";
			std::string* textureNames = nullptr;
			float* textureRatios = nullptr; 
			bool isVisible = false;
			bool isWireframe = false;
			bool useDiscardMask = false;
			bool isNavMesh = false;

	 		float xPos = 0;
	 		float yPos = 0;
	 		float zPos = 0;
	 		float xOri = 0;
	 		float yOri = 0;
	 		float zOri = 0;
	 		glm::vec3 scale = glm::vec3(0.0f);
	 		glm::vec3 customColor(0.0f, 0.0f, 0.0f);
			glm::vec2 uvOffsetSpeed = glm::vec2(0);
			float uvXSpeed = 0;
			float uvYSpeed = 0;
			float uvScale = 1.0f;
			float transparencyAlpha = 1.0f;
	 		bool useCustomColor = false;
			bool isSpooky = false; // Cool shader effect
	 

			cMesh* selectedMesh = nullptr;
			sPhysicsProperties* selectedObj = nullptr;
	 
	 		if (isExistingMesh)
	 		{
				selectedMesh = ActiveMeshVec[m_mesh_obj_idx];
				for (int i = m_mesh_obj_idx; i < PhysVec.size(); i++)
				{
					if (PhysVec[i]->getUniqueID() == selectedMesh->uniqueID)
					{
						selectedObj = PhysVec[i];
						break;
					}
				}
				//selectedObj = PhysVec[m_mesh_obj_idx];





	 			// PHYSICS DATA
				glm::vec3 objPosition = selectedObj->position;
	 			xPos = objPosition.x;
	 			yPos = objPosition.y;
	 			zPos = objPosition.z;
	 			glm::vec3 objEulerOri = selectedObj->get_eOrientation();
	 			xOri = objEulerOri.x;
	 			yOri = objEulerOri.y;
	 			zOri = objEulerOri.z;

				objID = selectedObj->getUniqueID();
	 			

				// GRAPHICS DATA
				scale = selectedMesh->scale;
	 			customColor = glm::vec3(selectedMesh->customColorRGBA);
	 			useCustomColor = selectedMesh->bUseCustomColors;
				transparencyAlpha = selectedMesh->transparencyAlpha;
				useDiscardMask = selectedMesh->bUseDiscardMaskTex;

				//uvOffsetSpeed = selectedMesh->uvOffsetSpeed;
				uvXSpeed = uvOffsetSpeed.x;
				uvYSpeed = uvOffsetSpeed.y;
				//uvScale = selectedMesh->uv_Offset_Scale.z;
				

				isNavMesh = selectedMesh->isNavMesh;

				
				friendlyName = selectedMesh->friendlyName;
				isVisible = selectedMesh->bIsVisible;
				isWireframe = selectedMesh->bIsWireframe;

				isSpooky = selectedMesh->isSpooky;

				//textureIdx = selectedMesh->textureIdx;
// 				for (unsigned int i = 0; i < cMesh::NUM_TEXTURES; i++)
// 				{
// 					if (selectedMesh->textureName[i] != "##")
// 					{
// 						for (unsigned int e = 0; e < m_AvailableTextures.size(); e++)
// 						{
// 							if (selectedMesh->textureName[i] == m_AvailableTextures[e])
// 							{
// 								textureIdx[i] = e;
// 								break;
// 							}
// 						}
// 					}
// 				}
	 		}

			// Nav mesh
			ImGui::Checkbox("Is Nav Mesh", &isNavMesh);

			// Position
	 		ImGui::SeparatorText("Position");
	 		ImGui::DragFloat("X", &xPos, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f");
			ImGui::SameLine();
			if (ImGui::Button("+20x")) xPos += 20;
			ImGui::SameLine();
			if (ImGui::Button("-20x")) xPos -= 20;
			ImGui::SameLine();
			if (ImGui::Button("+10x")) xPos += 10;
			ImGui::SameLine();
			if (ImGui::Button("-10x")) xPos -= 10;
	 		ImGui::DragFloat("Y", &yPos, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f");
			ImGui::SameLine();
			if (ImGui::Button("+20y")) yPos += 20;
			ImGui::SameLine();
			if (ImGui::Button("-20y")) yPos -= 20;
			ImGui::SameLine();
			if (ImGui::Button("+10y")) yPos += 10;
			ImGui::SameLine();
			if (ImGui::Button("-10y")) yPos -= 10;
	 		ImGui::DragFloat("Z", &zPos, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f");
			ImGui::SameLine();
			if (ImGui::Button("+20z")) zPos += 20;
			ImGui::SameLine();
			if (ImGui::Button("-20z")) zPos -= 20;
			ImGui::SameLine();
			if (ImGui::Button("+10z")) zPos += 10;
			ImGui::SameLine();
			if (ImGui::Button("-10z")) zPos -= 10;
			
			// Orientation
	 		ImGui::SeparatorText("Orientation");
	 		ImGui::DragFloat("X-Rotation", &xOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 		ImGui::DragFloat("Y-Rotation", &yOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 		ImGui::DragFloat("Z-Rotation", &zOri, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
			
			// Scale TODO separate into x y z
	 		ImGui::SeparatorText("Scale");
	 		ImGui::DragFloat("ScaleX", &scale.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
			ImGui::DragFloat("ScaleY", &scale.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
			ImGui::DragFloat("ScaleZ", &scale.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	 
			// Custom Color
	 		ImGui::SeparatorText("Custom Colors");
	 		ImGui::DragFloat("Red", &customColor.x, 0.005f, 0.0f, 1.0f, "%.3f");
	 		ImGui::DragFloat("Green", &customColor.y, 0.005f, 0.0f, 1.0f, "%.3f");
	 		ImGui::DragFloat("Blue", &customColor.z, 0.005f, 0.0f, 1.0f, "%.3f");
			ImGui::DragFloat("Alpha", &transparencyAlpha, 0.01f, 0.0f, 1.0f, "%.3f");
	 		ImGui::Checkbox("Use Custom Color", &useCustomColor);
	 
			

			//////// TEXTURES //////////
			int textype_current_idx[8];

			if (isExistingMesh)
			{
				

				int textureCount = m_AvailableTextures.size();

				ImGui::SeparatorText("Textures");
				ImGui::SameLine();
				// Toggle Button to Use Mask Discard
				if (useDiscardMask)
				{
					if (ImGui::Button("MaskDiscard: ON"))
					{
						selectedMesh->bUseDiscardMaskTex = false;
					}
				}
				else
				{
					if (ImGui::Button("MaskDiscard: OFF"))
					{
						selectedMesh->bUseDiscardMaskTex = true;
					}
				}

				
				// TODO move this to materials editor, maybe remove the spd
				// UV-Offset Speed
// 				ImGui::PushItemWidth(100);
// 				ImGui::DragFloat("uvX_Spd", &uvXSpeed, 0.001f, -10.0f, 10.0f, "%.3f");
// 				ImGui::SameLine();
// 				ImGui::DragFloat("uvY_Spd", &uvYSpeed, 0.001f, -10.0f, 10.0f, "%.3f");
// 				ImGui::SameLine();
// 				ImGui::DragFloat("uv_Scale", &uvScale, 0.001f, 0.01f, 40.0f, "%.3f");
// 				ImGui::PopItemWidth();
			}
			


			


			// Keep delete button at the bottom
	 		ImGui::Separator();
	 		if (ImGui::Button("Delete"))
	 		{
	 			if (isExistingMesh)
	 			{
					m_pEngineController->deleteObject(selectedMesh->uniqueID);
					m_mesh_obj_idx--;
	 				isExistingMesh = false;
					m_JustDeleted = true;
	 			}
	 		}
	 		ImGui::SameLine();
	 		ImGui::Text("  Deletes currently selected mesh");

			ImGui::Checkbox("coolShader", &isSpooky);
	 		
	 		// List all object attributes the user is able to edit
	 
	 		if (isExistingMesh)
	 		{
	 			glm::vec3 newPos = glm::vec3(xPos, yPos, zPos);
	 			glm::vec3 newOri = glm::vec3(xOri, yOri, zOri);
				selectedMesh->transparencyAlpha = transparencyAlpha;
				selectedMesh->scale = scale;

				selectedMesh->friendlyName = friendlyName;
			
				selectedMesh->bIsVisible = isVisible;
				selectedMesh->bIsWireframe = isWireframe;
				selectedMesh->bDoNotLight = doNotLight;
				selectedMesh->bUseCustomColors = useCustomColor;
				selectedMesh->customColorRGBA = glm::vec4(customColor, 1);

				selectedMesh->isNavMesh = isNavMesh;

				//selectedMesh->uvOffsetSpeed = glm::vec2(uvXSpeed, uvYSpeed);
				//selectedMesh->uv_Offset_Scale.z = uvScale;
				selectedMesh->isSpooky = isSpooky;
				
				// This will call 2 functions: graphics and physics

				//m_pEngineController->setMeshData(objID, friendlyName, textureIdx, textureRatios, isVisible, isWireframe, doNotLight, useCustomColor, glm::vec4(customColor, 1));
				m_pEngineController->setPhysData(objID, newPos, newOri);
	 			//updateSelectedMesh(mesh_obj_idx, "A NEW FRIENDLY NAME", newPos, newOri, customColor, scale, doNotLight, useCustomColor);
	 		}
	 		ImGui::End();
}


void cLevelEditor::MaterialEditor(cMesh* SelectedMesh)
{
	ImGui::Begin("Material Editor");

	std::string diffTex;
	std::string specTex;
	glm::vec4 power(0.0f);

	int difftex_current_idx = 0;
	int spectex_current_idx = 0;
	int diffTextureIdx = 0;
	glm::vec3 diffUVdata = glm::vec3(0.0f);
	bool isDiffMirrored = false;

	int specTextureIdx = 0;
	glm::vec3 specUVdata = glm::vec3(0.0f);
	bool isSpecMirrored = false;
	int textureCount = m_AvailableTextures.size();

	if (SelectedMesh != nullptr)
	{
		power = SelectedMesh->material.power;
		diffTex = SelectedMesh->material.diffuseTex;
		diffUVdata = SelectedMesh->material.diffuv_Offset_Scale;

		specTex = SelectedMesh->material.specularTex;

		isDiffMirrored = SelectedMesh->material.isDiffMirrored;
		isSpecMirrored = SelectedMesh->material.isSpecMirrored;
		specUVdata = SelectedMesh->material.specuv_Offset_Scale;

		// Find selected texture for diffuse (if any)

		for (unsigned int i = 0; i < m_AvailableTextures.size(); i++)
		{
			if (diffTex == m_AvailableTextures[i])
			{
				diffTextureIdx = i;
			}
			if (specTex == m_AvailableTextures[i])
			{
				specTextureIdx = i;
			}
		}
	}


	// Combo-boxes for texture selecting

	// DIFFUSE COMBO //
	{
		difftex_current_idx = diffTextureIdx; // Set id of texture in the vector
		const char* combo_preview_value = m_AvailableTextures[difftex_current_idx].c_str();
		ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f); //ImGui::PushItemWidth(300);
		if (ImGui::BeginCombo("diffuseTexture", combo_preview_value))
		{
			for (int n = 0; n < textureCount; n++)
			{
				const bool is_selected = (difftex_current_idx == n);
				if (ImGui::Selectable(m_AvailableTextures[n].c_str(), is_selected))
					difftex_current_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		// Mirrored Tex Toggle
		ImGui::SameLine();
		ImGui::Checkbox("DiffMirror", &isDiffMirrored);

		// UV Data Setting
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("diff_uvX", &diffUVdata.x, 0.001f, -1.0f, 1.0f, "%.3f");
		ImGui::SameLine();
		ImGui::DragFloat("diff_uvY", &diffUVdata.y, 0.001f, -1.0f, 1.0f, "%.3f");
		ImGui::SameLine();
		ImGui::DragFloat("diff_uv_Scale", &diffUVdata.z, 0.001f, FLT_MIN, FLT_MAX, "%.3f");
		ImGui::PopItemWidth();
	}

	// SPECULAR COMBO //
	{
		spectex_current_idx = specTextureIdx; // Set id of texture in the vector
		const char* combo_preview_value = m_AvailableTextures[spectex_current_idx].c_str();
		ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f); //ImGui::PushItemWidth(300);
		if (ImGui::BeginCombo("specularTexture", combo_preview_value))
		{
			for (int n = 0; n < textureCount; n++)
			{
				const bool is_selected = (spectex_current_idx == n);
				if (ImGui::Selectable(m_AvailableTextures[n].c_str(), is_selected))
					spectex_current_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		// Mirrored Tex Toggle
		ImGui::SameLine();
		ImGui::Checkbox("SpecMirror", &isSpecMirrored);

		// UV Data Setting
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("spec_uvX", &specUVdata.x, 0.001f, -1.0f, 1.0f, "%.3f");
		ImGui::SameLine();
		ImGui::DragFloat("spec_uvY", &specUVdata.y, 0.001f, -1.0f, 1.0f, "%.3f");
		ImGui::SameLine();
		ImGui::DragFloat("spec_uv_Scale", &specUVdata.z, 0.001f, FLT_MIN, FLT_MAX, "%.3f");
		ImGui::PopItemWidth();
	}


	// Power Slider
	ImGui::SeparatorText("Power");
	ImGui::DragFloat("AmbientPow", &power.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	ImGui::DragFloat("DiffusePow", &power.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	ImGui::DragFloat("NothingForNow", &power.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
	ImGui::DragFloat("Shininess", &power.w, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");



	if (SelectedMesh != nullptr)
	{
		SelectedMesh->material.power = power;
		SelectedMesh->material.diffuseTex = m_AvailableTextures[difftex_current_idx];
		SelectedMesh->material.specularTex = m_AvailableTextures[spectex_current_idx];
		SelectedMesh->material.isDiffMirrored = isDiffMirrored;
		SelectedMesh->material.isSpecMirrored = isSpecMirrored;
		SelectedMesh->material.diffuv_Offset_Scale = diffUVdata;
		SelectedMesh->material.specuv_Offset_Scale = specUVdata;
	}


	ImGui::End();
}

void cLevelEditor::PhysEditor(std::vector<sPhysicsProperties*> PhysVec)
{
	ImGui::Begin("Physics Editor");
	if (ImGui::BeginListBox("Physics Objects")) // List of active objects
	{
		for (int n = 0; n < PhysVec.size(); n++)
		{
			const bool is_selected = (m_phys_obj_idx == n);
			if (ImGui::Selectable(PhysVec[n]->friendlyName.c_str(), is_selected))
				m_phys_obj_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	if (m_phys_obj_idx >= PhysVec.size()) // When changing scenes, this makes sure it doesn't go outta bounds
		m_phys_obj_idx = 0;
	bool isExistingObj = false; // Assert we have at least one mesh
	if (PhysVec.size() > 0)
		isExistingObj = true;

	// Initialize variables to edit
	unsigned int shapeType = 0;
	float inv_mass = 0.0f;
	glm::vec3 objAccel = glm::vec3(0);
	sPhysicsProperties* currObj = nullptr;





	if (isExistingObj)
	{
		currObj = PhysVec[m_phys_obj_idx];


		shapeType = (unsigned int)PhysVec[m_phys_obj_idx]->shapeType;
		objAccel = currObj->acceleration;
		inv_mass = currObj->inverse_mass;

	}



	// BUTTON TO ADD HITBOX
	static char teleName[32] = ""; ImGui::InputText("name", teleName, 32);

	if ((ImGui::Button("Add TeleBoxes")) && (std::strlen(teleName) > 0))
	{
		sPhysicsProperties* newteleboxes = new sPhysicsProperties();
		newteleboxes->shapeType = sPhysicsProperties::HITBOX;
		newteleboxes->setShape(new sPhysicsProperties::sHitBox());
		newteleboxes->friendlyName = teleName;

		m_pEngineController->addPhysObj(newteleboxes);
	}




	const char* shapeTypes[] = { "UNKNOWN_OR_UNDEFINED", "SPHERE", "PLANE", "TRIANGLE", "AABB", "CAPSULE", "MESH_OF_TRIANGLES_INDIRECT", "MESH_OF_TRIANGLES_LOCAL_VERTICES", "HITBOX"};
	static int stype_current_idx = 0;
	stype_current_idx = shapeType; // Set selected light type to one stored in the light
	const char* combo_preview_value = shapeTypes[stype_current_idx];
	if (ImGui::BeginCombo("Shape Types", combo_preview_value))
	{
		for (int n = 0; n < IM_ARRAYSIZE(shapeTypes); n++)
		{
			const bool is_selected = (stype_current_idx == n);
			if (ImGui::Selectable(shapeTypes[n], is_selected))
				stype_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	shapeType = stype_current_idx; // Set shape type


	ImGui::SeparatorText("----Shape-Specific Options----");
	if (isExistingObj)
	{
		// All objects have restitution, for now...
		float restitution = currObj->restitution;
		ImGui::DragFloat("Restitution", &restitution, 0.1f, 0.0f, FLT_MAX, "%.3f");
		currObj->restitution = restitution;

		if (currObj->shapeType == sPhysicsProperties::SPHERE)
		{
			// Radius
			float radius = ((sPhysicsProperties::sSphere*)currObj->pShape)->radius;
			ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, FLT_MAX, "%.3f");
			((sPhysicsProperties::sSphere*)currObj->pShape)->radius = radius;
		}
		else if (currObj->shapeType == sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT)
		{
			// Idk meshname?
		}
		else if (currObj->shapeType == sPhysicsProperties::CAPSULE)
		{
			// Radius
			// Halflength
			// Upvec?
		}
		else if (currObj->shapeType == sPhysicsProperties::HITBOX)
		{
			sPhysicsProperties::sHitBox* hitbox = (sPhysicsProperties::sHitBox*)currObj->pShape;
			glm::vec3 srcPos = hitbox->srcCentre;
			glm::vec3 dstPos = hitbox->dstCentre;
			float boxScale = hitbox->scale;

			ImGui::SeparatorText("TeleBox Options");
			// Source Pos
			ImGui::DragFloat("srcX", &srcPos.x, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
			ImGui::DragFloat("srcY", &srcPos.y, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
			ImGui::DragFloat("srcZ", &srcPos.z, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
			// Destination Pos
			ImGui::Separator();
			ImGui::DragFloat("dstX", &dstPos.x, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
			ImGui::DragFloat("dstY", &dstPos.y, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
			ImGui::DragFloat("dstZ", &dstPos.z, 0.1f, -FLT_MAX, FLT_MAX, "%.3f");
			// Scale
			ImGui::Separator();
			ImGui::DragFloat("bxScle", &boxScale, 0.1f, 0.0f, FLT_MAX, "%.3f");
			ImGui::Separator();

			hitbox->srcCentre = srcPos;
			hitbox->dstCentre = dstPos;
			hitbox->scale = boxScale;

		}
	}

	ImGui::SeparatorText("----General Options----");
	if (isExistingObj)
	{
		// Mass 
		ImGui::DragFloat("InvMass", &inv_mass, 0.1f, -1.0f, FLT_MAX, "%.3f");
		ImGui::Separator();
		// Acceleration
		ImGui::DragFloat("AccelX", &objAccel.x, 0.1f, 0.0f, FLT_MAX, "%.3f");
		ImGui::DragFloat("AccelY", &objAccel.y, 0.1f, 0.0f, FLT_MAX, "%.3f");
		ImGui::DragFloat("AccelZ", &objAccel.z, 0.1f, 0.0f, FLT_MAX, "%.3f");
		

	}

	// Setting the values
	if (isExistingObj)
	{
		if (currObj->shapeType != (sPhysicsProperties::eShape)shapeType)
		{
			currObj->shapeType = (sPhysicsProperties::eShape)shapeType;
			switch (currObj->shapeType)
			{
			case(sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT):
				currObj->setShape(new sPhysicsProperties::sMeshOfTriangles_Indirect(currObj->pTheAssociatedMesh->getMeshName()));
				break;
			case(sPhysicsProperties::SPHERE):
				currObj->setShape(new sPhysicsProperties::sSphere(1.0f));
				break;
			case(sPhysicsProperties::CAPSULE):
				currObj->setShape(new sPhysicsProperties::sCapsule(1.5f, 1.0f));
				break;
			case(sPhysicsProperties::HITBOX):
				currObj->setShape(new sPhysicsProperties::sHitBox());
				break;
			}
			
		}

		// General Updates
		currObj->acceleration = objAccel;
		currObj->inverse_mass = inv_mass;
	}


	ImGui::End();
}

// Window for changing light properties
void cLevelEditor::LightEditor(cLightManager* TheLights)
{
 	ImGui::Begin("Light Editor");
 
 	if (ImGui::BeginListBox("Available Objects"))
 	{
 		for (int n = 0; n < TheLights->NUMBER_OF_LIGHTS_IM_USING; n++)
 		{
 			const bool is_selected = (m_light_obj_idx == n);
 			if (ImGui::Selectable(TheLights->theLights[n].friendlyName.c_str(), is_selected))
				m_light_obj_idx = n;
 
 			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
 			if (is_selected)
 				ImGui::SetItemDefaultFocus();
 		}
 		ImGui::EndListBox();
 	}
// 		bool isExistingLight = true;
// 		if (m_vec_pMeshesToDraw.size() > 0)
// 			isExistingLight = false;
 
	std::string friendlyName = "";
 	glm::vec4 lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
 	glm::vec4 lightDir = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
 	glm::vec4 lightDiff = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
 	glm::vec4 lightSpec = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
 	glm::vec4 lightAtten = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
 	glm::vec4 lightParam1 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
 	glm::vec4 lightParam2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
// 		if (!isExistingLight)
// 		{
	friendlyName = TheLights->theLights[m_light_obj_idx].friendlyName;
 	lightPos = TheLights->theLights[m_light_obj_idx].position;
 	lightDir = TheLights->theLights[m_light_obj_idx].direction;
 	lightDiff = TheLights->theLights[m_light_obj_idx].diffuse;
 	lightSpec = TheLights->theLights[m_light_obj_idx].specular; // rgb = highlight colour, w = power
 	lightAtten = TheLights->theLights[m_light_obj_idx].atten; // x = constant, y = linear, z = quadratic, w = DistanceCutOff
 	lightParam1 = TheLights->theLights[m_light_obj_idx].param1; // x: light type    y: inner angle    z: outer angle
 	lightParam2 = TheLights->theLights[m_light_obj_idx].param2; // x: light on(1) or off(0)
 	/*}*/
 	static char lightname[32] = ""; 
 	//strcpy_s(lightname, TheLights.theLights[light_obj_idx].friendlyName.c_str()); // TODO too long a name will prob break this
 
 	ImGui::InputText("Light Name", lightname, 32);
 	if (ImGui::Button("Set New Name")) // Button to set new light friendlyname
 	{
		if (strlen(lightname) > 0)
			friendlyName = lightname;
			//TheLights->theLights[light_obj_idx].friendlyName = lightname;
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
	ImGui::DragFloat("Light Power", &lightDiff.w, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::SeparatorText("Specular");
 	ImGui::DragFloat("Red Specular", &lightSpec.x, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::DragFloat("Green Specular", &lightSpec.y, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::DragFloat("Blue Specular", &lightSpec.z, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::DragFloat("Spec Power", &lightSpec.w, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::SeparatorText("Attenuation");
 	ImGui::DragFloat("Constant", &lightAtten.x, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::DragFloat("Linear", &lightAtten.y, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::DragFloat("Quadratic", &lightAtten.z, 0.00001f, -FLT_MAX, +FLT_MAX, "%.5f");
 	ImGui::DragFloat("Distance Cutoff", &lightAtten.w, 0.1f, -FLT_MAX, +FLT_MAX, "%.5f");
 
 	ImGui::SeparatorText("Other Light Options");
 	const char* lightTypes[] = { "Point Light", "Spot Light", "Directional Light", "Ambient Light"};
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

	ImGui::SeparatorText("Other Options");
	ImGui::DragFloat("DistCutoff Fade", &lightParam1.w, 0.01f, -FLT_MAX, +FLT_MAX, "%.5f");


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
 
 
	m_pEngineController->setLightData(m_light_obj_idx, friendlyName, lightPos, lightDiff, lightSpec, lightAtten, lightDir, lightParam1, lightParam2);
//  		if (isExistingLight)
//  		{
//  			updateSelectedLight(light_obj_idx, lightPos, lightDiff, lightSpec, lightAtten, lightDir, lightParam1, lightParam2); // TODO light update
//  		}
 
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
		if (AvailableSaves.size() > 0)
			m_pEngineController->loadScene(AvailableSaves[saves_idx]);
	 		//m_pSceneManager->loadScene(availSaves[saves_idx]); // Will load a selected item from a list in the future    // TODO load scene
	}
	 
	 
	static char saveNameBuf[32] = ""; ImGui::InputText("Save Name", saveNameBuf, 32);
	ImGui::SameLine();
	if (ImGui::Button("Save Current Scene"))
	{
	 	if (std::strlen(saveNameBuf) > 0)
	 	{
	 		//m_pSceneManager->saveScene(saveNameBuf, m_vec_pMeshesToDraw, m_pTheLights);  // TODO scene save
			m_pEngineController->saveScene(saveNameBuf);
	 		memset(saveNameBuf, 0, 32); // Reset buffer
	 	}
	 			
	}
	 
	ImGui::End();
}

void cLevelEditor::DuplicateObj(cMesh* mesh, char* name, std::vector<sPhysicsProperties*> physVec)
{
	cMesh* newMesh = new cMesh();

	newMesh->meshName = mesh->meshName;
	newMesh->bIsVisible = mesh->bIsVisible;

	newMesh->bUseCustomColors = mesh->bUseCustomColors;
	newMesh->customColorRGBA = mesh->customColorRGBA;

	newMesh->transparencyAlpha = mesh->transparencyAlpha;
	newMesh->bUseDiscardMaskTex = mesh->bUseDiscardMaskTex;

	newMesh->bIsWireframe = mesh->bIsWireframe;
	newMesh->bDoNotLight = mesh->bDoNotLight;
	newMesh->bUseReflect = mesh->bUseReflect;
	newMesh->bUseRefract = mesh->bUseRefract;

	newMesh->scale = mesh->scale;
	newMesh->material = mesh->material;
	newMesh->isNavMesh = mesh->isNavMesh;

	newMesh->friendlyName = name;



	sPhysicsProperties* newPhys = new sPhysicsProperties();
	newPhys->friendlyName = name;
	newPhys->pTheAssociatedMesh = newMesh;

	for (sPhysicsProperties* currPhys : physVec)
	{
		if (mesh->uniqueID == currPhys->getUniqueID())
		{
			newPhys->position = currPhys->position;
			newPhys->setRotationFromQuat(currPhys->get_qOrientation());
			
			if (currPhys->shapeType == sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT)
			{
				newPhys->setShape(new sPhysicsProperties::sMeshOfTriangles_Indirect(newPhys->pTheAssociatedMesh->getMeshName()));
				newPhys->shapeType = sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT;
			}
			break;
		}
	}
	
	
	newMesh->uniqueID = newPhys->getUniqueID();

	m_pEngineController->addCustomObject(newMesh, newPhys);
}

cLevelEditor::cLevelEditor(GLFWwindow* window)
	: m_ShowLightEditor(false)
	, m_ShowMeshEditor(false)
	, m_ShowSceneManager(false)
	, m_ShowMaterialEditor(false)
	, m_mesh_obj_idx(0)
	, m_phys_obj_idx(0)
	, m_light_obj_idx(0)
	, m_RenderDebug(false)
	, m_JustDeleted(false)
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
