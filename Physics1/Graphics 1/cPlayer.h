#pragma once



// #include <glm/glm.hpp>
// #include <glm/vec3.hpp>
// #include <vector>
// //#include "Physics/cPhysics.h"
#include "Physics/sPhysicsProperties.h"
#include "Other Graphics Stuff/OpenGLCommon.h"
#include "cInputHandler.h"

#include "cEngineController.h"

class cPhysics;
enum eCameraType
{
	FIRSTPERSON,
	THIRDPERSON,
	FLYCAM
};


class cPlayer
{
public:
	cPlayer(GLFWwindow* window);
	~cPlayer();

	void setPlayerObject(sPhysicsProperties* theObj);

	void Update(double deltaTime, glm::vec3& cameraPosition, glm::quat& cameraRotation); // Updates these based on inputs in the last frame

	

private:
	eCameraType m_CameraType = FLYCAM;

	sPhysicsProperties* m_pPlayerObject = nullptr;

	cPhysics* m_pThePhysics; // ONLY FOR TESTS OKAY?????


	cEngineController* m_pEngineController;
	GLFWwindow* m_window; // Reference to the thing we get inputs from, DUHH
	cInputHandler* m_pInput;
	float m_CameraSpeed = 100.0f; // While it's not an actual player
	float m_InverseSensitivity = 10.0f; // Higher = slower turning

	float m_KICKREACH;
	float m_MAXKICKFORCE;
	float m_BuildingKickPower = 0.0f;
	float m_KICKCHARGESPEED = 2.0f; // Scales off delta time (of course) i.e. how long to fully charge?

	float m_MAX_H_SPD = 15.0f;
	float m_PLAYERSPEED = 40.0f;
	float m_AIR_SPD_RED = 0.05f;
	float m_PLAYERJUMPFORCE = 10.0f;
};