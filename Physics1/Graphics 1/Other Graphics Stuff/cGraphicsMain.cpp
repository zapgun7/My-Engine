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
	glfwSwapInterval(0); // No stinky v-sync
	//glfwSwapInterval(1); // Stinky


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
		return 0;
	}


	//
	m_shaderProgramID = m_pShaderThing->getIDFromFriendlyName("shader01");

	m_pMeshManager = new cVAOManager();
	//::g_pPhysics->setVAOManager(m_pMeshManager); //UNPHYS

	m_pMeshManager->setBasePath("assets/models");

	m_pTheLights = new cLightManager();
	m_pTheLights->SetUniformLocations(m_shaderProgramID);

	// Default directional lights
	m_pTheLights->theLights[0].param1.x = 2;
	m_pTheLights->theLights[0].param2.x = 1;
	m_pTheLights->theLights[0].direction = glm::vec4(.2, -.6, .2, 0);

	m_pTheLights->theLights[1].param1.x = 2;
	m_pTheLights->theLights[1].param2.x = 1;
	m_pTheLights->theLights[1].direction = glm::vec4(-.05, .3, -.05, 0);



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

	//ImGui setup
// 	IMGUI_CHECKVERSION();
// 	ImGui::CreateContext();
// 	m_io = ImGui::GetIO(); (void)m_io;
// 	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
// 	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	//ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	//ImGui_ImplOpenGL3_Init(glsl_version);
	
	////// Throw in some modles to test textures

// 	cMesh* plane = new cMesh();
// 	plane->meshName = "Big_Flat_Mesh.ply";
// 	plane->friendlyName = "bigolplane";
// 	plane->setDrawPosition(glm::vec3(0.0f, -30.0f, 0.0f));
// 	plane->bDoNotLight = true;
// 
// 	//plane->textureName[0] = "Water_Texture_01.bmp";
// 	plane->textureName[0] = "rosewood.bmp";
// 	plane->textureRatios[0] = 1.0f;
// 	plane->textureIdx[0] = 1;
// 
// 	m_vec_pMeshesToDraw.push_back(plane);

	



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
	if (!m_pFBO_2->init(1920, 1080, FBOError))
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


	GenerateUBOs();

	return 1;
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


	return Update2(deltaTime);
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
	// First reduce all red values on the heatmapf
	if (false)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_pFBO_3->ID);
		float ratio;
		ratio = m_pFBO_3->width / (float)m_pFBO_3->height;
		glViewport(0, 0, m_pFBO_3->width, m_pFBO_3->height);

		//m_pFBO_3->clearBuffers(false, true); // Don't clear it, but instead reduce the red inside the shader >:)

		glm::vec3 camEye = m_cameraEye;
		glm::vec3 camTarget = m_cameraTarget;

		DrawPass_HeatMapReduction(m_shaderProgramID, m_pFBO_3->width, m_pFBO_3->height, deltaTime);
	}


	// Start with the spooky heatmap render
	if (false)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_pFBO_3->ID);
		float ratio;
		ratio = m_pFBO_3->width / (float)m_pFBO_3->height;
		glViewport(0, 0, m_pFBO_3->width, m_pFBO_3->height);

		m_pFBO_3->clearBuffers(true, true); 

		glm::vec3 camEye = m_cameraEye;
		glm::vec3 camTarget = m_cameraTarget;

		DrawPass_SpookyHeatmap(m_shaderProgramID, m_pFBO_3->width, m_pFBO_3->height, camEye, camTarget);
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

		DrawPass_1(m_shaderProgramID, m_pFBO_1->width, m_pFBO_1->height, scene_1_CameraEye, scene_1_CameraTarget);
	}

	if (false)
	{
		// Apply spooky effect, pass in heatmap and FBO 1 as textures and paste to some arbitrary quad to make texture for FSQ pass?
		glBindFramebuffer(GL_FRAMEBUFFER, m_pFBO_4->ID);
		float ratio;
		ratio = m_pFBO_4->width / (float)m_pFBO_4->height;
		glViewport(0, 0, m_pFBO_4->width, m_pFBO_4->height);

		m_pFBO_4->clearBuffers(true, true);

		glm::vec3 camEye = m_cameraEye;
		glm::vec3 camTarget = m_cameraTarget;

		DrawPass_ApplySpook(m_shaderProgramID, m_pFBO_4->width, m_pFBO_4->height);
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


		DrawPass_FSQ(m_shaderProgramID, screenWidth, screenHeight);
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

	matModel = matModel * matRotation;

	matModel = matModel * matScale;



	   //mat4x4_mul(mvp, p, m);
	//    glm::mat4 mvp = matProjection * matView * matModel;

	//    GLint mvp_location = glGetUniformLocation(shaderProgramID, "MVP");
	//    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
	//    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
	


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


			// uniform bool bDoNotLight;
	//GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");
	GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDontLight_CustomCol");
// 	if (pCurrentMesh->bDoNotLight)
// 	{
// 		// Set uniform to true
// 		glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
// 	}
// 	else
// 	{
// 		// Set uniform to false;
// 		glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
// 	}

	//uniform bool bUseDebugColour;	
	//GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramID, "bUseDebugColour");

	glUniform4f(bDoNotLight_UL,
		(GLfloat)pCurrentMesh->bDoNotLight,
		(GLfloat)pCurrentMesh->bUseCustomColors,
		0.0f,
		0.0f);

	if (pCurrentMesh->bUseCustomColors)
	{
		//glUniform1f(bUseDebugColour_UL, (GLfloat)GL_TRUE);
		//uniform vec4 debugColourRGBA;
		GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramID, "customColorRGBA");
		glUniform4f(debugColourRGBA_UL,
			pCurrentMesh->customColorRGBA.r,
			pCurrentMesh->customColorRGBA.g,
			pCurrentMesh->customColorRGBA.b,
			pCurrentMesh->customColorRGBA.a);
	}
