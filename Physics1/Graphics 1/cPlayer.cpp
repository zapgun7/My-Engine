#include "cPlayer.h"
#include <iostream>


cPlayer::cPlayer(GLFWwindow* window)
{
	m_window = window;
	m_pEngineController = cEngineController::GetEngineController();
}

cPlayer::~cPlayer()
{

}

void cPlayer::Update(double deltaTime, glm::vec3* cameraPosition, glm::quat* cameraRotation)
{
	// INPUT FOR THE ANIMATION PROJECT
	int tempState = glfwGetKey(m_window, GLFW_KEY_1); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(1.0f);
	tempState = glfwGetKey(m_window, GLFW_KEY_2); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(2.0f);
	tempState = glfwGetKey(m_window, GLFW_KEY_3); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(3.0f);
	tempState = glfwGetKey(m_window, GLFW_KEY_4); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(4.0f);
	tempState = glfwGetKey(m_window, GLFW_KEY_5); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(5.0f);

	static bool isSpacePressed = false;

	tempState = glfwGetKey(m_window, GLFW_KEY_SPACE);
	if (isSpacePressed)
	{
		if (tempState != GLFW_PRESS) isSpacePressed = false;
	}
	else
	{
		if (tempState == GLFW_PRESS) 
		{
			isSpacePressed = true;
			m_pEngineController->toggleRunningState();
		}
	}
	









	static bool isRightClicking = false;
	static double mouseXPos = 0;
	static double mouseYPos = 0;

	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);

	glm::vec3 forwardVector = glm::vec3(0, 0, 1) * (*cameraRotation); 
	glm::vec3 XZForwardVec(forwardVector.x, 0, forwardVector.z);

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

			float deltaX, deltaY, deltaZ;
			deltaY = deltaMouseX / m_InverseSensitivity;
			
			// For other pitch and roll start by getting signed ratios of the forward vector


			glm::vec3 normXZ = glm::normalize(XZForwardVec);
			float xLook = normXZ.x;
			float zLook = normXZ.z;



			deltaZ = deltaMouseY / m_InverseSensitivity * xLook;
			deltaX = -deltaMouseY / m_InverseSensitivity * zLook;

			glm::quat deltaQuat = glm::quat(glm::radians(glm::vec3(deltaX, deltaY, deltaZ)));

			(*cameraRotation) *= deltaQuat;


			glfwSetCursorPos(m_window, width / 2, height / 2);
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
			//m_cameraEye += glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
			*cameraPosition += glm::normalize(forwardVector) * m_CameraSpeed * (float)deltaTime;
		}
		state = glfwGetKey(m_window, GLFW_KEY_S);
		if (state == GLFW_PRESS) // Move backwards
		{
			*cameraPosition -= glm::normalize(forwardVector) * m_CameraSpeed * (float)deltaTime;
		}
		state = glfwGetKey(m_window, GLFW_KEY_A);
		if (state == GLFW_PRESS) // Move left
		{
			*cameraPosition += glm::normalize(glm::cross(glm::vec3(0,1,0), forwardVector)) * m_CameraSpeed * (float)deltaTime;
		}
		state = glfwGetKey(m_window, GLFW_KEY_D);
		if (state == GLFW_PRESS) // Move right
		{
			*cameraPosition -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardVector)) * m_CameraSpeed * (float)deltaTime;
		}
	}

	return;
}


