#pragma once



// #include <glm/glm.hpp>
// #include <glm/vec3.hpp>
// #include <vector>
// //#include "Physics/cPhysics.h"
#include "Physics/sPhsyicsProperties.h"
#include "Other Graphics Stuff/OpenGLCommon.h"

//class cPhysics;

class cPlayer
{
public:
	cPlayer();
	~cPlayer();



	std::vector<glm::vec3> Update(double deltaTime, GLFWwindow* theWindow, glm::vec3 currCamPos, glm::vec3 currCamDir, glm::vec3 currCamRot); // Returns updated camera data to be applied thereafter

	void setAssociatedPhysObj(sPhsyicsProperties* theObj);


private:
	sPhsyicsProperties* m_PlayerObject; // The object we'll update velocity to 

	float const m_MoveSpeed = 20.0f;
};