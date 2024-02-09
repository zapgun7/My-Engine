#include "cPlayer.h"
#include <iostream>


cPlayer::cPlayer(GLFWwindow* window)
{
	m_window = window;
	m_pEngineController = cEngineController::GetEngineController();
	m_CameraType = THIRDPERSON;

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
	// INPUT FOR THE ANIMATION PROJECT
// 	int tempState = glfwGetKey(m_window, GLFW_KEY_1); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(1.0f);
// 	tempState = glfwGetKey(m_window, GLFW_KEY_2); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(2.0f);
// 	tempState = glfwGetKey(m_window, GLFW_KEY_3); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(3.0f);
// 	tempState = glfwGetKey(m_window, GLFW_KEY_4); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(4.0f);
// 	tempState = glfwGetKey(m_window, GLFW_KEY_5); if (tempState == GLFW_PRESS) m_pEngineController->setTimescale(5.0f);
// 
// 	static bool isSpacePressed = false;
// 
// 	tempState = glfwGetKey(m_window, GLFW_KEY_SPACE);
// 	if (isSpacePressed)
// 	{
// 		if (tempState != GLFW_PRESS) isSpacePressed = false;
// 	}
// 	else
// 	{
// 		if (tempState == GLFW_PRESS) 
// 		{
// 			isSpacePressed = true;
// 			m_pEngineController->toggleRunningState();
// 		}
// 	}
	

	static bool isRightClicking = false;
	static double mouseXPos = 0;
	static double mouseYPos = 0;

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
		switch (m_CameraType)
		{
		case FLYCAM:
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
			break;
		case THIRDPERSON:

			glm::vec3 objForward = m_pPlayerObject->get_qOrientation() * glm::vec3(0, 0, 1.0f);
			objForward = glm::normalize(glm::vec3(objForward.x, 0, objForward.z));


			state = glfwGetKey(m_window, GLFW_KEY_W);
			if (state == GLFW_PRESS) // Move forward
			{
				//m_pPlayerObject->position += objForward * SPEED * static_cast<float>(deltaTime);
				m_pPlayerObject->velocity += objForward * SPEED * static_cast<float>(deltaTime);
				isMoving = true;
			}
			state = glfwGetKey(m_window, GLFW_KEY_S);
			if (state == GLFW_PRESS) // Move backwards
			{
				//m_pPlayerObject->position -= objForward * SPEED * static_cast<float>(deltaTime);
				m_pPlayerObject->velocity -= objForward * SPEED * static_cast<float>(deltaTime);
				isMoving = true;
			}
			state = glfwGetKey(m_window, GLFW_KEY_A);
			if (state == GLFW_PRESS) // Move left
			{
				//m_pPlayerObject->position += glm::normalize(glm::cross(glm::vec3(0, 1, 0), objForward)) * SPEED * static_cast<float>(deltaTime);
				m_pPlayerObject->velocity += glm::normalize(glm::cross(glm::vec3(0, 1, 0), objForward)) * SPEED * static_cast<float>(deltaTime);
				isMoving = true;
// 				glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, 3, 0)));
// 				m_pPlayerObject->setRotationFromQuat(m_pPlayerObject->get_qOrientation() * rotAdjust);
			}
			state = glfwGetKey(m_window, GLFW_KEY_D);
			if (state == GLFW_PRESS) // Move right
			{
				//m_pPlayerObject->position -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), objForward)) * SPEED * static_cast<float>(deltaTime);
				m_pPlayerObject->velocity -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), objForward)) * SPEED * static_cast<float>(deltaTime);
				isMoving = true;
// 				glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -3, 0)));
// 				m_pPlayerObject->setRotationFromQuat(m_pPlayerObject->get_qOrientation()* rotAdjust);
			}
			state = glfwGetKey(m_window, GLFW_KEY_Q); // Rotate left
			if (state == GLFW_PRESS) // Move right
			{
				glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, ROTATIONSPEED * static_cast<float>(deltaTime), 0)));
				m_pPlayerObject->setRotationFromQuat(m_pPlayerObject->get_qOrientation()* rotAdjust);
			}
			state = glfwGetKey(m_window, GLFW_KEY_E); // Rotate right
			if (state == GLFW_PRESS) // Move right
			{
				glm::quat rotAdjust = glm::quat(glm::radians(glm::vec3(0, -ROTATIONSPEED * static_cast<float>(deltaTime), 0)));
				m_pPlayerObject->setRotationFromQuat(m_pPlayerObject->get_qOrientation()* rotAdjust);
			}

			// Cap and reduce player object velocity
			if (glm::length(m_pPlayerObject->velocity) > SPEEDCAP)
			{
				m_pPlayerObject->velocity = glm::normalize(m_pPlayerObject->velocity) * SPEEDCAP;
			}
			else if (!isMoving)
			{
// 				float addSpdReduction = 10.0f;
// 				addSpdReduction -= glm::length(m_pPlayerObject->velocity);

				// Reduce speed over time
				m_pPlayerObject->velocity -= m_pPlayerObject->velocity * SPEEDREDUCTION * static_cast<float>(deltaTime);
// 				if (glm::length(m_pPlayerObject->velocity) < 2.0f)
// 					m_pPlayerObject->velocity -= m_pPlayerObject->velocity * SPEEDREDUCTION * static_cast<float>(deltaTime);
				if (glm::length(m_pPlayerObject->velocity) < 0.1f)
					m_pPlayerObject->velocity = glm::vec3(0);
			}

			// Now to set the camera to where it should be

			cameraPosition = m_pPlayerObject->position - forwardVector * CAMERADISTANCE;
			//printf("X: %.2f, Y: %.2f, Z: %.2f\n", cameraPosition.x, cameraPosition.y, cameraPosition.z);

			// Set object direction to camera
			//XZForwardVec.y = 0.01f;
			//m_pPlayerObject->setRotationFromQuat(glm::quatLookAtRH(-XZForwardVec, glm::vec3(0, 1, 0)));

			break;
		}
	}

	if ((m_CameraType == THIRDPERSON) && (!isMoving))
	{
		m_pPlayerObject->velocity -= m_pPlayerObject->velocity * SPEEDREDUCTION * static_cast<float>(deltaTime);
		// 				if (glm::length(m_pPlayerObject->velocity) < 2.0f)
		// 					m_pPlayerObject->velocity -= m_pPlayerObject->velocity * SPEEDREDUCTION * static_cast<float>(deltaTime);
		if (glm::length(m_pPlayerObject->velocity) < 0.1f)
			m_pPlayerObject->velocity = glm::vec3(0);
		cameraPosition = m_pPlayerObject->position - forwardVector * CAMERADISTANCE;
	}

	return;
}