// 	else
// 	{
// 		glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);
// 	}

	// Discard Mask bool
	GLint bDiscardMaskTex_UL = glGetUniformLocation(shaderProgramID, "bUseHeightmap_IsSkyBox_UseDiscard_NONE");

	glUniform4f(bDiscardMaskTex_UL, 0.0f, (GLfloat)pCurrentMesh->isSkybox, (GLfloat)pCurrentMesh->bUseDiscardMaskTex, 0.0f);

// 	if (pCurrentMesh->bUseDiscardMaskTex)
// 	{
// 		// Set uniform to true
// 		glUniform1f(bDiscardMaskTex_UL, (GLfloat)GL_TRUE);
// 	}
// 	else
// 	{
// 		// Set uniform to false;
// 		glUniform1f(bDiscardMaskTex_UL, (GLfloat)GL_FALSE);
// 	}

	/// REFLECTION & REFRACTION

	GLint bUseReflect_UL = glGetUniformLocation(shaderProgramID, "bReflect_Refract_fAlpha_NONE");
	glUniform4f(bUseReflect_UL, (GLfloat)pCurrentMesh->bUseReflect, (GLfloat)pCurrentMesh->bUseRefract, (GLfloat)pCurrentMesh->transparencyAlpha, 0.0f);
// 	if (pCurrentMesh->bUseReflect)
// 	{
// 		glUniform1f(bUseReflect_UL, (GLfloat)GL_TRUE);
// 	}
// 	else
// 	{
// 		glUniform1f(bUseReflect_UL, (GLfloat)GL_FALSE);
// 	}
// 
// 	GLint bUseRefract_UL = glGetUniformLocation(shaderProgramID, "bUseRefract");
// 	if (pCurrentMesh->bUseRefract)
// 	{
// 		glUniform1f(bUseRefract_UL, (GLfloat)GL_TRUE);
// 	}
// 	else
// 	{
// 		glUniform1f(bUseRefract_UL, (GLfloat)GL_FALSE);
// 	}
// 
// 	/// ALPHA TRANSPARECY
// 
// 	GLint fTransparencyAlpha_UL = glGetUniformLocation(shaderProgramID, "transparencyAlpha");
// 	glUniform1f(fTransparencyAlpha_UL, pCurrentMesh->transparencyAlpha);



	//////////////////// TEXTURE STUFF /////////////////////////

// 	GLint bUseVertexColours_UL = glGetUniformLocation(shaderProgramID, "bUseVertexColours");
// 	glUniform1f(bUseVertexColours_UL, (GLfloat)GL_FALSE);


	SetUpTextures(pCurrentMesh, shaderProgramID);


	// Pass in uv-offset
	GLint uvOffset_UL = glGetUniformLocation(shaderProgramID, "uv_Offset_Scale_NONE");
	glUniform4f(uvOffset_UL, pCurrentMesh->uv_Offset_Scale.x, pCurrentMesh->uv_Offset_Scale.y, pCurrentMesh->uv_Offset_Scale.z, 0.0f);

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

