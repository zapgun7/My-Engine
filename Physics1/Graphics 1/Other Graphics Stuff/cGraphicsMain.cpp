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


#include "../cInputHandler.h"





// void cGraphicsMain::UpdateVAO(std::string meshName, sModelDrawInfo modelInfo)
// {
// 	m_pMeshManager->UpdateVAOBuffers(meshName, modelInfo, m_shaderProgramID);
// }

cGraphicsMain* cGraphicsMain::m_pTheOnlyGraphicsMain = NULL;
//extern cPhysics* g_pPhysics;


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
	m_cameraEye = glm::vec3(0.0f, 0.0f, 0.0f);
	m_cameraTarget = glm::vec3(1.0f, -0.2f, 0.0f);
	m_cameraRotation = glm::vec3(0.0, 0.0f, 0.0f);
	m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	renderDebug = true;
	selectedMesh = -1;
	selectedLight = -1;
	//m_player = new cPlayer();
	//m_io = ImGui::GetIO();
}

bool cGraphicsMain::Initialize()
{
	//m_InputHandler = new cInputHandler(); // Will eventually move these 2 somewhere that makes sense TODO
	m_pSceneManager = new cSceneManagement();
	m_pSceneManager->Initialize();

	SetUpCycleArray();

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // This causes cubemap creation to crash on my laptop
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


	m_window = glfwCreateWindow(640, 480, "Ausgine Render", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwSetKeyCallback(m_window, key_callback);

	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//glfwSwapInterval(0); // No stinky v-sync
	glfwSwapInterval(1); // Stinky, but is easy on my gpu :)


	m_pShaderThing = new cShaderManager();
	m_pShaderThing->setBasePath("assets/shaders");


	// Default Shader
	cShaderManager::cShader vertexShader;
	vertexShader.fileName = "vertexShader01.glsl";

	cShaderManager::cShader fragmentShader;
	fragmentShader.fileName = "fragmentShader01.glsl";

	if (!m_pShaderThing->createProgramFromFile("shader01", vertexShader, fragmentShader))
	{
		std::cout << "Error: Couldn't compile or link:" << std::endl;
		std::cout << m_pShaderThing->getLastError() << std::endl;
		return 0;
	}


	// FSQ Render Shader
	cShaderManager::cShader FSQvertexShader;
	FSQvertexShader.fileName = "FSQVertexShader.glsl";

	cShaderManager::cShader FSQfragmentShader;
	FSQfragmentShader.fileName = "FSQFragmentShader.glsl";

	if (!m_pShaderThing->createProgramFromFile("FSQShader", FSQvertexShader, FSQfragmentShader))
	{
		std::cerr << "Error: Couldn't compile or link:" << std::endl;
		std::cerr << m_pShaderThing->getLastError() << std::endl;
		return 0;
	}


	// Compute Shader
	cShaderManager::cShader computeShader;
	computeShader.fileName = "computeShader01.glsl";

	if (!m_pShaderThing->createComputeProgramFromFile("compute01", computeShader))
	{
		std::cerr << "Error: Couldn't compile or link compute:" << std::endl;
		std::cerr << m_pShaderThing->getLastError() << std::endl;
		return 0;
	}



	// Heatmap Generation Shader
	cShaderManager::cShader HMvertexShader;
	HMvertexShader.fileName = "heatmapVertexShader.glsl";

	cShaderManager::cShader HMfragmentShader;
	HMfragmentShader.fileName = "heatmapFragmentShader.glsl";

	if (!m_pShaderThing->createProgramFromFile("HMShader", HMvertexShader, HMfragmentShader))
	{
		std::cerr << "Error: Couldn't compile or link:" << std::endl;
		std::cerr << m_pShaderThing->getLastError() << std::endl;
		return 0;
	}


	//
	m_shaderProgramID = m_pShaderThing->getIDFromFriendlyName("shader01");

	m_pMeshManager = new cVAOManager();
	//::g_pPhysics->setVAOManager(m_pMeshManager); //UNPHYS

	m_pMeshManager->setBasePath("assets/models");

	m_pTheLights = new cLightManager();
	m_pTheLights->SetUniformLocations(m_shaderProgramID);

	// Default ambient & directional light
	m_pTheLights->theLights[0].param1.x = 3;
	m_pTheLights->theLights[0].param2.x = 1;
	m_pTheLights->theLights[0].diffuse.w = 0.1f;
	//m_pTheLights->theLights[0].direction = glm::vec4(.2, -.6, .2, 0);
	m_pTheLights->theLights[0].friendlyName = "ambient";

	m_pTheLights->theLights[1].param1.x = 2;
	m_pTheLights->theLights[1].param2.x = 1;
	m_pTheLights->theLights[1].direction = glm::vec4(-.05, -.3, -.05, 0);
	m_pTheLights->theLights[1].friendlyName = "GenDir";



	////////////////// TEXTURE LOADING ///////////////
	m_pTextureManager = new cBasicTextureManager();
	m_pTextureManager->SetBasePath("assets/textures");

	//m_pTextureManager->Create2DTextureFromBMPFile("rosewood.bmp", true);
	//m_pTextureManager->Create2DTextureFromBMPFile("rosewood_n.bmp", true);
	//m_pTextureManager->Create2DTextureFromBMPFile("Water_Texture_01.bmp", true);



	/////////////////////////////////////////////////


	// MODEL LOADING /////////////////

	LoadModels();
	LoadTextures();


	////// PARTICLES //////////
	LoadParticles();


	// Initialize lights here if ya want em
	m_lastTime = glfwGetTime();



	// Generate additional frame buffers
	std::string FBOError;

	m_pFBO_1 = new cFBO();
	if (!m_pFBO_1->init(1920, 1080, FBOError))
	{
		std::cout << "Error creating FBO_1: " << FBOError << std::endl;
	}
	else
	{
		std::cout << "FBO_1 created OK" << std::endl;
	}

	m_pFBO_2 = new cFBO();
	if (!m_pFBO_2->init(240, 135, FBOError)) // Creating this at 1/2 resolution for blurring effect
	{
		std::cout << "Error creating FBO_2: " << FBOError << std::endl;
	}
	else
	{
		std::cout << "FBO_2 created OK" << std::endl;
	}

	m_pFBO_3 = new cFBO();
	if (!m_pFBO_3->init(1920, 1080, FBOError))
	{
		std::cout << "Error creating FBO_3: " << FBOError << std::endl;
	}
	else
	{
		std::cout << "FBO_3 created OK" << std::endl;
	}
	m_pFBO_3->clearBuffers(true, true);

	m_pFBO_4 = new cFBO();
	if (!m_pFBO_4->init(1920, 1080, FBOError))
	{
		std::cout << "Error creating FBO_4: " << FBOError << std::endl;
	}
	else
	{
		std::cout << "FBO_4 created OK" << std::endl;
	}

	m_pFBO_5 = new cFBO();
	if (!m_pFBO_5->init(1920, 1080, FBOError))
	{
		std::cout << "Error creating FBO_5: " << FBOError << std::endl;
	}
	else
	{
		std::cout << "FBO_5 created OK" << std::endl;
	}

	m_pFBO_6 = new cFBO();
	if (!m_pFBO_6->init(1920, 1080, FBOError))
	{
		std::cout << "Error creating FBO_6: " << FBOError << std::endl;
	}
	else
	{
		std::cout << "FBO_6 created OK" << std::endl;
	}

	m_pFBO_7 = new cFBO();
	if (!m_pFBO_7->init(1920, 1080, FBOError))
	{
		std::cout << "Error creating FBO_7: " << FBOError << std::endl;
	}
	else
	{
		std::cout << "FBO_7 created OK" << std::endl;
	}


// 	glGenTextures(1, &computeTexOutput);
// 	glBindTexture(GL_TEXTURE_2D, computeTexOutput);
// 
// 	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 1920, 1080);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// // 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1920, 1080, 0, GL_RGBA,
// // 		GL_FLOAT, NULL);
// 
// 	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &computeTexOutput);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, computeTexOutput);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (unsigned int)1920, (unsigned int)1080, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(0, computeTexOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


	// Making the small buffer to communicate with the compute
	glm::vec4 tempForDataFill(0.0f);
	glGenBuffers(1, &computeCommunicationBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeCommunicationBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER,
				 sizeof(glm::vec4),
				 &tempForDataFill,
				 GL_DYNAMIC_DRAW);




	GenerateUBOs();

	return 1;
}


void cGraphicsMain::SetUpCycleArray(void)
{
	const int CELLCOUNT = 44;
	
	m_iBlurOffsetIndices_pulse = new int[CELLCOUNT]; // Closest cells -> farthest
	

	// TODO!!! Make these below

	//m_iBlurOffsetIndices_wave = new int[CELLCOUNT]; // Travels in a circle around the target pixel
	m_iBlurOffsetIndices_dispersed = new int[4]; // Dispersed noisy distribution


	
	for (unsigned int cellIDX = 0; cellIDX < CELLCOUNT; cellIDX++)
	{
		// Designed this way (closest to farthest)
		m_iBlurOffsetIndices_pulse[cellIDX] = cellIDX;
	}

	m_iBlurOffsetIndices_dispersed[0] = 1;
	m_iBlurOffsetIndices_dispersed[1] = 3;
	m_iBlurOffsetIndices_dispersed[2] = 2;
	m_iBlurOffsetIndices_dispersed[3] = 4;


// 	m_iBlurOffsetIndices_dispersed[0] = 1;
// 	m_iBlurOffsetIndices_dispersed[1] = 16;
// 	m_iBlurOffsetIndices_dispersed[2] = 14;
// 	m_iBlurOffsetIndices_dispersed[3] = 5;
// 	m_iBlurOffsetIndices_dispersed[4] = 15;
// 	m_iBlurOffsetIndices_dispersed[5] = 10;
// 	m_iBlurOffsetIndices_dispersed[6] = 7;
// 	m_iBlurOffsetIndices_dispersed[7] = 4;
// 	m_iBlurOffsetIndices_dispersed[8] = 13;
// 	m_iBlurOffsetIndices_dispersed[9] = 17;
// 	m_iBlurOffsetIndices_dispersed[10] = 2;
// 	m_iBlurOffsetIndices_dispersed[11] = 19;
// 	m_iBlurOffsetIndices_dispersed[12] = 9;
// 	m_iBlurOffsetIndices_dispersed[13] = 11;
// 	m_iBlurOffsetIndices_dispersed[14] = 8;
// 	m_iBlurOffsetIndices_dispersed[15] = 6;
// 	m_iBlurOffsetIndices_dispersed[16] = 18;
// 	m_iBlurOffsetIndices_dispersed[17] = 12;
	

}


int cGraphicsMain::GenerateUBOs(void)
{
	glGenBuffers(1, &m_UBOMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBOMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * (sizeof(glm::mat4)), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Bind buffer to binding point 0
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_UBOMatrices, 0, 2 * sizeof(glm::mat4));

	return 1;
}

bool cGraphicsMain::UpdateOLD(double deltaTime) // Main "loop" of the window. Not really a loop, just gets called every tick
{
	// Check input for camera movement
	//m_InputHandler->queryKeys(m_window);


	//return Update2(deltaTime);
	//return UpdateProject(deltaTime);



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





	//flyCameraInput(width, height); // UPDATE CAMERA STATS





	GLint eyeLocation_UL = glGetUniformLocation(m_shaderProgramID, "eyeLocation");
	glUniform4f(eyeLocation_UL,
		m_cameraEye.x, m_cameraEye.y, m_cameraEye.z, 1.0f);



	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 matProjection = glm::perspective(0.7f,
		ratio,
		0.1f,
		1100.0f); // n/f plane



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

	/////////// OFFSET UPDATE /////////////
	for (unsigned int i = 0; i < m_vec_pAllMeshes.size(); i++)
	{
		m_vec_pAllMeshes[i]->uv_Offset_Scale.x += m_vec_pAllMeshes[i]->uvOffsetSpeed.x * static_cast<float>(deltaTime);
		m_vec_pAllMeshes[i]->uv_Offset_Scale.y += m_vec_pAllMeshes[i]->uvOffsetSpeed.y * static_cast<float>(deltaTime);

		// Keeps values within proper range (good for longevity)
// 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.x > 1.0f)
// 			m_vec_pAllMeshes[i]->uv_Offset_Scale.x -= floor(m_vec_pAllMeshes[i]->uv_Offset_Scale.x);
// 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.x < 0.0f)
// 			m_vec_pAllMeshes[i]->uv_Offset_Scale.x += ceil(m_vec_pAllMeshes[i]->uv_Offset_Scale.x);
// 
// 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.y > 1.0f)
// 			m_vec_pAllMeshes[i]->uv_Offset_Scale.y -= floor(m_vec_pAllMeshes[i]->uv_Offset_Scale.y);
// 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.y < 0.0f)
// 			m_vec_pAllMeshes[i]->uv_Offset_Scale.y += ceil(m_vec_pAllMeshes[i]->uv_Offset_Scale.y);
	}



	// *********************************************************************
	// Draw all the objects
	for (unsigned int index = 0; index != m_vec_pMeshesToDraw.size(); index++) // Prob black or smthn
	{
		cMesh* pCurrentMesh = m_vec_pMeshesToDraw[index];
		if (pCurrentMesh->transparencyAlpha < 0.99f)
		{
			m_vec_pTransMeshesToDraw.push_back(pCurrentMesh);
			m_vec_pMeshesToDraw.erase(m_vec_pMeshesToDraw.begin() + index);
			index--;
			continue;
		}

		if (pCurrentMesh->bIsVisible)
		{

			glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

			DrawObject(pCurrentMesh, matModel, m_shaderProgramID);
		}//if (pCurrentMesh->bIsVisible)

	}//for ( unsigned int index



	//////////// PARTICLE RENDERING ///////////
// 	m_pParticleManager->Update(deltaTime);
// 
// 	std::vector<cParticleSystem::sParticleRender> vecParticles_to_draw;
// 	m_pParticleManager->getParticleList(vecParticles_to_draw);
// 
// 	for (cParticleSystem::sParticleRender& curParticle : vecParticles_to_draw)
// 	{
// 		glm::mat4 matModel = glm::mat4(1.0f);
// 
// 		m_pBasicParticle->drawPosition = curParticle.position;
// 
// 		DrawObject(m_pBasicParticle, matModel, m_shaderProgramID);
// 	}






	// Time per frame (more or less)
// 	double currentTime = glfwGetTime();
// 	double deltaTime = currentTime - m_lastTime;
// 	//        std::cout << deltaTime << std::endl;
// 	m_lastTime = currentTime;


	// Quickly Draw hardcoded skybox
	{
		// HACK: I'm making this here, but hey...
		cMesh theSkyBox;
		theSkyBox.meshName = "Sphere_1_unit_Radius.ply";
		theSkyBox.isSkybox = true;
		//theSkyBox.setUniformDrawScale(10.0f);

		theSkyBox.setUniformDrawScale(1000.0f);
		theSkyBox.setDrawPosition(m_cameraEye);
		//            theSkyBox.bIsWireframe = true;

					// Depth test
		//            glDisable(GL_DEPTH_TEST);       // Writes no matter what
					// Write to depth buffer (depth mask)
		//            glDepthMask(GL_FALSE);          // Won't write to the depth buffer

					// uniform bool bIsSkyBox;
		//GLint bIsSkyBox_UL = glGetUniformLocation(m_shaderProgramID, "bUseHeightmap_IsSkyBox_UseDiscard_NONE");
		//glUniform4f(bIsSkyBox_UL, (GLfloat)0.0f, (GLfloat)1.0f, (GLfloat)0.0f, (GLfloat)0.0f);

		// The normals for this sphere are facing "out" but we are inside the sphere
		glCullFace(GL_FRONT);

		DrawObject(&theSkyBox, glm::mat4(1.0f), m_shaderProgramID);

		/*glUniform1f(bIsSkyBox_UL, (GLfloat)GL_FALSE);*/
		//glUniform4f(bIsSkyBox_UL, (GLfloat)0.0f, (GLfloat)0.0f, (GLfloat)0.0f, (GLfloat)0.0f);

		// Put the culling back to "normal" (back facing are not drawn)
		glCullFace(GL_BACK);
	}

	/// End of skybox

	// Now we draw all transparent meshes


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (unsigned int index = 0; index != m_vec_pTransMeshesToDraw.size(); index++)
	{
		cMesh* pCurrentMesh = m_vec_pTransMeshesToDraw[index];
		if (pCurrentMesh->transparencyAlpha > 0.99f)
		{
			m_vec_pMeshesToDraw.push_back(pCurrentMesh);
			m_vec_pTransMeshesToDraw.erase(m_vec_pTransMeshesToDraw.begin() + index);
			index--;
			continue;
		}

		if (pCurrentMesh->bIsVisible)
		{
			if (index < m_vec_pTransMeshesToDraw.size() - 1) // If not the last in vec, try to sort by distance
			{
				if (glm::distance(m_vec_pTransMeshesToDraw[index]->drawPosition, m_cameraEye) < glm::distance(m_vec_pTransMeshesToDraw[index + 1]->drawPosition, m_cameraEye))
				{
					cMesh* tempMesh = m_vec_pTransMeshesToDraw[index];
					m_vec_pTransMeshesToDraw[index] = m_vec_pTransMeshesToDraw[index + 1];
					m_vec_pTransMeshesToDraw[index + 1] = tempMesh;
					pCurrentMesh = m_vec_pTransMeshesToDraw[index];
				}
			}

			glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

			DrawObject(pCurrentMesh, matModel, m_shaderProgramID);
		}//if (pCurrentMesh->bIsVisible)

	}
	glDisable(GL_BLEND);



	// Now draw debug stuff
	glDisable(GL_DEPTH_TEST);

	if (renderDebug)
	{
		glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix
		cMesh basicSphere;
		basicSphere.meshName = "Sphere_1_unit_Radius.ply";
		basicSphere.bDoNotLight = true;
		basicSphere.bUseCustomColors = true;
		basicSphere.customColorRGBA = glm::vec4(0, 1, 0, 1); // Green
		basicSphere.bIsWireframe = true;

		// Start by drawing a basic sphere at all the active light locations
		for (unsigned int i = 0; i < m_pTheLights->NUMBER_OF_LIGHTS_IM_USING; i++)
		{
			if (m_pTheLights->theLights[i].param2.x == 1) // If light is on
			{
				if (selectedLight == i) // Have different color for selected light
					basicSphere.customColorRGBA = glm::vec4(1, 0, 0, 1);

				basicSphere.drawPosition = m_pTheLights->theLights[i].position;
				DrawObject(&basicSphere, matModel, m_shaderProgramID);
				basicSphere.customColorRGBA = glm::vec4(0, 1, 0, 1);
			}
		}

		// Draw a Cyan sphere at the selected mesh position
		if (selectedMesh > -1)
		{
			basicSphere.drawPosition = m_vec_pAllMeshes[selectedMesh]->drawPosition;
			basicSphere.customColorRGBA = glm::vec4(0, .5, .5, 1);
			DrawObject(&basicSphere, matModel, m_shaderProgramID);
		}


	}


	glEnable(GL_DEPTH_TEST);

	glfwPollEvents();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);

	if (glfwWindowShouldClose(m_window))
		return 1;
	else
		return 0;
}


