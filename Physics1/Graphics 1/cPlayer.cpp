#include "cPlayer.h"
//#include "Physics/cPhysics.h"





cPlayer::cPlayer()
{

}

cPlayer::~cPlayer()
{

}

// Adds velocity to the ball 
std::vector<glm::vec3> cPlayer::Update(double deltaTime, GLFWwindow* theWindow, glm::vec3 currCamPos, glm::vec3 currCamDir, glm::vec3 currCamRot)
{
	std::vector<glm::vec3> camInfo;
	camInfo.push_back(currCamPos);
	camInfo.push_back(currCamDir);
	camInfo.push_back(currCamRot);
	static double mouseXPos;
	static double mouseYPos;

	double deltaMouseX, deltaMouseY;
	glfwGetCursorPos(theWindow, &deltaMouseX, &deltaMouseY); // Get current mouse position
	deltaMouseX -= mouseXPos; // Set the delta mouse positions
	deltaMouseY -= mouseYPos; // for this tick

	// Camera rotation here :)
	camInfo[2].x -= deltaMouseX / 1000;

	camInfo[2].y -= deltaMouseY / 2000;
	camInfo[1].y = camInfo[2].y;     // This is pitch
	camInfo[1].x = sin(camInfo[2].x);         // This is just y-rotation
	camInfo[1].z = sin(camInfo[2].x + 1.57);  //
	camInfo[1] = glm::normalize(camInfo[1]);
	camInfo[1].y *= 2;

	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(theWindow, &width, &height);
	glfwSetCursorPos(theWindow, width / 2, height / 2);
	glfwGetCursorPos(theWindow, &mouseXPos, &mouseYPos); // Update this for next loop

	// Now place camera at proper distance from obj
	glm::vec3 finalCamPos = this->m_PlayerObject->position;
	finalCamPos -= (20.0f * camInfo[1]);

	camInfo[0] = finalCamPos;

	// Now query WASD to add velocity to the ball
	glm::vec3 forwardDir = glm::vec3(camInfo[1].x, 0, camInfo[1].z); // 0 y cause we only care about horizontal movement here
	forwardDir = glm::normalize(forwardDir);

	int state;
	state = glfwGetKey(theWindow, GLFW_KEY_W);
	if (state == GLFW_PRESS) // Move forward
	{
		this->m_PlayerObject->velocity += forwardDir * m_MoveSpeed * (float)deltaTime;
	}
	state = glfwGetKey(theWindow, GLFW_KEY_S);
	if (state == GLFW_PRESS) // Move backwards
	{
		this->m_PlayerObject->velocity -= forwardDir * m_MoveSpeed * (float)deltaTime;
	}
	state = glfwGetKey(theWindow, GLFW_KEY_A);
	if (state == GLFW_PRESS) // Move left
	{
		//m_cameraEye += glm::normalize(glm::cross(m_upVector, m_cameraTarget)) * m_FlyCamSpeed;
		this->m_PlayerObject->velocity += glm::normalize(glm::cross(glm::vec3(0,1,0), forwardDir)) * m_MoveSpeed * (float)deltaTime;
	}
	state = glfwGetKey(theWindow, GLFW_KEY_D);
	if (state == GLFW_PRESS) // Move right
	{
		this->m_PlayerObject->velocity -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardDir)) * m_MoveSpeed * (float)deltaTime;
	}


	// TODO jump later..

	return camInfo;
}

void cPlayer::setAssociatedPhysObj(sPhsyicsProperties* theObj)
{
	m_PlayerObject = theObj;
	return;
}

