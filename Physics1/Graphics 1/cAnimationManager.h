#pragma once

#include "Other Graphics Stuff/OpenGLCommon.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <vector>



class sPhysicsProperties; // This will be the medium of updating position, orientation, etc.


struct sAnimInfo // Individual instances of animation setups
{
	struct sAnimNode // Will have an array of these; details on each keyframe
	{
		enum Interpolation
		{
			LINEAR,
			EASEIN,
			EASEOUT,
			EASEINOUT
		};



		glm::vec3 deltaValue; // Change in position, orientation, scale, etc.
		double time; // Time since start at this keyframe
		Interpolation interp_func;
	};

	sPhysicsProperties* theObj = nullptr;
	double timeInAnimation = 0;
	int loopCount = 0; // -1 = inf
	unsigned lastKeyFrame = 0; // Most recently passed keyframe

	std::vector<sAnimNode> moveKeyFrames;
	std::vector<sAnimNode> orientKeyFrames;
	std::vector<sAnimNode> scaleKeyFrames;
};

class cAnimationManager
{
public:
	static cAnimationManager* GetInstance(void);
	~cAnimationManager();

	void Update(double dt);
	bool Initialize(void);
	bool Destroy(void);
	


private:
	std::vector<sAnimInfo*> m_Animations; // The animations to update


	cAnimationManager();
	static cAnimationManager* m_TheOneManager;
};