// Update for framebuffer stuff
bool cGraphicsMain::Update(double deltaTime)
{
	// Better system here, checking every ~.2s but still lowers frames a good amount when up close
	const float TIMETILLUPDATEINTERVAL = 0.2f;
	static float timeTillUpdate = TIMETILLUPDATEINTERVAL;
	timeTillUpdate -= static_cast<float>(deltaTime);



	// Start by checking if the shaded object is in view (based off of last frame)
	if (timeTillUpdate <= 0)
	{
		timeTillUpdate = TIMETILLUPDATEINTERVAL;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeCommunicationBuffer);
		glm::vec4* computeInfo =
			(glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
				0,
				sizeof(glm::vec4),
				GL_MAP_READ_BIT); // We want to read and reset it

		isShadedObjVisible = (bool)computeInfo->x; // Get info as a bool
		//computeInfo->x = 0.0f; // Reset it

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// Set vec4 to 0's
		static float temp = 0.0f;
		glBufferSubData(GL_SHADER_STORAGE_BUFFER,
						0,
						sizeof(float),
						&temp);

		// unbind
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}


	{
		float ratio;
		ratio = m_pFBO_1->width / (float)m_pFBO_1->height;

		glBindFramebuffer(GL_FRAMEBUFFER, m_pFBO_1->ID);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, m_pFBO_1->width, m_pFBO_1->height);

		m_pFBO_1->clearBuffers(true, true);

		glm::vec3 scene_1_CameraEye = m_cameraEye;
		glm::vec3 scene_1_CameraTarget = m_cameraTarget;

		DrawPass_1(m_shaderProgramID, m_pFBO_1->width, m_pFBO_1->height, glm::vec4(scene_1_CameraEye, 1.0f), scene_1_CameraTarget);
	}

	// Draw HeatMap
	{
		float ratio;
		ratio = m_pFBO_2->width / (float)m_pFBO_2->height;
		glBindFramebuffer(GL_FRAMEBUFFER, m_pFBO_2->ID);

		glViewport(0, 0, m_pFBO_2->width, m_pFBO_2->height);

		m_pFBO_2->clearBuffers(true, true);

		unsigned int HMID = m_pShaderThing->getIDFromFriendlyName("HMShader");
		
		glUseProgram(HMID);

		DrawPass_HM(HMID, m_pFBO_2->width, m_pFBO_2->height, glm::vec4(m_cameraEye, 1.0f), m_cameraTarget, deltaTime);
	}


	// Compute shader
	if (true)
	{
		cShaderManager::cShaderProgram* computeProg = m_pShaderThing->GetShaderProgramFromFriendlyName("compute01");
		//unsigned int computeID = m_pShaderThing->getIDFromFriendlyName("compute01");
		//m_pShaderThing->useShaderProgram("compute01");
		glUseProgram(computeProg->ID);
		


		// Output
		glBindImageTexture(0, computeTexOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		GLint output_UL = glGetUniformLocation(computeProg->ID, "imgOutput");
		glUniform1i(output_UL, computeTexOutput);

		// Regular Render FBO
		glBindImageTexture(1, m_pFBO_1->colourTexture_0_ID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		GLint inputVals_UL = glGetUniformLocation(computeProg->ID, "imgInput");
		glUniform1i(inputVals_UL, m_pFBO_1->colourTexture_0_ID);

		// Heatmap FBO
// 		GLint hmUnitNumber = 51;
// 		glBindImageTexture(2, m_pFBO_2->colourTexture_0_ID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
// 		GLint heatMap_UL = glGetUniformLocation(computeProg->ID, "heatMap");
// 		glUniform1i(heatMap_UL, m_pFBO_2->colourTexture_0_ID);
		GLint hmUnitNumber = 51;
		glActiveTexture(GL_TEXTURE0 + hmUnitNumber);
		//glBindTexture(GL_TEXTURE_2D, m_pFBO_1->colourTexture_0_ID);//m_pFBO_1->colourTexture_0_ID);
		glBindTexture(GL_TEXTURE_2D, m_pFBO_2->colourTexture_0_ID);

		GLint hmTex_UL = glGetUniformLocation(computeProg->ID, "heatMap");
		glUniform1i(hmTex_UL, hmUnitNumber);



		// Noise Texture
		GLint textureUnitNumber = 50; // TODO find a similar perlin noise texture with more "holes"; they look spooky
		GLuint noiseTex = m_pTextureManager->getTextureIDFromName("perlinnoise.bmp");//("perlinnoise.bmp");
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, noiseTex);
		GLint nouseTex_UL = glGetUniformLocation(computeProg->ID, "noise");
		glUniform1i(nouseTex_UL, textureUnitNumber);


		// Bind buffer for bool communication
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, computeCommunicationBuffer);


		static cInputHandler* input = cInputHandler::GetInstance();
		static float mouseX = 0;
		static float actualMouseX = 0;
		static float mouseY = 0;
		static float actualMouseY = 0;

		double deltaX, deltaY;

		input->GetMouseDeltas(deltaX, deltaY);
		if (input->IsMousePressed(GLFW_MOUSE_BUTTON_RIGHT)) // For now using the editor
		{
			mouseX += deltaX * 0.8f;
			mouseY += deltaY * 0.9f;
			actualMouseX += deltaX;
			actualMouseY += deltaY;
		}
		// Have the noise delay-follow the mouse
		mouseX += (actualMouseX - mouseX) * static_cast<float>(deltaTime) * 0.6f;
		mouseY += (actualMouseY - mouseY) * static_cast<float>(deltaTime) * 0.2f;




		////////// BLUR CYCLE //////////
 		const int CYCLECOUNT = 44;
		//const float CYCLECOUNTHALF = CYCLECOUNT / 2.0f;

 		static int cycle = 0;
		// Setup delay time for pulse use
// 		const float CYCLEINTERVAL = 0.05f;
// 		const float EXTREMETYTIMEBOOST = 0.1f;
// 		static float timeTillCycleUpdate = CYCLEINTERVAL;
// 		timeTillCycleUpdate -= static_cast<float>(deltaTime);
// 		if (timeTillCycleUpdate <= 0)
// 		{
// 			float temp = cycle % CYCLECOUNT - CYCLECOUNTHALF; // [-x, x]
// 			temp = abs(abs(temp) - CYCLECOUNTHALF) / CYCLECOUNTHALF; // temp is now a 0.0f - 1.0f
// 
// 			//std::cout << temp << std::endl;
// 
// 			timeTillCycleUpdate += CYCLEINTERVAL + EXTREMETYTIMEBOOST * temp;
// 			cycle = (cycle + 1) % (CYCLECOUNT * 2);
// 			cycle = cycle == 0 ? 1 : cycle;
// 			//std::cout << cycle << std::endl;
// 			int tempInt = abs(CYCLECOUNT - cycle) % CYCLECOUNT; // 44 - 0: 44      44 - 87: -43 
// 			//std::cout << tempInt << std::endl;
// // 			if (tempInt == 0)
// // 				std::cout << "tempInt" << std::endl;
// 		}
		
		
		
		//int tempInt = abs(CYCLECOUNT - cycle - 1);
		//std::cout << cycle << std::endl;
		//int actualCycle =  m_iBlurOffsetIndices_pulse[abs(CYCLECOUNT - cycle) % CYCLECOUNT];
		////////// //// ///// //////////

		cycle = (++cycle) % 4;
		int actualCycle = m_iBlurOffsetIndices_dispersed[cycle];

		// Set Current Time and mouse info
		glm::vec4 currTime = glm::vec4(glfwGetTime(), static_cast<float>(mouseX), static_cast<float>(mouseY), actualCycle);
		static std::string timeVarName = "currTime";
		computeProg->setULValue(timeVarName, &currTime);


		glDispatchCompute((unsigned int)1920, (unsigned int)1080, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT & GL_SHADER_STORAGE_BARRIER_BIT);


	}


	// Full screen quad
	if (true)
	{
		// Output directed to screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		float ratio;
		int screenWidth, screenHeight;
		glfwGetFramebufferSize(m_window, &screenWidth, &screenHeight);
		ratio = screenWidth / (float)screenHeight;
		//            ratio = ::g_pFBO_1->width / (float)::g_pFBO_1->height;
		//            glViewport(0, 0, ::g_pFBO_1->width, (float)::g_pFBO_1->height);

		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//        ::g_pFBO_1->clearBuffers(true, true);

		unsigned int FSQShaderID = m_pShaderThing->getIDFromFriendlyName("FSQShader");
		DrawPass_FSQ(FSQShaderID, screenWidth, screenHeight);
		//DrawPass_FSQ(m_shaderProgramID, screenWidth, screenHeight);
	}


	/*glEnable(GL_DEPTH_TEST);*/

	glfwPollEvents();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);