void cGraphicsMain::DrawPass_HeatMapReduction(GLuint shaderProgramID, int screenWidth, int screenHeight, double deltaTime)
{
	float ratio;

	glUseProgram(m_shaderProgramID);

	//glfwGetFramebufferSize(pWindow, &width, &height);
	ratio = screenWidth / (float)screenHeight;


	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// Camera is pointing directly at the full screen quad
	glm::vec3 FSQ_CameraEye = glm::vec3(0.0, 0.0, 5.0f);
	glm::vec3 FSQ_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


	//uniform vec4 eyeLocation;
	GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
	glUniform4f(eyeLocation_UL,
		FSQ_CameraEye.x, FSQ_CameraEye.y, FSQ_CameraEye.z, 1.0f);



	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 matProjection = glm::perspective(0.6f,
		ratio,
		0.1f,        // Near (as big)
		100.0f);    // Far (as small)

	glm::mat4 matView = glm::lookAt(FSQ_CameraEye,
		FSQ_CameraTarget,
		m_upVector);

	GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

	GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

	// Set up the textures for this offscreen quad
	//uniform bool bIsOffScreenTextureQuad;
// 	GLint bIsOffScreenTextureQuad_UL = glGetUniformLocation(shaderProgramID, "bIsOffScreenTextureQuad");
// 	glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_TRUE);

	// uniform vec2 screenWidthAndHeight;	// x is width
	GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");
	glUniform2f(screenWidthAndHeight_UL,
		(GLfloat)screenWidth,
		(GLfloat)screenHeight);


	// Point the FBO from the 1st pass to this texture...

// 	GLint textureUnitNumber = 70;
// 	glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
// 	glBindTexture(GL_TEXTURE_2D, m_pFBO_1->colourTexture_0_ID);
// 
// 	//uniform sampler2D textureOffScreen;
// 	GLint textureOffScreen_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
// 	glUniform1i(textureOffScreen_UL, textureUnitNumber);



	// Setting the spooky heatmap here too, as we want to influence how we read the above texture around this area
	GLint textureUnitNumberSpooky = 71;
	glActiveTexture(GL_TEXTURE0 + textureUnitNumberSpooky);
	glBindTexture(GL_TEXTURE_2D, m_pFBO_3->colourTexture_0_ID);

	//uniform sampler2D textureOffScreen;
	GLint spookyTexture_UL = glGetUniformLocation(shaderProgramID, "spookyHeatMap");
	glUniform1i(spookyTexture_UL, textureUnitNumberSpooky);

	GLint spookyBool_UL = glGetUniformLocation(m_shaderProgramID, "isSpooky");
	glUniform4f(spookyBool_UL, GL_FALSE, GL_FALSE, GL_TRUE, static_cast<GLfloat>(deltaTime)); // Reduce all red values on heatmap



	cMesh fullScreenQuad;
	fullScreenQuad.meshName = "Quad_1_sided_aligned_on_XY_plane.ply";


	//fullScreenQuad.textureName[0] = "cyan.bmp";
	//fullScreenQuad.textureRatios[0] = 1.0f;
	//fullScreenQuad.setUniformDrawScale(5.0f);
	fullScreenQuad.scale = glm::vec3(7.0f, 5.0f, 5.0f); // Try to fit the screen
	fullScreenQuad.drawPosition = glm::vec3(0.0f);
	//fullScreenQuad.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
	fullScreenQuad.adjustRotationAngleFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));

	DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);


	//glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_FALSE);

	return;
}

