#include "cPlayer.h"
#include <iostream>


//#include <glm/gtc/constants.hpp>
#include "Physics/cPhysics.h" // For tests
#include "cTimer.h"


cPlayer::cPlayer(GLFWwindow* window)
{
	m_window = window;
	m_pEngineController = cEngineController::GetEngineController();
	m_pInput = cInputHandler::GetInstance();
	m_pThePhysics = cPhysics::GetInstance();
	m_CameraType = FIRSTPERSON;

	m_KICKREACH = 15.0f;
	m_MAXKICKFORCE = 60.0f;
	m_LOOKDIRKICKINFLUENCE = 0.1f; // Scale of how much player's look vec influences the overall norm that is kicked off of
	m_YVELRED = 20.0f;
	m_AIR_SPD_RED = 0.2f;



	m_BuildingKickPower = 0.0f; // Builds as player holds left-click
	m_KICKCHARGESPEED = 4.0f; // Scales off delta time (of course) i.e. how long to fully charge?
	

	m_MAX_GROUNDED_SPD = 10.0f; // Max horizontal speed achieved on foot
	m_SPRINT_H_SPD_INCR = 10.0f; // Additional max horizontal speed achieved on foot while sprinting
	m_PLAYERSPEED = 35.0f; // Player acceleration
	m_PLAYERJUMPFORCE = 10.0f; // Regular jump (SPACE) force


	m_pLuaSoundCall = cSoundLuaBrain::GetInstance();
	//m_pDatabaseManager = cDatabaseManager::GetInstance();

}

cPlayer::~cPlayer()
{

}

void cPlayer::setPlayerObject(sPhysicsProperties* theObj)
{
	m_pPlayerObject = theObj;
	m_pPlayerObject->playerInfo->maxGroundedSpeed = m_MAX_GROUNDED_SPD;
	m_pPlayerObject->playerInfo->sprintSpeedIncrease = m_SPRINT_H_SPD_INCR;

	return;
}

void cPlayer::setPlayerVerlet(cSoftBodyVerlet* theObj)
{
	m_pPlayerVerlet = theObj;
}

void cPlayer::setPlayerNavMesh(cNavMesh* navmesh)
{
	m_pNavMesh = navmesh;
	m_pCurrTri = m_pNavMesh->getClosestTri(m_pPlayerObject->position);
	return;
}