// 	glfwSwapBuffers(m_window);
// 	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
// 	glfwPollEvents();


	if (glfwWindowShouldClose(m_window))
		return 1;
	else
		return 0;

}

void cGraphicsMain::Destroy()
{

	glfwDestroyWindow(m_window);
	glfwTerminate();

	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();

	exit(EXIT_SUCCESS);
}


void cGraphicsMain::removeFromDrawMesh(int ID) // Shouldn't be used as it doesn't delete the physics obj
{
	// Scan through the all vec and just erase it m_vec_pAllMeshes
	for (unsigned int i = 0; i < m_vec_pAllMeshes.size(); i++)
	{
		if (m_vec_pAllMeshes[i]->uniqueID == ID)
		{
			m_vec_pAllMeshes.erase(m_vec_pAllMeshes.begin() + i);
			break;
		}
	}
	// Scan through regular vec
	for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++)
	{
		if (m_vec_pMeshesToDraw[i]->uniqueID == ID)
		{
			delete m_vec_pMeshesToDraw[i]; // Prob breaks it
			m_vec_pMeshesToDraw.erase(m_vec_pMeshesToDraw.begin() + i);
			return;
		}
	}
	// Scan through transparent vec
	for (unsigned int i = 0; i < m_vec_pTransMeshesToDraw.size(); i++)
	{
		if (m_vec_pTransMeshesToDraw[i]->uniqueID == ID)
		{
			delete m_vec_pTransMeshesToDraw[i]; // Prob breaks it
			m_vec_pTransMeshesToDraw.erase(m_vec_pTransMeshesToDraw.begin() + i);
			return;
		}
	}
}

