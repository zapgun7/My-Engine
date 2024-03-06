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
// #include "../cInputHandler.h"
#include "../Basic Shader Manager/cShaderManager.h"
#include "cLightManager.h"
#include "../cSceneManagement.h"

#include "../TextureManager/cBasicTextureManager.h"


#include "FBO/cFBO.h"

//#include "cParticleSystem.h"
#include "../cParticleManager.h"



//#include "../cPlayer.h"

class cGraphicsMain
{
public:
	static cGraphicsMain* getGraphicsMain(void);


	bool Update(double deltaTime); // return false when window is triggered to close
	bool Update2(double deltaTime);
	void Destroy();
	//void addToDrawMesh(cMesh* newMesh);
	void removeFromDrawMesh(int ID);
	void switchScene(std::vector< cMesh* > newMeshVec, std::vector<cLight> newLights); // New mesh vector to copy over (will properly release all resources tied to the old one); vector of lights to copy over to the new one

	void setCameraParams(glm::vec3 camPos, glm::vec3 camDir);

	cVAOManager* getVAOManager(void); // For setting the physics' vaoManager

	// Level Editor Calls
	GLFWwindow* getWindow(void);
	void getAvailableModels(std::vector<std::string>* ModelVec);
	void getAvailableTextures(std::vector<std::string>* TexVec);
	void getActiveMeshes(std::vector<cMesh*>* MeshVec);
	void getActiveLights(cLightManager* TheLights);

	// Level Editor Setting
	void updateMesh(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor);  // Updates and existing object by reference to its friendly name (also from the gui)
	void updateSelectedLight(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2);
	void updateDebugStates(bool useDebug, int selectedMesh, int selectedLight);

	// Level Editor Adding
	void addNewMesh(cMesh* newMesh); // Adding new objects from the gui


	void UpdateCamera(glm::vec3 position, glm::quat qRotation);


	// Bools for debugging mode or whatever for the level editor!
	bool renderDebug;
	int selectedMesh;  // -1 if nothing selected
	int selectedLight; //
	

private:
	cGraphicsMain();
	bool Initialize();

	cMesh* m_pFindMeshByFriendlyName(std::string friendlyNameToFind);
	void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID);
	bool LoadModels(void);
	bool LoadTextures(void);
	bool LoadParticles(void);

	// Draw Passes
	void DrawPass_1(GLuint shaderProgramID, int screenWidth, int screenHeight, glm::vec3 cameraEye, glm::vec3 cameraTarget);
	void DrawPass_FSQ(GLuint shaderProgramID, int screenWidth, int screeneight);


	//void updateSelectedMesh(int meshIdx, std::string friendlyName, glm::vec3 newPos, glm::vec3 newOri, glm::vec3 customColor, float newScale, bool doNotLight, bool useCustomColor);  // Updates and existing object by reference to its friendly name (also from the gui)
	
	void addNewLight(char* friendlyName);
	
	void duplicateMesh(int meshIdx, char* newName); // Duplicates the currently selected mesh

	void flyCameraInput(int width, int height);
	float m_FlyCamSpeed = 1.0f;


	void SetUpTextures(cMesh* pCurrentMesh, GLuint shaderProgramID);


	std::vector<std::string> m_AvailableModels; // String of model file names to choose from
	std::vector<std::string> m_AvailableTextures;

	//cPlayer* m_player;

	glm::vec3 m_cameraEye;
	glm::vec3 m_cameraTarget;
	glm::quat m_qCameraRotation;
	glm::vec3 m_cameraRotation;
	glm::vec3 m_upVector;


	cShaderManager* m_pShaderThing;
	GLuint m_shaderProgramID;

	

	double m_lastTime;
	GLFWwindow* m_window;
	cVAOManager* m_pMeshManager = NULL;
	cBasicTextureManager* m_pTextureManager = NULL;

	std::vector< cMesh* > m_vec_pMeshesToDraw;
	std::vector < cMesh* > m_vec_pTransMeshesToDraw;
	std::vector < cMesh* > m_vec_pAllMeshes; // All meshes, keeps them in order when passing to level editor

	cLightManager* m_pTheLights;


	/////// PARTICLE STUFF ////////
	cMesh* m_pBasicParticle = nullptr;
	cParticleManager* m_pParticleManager = nullptr;



	///////// FBO STUFF ///////////
	cFBO* m_pFBO_1 = nullptr;
	cFBO* m_pFBO_2 = nullptr;



	////////// VERLET DRAWING ///////
	cMesh* VerletObject = nullptr;
// public:
// 	void UpdateVAO(std::string meshName, sModelDrawInfo modelInfo);
// private:



	ImGuiIO m_io; // ImGui io
	//cInputHandler* m_InputHandler;

	cSceneManagement* m_pSceneManager;

	static cGraphicsMain* m_pTheOnlyGraphicsMain;
};