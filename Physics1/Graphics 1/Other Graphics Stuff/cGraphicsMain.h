#pragma once

#include "OpenGLCommon.h"
#include <glm/vec3.hpp>
#include "cMesh.h"
#include <vector>
#include <string>
#include <glad/glad.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "../cVAOManager/cVAOManager.h"
#include "../cInputHandler.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include "cLightManager.h"
#include "../cSceneManagement.h"

#include "../TextureManager/cBasicTextureManager.h"



//#include "../cPlayer.h"

class cGraphicsMain
{
public:
	static cGraphicsMain* getGraphicsMain(void);


	bool Update(double deltaTime); // return false when window is triggered to close
	void Destroy();
	//void addToDrawMesh(cMesh* newMesh);
	void removeFromDrawMesh(int ID);
	void switchScene(std::vector< cMesh* > newMeshVec, std::vector<cLight> newLights); // New mesh vector to copy over (will properly release all resources tied to the old one); vector of lights to copy over to the new one

	void setCameraParams(glm::vec3 camPos, glm::vec3 camDir);

	// Level Editor Calls
	GLFWwindow* getWindow(void);
	void getAvailableModels(std::vector<std::string>* ModelVec);
	void getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights);

	// Level Editor Setting
	void updateMesh(int meshID, std::string newFriendlyName, std::string newTextureNames[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor);  // Updates and existing object by reference to its friendly name (also from the gui)



private:
	cGraphicsMain();
	bool Initialize();

	cMesh* m_pFindMeshByFriendlyName(std::string friendlyNameToFind);
	void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID);
	bool LoadModels(void);
	bool LoadTextures(void);

	void addNewMesh(std::string fileName, char* friendlyName); // Adding new objects from the gui
	//void updateSelectedMesh(int meshIdx, std::string friendlyName, glm::vec3 newPos, glm::vec3 newOri, glm::vec3 customColor, float newScale, bool doNotLight, bool useCustomColor);  // Updates and existing object by reference to its friendly name (also from the gui)
	
	void addNewLight(char* friendlyName);
	void updateSelectedLight(int lightIdx, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2);
	void duplicateMesh(int meshIdx, char* newName); // Duplicates the currently selected mesh

	void deleteMesh(int meshIDX);

	void flyCameraInput(int width, int height);
	float m_FlyCamSpeed = 0.2f;


	void SetUpTextures(cMesh* pCurrentMesh, GLuint shaderProgramID);


	std::vector<std::string> m_AvailableModels; // String of model file names to choose from
	std::vector<std::string> m_AvailableTextures;

	//cPlayer* m_player;

	glm::vec3 m_cameraEye;
	glm::vec3 m_cameraTarget;
	glm::vec3 m_cameraRotation;
	glm::vec3 m_upVector;


	cShaderManager* m_pShaderThing;
	GLuint m_shaderProgramID;

	//bool m_isShieldOn = false;
	//int m_explosionIDs = -1; // IDs for explosions and grey spheres, use negatives to separate from all other mesh ID's (which are in the positives)
	double m_lastTime;
	GLFWwindow* m_window;
	cVAOManager* m_pMeshManager = NULL;
	cBasicTextureManager* m_pTextureManager = NULL;

	std::vector< cMesh* > m_vec_pMeshesToDraw;

	cLightManager* m_pTheLights;

	// ImGui
	bool m_ShowMeshEditor;   // 
	bool m_ShowLightEditor;  // Windows to edit existing meshes and lights respectively
	bool m_ShowSceneManager; //

	ImGuiIO m_io; // ImGui io
	cInputHandler* m_InputHandler;

	cSceneManagement* m_pSceneManager;

	static cGraphicsMain* m_pTheOnlyGraphicsMain;
};