// Will replace all meshes and lights with the ones provided
void cGraphicsMain::switchScene(std::vector< cMesh* > newMeshVec, std::vector<cLight> newLights) // TODO have to load in physics side of objects
{
	for (unsigned int i = 0; i < m_vec_pAllMeshes.size(); i++) // Delete all pointers to meshes
	{
		delete m_vec_pAllMeshes[i];
	}
	m_vec_pMeshesToDraw.clear();
	m_vec_pTransMeshesToDraw.clear();

	m_vec_pAllMeshes = newMeshVec; // Set new mesh vector
	m_vec_pMeshesToDraw = newMeshVec;


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

cVAOManager* cGraphicsMain::getVAOManager(void)
{
	return m_pMeshManager;
}

GLFWwindow* cGraphicsMain::getWindow(void)
{
	return m_window;
}

void cGraphicsMain::getAvailableModels(std::vector<std::string>* ModelVec)
{
	//ModelVec = &m_AvailableModels; // Doesn't work
	for (unsigned int i = 0; i < m_AvailableModels.size(); i++)
	{
		ModelVec->push_back(m_AvailableModels[i]);
	}
	return;
}

void cGraphicsMain::getAvailableTextures(std::vector<std::string>* TexVec)
{
	for (unsigned int i = 0; i < m_AvailableTextures.size(); i++)
	{
		TexVec->push_back(m_AvailableTextures[i]);
	}
	return;
}

void cGraphicsMain::getActiveMeshes(std::vector<cMesh*>* MeshVec)
{
	// MeshVec = &m_vec_pMeshesToDraw; // No work :(
	for (unsigned int i = 0; i < m_vec_pAllMeshes.size(); i++)
	{
		//MeshVec->push_back(m_vec_pMeshesToDraw[i]); 
		MeshVec->push_back(m_vec_pAllMeshes[i]);
	}

	return;
}

void cGraphicsMain::getActiveLights(cLightManager* TheLights)
{
	*TheLights = *m_pTheLights;

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
	cShaderManager::cShaderProgram* currProg = m_pShaderThing->getActiveShader();


	//         mat4x4_identity(m);
	glm::mat4 matModel = matModelParent;



	// Translation
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
		glm::vec3(pCurrentMesh->drawPosition.x,
			pCurrentMesh->drawPosition.y,        
			pCurrentMesh->drawPosition.z));


	// Quaternion Rotation
	glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());


	// Scaling matrix
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		pCurrentMesh->scale);
	//--------------------------------------------------------------

	// Combine all these transformation
	matModel = matModel * matTranslate;

	matModel = matModel * matRotation;

	matModel = matModel * matScale;


	 /// BONES ///
	GLint useBones_UL = glGetUniformLocation(shaderProgramID, "bUseBones");
	GLint useBonesFrag_UL = glGetUniformLocation(shaderProgramID, "bBoneFrag");
	if (pCurrentMesh->friendlyName == "a") // Boned model
	{
		glDisable(GL_CULL_FACE);
		sModelDrawInfo theModel;
		m_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshName, theModel);
		for (unsigned int i = 0; i < theModel.BoneInfoVec.size(); i++)
		{
			std::string boneUL;
			boneUL = "BoneMatrices[" + std::to_string(i) + "]";
			GLint boneMatrix_UL = glGetUniformLocation(shaderProgramID, boneUL.c_str());
			glUniformMatrix4fv(boneMatrix_UL, 1, GL_FALSE, glm::value_ptr(theModel.BoneInfoVec[i].FinalTransformation));
		}
		
		glUniform4f(useBones_UL, 1.0f, 0.0f, 0.0f, 0.0f);//(GLfloat)GL_TRUE);
		glUniform4f(useBonesFrag_UL, 1.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		glUniform4f(useBones_UL, 0.0f, 0.0f, 0.0f, 0.0f);//(GLfloat)GL_FALSE);
		glUniform4f(useBonesFrag_UL, 0.0f, 0.0f, 0.0f, 0.0f);
	}
	/// ///// ///


	static std::string matModelUName("matModel");
	currProg->setULValue(matModelUName, &matModel);

	// Also calculate and pass the "inverse transpose" for the model matrix
	glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

	static std::string matModelITUName("matModel_IT");
	currProg->setULValue(matModelITUName, &matModel_InverseTranspose);

	
	// Set Material Properties