void cPlayer::Update(double deltaTime, glm::vec3& cameraPosition, glm::quat& cameraRotation)
{

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
				deltaY = static_cast<float>(deltaMouseX) / m_InverseSensitivity;

				// For other pitch and roll start by getting signed ratios of the forward vector


				glm::vec3 normXZ = glm::normalize(XZForwardVec);
				float xLook = normXZ.x;
				float zLook = normXZ.z;



				deltaZ = static_cast<float>(deltaMouseY) / m_InverseSensitivity * xLook;
				deltaX = static_cast<float>(-deltaMouseY) / m_InverseSensitivity * zLook;

				glm::quat deltaQuat = glm::quat(glm::radians(glm::vec3(deltaX, deltaY, deltaZ)));

				(cameraRotation) *= deltaQuat;

				glm::vec3 RotVec = glm::vec3(0, 1, 0) * cameraRotation;
				if (RotVec.y <= 0.01)
				{
					// The camera's gonna go wacko fucky. Undo it before it does!
					//deltaZ = -deltaMouseY / m_InverseSensitivity * xLook;
					//deltaX = deltaMouseY / m_InverseSensitivity * zLook;

					glm::quat fixQuat = glm::quat(glm::radians(glm::vec3(-deltaX, 0, -deltaZ)));
					(cameraRotation) *= fixQuat;

				}
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
		if (m_pPlayerObject->position.y < -100.0f)
		{
			m_pPlayerObject->position = glm::vec3(0, 10, -15);
			m_pPlayerObject->oldPosition = m_pPlayerObject->position;
			m_pPlayerObject->velocity = glm::vec3(0.0f);
		}
		// Start by updating the TOPSPD stat
// 		float storedSpd = m_pDatabaseManager->GetTopSpeed();//m_pDatabaseManager->SelectData(0); // Player ID 0
 		float actualSpd = glm::length(m_pPlayerObject->velocity);
// 
// 		if (storedSpd < actualSpd)
// 		{
// 			// update stored data
// 			//m_pDatabaseManager->UpdateData(0, actualSpd);
// 			m_pDatabaseManager->UpdateTopSpeed(actualSpd);
// 		}
// 
// 		// Add button press to retrieve the stored "high score" of the player's speed
// 		if (m_pInput->IsPressedEvent(GLFW_KEY_F))
// 		{
// 			storedSpd = m_pDatabaseManager->GetTopSpeed(); //m_pDatabaseManager->SelectData(0);
// 
// 			//std::cout << "Highest Speed Achieved: " << storedSpd << std::endl;
// 			printf("Highest Speed Achieved: %.3f\n", storedSpd);
// 		}
// 
// 		if (m_pInput->IsPressedEvent(GLFW_KEY_Z))
// 		{
// 			//m_pDatabaseManager->UpdateData(0, 0.0f);
// 			m_pDatabaseManager->ResetData();
// 			std::cout << "Successfully wiped stored speed record" << std::endl;
// 		}
		static bool isNaved = true;

		static sTimer* recalcTriLocationDelay = cTimer::MakeNewTimer(2.0f, sTimer::ONEOFF);

		if (m_pPlayerObject->playerInfo->hasTeleported)
		{
			m_pPlayerObject->playerInfo->hasTeleported = false;
			recalcTriLocationDelay->Reset();
		}

		if (recalcTriLocationDelay->CheckInterval() && (isNaved))
		{
			m_pCurrTri = m_pNavMesh->getClosestTri(m_pPlayerObject->position);
			//printf("FINDING TRI...\n");
		}




// 		static sTimer* triUpdateTimer = cTimer::MakeNewTimer(0.2f, sTimer::eTimerType::REPEAT);
// 
// 		if ((triUpdateTimer->CheckInterval()) && (isNaved))
// 		{
 			m_pCurrTri = m_pNavMesh->getClosestTri(m_pCurrTri, m_pPlayerObject->position);
// 			printf("%d\n", m_pCurrTri->id);
// 		}


		// Setup Area for Lua sound calls
		static bool isGrounded = false;
		static float deltaDistSinceStep = 0.0f;
		static glm::vec3 oldPosXZ = glm::vec3(m_pPlayerObject->position.x, 0, m_pPlayerObject->position.z);


		if (isGrounded)
		{
			if (!m_pPlayerObject->playerInfo->isGrounded)
			{
				isGrounded = false;
			}
			else
			{
				deltaDistSinceStep += glm::distance(oldPosXZ, glm::vec3(m_pPlayerObject->position.x, 0, m_pPlayerObject->position.z));
				if (deltaDistSinceStep > 5.0f + actualSpd / 20.0f)
				{
					deltaDistSinceStep = 0.0f;
					StepSound();
				}
			}
		}
		else
		{
			deltaDistSinceStep = 0.0f;
			if (m_pPlayerObject->playerInfo->isGrounded)
			{
				isGrounded = true;
				// Call step
				StepSound();
			}
		}

		oldPosXZ = glm::vec3(m_pPlayerObject->position.x, 0, m_pPlayerObject->position.z);



		if (m_pInput->IsPressedEvent(GLFW_KEY_TAB))
		{
			m_pPlayerObject->position = glm::vec3(50, 15, 50);
			m_pPlayerObject->velocity = glm::vec3(0);
		}

		glm::vec3 forwardVector = glm::vec3(0, 0, 1) * (cameraRotation);
		glm::vec3 XZForwardVec(forwardVector.x, 0, forwardVector.z);


		m_pInput->ChangeMouseState(HIDDEN);


		double deltaMouseX, deltaMouseY;
		m_pInput->GetMouseDeltas(deltaMouseX, deltaMouseY);


		float deltaX, deltaY, deltaZ;
		deltaY = static_cast<float>(deltaMouseX) / m_InverseSensitivity;


		glm::vec3 normXZ = glm::normalize(XZForwardVec);
		float xLook = normXZ.x;
		float zLook = normXZ.z;



		deltaZ = static_cast<float>(deltaMouseY) / m_InverseSensitivity * xLook;
		deltaX = static_cast<float>(-deltaMouseY) / m_InverseSensitivity * zLook;

		glm::quat deltaQuat = glm::quat(glm::radians(glm::vec3(deltaX, deltaY, deltaZ)));

		(cameraRotation) *= deltaQuat;

		glm::vec3 RotVec = glm::vec3(0, 1, 0) * cameraRotation;
		if (RotVec.y <= 0.01f)
		{

			glm::quat fixQuat = glm::quat(glm::radians(glm::vec3(-deltaX, 0, -deltaZ)));
			(cameraRotation) *= fixQuat;

		}

		//std::cout << "X:" << cameraRotation.x << "  Y:" << cameraRotation.y << "  Z:" << cameraRotation.z << "  W:" << cameraRotation.w << std::endl;
		// TODO maybe. Remake XZ forward after the new one has been established

		//glfwSetCursorPos(m_window, width / 2, height / 2);

	

		float modifiedPlayerSpeed;
		if (m_pPlayerObject->playerInfo->isGrounded) // Is Grounded
		{
			modifiedPlayerSpeed = m_PLAYERSPEED;
		}
		else // Airborne
		{
			modifiedPlayerSpeed = m_PLAYERSPEED * m_AIR_SPD_RED;
		}

		float preVelLen = glm::length(glm::vec3(m_pPlayerObject->velocity.x, 0, m_pPlayerObject->velocity.z));

		bool isSprinting = false;
		float sprintSpdIncr = 0.0f;
		if (m_pInput->IsPressed(GLFW_KEY_LEFT_SHIFT))
		{
			m_pPlayerObject->playerInfo->isSprinting = true;
			isSprinting = true;
			sprintSpdIncr = m_SPRINT_H_SPD_INCR;
		}
		else
		{
			m_pPlayerObject->playerInfo->isSprinting = false;
		}

		unsigned __int8 isInputMove = 0;
		glm::vec3 deltaMove = glm::vec3(0);
		if (m_pInput->IsPressed(GLFW_KEY_W)) // Move forward
		{
			glm::vec3 moveAmt = glm::normalize(XZForwardVec) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			deltaMove += moveAmt;
			isInputMove ^= 1;
		}

		if (m_pInput->IsPressed(GLFW_KEY_S)) // Move backwards
		{
			glm::vec3 moveAmt = glm::normalize(XZForwardVec) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			deltaMove -= moveAmt;
			isInputMove ^= 1;
		}

		if (m_pInput->IsPressed(GLFW_KEY_A)) // Move left
		{
			glm::vec3 moveAmt = glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec)) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			deltaMove += moveAmt;
			isInputMove ^= 2;
		}

		if (m_pInput->IsPressed(GLFW_KEY_D)) // Move right
		{
			glm::vec3 moveAmt = glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec)) * m_PLAYERSPEED * static_cast<float>(deltaTime);
			deltaMove -= moveAmt;
			isInputMove ^= 2;
		}
		m_pPlayerObject->playerInfo->isInputting = isInputMove; // Let physics know if we're moving the player

		if (isInputMove)
		{
			if (m_pPlayerObject->playerInfo->isGrounded)
			{
				// Move player along the plane they're standing on
				glm::vec3 projSubVec = glm::dot(deltaMove, m_pPlayerObject->playerInfo->groundNorm) * m_pPlayerObject->playerInfo->groundNorm;
				deltaMove = (deltaMove - projSubVec);

				// Transfer some of the ond velocity to the new direction
				float transferScale = (glm::length(m_pPlayerObject->velocity)) * static_cast<float>(deltaTime);
				glm::vec3 amtToTransfer = transferScale * m_pPlayerObject->velocity;
				m_pPlayerObject->velocity -= amtToTransfer;
				transferScale = glm::length(amtToTransfer);
				deltaMove += transferScale * normalize(deltaMove);
			}
			else
			{
// 				glm::vec3 playerVelXZ = glm::vec3(m_pPlayerObject->velocity.x, 0.0f, m_pPlayerObject->velocity.z);
// 				float transferScale = (glm::length(playerVelXZ)) * static_cast<float>(deltaTime);
// 				//glm::vec3 amtToTransfer = transferScale * deltaMove;
// 				m_pPlayerObject->velocity -= playerVelXZ * transferScale * m_AIR_SPD_RED;
// 				glm::vec3 mvmtToTransfer = transferScale * deltaMove;

				deltaMove *= m_AIR_SPD_RED;
				//deltaMove += mvmtToTransfer;
			}

			// 		glm::vec3 projSubVec = glm::dot(deltaMove, m_pPlayerObject->playerInfo->groundNorm) * m_pPlayerObject->playerInfo->groundNorm;
			// 		deltaMove = (deltaMove - projSubVec);

			m_pPlayerObject->velocity += deltaMove;
		}