void cGraphicsMain::DrawPass_SpookyHeatmap(GLuint shaderProgramID, int screenWidth, int screenHeight, glm::vec3 sceneEye, glm::vec3 sceneTarget)
{
	float ratio; //= screenWidth / (float)screenHeight;
	//int width, height;

	glUseProgram(m_shaderProgramID);


	//glfwGetFramebufferSize(m_window, &width, &height);
	ratio = screenWidth / (float)screenHeight;


	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);



	//m_pTheLights->UpdateUniformValues(m_shaderProgramID);




	//uniform vec4 eyeLocation;
	GLint eyeLocation_UL = glGetUniformLocation(m_shaderProgramID, "eyeLocation");

	glUniform4f(eyeLocation_UL,
		sceneEye.x, sceneEye.y, sceneEye.z, 1.0f);



	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 matProjection = glm::perspective(m_FOV,
		ratio,
		0.1f,
		1100.0f); // n/f plane


	glm::mat4 matView = glm::lookAt(sceneEye,
		sceneEye + sceneTarget,
		m_upVector);



	GLint matProjection_UL = glGetUniformLocation(m_shaderProgramID, "matProjection");
	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

	GLint matView_UL = glGetUniformLocation(m_shaderProgramID, "matView");
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

	
	GLint spookyBool_UL = glGetUniformLocation(m_shaderProgramID, "isSpooky");
	glUniform4f(spookyBool_UL, GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE); // Capture render as heatmap




	// *********************************************************************
	// Draw just the spooky objects

	glDisable(GL_CULL_FACE);
	glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix
	for (unsigned int index = 0; index != m_vec_pAllMeshes.size(); index++)
	{
		cMesh* pCurrentMesh = m_vec_pAllMeshes[index];

		if (pCurrentMesh->isSpooky)
		{
			DrawObject(pCurrentMesh, matModel, m_shaderProgramID);
		}
	}
	glEnable(GL_CULL_FACE);



	return;
}

void cGraphicsMain::DrawPass_1(GLuint shaderProgramID, int screenWidth, int screenHeight, glm::vec3 sceneEye, glm::vec3 sceneTarget)
{

	float ratio; //= screenWidth / (float)screenHeight;
	//int width, height;

	//glUseProgram(m_shaderProgramID);
	m_pShaderThing->useShaderProgram("shader01");
	cShaderManager::cShaderProgram* currProg = m_pShaderThing->pGetShaderProgramFromFriendlyName("shader01");


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
	m_pTheLights->UpdateUniformValues(m_shaderProgramID);


	// *****************************************************************
			
	//uniform vec4 eyeLocation;
	GLint eyeLocation_UL = glGetUniformLocation(m_shaderProgramID, "eyeLocation");

	glUniform4f(eyeLocation_UL,
		sceneEye.x, sceneEye.y, sceneEye.z, 1.0f);
	//currProg->setULValue("eyeLocation", &glm::vec4(sceneEye.x, sceneEye.y, sceneEye.z, 1.0f));

	// Set Camera Matrices
	glm::mat4 projMat = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 1100.0f);
	glm::mat4 viewMat = glm::lookAt(sceneEye, sceneEye + sceneTarget, m_upVector);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBOMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projMat));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
// 	glm::mat4 matProjection = glm::perspective(m_FOV,
// 		ratio,
// 		0.1f,
// 		1100.0f); // n/f plane


// 	glm::mat4 matView = glm::lookAt(sceneEye,
// 		sceneEye + sceneTarget,
// 		m_upVector);



