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
//#include "cVAOManager/cVAOManager.h"

#include "cAnimationBuilder.h"


struct sNode;
class sPhysicsProperties; // This will be the medium of updating position, orientation, etc.
class cVAOManager;



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
		sAnimNode() { };
		sAnimNode(glm::vec3 val, double tm) : value(val), time(tm) { };

		glm::vec3 value = glm::vec3(0); // Change in position, orientation, scale, etc.
		double time = 0; // Time since start at this keyframe
		Interpolation interp_func = LINEAR;
		Interpolation2 interp_spec = SINE;
	};


	//sPhysicsProperties* theObj = nullptr;
	std::string name;


	sNode* RootNode = nullptr;
	double timeInAnimation = 0;
	int loopCount = 0; // -1 = inf

	int finishedVecs = 0; // Bitmask: move scale orient
	const static int BITMASKMAX = 7; // Sum of used bits

	std::vector<sAnimNode> mveKeyFrames;
	std::vector < std::vector<sAnimNode> > moveKeyFrames;
	std::vector<sAnimNode> sclKeyFrames;
	std::vector < std::vector<sAnimNode> > scaleKeyFrames;
	std::vector<sAnimNode> oriKeyFrames;
	std::vector < std::vector<sAnimNode> > orientKeyFrames;
	
};

struct sBonedAnimation
{
	std::string name;

	std::vector<sAnimInfo*> BoneAnimations;
	sModelDrawInfo theModel;
	std::map<sNode*, sAnimInfo*> map_NodeToAnimationData;
	std::map<std::string, sAnimInfo*> map_NameToAnimationData;

	sNode* rootNode;
	double TicksPerSecond;
	double Duration;
	double TimeSoFar = 0.0f;

	sAnimInfo* FindNodeData(std::string name)
	{
		std::map<std::string, sAnimInfo*>::iterator it_NodeData = map_NameToAnimationData.find(name);
		if (it_NodeData != map_NameToAnimationData.end())
			return it_NodeData->second;
		else
			return nullptr;
	}
};

class cAnimationManager
{
public:
	static cAnimationManager* GetInstance(void);
	~cAnimationManager();

	void SetVAOManager(cVAOManager* theMan);

	void Update(double dt);
	bool Initialize(void);
	bool Destroy(void);

	void AddAnimationObj(sPhysicsProperties* theObj);

	void AddBonedAnimation(sBonedAnimation* newAnimation, std::string name);
	
	// FOR THE PROJECT
	void setTimescale(float newTS);
	void toggleRunning(void);

	void CalculateMatrices(/*sModelDrawInfo* model, */sBonedAnimation* animation, sNode* node, const glm::mat4& parentTransformationMatrix, double keyFrameTime);


private:
	std::vector<sAnimInfo*> m_Animations; // The animations to update
	
	cAnimationBuilder* m_pAnimBuilder;

	// Boned animation stuff
	std::vector<sBonedAnimation*> m_BonedAnimations;
	std::map<std::string, sBonedAnimation*> map_NameToAnimation;

	glm::vec3 GetAnimationPosition(std::vector<sAnimInfo::sAnimNode>& data, double keyFrameTime);


	bool m_bIsRunning = true;
	float m_TimeScale = 1.0f;

	cVAOManager* m_pVAOManager;

	cAnimationManager();
	static cAnimationManager* m_TheOneManager;
};