#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "cSceneManagement.h"

#include "cEngineController.h" // Callbacks to edit object parameters


#include <vector>
#include <string>



class cLevelEditor
{
public:
	//cLevelEditor* GetInstance(void);
	cLevelEditor(GLFWwindow* window);
	~cLevelEditor();

	void Update(); // The running loop of the editor

	// Editor Windows
	void RootWindow(std::vector<cMesh*> ActiveMeshVec);
	void MeshEditor(std::vector<cMesh*> ActiveMeshVec);
	void LightEditor();
	void SceneManager();

private:
	

	// Window States
	bool m_ShowMeshEditor;   // 
	bool m_ShowLightEditor;  // Windows to edit existing meshes and lights respectively
	bool m_ShowSceneManager; //

	// Stored Info
	std::vector<std::string> m_AvailableModels;

	// Saved Navigation Info
	int m_mesh_obj_idx;



	ImGuiIO m_io; // ImGui io
	//cInputHandler* m_InputHandler; // Don't know exactly how to manage a general input
	GLFWwindow* m_window;

	cSceneManagement* m_pSceneManager;
	cEngineController* m_pEngineController;
};