// 	GLint material_UL = glGetUniformLocation(shaderProgramID, "material.ambient");
// 	glUniform4f(material_UL, pCurrentMesh->material.ambient.x, pCurrentMesh->material.ambient.y, pCurrentMesh->material.ambient.z, pCurrentMesh->material.ambient.w);
// 	material_UL = glGetUniformLocation(shaderProgramID, "material.diffuse");
// 	glUniform4f(material_UL, pCurrentMesh->material.diffuse.x, pCurrentMesh->material.diffuse.y, pCurrentMesh->material.diffuse.z, pCurrentMesh->material.diffuse.w);
// 	material_UL = glGetUniformLocation(shaderProgramID, "material.specular");
// 	glUniform4f(material_UL, pCurrentMesh->material.specular.x, pCurrentMesh->material.specular.y, pCurrentMesh->material.specular.z, pCurrentMesh->material.specular.w);



	if (pCurrentMesh->bIsWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	static std::string dontLight_CustomCol("bDontLight_CustomCol");
	static glm::vec4 noLight_CustCol = glm::vec4(0.0f);
	noLight_CustCol.x = (GLfloat)pCurrentMesh->bDoNotLight; 
	noLight_CustCol.y = (GLfloat)pCurrentMesh->bUseCustomColors;
	currProg->setULValue(dontLight_CustomCol, &noLight_CustCol);

	if (pCurrentMesh->bUseCustomColors)
	{
		static std::string customColorUName("customColorRGBA");
		currProg->setULValue(customColorUName, &pCurrentMesh->customColorRGBA);
	}

	
	static std::string height_sky_discard("bUseHeightmap_IsSkyBox_UseDiscard_NONE");
	static glm::vec4 height_sky_discardVal(0.0f);
	height_sky_discardVal.y = (GLfloat)pCurrentMesh->isSkybox;
	height_sky_discardVal.z = (GLfloat)pCurrentMesh->bUseDiscardMaskTex;

	currProg->setULValue(height_sky_discard, &height_sky_discardVal);

	


	/// REFLECTION & REFRACTION & ALPHA ///

	static std::string reflect_refract_alpha("bReflect_Refract_fAlpha_NONE");
	static glm::vec4 reflect_refract_alphaVal(0.0f);
	reflect_refract_alphaVal.x = pCurrentMesh->bUseReflect;
	reflect_refract_alphaVal.y = pCurrentMesh->bUseRefract;
	reflect_refract_alphaVal.z = pCurrentMesh->transparencyAlpha;

	currProg->setULValue(reflect_refract_alpha, &reflect_refract_alphaVal);


	//////////////////// TEXTURE STUFF /////////////////////////
	SetUpTextures(pCurrentMesh, shaderProgramID);


	/// UV-OFFSET & SCALE ///
	static std::string uvOffset_scale("uv_Offset_Scale_NONE");
	static glm::vec4 uvOffset_scaleVal(0.0f);
	uvOffset_scaleVal.x = pCurrentMesh->uv_Offset_Scale.x;
	uvOffset_scaleVal.y = pCurrentMesh->uv_Offset_Scale.y;
	uvOffset_scaleVal.z = pCurrentMesh->uv_Offset_Scale.z;

	currProg->setULValue(uvOffset_scale, &uvOffset_scaleVal);

	/////////////////////////////////////////////////////////////


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

// Zooms in/out the shader effect
static void UpdateShaderZoom(float& currZoom, double& dt, bool isOnScreen)
{
	float distToMax = (1.0f - currZoom); // Small = fast zoom out, slow zoom in   Big = slow zoom out, fast zoom in
	if (isOnScreen) // Zoom in
	{
		const float ZOOMINSPD = 0.6f; // Slower zoom in to appreciate the effects or whatever
		currZoom += (static_cast<float>(dt) * distToMax) * ZOOMINSPD;
	}
	else // Zoom out
	{
		currZoom -= static_cast<float>(dt) * (1.0f - distToMax);
	}

	glm::clamp(currZoom, 0.0f, 1.0f);

	return;
}

void cGraphicsMain::DrawHMObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID, double& dt)
{
	cShaderManager::cShaderProgram* currProg = m_pShaderThing->getActiveShader();

	glm::mat4 matModel = matModelParent;
	glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
		glm::vec3(pCurrentMesh->drawPosition.x,
			pCurrentMesh->drawPosition.y,
			pCurrentMesh->drawPosition.z));


	// Quaternion Rotation
	glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());


	// Scaling matrix
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		pCurrentMesh->scale);
	//--------------------------------------------------------------

	// Combine all these transformation
	matModel = matModel * matTranslate;
	matModel = matModel * matRotation;
	matModel = matModel * matScale;



	static std::string matModelUName("matModel");
	currProg->setULValue(matModelUName, &matModel);

	// Also calculate and pass the "inverse transpose" for the model matrix
	glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

	static std::string matModelITUName("matModel_IT");
	currProg->setULValue(matModelITUName, &matModel_InverseTranspose);

	static std::string intensityUName("intensity");


	glm::vec3 objToCam = m_cameraEye - pCurrentMesh->drawPosition;
	float objToCamDist = glm::length(objToCam);
	objToCam = glm::normalize(objToCam);



	// Scale the effect of the "pop-out" based on if the player is looking at it
	// How to tell if the object is on screen? idk. // Set a specific pixel (i.e. 0,0) in some buffer 
	// to a certain value in the compute when the heatmap is read

	static float zoomAmp = 0.0f; // 0.0f - 1.0f range
	const float ZOOMMAX = 0.037f;

	UpdateShaderZoom(zoomAmp, dt, isShadedObjVisible);


	sModelDrawInfo modelInfo;
	if (m_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo))
	{
		// Found it!!!

		glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)

		// Loop through drawing progressively bigger versions of the model
		// As the stencil buffer is enabled, we only draw the new band around the previous
		for (unsigned int i = 0; i < 20; i++)
		{
			// Update current intensity to draw
			glm::vec4 intensityVals(1.0f - (i * 0.05f), 0.0f, 0.0f, 0.0f);
			currProg->setULValue(intensityUName, &intensityVals);

			// Update position of model we're passing in
			matModel = matModelParent;
			float finalZoomAmp = (i * 1.1f) * zoomAmp * ZOOMMAX * objToCamDist;
			glm::mat4 incrementMatTranslate = glm::translate(glm::mat4(1.0f), // TODO have this scale off of your distance to it
				glm::vec3(pCurrentMesh->drawPosition.x + objToCam.x * (finalZoomAmp),
					pCurrentMesh->drawPosition.y + objToCam.y * (finalZoomAmp),
					pCurrentMesh->drawPosition.z + objToCam.z * (finalZoomAmp)));

			matModel = matModel * incrementMatTranslate;

			matModel = matModel * matRotation;

			matModel = matModel * matScale;


			currProg->setULValue(matModelUName, &matModel);

			glDrawElements(GL_TRIANGLES,
				modelInfo.numberOfIndices,
				GL_UNSIGNED_INT,
				0);
		}
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

