#include "cPlayer.h"
#include <iostream>

#include "Physics/cPhysics.h" // For tests


cPlayer::cPlayer(GLFWwindow* window)
{
	m_window = window;
	m_pEngineController = cEngineController::GetEngineController();
	m_pInput = cInputHandler::GetInstance();
	m_pThePhysics = cPhysics::GetInstance();
	m_CameraType = FLYCAM;

	m_KICKREACH = 50.0f;

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
// 	int stat = glfwGetKey(m_window, GLFW_KEY_ESCAPE);
// 	static bool isPressed = false;

	if (m_pInput->IsPressedEvent(GLFW_KEY_ESCAPE))
	{
		if (m_CameraType == FIRSTPERSON)
		{
			m_CameraType = FLYCAM;
			m_pInput->ChangeMouseState(NORMAL);
		}
		else
		{
			m_CameraType = FIRSTPERSON;
		}
	}






	//static double mouseXPos = 0;
	//static double mouseYPos = 0;
	if (m_CameraType == FLYCAM)
	{
		glm::vec3 forwardVector = glm::vec3(0, 0, 1) * (cameraRotation);
		glm::vec3 XZForwardVec(forwardVector.x, 0, forwardVector.z);



		if (m_pInput->IsMousePressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			//if (!isRightClicking) // start tracking
			if (m_pInput->IsMousePressedEvent(GLFW_MOUSE_BUTTON_RIGHT))
			{
				m_pInput->ChangeMouseState(HIDDEN);
			}
			else
			{
				double deltaMouseX, deltaMouseY;
				m_pInput->GetMouseDeltas(deltaMouseX, deltaMouseY);

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
			}

		}
		else if (m_pInput->IsMouseReleasedEvent(GLFW_MOUSE_BUTTON_RIGHT))
		{
			m_pInput->ChangeMouseState(NORMAL);
		}

		bool isMoving = false;
		const float SPEED = 40.0f;
		const float CAMERADISTANCE = 50.0f;
		const float SPEEDCAP = 30.0f;
		const float SPEEDREDUCTION = 1.0f;
		const float ROTATIONSPEED = 90.0f;



		//if (isRightClicking) // Have movement tied to right-clicking too
		if (m_pInput->IsMousePressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			if (m_pInput->IsPressed(GLFW_KEY_W))
			{
				//m_cameraEye += glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
				cameraPosition += glm::normalize(forwardVector) * m_CameraSpeed * static_cast<float>(deltaTime);
			}
			if (m_pInput->IsPressed(GLFW_KEY_S))
			{
				cameraPosition -= glm::normalize(forwardVector) * m_CameraSpeed * static_cast<float>(deltaTime);
			}
			if (m_pInput->IsPressed(GLFW_KEY_A))
			{
				cameraPosition += glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardVector)) * m_CameraSpeed * static_cast<float>(deltaTime);
			}
			if (m_pInput->IsPressed(GLFW_KEY_D))
			{
				cameraPosition -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardVector)) * m_CameraSpeed * static_cast<float>(deltaTime);
			}

		}
	}
	else if (m_CameraType == FIRSTPERSON) // The "player" character
	{
// 		static double mouseXPos = 0;
// 		static double mouseYPos = 0;

// 		int width, height;
// 		glfwGetFramebufferSize(m_window, &width, &height);

		glm::vec3 forwardVector = glm::vec3(0, 0, 1) * (cameraRotation);
		glm::vec3 XZForwardVec(forwardVector.x, 0, forwardVector.z);

// 		mouseXPos = width / 2;
// 		mouseYPos = height / 2;
		//glfwSetCursorPos(m_window, width / 2, height / 2);
		//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_pInput->ChangeMouseState(HIDDEN);


		double deltaMouseX, deltaMouseY;
		m_pInput->GetMouseDeltas(deltaMouseX, deltaMouseY);
		//glfwGetCursorPos(m_window, &deltaMouseX, &deltaMouseY); // Get current mouse position
// 		deltaMouseX -= mouseXPos; // Set the delta mouse positions
// 		deltaMouseY -= mouseYPos; // for this tick

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

		//glfwSetCursorPos(m_window, width / 2, height / 2);

		bool isInputMove = false;

		if (m_pInput->IsPressed(GLFW_KEY_W)) // Move forward
		{
			//m_cameraEye += glm::normalize(m_cameraTarget) * m_FlyCamSpeed;
			m_pPlayerObject->velocity += glm::normalize(XZForwardVec) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}

		if (m_pInput->IsPressed(GLFW_KEY_S)) // Move backwards
		{
			m_pPlayerObject->velocity -= glm::normalize(XZForwardVec) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}

		if (m_pInput->IsPressed(GLFW_KEY_A)) // Move left
		{
			m_pPlayerObject->velocity += glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec)) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}

		if (m_pInput->IsPressed(GLFW_KEY_D)) // Move right
		{
			m_pPlayerObject->velocity -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec)) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			isInputMove = true;
		}

		if ((m_pInput->IsPressed(GLFW_KEY_SPACE)) && (m_pPlayerObject->jumpNormThisFrame)) // Jump
		{
			m_pPlayerObject->velocity += glm::vec3(0, m_PLAYERJUMPFORCE, 0);
		}


		// KICK HANDLING


		if (m_pInput->IsMousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{

		}
		else if (m_pInput->IsMouseReleasedEvent(GLFW_MOUSE_BUTTON_LEFT))
		{
			glm::vec3 norm = glm::vec3(0.0f);
			if (m_pThePhysics->GetKickNorm(m_pPlayerObject->position, forwardVector, m_KICKREACH, norm))
			{
				m_pPlayerObject->velocity += norm * 50.0f;
			}
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