// 	GLint matProjection_UL = glGetUniformLocation(m_shaderProgramID, "matProjection");
// 	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));
// 
// 	GLint matView_UL = glGetUniformLocation(m_shaderProgramID, "matView");
// 	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

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


	// Not getting spooky heatmap, so disable it
	GLint spookyBool_UL = glGetUniformLocation(m_shaderProgramID, "isSpooky");
	glUniform4f(spookyBool_UL, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable heatmap capture


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
		GLint bIsSkyBox_UL = glGetUniformLocation(m_shaderProgramID, "bUseHeightmap_IsSkyBox_UseDiscard_NONE");
		glUniform4f(bIsSkyBox_UL, 0.0f, 1.0f, 0.0f, 0.0f);

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

void cGraphicsMain::DrawPass_ApplySpook(GLuint shaderProgramID, int screenWidth, int screenHeight)
{
	float ratio;

	glUseProgram(m_shaderProgramID);

	//glfwGetFramebufferSize(pWindow, &width, &height);
	ratio = screenWidth / (float)screenHeight;


	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// Camera is pointing directly at the full screen quad
	glm::vec3 FSQ_CameraEye = glm::vec3(0.0, 0.0, 5.0f);
	glm::vec3 FSQ_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


	//uniform vec4 eyeLocation;
	GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
	glUniform4f(eyeLocation_UL,
		FSQ_CameraEye.x, FSQ_CameraEye.y, FSQ_CameraEye.z, 1.0f);



	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 matProjection = glm::perspective(0.6f,
		ratio,
		0.1f,        // Near (as big)
		100.0f);    // Far (as small)

	glm::mat4 matView = glm::lookAt(FSQ_CameraEye,
		FSQ_CameraTarget,
		m_upVector);

	GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

	GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

	// Set up the textures for this offscreen quad
	//uniform bool bIsOffScreenTextureQuad;
// 	GLint bIsOffScreenTextureQuad_UL = glGetUniformLocation(shaderProgramID, "bIsOffScreenTextureQuad");
// 	glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_TRUE);

	// uniform vec2 screenWidthAndHeight;	// x is width
	GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");
	glUniform2f(screenWidthAndHeight_UL,
		(GLfloat)screenWidth,
		(GLfloat)screenHeight);


	// Point the FBO from the 1st pass to this texture...

	GLint textureUnitNumber = 70;
	glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
	glBindTexture(GL_TEXTURE_2D, m_pFBO_1->colourTexture_0_ID);

	//uniform sampler2D textureOffScreen;
	GLint textureOffScreen_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
	glUniform1i(textureOffScreen_UL, textureUnitNumber);



	// Setting the spooky heatmap here too, as we want to influence how we read the above texture around this area
	GLint textureUnitNumberSpooky = 71;
	glActiveTexture(GL_TEXTURE0 + textureUnitNumberSpooky);
	glBindTexture(GL_TEXTURE_2D, m_pFBO_3->colourTexture_0_ID);

	//uniform sampler2D textureOffScreen;
	GLint spookyTexture_UL = glGetUniformLocation(shaderProgramID, "spookyHeatMap");
	glUniform1i(spookyTexture_UL, textureUnitNumberSpooky);

	GLint spookyBool_UL = glGetUniformLocation(m_shaderProgramID, "isSpooky");
	glUniform4f(spookyBool_UL, GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE); // Apply heatmap to base capture



	cMesh fullScreenQuad;
	fullScreenQuad.meshName = "Quad_1_sided_aligned_on_XY_plane.ply";


	//fullScreenQuad.textureName[0] = "cyan.bmp";
	//fullScreenQuad.textureRatios[0] = 1.0f;
	//fullScreenQuad.setUniformDrawScale(5.0f);
	fullScreenQuad.scale = glm::vec3(7.0f, 5.0f, 5.0f); // Try to fit the screen
	fullScreenQuad.drawPosition = glm::vec3(0.0f);
	//fullScreenQuad.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
	fullScreenQuad.adjustRotationAngleFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));

	DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);


	//glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_FALSE);


	return;
}

void cGraphicsMain::DrawPass_FSQ(GLuint shaderProgramID, int screenWidth, int screenHeight)
{
	float ratio;

	glUseProgram(m_shaderProgramID);

	//glfwGetFramebufferSize(pWindow, &width, &height);
	ratio = screenWidth / (float)screenHeight;


	// While drawing a pixel, see if the pixel that's already there is closer or not?
	glEnable(GL_DEPTH_TEST);
	// (Usually) the default - does NOT draw "back facing" triangles
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	// Camera is pointing directly at the full screen quad
	glm::vec3 FSQ_CameraEye = glm::vec3(0.0, 0.0, 5.0f);
	glm::vec3 FSQ_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


	//uniform vec4 eyeLocation;
	GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
	glUniform4f(eyeLocation_UL,
		FSQ_CameraEye.x, FSQ_CameraEye.y, FSQ_CameraEye.z, 1.0f);


	// Set Camera Matrices
	glm::mat4 projMat = glm::perspective(0.6f, ratio, 0.1f, 100.0f);
	glm::mat4 viewMat = glm::lookAt(FSQ_CameraEye, FSQ_CameraTarget, m_upVector);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBOMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projMat));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
// 	glm::mat4 matProjection = glm::perspective(0.6f,
// 		ratio,
// 		0.1f,        // Near (as big)
// 		100.0f);    // Far (as small)
// 
// 	glm::mat4 matView = glm::lookAt(FSQ_CameraEye,
// 		FSQ_CameraTarget,
// 		m_upVector);
// 
// 	GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
// 	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));
// 
// 	GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
// 	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

	// Set up the textures for this offscreen quad
	//uniform bool bIsOffScreenTextureQuad;
	GLint bIsOffScreenTextureQuad_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight_bIsOffScreen");
	//glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_TRUE);
	glUniform4f(bIsOffScreenTextureQuad_UL, (GLfloat)screenWidth, (GLfloat)screenHeight, (GLfloat)GL_TRUE, 0.0f);
	// uniform vec2 screenWidthAndHeight;	// x is width