bool cGraphicsMain::LoadTextures(void)
{
	m_AvailableTextures.push_back("##"); // Option to not have textures
	std::ifstream texturesToLoad("assets/textures/textures.txt");
	if (!texturesToLoad.is_open())
	{
		// didn't open :(
		std::cout << "ERROR: Failed to open the texture list file!" << std::endl;
		std::cout << texturesToLoad.is_open();
	}
	std::string line = "";

	while (std::getline(texturesToLoad, line))
	{
		//m_pMeshManager->LoadModelIntoVAO(line.c_str(),
		//	modelDrawingInfo, m_shaderProgramID);
		m_pTextureManager->Create2DTextureFromBMPFile(line.c_str(), true);
		std::cout << "Loaded texture: " << line << " vertices" << std::endl;
		m_AvailableTextures.push_back(line.c_str());
	}

	texturesToLoad.close();

	// Load cubemap
	m_pTextureManager->SetBasePath("assets/textures/CubeMaps");

	std::string errors;
	m_pTextureManager->CreateCubeTextureFromBMPFiles("SunnyDay",
													 "TropicalSunnyDayLeft2048.bmp",
													 "TropicalSunnyDayRight2048.bmp",
													 "TropicalSunnyDayUp2048.bmp",
													 "TropicalSunnyDayDown2048.bmp",
													 "TropicalSunnyDayFront2048.bmp",
													 "TropicalSunnyDayBack2048.bmp",
													 true,
													 errors);


	return true;
	
}

bool cGraphicsMain::LoadParticles(void)
{
	m_pBasicParticle = new cMesh();
	m_pBasicParticle->setUniformDrawScale(0.2f);
	//m_pBasicParticle->meshName = "Sphere_1_unit_Radius.ply";
	m_pBasicParticle->meshName = "Icosahedron.ply";
	m_pBasicParticle->bDoNotLight = false;
	m_pBasicParticle->bUseCustomColors = true;
	m_pBasicParticle->customColorRGBA = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

	m_pParticleManager = cParticleManager::GetInstance();

	if (m_pParticleManager == nullptr)
	{
		printf("Failed to load Particle Manager\n");
		return false;
	}


	// Load in default particle setup for testing!
	//m_pParticleManager->startEmitter(DEFAULT, glm::vec3(0.0f));


	return true;
}


void cGraphicsMain::DrawPass_1(GLuint shaderProgramID, int screenWidth, int screenHeight, glm::vec4 sceneEye, glm::vec3 sceneTarget)
{

	float ratio; //= screenWidth / (float)screenHeight;
	//int width, height;

	//glUseProgram(m_shaderProgramID);
	m_pShaderThing->useShaderProgram("shader01");
	cShaderManager::cShaderProgram* currProg = m_pShaderThing->GetShaderProgramFromFriendlyName("shader01");


	//glfwGetFramebufferSize(m_window, &width, &height);
	ratio = screenWidth / (float)screenHeight;

	//glViewport(0, 0, width, height);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// *****************************************************************
	// if ya want lights
	m_pTheLights->UpdateUniformValues(m_shaderProgramID); // TODO make this a uniform block, only updating when a light's data is changed


	// *****************************************************************
			
	//uniform vec4 eyeLocation;
	//GLint eyeLocation_UL = glGetUniformLocation(m_shaderProgramID, "eyeLocation");

// 	glUniform4f(eyeLocation_UL,
// 		sceneEye.x, sceneEye.y, sceneEye.z, 1.0f);


	static std::string eyeLoc = "eyeLocation"; 
	currProg->setULValue(eyeLoc, &sceneEye);
	//currProg->setULValue("eyeLocation", &glm::vec4(sceneEye.x, sceneEye.y, sceneEye.z, 1.0f));

	// Set Camera Matrices
	glm::mat4 projMat = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 1100.0f);
	glm::mat4 viewMat = glm::lookAt((glm::vec3)sceneEye, (glm::vec3)sceneEye + sceneTarget, m_upVector);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBOMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projMat));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);



	/////////// UV OFFSET UPDATE /////////////
// 	for (unsigned int i = 0; i < m_vec_pAllMeshes.size(); i++)
// 	{
// 		m_vec_pAllMeshes[i]->uv_Offset_Scale.x += m_vec_pAllMeshes[i]->uvOffsetSpeed.x * deltaTime;
// 		m_vec_pAllMeshes[i]->uv_Offset_Scale.y += m_vec_pAllMeshes[i]->uvOffsetSpeed.y * deltaTime;
// 
// 		// Keeps values within proper range (good for longevity)
// // 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.x > 1.0f)
// // 			m_vec_pAllMeshes[i]->uv_Offset_Scale.x -= floor(m_vec_pAllMeshes[i]->uv_Offset_Scale.x);
// // 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.x < 0.0f)
// // 			m_vec_pAllMeshes[i]->uv_Offset_Scale.x += ceil(m_vec_pAllMeshes[i]->uv_Offset_Scale.x);
// // 
// // 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.y > 1.0f)
// // 			m_vec_pAllMeshes[i]->uv_Offset_Scale.y -= floor(m_vec_pAllMeshes[i]->uv_Offset_Scale.y);
// // 		if (m_vec_pAllMeshes[i]->uv_Offset_Scale.y < 0.0f)
// // 			m_vec_pAllMeshes[i]->uv_Offset_Scale.y += ceil(m_vec_pAllMeshes[i]->uv_Offset_Scale.y);
// 	}



	// *********************************************************************
	// Draw all the objects
	for (unsigned int index = 0; index != m_vec_pMeshesToDraw.size(); index++) // Prob black or smthn
	{
		cMesh* pCurrentMesh = m_vec_pMeshesToDraw[index];
		if (pCurrentMesh->transparencyAlpha < 0.99f)
		{
			m_vec_pTransMeshesToDraw.push_back(pCurrentMesh);
			m_vec_pMeshesToDraw.erase(m_vec_pMeshesToDraw.begin() + index);
			index--;
			continue;
		}

		if (pCurrentMesh->bIsVisible)
		{

			glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

			if (pCurrentMesh->isDoubleSided)
				glDisable(GL_CULL_FACE);

			
			DrawObject(pCurrentMesh, matModel, m_shaderProgramID);

			if (pCurrentMesh->isDoubleSided)
				glEnable(GL_CULL_FACE);

		}//if (pCurrentMesh->bIsVisible)

	}//for ( unsigned int index

	// Draw verlet object
	//glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

	//DrawObject(VerletObject, matModel, m_shaderProgramID);

	//////////// PARTICLE RENDERING ///////////
// 	m_pParticleManager->Update(deltaTime);
// 
// 	std::vector<cParticleSystem::sParticleRender> vecParticles_to_draw;
// 	m_pParticleManager->getParticleList(vecParticles_to_draw);
// 
// 	for (cParticleSystem::sParticleRender& curParticle : vecParticles_to_draw)
// 	{
// 		glm::mat4 matModel = glm::mat4(1.0f);
// 
// 		m_pBasicParticle->drawPosition = curParticle.position;
// 
// 		DrawObject(m_pBasicParticle, matModel, m_shaderProgramID);
// 	}




	// Time per frame (more or less)