// 		if (m_pPlayerObject->playerInfo->isGrounded)
// 		{
// 			// Move player along the plane they're standing on
// 			glm::vec3 projSubVec = glm::dot(deltaMove, m_pPlayerObject->playerInfo->groundNorm) * m_pPlayerObject->playerInfo->groundNorm;
// 			deltaMove = (deltaMove - projSubVec);
// 
// 			// Transfer some of the onld velocity to the new direction
// 			glm::vec3 playerVelXZ = glm::vec3(m_pPlayerObject->velocity.x, 0.0f, m_pPlayerObject->velocity.z);
// 			float transferScale = (glm::length(playerVelXZ)) * static_cast<float>(deltaTime);
// 			//glm::vec3 amtToTransfer = transferScale * deltaMove;
// 			m_pPlayerObject->velocity -= playerVelXZ * transferScale;
// 			deltaMove += transferScale * deltaMove;
// 		}
		
// 		if (m_pInput->IsMousePressed(GLFW_MOUSE_BUTTON_RIGHT))
// 		{
// 			
// 			if (m_pPlayerObject->velocity.y > 0)
// 			{
// 				float redSpd = m_pPlayerObject->velocity.y * 0.5f * static_cast<float>(deltaTime);
// 				m_pPlayerObject->velocity.y -= redSpd;
// 			}
// 			m_pPlayerObject->velocity.y -= m_YVELRED * static_cast<float>(deltaTime);
// 		}

		float plyrSpd = glm::length(m_pPlayerObject->velocity);
		if (m_pPlayerObject->playerInfo->isGrounded)
		{
			if (plyrSpd > m_MAX_GROUNDED_SPD + sprintSpdIncr) // Simple speed limit TODO smooth this
			{
				m_pPlayerObject->velocity = glm::normalize(m_pPlayerObject->velocity) * (m_MAX_GROUNDED_SPD + sprintSpdIncr);
			}
		}

		
