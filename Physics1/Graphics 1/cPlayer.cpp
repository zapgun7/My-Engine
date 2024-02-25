#include "cPlayer.h"
#include <iostream>


cPlayer::cPlayer(GLFWwindow* window)
{
	m_window = window;
	m_pEngineController = cEngineController::GetEngineController();
	m_CameraType = FIRSTPERSON;

}

cPlayer::~cPlayer()
{

}

void cPlayer::setPlayerObject(sPhysicsProperties* theObj)
{
	m_pPlayerObject = theObj;
	return;
}

void cPlayer::Update(double deltaTime, glm::vec3& cameraPosition, glm::quat& cameraRotation)
{
	int stat = glfwGetKey(m_window, GLFW_KEY_ESCAPE);
	static bool isPressed = false;

	if ((stat) && (!isPressed))
	{
		if (m_CameraType == FIRSTPERSON)
			m_CameraType = FLYCAM;
		else
			m_CameraType = FIRSTPERSON;
		isPressed = true;
	}
	else if (!stat)
	{
		isPressed = false;
	}





	static double mouseXPos = 0;
	static double mouseYPos = 0;
	if (m_CameraType == FLYCAM)
	{
		static bool isRightClicking = false;

		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		glm::vec3 forwardVector = glm::vec3(0, 0, 1) * (cameraRotation);
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

				(cameraRotation) *= deltaQuat;


				glfwSetCursorPos(m_window, width / 2, height / 2);
			}

		}
		else if (isRightClicking)
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			isRightClicking = false;
		}

		bool isMoving = false;
		const float SPEED = 40.0f;
		const float CAMERADISTANCE = 50.0f;
		const float SPEEDCAP = 30.0f;
		const float SPEEDREDUCTION = 1.0f;
		const float ROTATIONSPEED = 90.0f;



		if (isRightClicking) // Have movement tied to right-clicking too
		{
			state = glfwGetKey(m_window, GLFW_KEY_W);
			if (state == GLFW_PRESS) // Move forward
			{
				//m_cameraEye += glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
				cameraPosition += glm::normalize(forwardVector) * m_CameraSpeed * static_cast<float>(deltaTime);
			}
			state = glfwGetKey(m_window, GLFW_KEY_S);
			if (state == GLFW_PRESS) // Move backwards
			{
				cameraPosition -= glm::normalize(forwardVector) * m_CameraSpeed * static_cast<float>(deltaTime);
			}
			state = glfwGetKey(m_window, GLFW_KEY_A);
			if (state == GLFW_PRESS) // Move left
			{
				cameraPosition += glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardVector)) * m_CameraSpeed * static_cast<float>(deltaTime);
			}
			state = glfwGetKey(m_window, GLFW_KEY_D);
			if (state == GLFW_PRESS) // Move right
			{
				cameraPosition -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardVector)) * m_CameraSpeed * static_cast<float>(deltaTime);
			}

		}
	}
	else if (m_CameraType == FIRSTPERSON)
	{
// 		static double mouseXPos = 0;
// 		static double mouseYPos = 0;

		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		glm::vec3 forwardVector = glm::vec3(0, 0, 1) * (cameraRotation);
		glm::vec3 XZForwardVec(forwardVector.x, 0, forwardVector.z);

		mouseXPos = width / 2;
		mouseYPos = height / 2;
		//glfwSetCursorPos(m_window, width / 2, height / 2);
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


		double deltaMouseX, deltaMouseY;
		glfwGetCursorPos(m_window, &deltaMouseX, &deltaMouseY); // Get current mouse position
		deltaMouseX -= mouseXPos; // Set the delta mouse positions
		deltaMouseY -= mouseYPos; // for this tick

		float deltaX, deltaY, deltaZ;
		deltaY = deltaMouseX / m_InverseSensitivity;


		glm::vec3 normXZ = glm::normalize(XZForwardVec);
		float xLook = normXZ.x;
		float zLook = normXZ.z;



		deltaZ = deltaMouseY / m_InverseSensitivity * xLook;
		deltaX = -deltaMouseY / m_InverseSensitivity * zLook;

		glm::quat deltaQuat = glm::quat(glm::radians(glm::vec3(deltaX, deltaY, deltaZ)));

		(cameraRotation) *= deltaQuat;
		// TODO maybe. Remake XZ forward after the new one has been established

		glfwSetCursorPos(m_window, width / 2, height / 2);

		bool isInputMove = false;

		int state = glfwGetKey(m_window, GLFW_KEY_W);
		if (state == GLFW_PRESS) // Move forward
		{
			//m_cameraEye += glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
			m_pPlayerObject->velocity += glm::normalize(XZForwardVec) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}
		state = glfwGetKey(m_window, GLFW_KEY_S);
		if (state == GLFW_PRESS) // Move backwards
		{
			m_pPlayerObject->velocity -= glm::normalize(XZForwardVec) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}
		state = glfwGetKey(m_window, GLFW_KEY_A);
		if (state == GLFW_PRESS) // Move left
		{
			m_pPlayerObject->velocity += glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec)) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}
		state = glfwGetKey(m_window, GLFW_KEY_D);
		if (state == GLFW_PRESS) // Move right
		{
			m_pPlayerObject->velocity -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec)) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}
		state = glfwGetKey(m_window, GLFW_KEY_SPACE);
		if ((state == GLFW_PRESS) && (m_pPlayerObject->jumpNormThisFrame)) // Move right
		{
			m_pPlayerObject->velocity += glm::vec3(0, m_PLAYERJUMPFORCE, 0);
		}
		cameraPosition = m_pPlayerObject->position + glm::vec3(0, 1, 0);


		if (isInputMove)
		{
			m_pPlayerObject->friction = 1.0f;
		}
		else
		{
			m_pPlayerObject->friction = 0.95f;
		}
	}
		

	return;
}