// 	double currentTime = glfwGetTime();
// 	double deltaTime = currentTime - m_lastTime;
// 	//        std::cout << deltaTime << std::endl;
// 	m_lastTime = currentTime;


	// Quickly Draw hardcoded skybox
	{
		// HACK: I'm making this here, but hey...
		cMesh theSkyBox;
		theSkyBox.bDoNotLight = true;
		theSkyBox.isSkybox = true;
		theSkyBox.meshName = "Sphere_1_unit_Radius.ply";
		//theSkyBox.setUniformDrawScale(10.0f);

		theSkyBox.setUniformDrawScale(1000.0f);
		theSkyBox.setDrawPosition(sceneEye);
		//            theSkyBox.bIsWireframe = true;

					// Depth test
		//            glDisable(GL_DEPTH_TEST);       // Writes no matter what
					// Write to depth buffer (depth mask)
		//            glDepthMask(GL_FALSE);          // Won't write to the depth buffer

					// uniform bool bIsSkyBox;
				
		// !!! Don't need to set UL's here anymore, they're set in drawObject
// 		GLint bIsSkyBox_UL = glGetUniformLocation(m_shaderProgramID, "bUseHeightmap_IsSkyBox_UseDiscard_NONE");
// 		glUniform4f(bIsSkyBox_UL, 0.0f, 1.0f, 0.0f, 0.0f);

// 		static std::string hMap_sky_discard("bUseHeightmap_IsSkyBox_UseDiscard_NONE");
// 		static glm::vec4 enableSkybox = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
// 		currProg->setULValue(hMap_sky_discard, &enableSkybox);

		// The normals for this sphere are facing "out" but we are inside the sphere
		glCullFace(GL_FRONT);

		DrawObject(&theSkyBox, glm::mat4(1.0f), m_shaderProgramID);

		//glUniform1f(bIsSkyBox_UL, (GLfloat)GL_FALSE);

		// Put the culling back to "normal" (back facing are not drawn)
		glCullFace(GL_BACK);
	}

	/// End of skybox

	// Now we draw all transparent meshes


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (unsigned int index = 0; index != m_vec_pTransMeshesToDraw.size(); index++)
	{
		cMesh* pCurrentMesh = m_vec_pTransMeshesToDraw[index];
		if (pCurrentMesh->transparencyAlpha > 0.99f)
		{
			m_vec_pMeshesToDraw.push_back(pCurrentMesh);
			m_vec_pTransMeshesToDraw.erase(m_vec_pTransMeshesToDraw.begin() + index);
			index--;
			continue;
		}

		if (pCurrentMesh->bIsVisible)
		{
			if (index < m_vec_pTransMeshesToDraw.size() - 1) // If not the last in vec, try to sort by distance
			{
				if (glm::distance(m_vec_pTransMeshesToDraw[index]->drawPosition, m_cameraEye) < glm::distance(m_vec_pTransMeshesToDraw[index + 1]->drawPosition, m_cameraEye))
				{
					cMesh* tempMesh = m_vec_pTransMeshesToDraw[index];
					m_vec_pTransMeshesToDraw[index] = m_vec_pTransMeshesToDraw[index + 1];
					m_vec_pTransMeshesToDraw[index + 1] = tempMesh;
					pCurrentMesh = m_vec_pTransMeshesToDraw[index];
				}
			}

			glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

			DrawObject(pCurrentMesh, matModel, m_shaderProgramID);
		}//if (pCurrentMesh->bIsVisible)

	}
	glDisable(GL_BLEND);



	// Now draw debug stuff
	glDisable(GL_DEPTH_TEST);

	if (renderDebug)
	{
		glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix
		cMesh basicSphere;
		basicSphere.meshName = "Sphere_1_unit_Radius.ply";
		basicSphere.bDoNotLight = true;
		basicSphere.bUseCustomColors = true;
		basicSphere.customColorRGBA = glm::vec4(0, 1, 0, 1); // Green
		basicSphere.bIsWireframe = true;

		// Start by drawing a basic sphere at all the active light locations
		for (unsigned int i = 0; i < m_pTheLights->NUMBER_OF_LIGHTS_IM_USING; i++)
		{
			if (m_pTheLights->theLights[i].param2.x == 1) // If light is on
			{
				if (selectedLight == i) // Have different color for selected light
					basicSphere.customColorRGBA = glm::vec4(1, 0, 0, 1);

				basicSphere.drawPosition = m_pTheLights->theLights[i].position;
				DrawObject(&basicSphere, matModel, m_shaderProgramID);
				basicSphere.customColorRGBA = glm::vec4(0, 1, 0, 1);
			}
		}

		// Draw a Cyan sphere at the selected mesh position
		if (selectedMesh > -1)
		{
			basicSphere.drawPosition = m_vec_pAllMeshes[selectedMesh]->drawPosition;
			basicSphere.customColorRGBA = glm::vec4(0, .5, .5, 1);
			DrawObject(&basicSphere, matModel, m_shaderProgramID);
		}


	}

	glEnable(GL_DEPTH_TEST);


	return;

// 	glfwPollEvents();
// 
// 	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
// 
// 	glfwSwapBuffers(m_window);

// 	if (glfwWindowShouldClose(m_window))
// 		return -1;
// 	else
// 		return 0;
}

// For generating the heatmap for cool object effects
void cGraphicsMain::DrawPass_HM(GLuint shaderProgramID, int screenWidth, int screenHeight, glm::vec4 sceneEye, glm::vec3 sceneTarget, double& dt)
{
	float ratio = screenWidth / (float)screenHeight;
	m_pShaderThing->useShaderProgram("HMShader");
	cShaderManager::cShaderProgram* currProg = m_pShaderThing->GetShaderProgramFromFriendlyName("HMShader");

	glCullFace(GL_BACK);

	// Set Camera Matrices
	glm::mat4 projMat = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 1100.0f);
	glm::mat4 viewMat = glm::lookAt((glm::vec3)sceneEye, (glm::vec3)sceneEye + sceneTarget, m_upVector);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBOMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projMat));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	/// Stencil Setup ///
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, // Test fails? Keep stored stencil value
				GL_KEEP, //
				GL_REPLACE); // Depth and Stencil pass? Then stencil value with reference value (stored below in Func)

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // All drawn fragments auto-pass the test, setting to 1
	glStencilMask(0xFF); // Enable writing to the stencil buffer?


	for (unsigned int index = 0; index != m_vec_pAllMeshes.size(); index++) // Go through ALL meshes
	{
		cMesh* pCurrentMesh = m_vec_pMeshesToDraw[index];
		if (!pCurrentMesh->isSpooky) continue;

		if (pCurrentMesh->bIsVisible)
		{

			glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

			if (pCurrentMesh->isDoubleSided)
				glDisable(GL_CULL_FACE);

			DrawHMObject(pCurrentMesh, matModel, currProg->ID, dt);

			if (pCurrentMesh->isDoubleSided)
				glEnable(GL_CULL_FACE);

		}//if (pCurrentMesh->bIsVisible)

	}//for ( unsigned int index

	glDisable(GL_STENCIL_TEST);
}