// 		if (plyrSpd > m_MAX_GROUNDED_SPD + sprintSpdIncr)
// 		{
// 			m_pPlayerObject->velocity = glm::normalize(m_pPlayerObject->velocity) * (m_MAX_GROUNDED_SPD + sprintSpdIncr);
// 		}

// 		glm::vec3 XZVel = glm::vec3(m_pPlayerObject->velocity.x, 0, m_pPlayerObject->velocity.z);
// 		float hVel = glm::length(XZVel);
// 		if (hVel > m_MAX_H_SPD + sprintSpdIncr)
// 		{
// 			//m_pPlayerObject->velocity -= deltaMove; // Remove any velocity that was gained
// 			if (hVel > preVelLen) // Only if the player tries to go faster
// 			{
// 				XZVel = glm::normalize(XZVel) * preVelLen;
// 				m_pPlayerObject->velocity.x = XZVel.x;
// 				m_pPlayerObject->velocity.z = XZVel.z;
// 				//m_pPlayerObject->velocity = glm::normalize(m_pPlayerObject->velocity) * preVelLen; // This keeps what direction the player was trying to change to
// 			}
// 		}


		if ((m_pInput->IsPressed(GLFW_KEY_SPACE)) && (m_pPlayerObject->playerInfo->isGrounded)) // Jump    TODO should use isGrounded but breaks when it does
		{
			m_pPlayerObject->velocity += m_pPlayerObject->playerInfo->groundNorm * m_PLAYERJUMPFORCE;// glm::vec3(0, m_PLAYERJUMPFORCE, 0);
			m_pPlayerObject->playerInfo->framesAirborne = 5;
			m_pPlayerObject->playerInfo->isGrounded = false;
		}

