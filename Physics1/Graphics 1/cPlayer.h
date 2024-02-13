#pragma once



// #include <glm/glm.hpp>
// #include <glm/vec3.hpp>
// #include <vector>
// //#include "Physics/cPhysics.h"
#include "Physics/sPhysicsProperties.h"
#include "Other Graphics Stuff/OpenGLCommon.h"

#include "cEngineController.h"

//class cPhysics;
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

	cEngineController* m_pEngineController;
	GLFWwindow* m_window; // Reference to the thing we get inputs from, DUHH
	float m_CameraSpeed = 100.0f; // While it's not an actual player
	float m_InverseSensitivity = 10.0f; // Higher = slower turning


	float m_PLAYERSPEED = 20.0f;
	float m_PLAYERJUMPFORCE = 10.0f;
};