// 	GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");
// 	glUniform2f(screenWidthAndHeight_UL,
// 		(GLfloat)screenWidth,
// 		(GLfloat)screenHeight);


	// Point the FBO from the 1st pass to this texture...

	GLint textureUnitNumber = 70;
	glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
	glBindTexture(GL_TEXTURE_2D, m_pFBO_1->colourTexture_0_ID);//m_pFBO_1->colourTexture_0_ID);

	//uniform sampler2D textureOffScreen;
	GLint textureOffScreen_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
	glUniform1i(textureOffScreen_UL, textureUnitNumber);



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



	cMesh fullScreenQuad;
	fullScreenQuad.meshName = "Quad_1_sided_aligned_on_XY_plane.ply";
	//fullScreenQuad.meshName = "Sphere_1_unit_Radius.ply";
	//fullScreenQuad.meshName = "legospiderman_head_xyz_n_rgba_uv_at_Origin.ply";

	    fullScreenQuad.textureName[0] = "cyan.bmp";
	    fullScreenQuad.textureRatios[0] = 1.0f;
	fullScreenQuad.setUniformDrawScale(5.0f);
	fullScreenQuad.drawPosition = glm::vec3(0.0f);
	//fullScreenQuad.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
	fullScreenQuad.adjustRotationAngleFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));

	DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);


	//glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_FALSE);
	glUniform4f(bIsOffScreenTextureQuad_UL, (GLfloat)screenWidth, (GLfloat)screenHeight, (GLfloat)GL_FALSE, 0.0f);


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

void cGraphicsMain::updateMesh(int meshID, std::string newFriendlyName, int newTextureIdx[], float newRatios[], bool isVisible, bool isWireframe, bool doNotLight, bool useDebugColor, glm::vec4 debugColor)
{
	// Start by finding mesh with corresponding ID
	// Should have this in a map, I'll do it later
	cMesh* meshToUpdate = nullptr;
	for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++)
	{
		if (m_vec_pMeshesToDraw[i]->uniqueID == meshID)
		{
			meshToUpdate = m_vec_pMeshesToDraw[i];
			break;
		}
	}
	for (unsigned int i = 0; i < m_vec_pTransMeshesToDraw.size(); i++)
	{
		if (m_vec_pTransMeshesToDraw[i]->uniqueID == meshID)
		{
			meshToUpdate = m_vec_pTransMeshesToDraw[i];
			break;
		}
	}
	if (meshToUpdate == nullptr)
		return;


	meshToUpdate->friendlyName = newFriendlyName;
	for (unsigned int i = 0; i < meshToUpdate->NUM_TEXTURES; i++)
	{
		meshToUpdate->textureName[i] = m_AvailableTextures[newTextureIdx[i]];
		meshToUpdate->textureRatios[i] = newRatios[i];
		meshToUpdate->textureIdx[i] = newTextureIdx[i];
	}
	meshToUpdate->bIsVisible = isVisible;
	meshToUpdate->bIsWireframe = isWireframe;
	meshToUpdate->bDoNotLight = doNotLight;
	meshToUpdate->bUseCustomColors = useDebugColor;
	meshToUpdate->customColorRGBA = debugColor;


	return;
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
	{
		GLint textureUnitNumber = 0;
		GLuint Texture00 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, Texture00);
		GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "texture_00");
		glUniform1i(texture_00_UL, textureUnitNumber);
	}

	{
		GLint textureUnitNumber = 1;
		GLuint Texture01 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, Texture01);
		GLint texture_01_UL = glGetUniformLocation(shaderProgramID, "texture_01");
		glUniform1i(texture_01_UL, textureUnitNumber);
	}

	{
		GLint textureUnitNumber = 2;
		GLuint Texture02 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, Texture02);
		GLint texture_02_UL = glGetUniformLocation(shaderProgramID, "texture_02");
		glUniform1i(texture_02_UL, textureUnitNumber);
	}

	{
		GLint textureUnitNumber = 3;
		GLuint Texture03 = m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, Texture03);
		GLint texture_03_UL = glGetUniformLocation(shaderProgramID, "texture_03");
		glUniform1i(texture_03_UL, textureUnitNumber);
	}
	// and so on to however many texture you are using

//    uniform vec4 textureMixRatio_0_3;
//    uniform vec4 textureMixRatio_4_7;

	GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_0_3");
	//    GLint textureMixRatio_4_7_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_4_7");

	glUniform4f(textureMixRatio_0_3_UL,
		pCurrentMesh->textureRatios[0],
		pCurrentMesh->textureRatios[1],
		pCurrentMesh->textureRatios[2],
		pCurrentMesh->textureRatios[3]);
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