// 		static sTimer* testTimer = cTimer::MakeNewTimer(0.5f, sTimer::REPEAT);
// 		static int airborne = 0;
// 		if (!m_pPlayerObject->playerInfo->isGrounded)
// 			airborne++;
// 		if (testTimer->CheckInterval())
// 		{
// 			float pspd = glm::length(m_pPlayerObject->velocity);
// 			printf("//////////////////////////////\nVelocity: %.3f\nisAirborne: %d\n", pspd, airborne);
// 			airborne = 0;
// 		}

		// KICK HANDLING

		//std::cout << "Speed: " << glm::length(m_pPlayerObject->velocity) << std::endl;

// 		if (m_pInput->IsMousePressed(GLFW_MOUSE_BUTTON_LEFT))
// 		{
// 			m_BuildingKickPower += static_cast<float>(deltaTime) / m_KICKCHARGESPEED;
// 			m_BuildingKickPower = m_BuildingKickPower > 1 ? 1 : m_BuildingKickPower; // Cap at 1.0f
// 		}
// 		else if (m_pInput->IsMouseReleasedEvent(GLFW_MOUSE_BUTTON_LEFT))
// 		{
// 			glm::vec3 norm = glm::vec3(0.0f);
// 			if (m_pThePhysics->GetKickNorm(m_pPlayerObject->position, forwardVector, m_KICKREACH, norm))
// 			{
// 				Kick(norm, forwardVector);
// 
// 				// Play sound
// 				m_pLuaSoundCall->RunScriptImmediately("PlaySound(1.0, 'kick')");
// 
// // 				norm = glm::normalize(norm - forwardVector * 0.5f);
// // 				m_pPlayerObject->velocity += norm * m_MAXKICKFORCE * m_BuildingKickPower;
// 			}
// 			m_BuildingKickPower = 0.0f;
// 		}



		// Set camera positon to a little above the capsule midsection
		cameraPosition = m_pPlayerObject->position + glm::vec3(0, 1, 0);
	}
	else if (m_CameraType == THIRDPERSON)
	{
		glm::vec3 forwardVector = glm::vec3(0, 0, 1) * (cameraRotation);
		glm::vec3 XZForwardVec(forwardVector.x, 0, forwardVector.z);


		m_pInput->ChangeMouseState(HIDDEN);


		double deltaMouseX, deltaMouseY;
		m_pInput->GetMouseDeltas(deltaMouseX, deltaMouseY);


		float deltaX, deltaY, deltaZ;
		deltaY = static_cast<float>(deltaMouseX) / m_InverseSensitivity;


		glm::vec3 normXZ = glm::normalize(XZForwardVec);
		float xLook = normXZ.x;
		float zLook = normXZ.z;



		deltaZ = static_cast<float>(deltaMouseY) / m_InverseSensitivity * xLook;
		deltaX = static_cast<float>(-deltaMouseY) / m_InverseSensitivity * zLook;

		glm::quat deltaQuat = glm::quat(glm::radians(glm::vec3(deltaX, deltaY, deltaZ)));

		(cameraRotation) *= deltaQuat;

		glm::vec3 RotVec = glm::vec3(0, 1, 0) * cameraRotation;
		if (RotVec.y <= 0.01f)
		{

			glm::quat fixQuat = glm::quat(glm::radians(glm::vec3(-deltaX, 0, -deltaZ)));
			(cameraRotation) *= fixQuat;

		}

		// Blob Controls

		if (m_pInput->IsPressed(GLFW_KEY_SPACE)) // Jump
		{
			m_pPlayerVerlet->Jump(deltaTime);
		}

		glm::vec3 moveDir = glm::vec3(0.0f);

		if (m_pInput->IsPressed(GLFW_KEY_W)) // Move forward
		{
			moveDir += glm::normalize(XZForwardVec);
			//m_pPlayerVerlet->Move(moveDir, deltaTime);
		}

		if (m_pInput->IsPressed(GLFW_KEY_S)) // Move backwards
		{
			moveDir += -glm::normalize(XZForwardVec);
			//m_pPlayerVerlet->Move(moveDir, deltaTime);
		}

		if (m_pInput->IsPressed(GLFW_KEY_A)) // Move left
		{
			moveDir += glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec));
			//m_pPlayerVerlet->Move(moveDir, deltaTime);
		}

		if (m_pInput->IsPressed(GLFW_KEY_D)) // Move right
		{
			moveDir += -glm::normalize(glm::cross(glm::vec3(0, 1, 0), XZForwardVec));
			//m_pPlayerVerlet->Move(moveDir, deltaTime);
		}

		m_pPlayerVerlet->Move(moveDir, deltaTime);


		// Blob Camera Setting

		const float DIST_TO_BLOB = 40.0f;
		glm::vec3 blobCentre = m_pPlayerVerlet->getCentrePoint();

		cameraPosition = blobCentre - forwardVector * DIST_TO_BLOB;
	}
		

	return;
}

