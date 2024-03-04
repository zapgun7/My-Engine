#pragma once

#include "Other Graphics Stuff/OpenGLCommon.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

#include "cVAOManager/sModelDrawInfo.h"

#include "cAnimationBuilder.h"


struct sNode;
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
		enum Interpolation2
		{
			SINE,
			QUAD,
			CUBIC,
			QUART,
			QUINT,
			EXPO,
			CIRC,
			BACK,
			ELASTIC,
			BOUNCE
		};



		glm::vec3 deltaValue = glm::vec3(0); // Change in position, orientation, scale, etc.
		double time = 0; // Time since start at this keyframe
		Interpolation interp_func = LINEAR;
		Interpolation2 interp_spec = SINE;
	};

	//sPhysicsProperties* theObj = nullptr;
	sNode* RootNode = nullptr;
	double timeInAnimation = 0;
	int loopCount = 0; // -1 = inf
	unsigned lastKeyFrame = 0; // Most recently passed keyframe

	int finishedVecs = 0; // Bitmask: move scale orient
	const static int BITMASKMAX = 7; // Sum of used bits

	//std::vector<sAnimNode> moveKeyFrames;
	std::vector < std::vector<sAnimNode> > moveKeyFrames;
	//std::vector<sAnimNode> scaleKeyFrames;
	std::vector < std::vector<sAnimNode> > scaleKeyFrames;
	//std::vector<sAnimNode> orientKeyFrames;
	std::vector < std::vector<sAnimNode> > orientKeyFrames;
	
};

class cAnimationManager
{
public:
	static cAnimationManager* GetInstance(void);
	~cAnimationManager();

	void Update(double dt);
	bool Initialize(void);
	bool Destroy(void);

	void AddAnimationObj(sPhysicsProperties* theObj);
	
	// FOR THE PROJECT
	void setTimescale(float newTS);
	void toggleRunning(void);

	void CalculateMatrices(sModelDrawInfo* model, sAnimInfo* animation, sNode* node, const glm::mat4& parentTransformationMatrix, double keyFrameTime);


private:
	std::vector<sAnimInfo*> m_Animations; // The animations to update
	cAnimationBuilder* m_pAnimBuilder;


	bool m_bIsRunning = true;
	float m_TimeScale = 1.0f;


	cAnimationManager();
	static cAnimationManager* m_TheOneManager;
};