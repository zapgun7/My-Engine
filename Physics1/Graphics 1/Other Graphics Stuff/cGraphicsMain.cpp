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
	m_cameraEye = glm::vec3(-120.0f, 40.0f, 0.0f);
	m_cameraTarget = glm::vec3(1.0f, -0.2f, 0.0f);
	m_cameraRotation = glm::vec3(0.0, 0.0f, 0.0f);
	m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_ShowLightEditor = false;
	m_ShowMeshEditor = false;
	//m_player = new cPlayer();
	//m_io = ImGui::GetIO();
}

bool cGraphicsMain::Initialize()
{
	m_InputHandler = new cInputHandler(); // Will eventually move these 2 somewhere that makes sense TODO
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

	m_window = glfwCreateWindow(640, 480, "Ausgine Render", NULL, NULL);
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
	//::g_pPhysics->setVAOManager(m_pMeshManager); //UNPHYS

	m_pMeshManager->setBasePath("assets/models");

	m_pTheLights = new cLightManager();
	m_pTheLights->SetUniformLocations(m_shaderProgramID);

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

	



	return 1;
}






bool cGraphicsMain::Update(double deltaTime) // Main "loop" of the window. Not really a loop, just gets called every tick
{
	// Check input for camera movement
	m_InputHandler->queryKeys(m_window);


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







	flyCameraInput(width, height); // UPDATE CAMERA STATS





	GLint eyeLocation_UL = glGetUniformLocation(m_shaderProgramID, "eyeLocation");
	glUniform4f(eyeLocation_UL,
		m_cameraEye.x, m_cameraEye.y, m_cameraEye.z, 1.0f);



	//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 matProjection = glm::perspective(0.7f,
		ratio,
		0.01f,
		5000.0f); // n/f plane



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
		theSkyBox.setUniformDrawScale(10.0f);

		theSkyBox.setUniformDrawScale(4'000.0f);
		theSkyBox.setDrawPosition(m_cameraEye);
		//            theSkyBox.bIsWireframe = true;

					// Depth test
		//            glDisable(GL_DEPTH_TEST);       // Writes no matter what
					// Write to depth buffer (depth mask)
		//            glDepthMask(GL_FALSE);          // Won't write to the depth buffer

					// uniform bool bIsSkyBox;
		GLint bIsSkyBox_UL = glGetUniformLocation(m_shaderProgramID, "bIsSkyBox");
		glUniform1f(bIsSkyBox_UL, (GLfloat)GL_TRUE);

		// The normals for this sphere are facing "out" but we are inside the sphere
		glCullFace(GL_FRONT);

		DrawObject(&theSkyBox, glm::mat4(1.0f), m_shaderProgramID);

		glUniform1f(bIsSkyBox_UL, (GLfloat)GL_FALSE);

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
	for (unsigned int i = 0; i < m_vec_pMeshesToDraw.size(); i++) // Delete all pointers to meshes
	{
		delete m_vec_pMeshesToDraw[i];
	}
	m_vec_pMeshesToDraw = newMeshVec; // Set new mesh vector

	// Delete all current physics objects
	//::g_pPhysics->deleteAllObjects(); //UNPHYS


	for (cMesh* meshObj : m_vec_pMeshesToDraw) // Attach physics objects to all new objects
	{
		sPhysicsProperties* newShape;
		if (meshObj->meshName == "Sphere_1_unit_Radius.ply")
		{
			newShape = new sPhysicsProperties();
			newShape->shapeType = sPhysicsProperties::SPHERE;
			newShape->setShape(new sPhysicsProperties::sSphere(1.0f)); // Since a unit sphere, radius of .5 
			newShape->pTheAssociatedMesh = meshObj;
			newShape->inverse_mass = 1.0f; // Idk what to set this
			newShape->friendlyName = "Sphere";
			newShape->acceleration.y = -20.0f;
			newShape->position = meshObj->drawPosition;
			newShape->oldPosition = meshObj->drawPosition;
			newShape->restitution = 0.5f;
			// ::g_pPhysics->AddShape(newShape); //UNPHYS
		}
		else // Just make it an indirect triangle mesh
		{
			newShape = new sPhysicsProperties();
			newShape->shapeType = sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT;
			newShape->setShape(new sPhysicsProperties::sMeshOfTriangles_Indirect(meshObj->meshName));
			newShape->pTheAssociatedMesh = meshObj;
			newShape->inverse_mass = 0.0f; // Idk what to set this
			newShape->friendlyName = "IndirectMesh";
			newShape->setRotationFromEuler(meshObj->getEulerOrientation());
			newShape->position = meshObj->drawPosition;
			newShape->oldPosition = meshObj->drawPosition;
			//::g_pPhysics->AddShape(newShape); //UNPHYS
		}
		meshObj->uniqueID = newShape->getUniqueID(); // Set mesh ID to match associated physics object's ID
	}


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

GLFWwindow* cGraphicsMain::getWindow(void)
{
	return m_window;
}

void cGraphicsMain::getAvailableModels(std::vector<std::string>* ModelVec, std::vector<std::string>* TexVec)
{
	//ModelVec = &m_AvailableModels; // Doesn't work
	for (unsigned int i = 0; i < m_AvailableModels.size(); i++)
	{
		ModelVec->push_back(m_AvailableModels[i]);
	}
	for (unsigned int i = 0; i < m_AvailableTextures.size(); i++)
	{
		TexVec->push_back(m_AvailableTextures[i]);
	}
	return;
}

void cGraphicsMain::getActiveMeshNLights(std::vector<cMesh*>* MeshVec, cLightManager* TheLights)
{
	// MeshVec = &m_vec_pMeshesToDraw; // No work :(
	for (unsigned int i = 0; i < m_vec_pAllMeshes.size(); i++)
	{
		//MeshVec->push_back(m_vec_pMeshesToDraw[i]); 
		MeshVec->push_back(m_vec_pAllMeshes[i]);
	}
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

	/// REFLECTION & REFRACTION

	GLint bUseReflect_UL = glGetUniformLocation(shaderProgramID, "bUseReflect");
	if (pCurrentMesh->bUseReflect)
	{
		glUniform1f(bUseReflect_UL, (GLfloat)GL_TRUE);
	}
	else
	{
		glUniform1f(bUseReflect_UL, (GLfloat)GL_FALSE);
	}

	GLint bUseRefract_UL = glGetUniformLocation(shaderProgramID, "bUseRefract");
	if (pCurrentMesh->bUseRefract)
	{
		glUniform1f(bUseRefract_UL, (GLfloat)GL_TRUE);
	}
	else
	{
		glUniform1f(bUseRefract_UL, (GLfloat)GL_FALSE);
	}

	/// ALPHA TRANSPARECY

	GLint fTransparencyAlpha_UL = glGetUniformLocation(shaderProgramID, "transparencyAlpha");
	glUniform1f(fTransparencyAlpha_UL, pCurrentMesh->transparencyAlpha);



	//////////////////// TEXTURE STUFF /////////////////////////

	GLint bUseVertexColours_UL = glGetUniformLocation(shaderProgramID, "bUseVertexColours");
	glUniform1f(bUseVertexColours_UL, (GLfloat)GL_FALSE);


	SetUpTextures(pCurrentMesh, shaderProgramID);

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

// Adds new object to the meshestodraw
void cGraphicsMain::addNewMesh(cMesh* newMesh) // Just adds new mesh pointer to the vector
{
	m_vec_pMeshesToDraw.push_back(newMesh);
	m_vec_pAllMeshes.push_back(newMesh);


	return;
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
	meshToUpdate->bUseDebugColours = useDebugColor;
	meshToUpdate->wholeObjectDebugColourRGBA = debugColor;


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
		GLint textureUnit30 = 30;
		GLuint skyBoxID = m_pTextureManager->getTextureIDFromName("SunnyDay");
		glActiveTexture(GL_TEXTURE0 + textureUnit30);
		// NOTE: Binding is NOT to GL_TEXTURE_2D
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxID);
		GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgramID, "skyBoxTexture");
		glUniform1i(skyBoxSampler_UL, textureUnit30);
	}


	return;
}