void cPlayer::setCamSpeed(int val)
{
// 	m_CameraSpeed += val;
// 	if (m_CameraSpeed < 0.1f) m_CameraSpeed = 0.1f;
	m_CameraSpeed = val;
	
	return;
}

void* cPlayer::GetCurrTri(void)
{
	return (void*)m_pCurrTri;
}

void cPlayer::Kick(glm::vec3& hitNorm, glm::vec3& lookVec)
{
	// We want the kick to, under certain conditions, redirect some of the velocity the player has, rather than just adding to the overall velocity
	// 90 degree reflection would be the ideal max redirect, decreasing gradient from there. There should be ALMOST no reflection on 180 (so the player can't infinitely pogo)
	///////// STEPS: ///////
	// 1. Reflect Move vec + look vec (reduced weight on look) 
	// 2. Degree diff from move vec and this reflected vector will provide scaling on velocity transference 
	// 3. Max transference will be at 90 deg difference; reducing to smaller (not 0) amount and 180 and 0

	// RANGE: abs(degDiff - pi/2) --> This way we can work with blocks of range rather than two, yucky
	// 
	// 0 - 15   Optimal transference 
	// 15+ - 40 Okay transference     
	// 40+ - 70 Meh transference      
	// 70+ - 90 Minimal transference 

	// For additional speed gained from kicking, it will be some gradient based off of the player's current speed (calc'd after or before velocity redirection?)
	// Higher speed will gain less, lower speed will gain more


// 	hitNorm = glm::normalize(hitNorm - lookVec * 0.5f);
// 	m_pPlayerObject->velocity += hitNorm * m_MAXKICKFORCE * m_BuildingKickPower;

	glm::vec3 finalHitNorm = glm::normalize(hitNorm - lookVec * m_LOOKDIRKICKINFLUENCE);
	glm::vec3 moveVec = glm::normalize(glm::vec3 (m_pPlayerObject->velocity.x, m_pPlayerObject->velocity.y + std::numeric_limits<float>::epsilon(), m_pPlayerObject->velocity.z));

	glm::vec3 reflectedVec = glm::reflect(moveVec, finalHitNorm);
	float degDiff = acos(glm::dot(reflectedVec, moveVec));

	degDiff = degDiff - glm::half_pi<float>();

	float maxRedirectedLen = m_BuildingKickPower * 150.0f;
	float currMoveLen = glm::length(m_pPlayerObject->velocity);

	float redirectedLen = currMoveLen <= maxRedirectedLen ? currMoveLen : maxRedirectedLen;
	glm::vec3 finalVel = m_pPlayerObject->velocity;



	// Project move vec onto norm
	glm::vec3 velLenOnNorm = glm::dot(m_pPlayerObject->velocity, hitNorm) * hitNorm;
	float velLenOnNormLen = glm::length(velLenOnNorm);

	float addVelMod = 100.0f - velLenOnNormLen;
	addVelMod = addVelMod < 5.0f ? 5.0f : addVelMod; 
	addVelMod /= 100.0f;

	float additionalVel = m_BuildingKickPower * m_MAXKICKFORCE * addVelMod;
	glm::vec3 addVelVec = additionalVel * hitNorm;


	const float transferRate1 = 0.99f;
	const float transferRate2 = 0.90f;
	const float transferRate3 = 0.60f;
	const float transferRate4 = 0.30f;

	// This must also somehow scall off of the kick power
	if (degDiff < 0.26178f) // ~15 deg
	{
		finalVel -= moveVec * transferRate1 * redirectedLen;
		finalVel += reflectedVec * transferRate1 * redirectedLen;
		//finalVel += additionalVel * hitNorm;
	}
	else if (degDiff < 0.698133f) // ~40 deg
	{
		finalVel -= moveVec * transferRate2 * redirectedLen;
		finalVel += reflectedVec * transferRate2 * redirectedLen;
		//finalVel += additionalVel * hitNorm;
	}
	else if (degDiff < 1.22174f) // ~70 deg
	{
		finalVel -= moveVec * transferRate3 * redirectedLen;
		finalVel += reflectedVec * transferRate3 * redirectedLen;
		//finalVel += additionalVel * hitNorm;
	}
	else // Everything else
	{
		finalVel -= moveVec * transferRate4 * redirectedLen;
		finalVel += reflectedVec * transferRate4 * redirectedLen;
		//finalVel += additionalVel * hitNorm;
	}

	m_pPlayerObject->velocity.x = finalVel.x;
	m_pPlayerObject->velocity.z = finalVel.z;
	if (m_pPlayerObject->velocity.y < 0)
		m_pPlayerObject->velocity.y = finalVel.y * 0.1;
	else
		m_pPlayerObject->velocity.y = finalVel.y;
	m_pPlayerObject->velocity += addVelVec;

}

void cPlayer::StepSound(void)
{
	float stepPitch = 1.0f;
	float deltaPitch = (rand() % 100) / 1000.0f;
	if (rand() % 2 == 0)
		stepPitch += deltaPitch;
	else
		stepPitch -= deltaPitch;

	std::string commandCall = "PlaySound(" + std::to_string(stepPitch) + ", 'woodstep1')";

	m_pLuaSoundCall->RunScriptImmediately(commandCall.c_str());
	//m_pLuaSoundCall->RunScriptImmediately("PlaySound(true, 'woodstep1')");
	
	return;
}

