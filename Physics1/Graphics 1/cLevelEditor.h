#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
//#include "cSceneManagement.h"

#include "cEngineController.h" // Callbacks to edit object parameters


#include <vector>
#include <string>

//struct sPhysicsProperties;

class cLevelEditor
{
public:
	//cLevelEditor* GetInstance(void);
	cLevelEditor(GLFWwindow* window);
	~cLevelEditor();

	void Update(double deltaTime); // The running loop of the editor

	// Editor Windows
	void RootWindow(std::vector<cMesh*> ActiveMeshVec, double dt);
	void MeshEditor(std::vector<cMesh*> ActiveMeshVec, std::vector<sPhysicsProperties*> PhysVec);
	void MaterialEditor(cMesh* SelectedMesh);
	void PhysEditor(std::vector<sPhysicsProperties*> PhysVec);
	void LightEditor(cLightManager* TheLights);
	void SceneManager(std::vector<std::string> AvailableSaves);

private:
	

	// Window States
	bool m_ShowMeshEditor;    // 
	bool m_ShowLightEditor;   // Windows to edit existing meshes and lights respectively
	bool m_ShowSceneManager;  //
	bool m_ShowPhysicsEditor; //
	bool m_ShowMaterialEditor;//

	bool m_RenderDebug;
	bool m_JustDeleted; // Bandaid fix

	// Stored Info
	std::vector<std::string> m_AvailableModels;
	std::vector<std::string> m_AvailableTextures;

	// Saved Navigation Info
	int m_mesh_obj_idx;
	int m_phys_obj_idx;
	int m_light_obj_idx;



	ImGuiIO m_io; // ImGui io
	//cInputHandler* m_InputHandler; // Don't know exactly how to manage a general input
	GLFWwindow* m_window;

	//cSceneManagement* m_pSceneManager;
	cEngineController* m_pEngineController;
};