void cGraphicsMain::DrawPass_FSQ(GLuint shaderProgramID, int screenWidth, int screenHeight)
{
	float ratio;

	glUseProgram(shaderProgramID); // FSQ Shader

	//glfwGetFramebufferSize(pWindow, &width, &height);
	ratio = screenWidth / (float)screenHeight;


	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	GLint widthHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidth_Height");
	glUniform4f(widthHeight_UL, (GLfloat)screenWidth, (GLfloat)screenHeight, 0.0f, 0.0f);



	// Point the FBO from the 1st pass to this texture...

	GLint textureUnitNumber = 70;
	glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
	//glBindTexture(GL_TEXTURE_2D, m_pFBO_1->colourTexture_0_ID);//m_pFBO_1->colourTexture_0_ID);
	glBindTexture(GL_TEXTURE_2D, computeTexOutput);

	GLint FSQTex_UL = glGetUniformLocation(shaderProgramID, "FSQTex");
	glUniform1i(FSQTex_UL, textureUnitNumber);


	// Setting the spooky heatmap here too, as we want to influence how we read the above texture around this area
// 	GLint textureUnitNumberSpooky = 71;
// 	glActiveTexture(GL_TEXTURE0 + textureUnitNumberSpooky);
// 	glBindTexture(GL_TEXTURE_2D, m_pFBO_3->colourTexture_0_ID);
// 
// 	//uniform sampler2D textureOffScreen;
// 	GLint spookyTexture_UL = glGetUniformLocation(shaderProgramID, "spookyHeatMap");
// 	glUniform1i(spookyTexture_UL, textureUnitNumberSpooky);

	GLint spookyBool_UL = glGetUniformLocation(m_shaderProgramID, "isSpooky");
	glUniform4f(spookyBool_UL, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable all these, as this pass only draws texture to quad!


	//DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);

	sModelDrawInfo modelInfo;
	if (m_pMeshManager->FindDrawInfoByModelName("Quad_1_sided_aligned_on_XY_plane.ply", modelInfo))
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

// Adds new object to the meshestodraw
void cGraphicsMain::addNewMesh(cMesh* newMesh) // Just adds new mesh pointer to the vector
{
	m_vec_pMeshesToDraw.push_back(newMesh);
	m_vec_pAllMeshes.push_back(newMesh);


	return;
}

void cGraphicsMain::UpdateCamera(glm::vec3 position, glm::quat qRotation)
{
	m_cameraEye = position;
	m_qCameraRotation = qRotation;

	m_cameraTarget = glm::vec3(0, 0, 1) * m_qCameraRotation;
}


void cGraphicsMain::addNewLight(char* friendlyName)
{
	int newLightIdx = m_pTheLights->nextLightIdx++;
	m_pTheLights->theLights[newLightIdx].param2.x = 1.0f; // Turn light on
	m_pTheLights->theLights[newLightIdx].friendlyName = friendlyName; // Name the light
}

void cGraphicsMain::updateSelectedLight(int lightIdx, std::string friendlyName, glm::vec4 newPos, glm::vec4 newDiff, glm::vec4 newSpec, glm::vec4 newAtten, glm::vec4 newDir, glm::vec4 newParam1, glm::vec4 newParam2)
{
	m_pTheLights->theLights[lightIdx].friendlyName = friendlyName;
	m_pTheLights->theLights[lightIdx].position = newPos;
	m_pTheLights->theLights[lightIdx].diffuse = newDiff;
	m_pTheLights->theLights[lightIdx].specular = newSpec;
	m_pTheLights->theLights[lightIdx].atten = newAtten;
	m_pTheLights->theLights[lightIdx].direction = newDir;
	m_pTheLights->theLights[lightIdx].param1 = newParam1;
	m_pTheLights->theLights[lightIdx].param2 = newParam2;

	return;
}

void cGraphicsMain::updateDebugStates(bool useDebug, int selMesh, int selLight)
{
	renderDebug = useDebug;
	selectedMesh = selMesh;
	selectedLight = selLight;
	// Other things here for selected mesh object and selected light
}

void cGraphicsMain::duplicateMesh(int meshIdx, char* newName) // TODO also duplicate physics properties
{
	cMesh* dupedMesh = new cMesh();
	cMesh* meshToCopy = m_vec_pMeshesToDraw[meshIdx];

	dupedMesh->meshName = meshToCopy->meshName;
	dupedMesh->friendlyName = newName;
	dupedMesh->drawPosition = meshToCopy->drawPosition;
	dupedMesh->eulerOrientation = meshToCopy->eulerOrientation;
	dupedMesh->customColorRGBA = meshToCopy->customColorRGBA;
	dupedMesh->scale = meshToCopy->scale;
	dupedMesh->bIsVisible = meshToCopy->bIsVisible;
	dupedMesh->bIsWireframe = meshToCopy->bIsWireframe;
	dupedMesh->bDoNotLight = meshToCopy->bDoNotLight;
	dupedMesh->bUseCustomColors = meshToCopy->bUseCustomColors;
	dupedMesh->setRotationFromEuler(dupedMesh->getEulerOrientation());
	m_vec_pMeshesToDraw.push_back(dupedMesh);
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
			m_cameraRotation.x -= static_cast<float>(deltaMouseX) / 1000;

			m_cameraRotation.y -= static_cast<float>(deltaMouseY) / 1000;
			m_cameraTarget.y = m_cameraRotation.y;     // This is pitch
			m_cameraTarget.x = sin(m_cameraRotation.x);         // This is just y-rotation
			m_cameraTarget.z = sin(m_cameraRotation.x + 1.57f);  //
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


void cGraphicsMain::SetUpTextures(cMesh* pCurrentMesh, GLuint shaderProgramID)
{
	// New texture stuff (diffuse + specular)
	{
		GLint textureUnitNumber = 10;
		GLuint textureDiff = m_pTextureManager->getTextureIDFromName(pCurrentMesh->material.diffuseTex);
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, textureDiff);

		if (pCurrentMesh->material.isDiffMirrored)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		GLint textureDiff_UL = glGetUniformLocation(shaderProgramID, "material.diffuse");
		glUniform1i(textureDiff_UL, textureUnitNumber);
	}
	{
		GLint textureUnitNumber = 11;
		GLuint textureDiff = m_pTextureManager->getTextureIDFromName(pCurrentMesh->material.specularTex);
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, textureDiff);

		if (pCurrentMesh->material.isSpecMirrored)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		GLint textureDiff_UL = glGetUniformLocation(shaderProgramID, "material.specular");
		glUniform1i(textureDiff_UL, textureUnitNumber);
	}

	// Set power
	{
		GLint matPower_UL = glGetUniformLocation(shaderProgramID, "material.power");
		glUniform4f(matPower_UL, pCurrentMesh->material.power.x, pCurrentMesh->material.power.y, pCurrentMesh->material.power.z, pCurrentMesh->material.power.w);
	}



// 	{
// 		GLint textureUnitNumber = 0;
// 		GLuint Texture00 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
// 		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
// 		glBindTexture(GL_TEXTURE_2D, Texture00);
// 		GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "texture_00");
// 		glUniform1i(texture_00_UL, textureUnitNumber);
// 	}
// 
// 	{
// 		GLint textureUnitNumber = 1;
// 		GLuint Texture01 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
// 		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
// 		glBindTexture(GL_TEXTURE_2D, Texture01);
// 		GLint texture_01_UL = glGetUniformLocation(shaderProgramID, "texture_01");
// 		glUniform1i(texture_01_UL, textureUnitNumber);
// 	}
// 
// 	{
// 		GLint textureUnitNumber = 2;
// 		GLuint Texture02 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
// 		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
// 		glBindTexture(GL_TEXTURE_2D, Texture02);
// 		GLint texture_02_UL = glGetUniformLocation(shaderProgramID, "texture_02");
// 		glUniform1i(texture_02_UL, textureUnitNumber);
// 	}
// 
// 	{
// 		GLint textureUnitNumber = 3;
// 		GLuint Texture03 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
// 		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
// 		glBindTexture(GL_TEXTURE_2D, Texture03);
// 		GLint texture_03_UL = glGetUniformLocation(shaderProgramID, "texture_03");
// 		glUniform1i(texture_03_UL, textureUnitNumber);
// 	}
	// and so on to however many texture you are using

//    uniform vec4 textureMixRatio_0_3;
//    uniform vec4 textureMixRatio_4_7;

	//GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_0_3");
	//    GLint textureMixRatio_4_7_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_4_7");

// 	glUniform4f(textureMixRatio_0_3_UL,
// 		pCurrentMesh->textureRatios[0],
// 		pCurrentMesh->textureRatios[1],
// 		pCurrentMesh->textureRatios[2],
// 		pCurrentMesh->textureRatios[3]);
	//    glUniform4f(textureMixRatio_4_7_UL,
	//                pCurrentMesh->textureRatios[4],
	//                pCurrentMesh->textureRatios[5],
	//                pCurrentMesh->textureRatios[6],
	//                pCurrentMesh->textureRatios[7]);


		// Also set up the height map and discard texture

	{
		// uniform sampler2D heightMapSampler;		// Texture unit 20
		GLint textureUnitNumber = 20;
		GLuint Texture20 = m_pTextureManager->getTextureIDFromName("NvF5e_height_map.bmp");
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, Texture20);
		GLint texture_20_UL = glGetUniformLocation(shaderProgramID, "heightMapSampler");
		glUniform1i(texture_20_UL, textureUnitNumber);
	}



	// Set up a skybox
	{
		// uniform samplerCube skyBoxTexture;		// Texture unit 30
		GLint textureUnit21 = 21;
		GLuint skyBoxID = m_pTextureManager->getTextureIDFromName("SunnyDay");
		glActiveTexture(GL_TEXTURE0 + textureUnit21);
		// NOTE: Binding is NOT to GL_TEXTURE_2D
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxID);
		GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgramID, "skyBoxTexture");
		glUniform1i(skyBoxSampler_UL, textureUnit21);
	}


	return;
}
