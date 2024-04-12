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
struct sPhysicsProperties; // This will be the medium of updating position, orientation, etc.
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

	struct sKeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};
	struct sKeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};
	struct sKeyScale
	{
		glm::vec3 scale;
		float timeStamp;
	};


	//sPhysicsProperties* theObj = nullptr;
	std::string name;


	sNode* RootNode = nullptr;
	double timeInAnimation = 0;
	int loopCount = 0; // -1 = inf

	int finishedVecs = 0; // Bitmask: move scale orient
	const static int BITMASKMAX = 7; // Sum of used bits

	std::vector<sKeyPosition> m_Positions;
	std::vector<sKeyRotation> m_Orientations;
	std::vector<sKeyScale> m_Scales;

	int posSize;
	int oriSize;
	int sclSize;



	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < posSize - 1; ++index)
		{
			if (animationTime < m_Positions[index + 1].timeStamp)
				return index;
		}
		return 0;
	}
	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < oriSize - 1; ++index)
		{
			if (animationTime < m_Orientations[index + 1].timeStamp)
				return index;
		}
		return 0;
	}
	int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < sclSize - 1; ++index)
		{
			if (animationTime < m_Scales[index + 1].timeStamp)
				return index;
		}
		return 0;
	}
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	glm::mat4 InterpolatePosition(float animationTime)
	{
		if (1 == posSize)
			return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
										   m_Positions[p1Index].timeStamp, 
										   animationTime);
		glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position,
										   m_Positions[p1Index].position, 
										   scaleFactor);
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 InterpolateScaling(float animationTime)
	{
		if (1 == sclSize)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
			m_Scales[p1Index].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
			, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}
	glm::mat4 InterpolateRotation(float animationTime)
	{
		if (1 == oriSize)
		{
			//auto rotation = glm::normalize(m_Orientations[0].orientation);
			glm::quat rotation = glm::normalize(m_Orientations[0].orientation);
			//return glm::toMat4(rotation);
			return glm::mat4(rotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Orientations[p0Index].timeStamp,
										   m_Orientations[p1Index].timeStamp, 
										   animationTime);
		glm::quat finalRotation = glm::slerp(m_Orientations[p0Index].orientation,
											 m_Orientations[p1Index].orientation,
											 scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		//return glm::toMat4(finalRotation);
		return glm::mat4(finalRotation);
	}


	//std::vector<sAnimNode> mveKeyFrames;
	//std::vector < std::vector<sAnimNode> > moveKeyFrames;
	//std::vector<sAnimNode> sclKeyFrames;
	//std::vector < std::vector<sAnimNode> > scaleKeyFrames;
	//std::vector<sAnimNode> oriKeyFrames;
	//std::vector < std::vector<sAnimNode> > orientKeyFrames;
	
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

	void SetActiveAnimation(unsigned int newIDX)
	{
		m_BonedAnimations[animIDX]->TimeSoFar = 0;
		animIDX = newIDX;
	}

private:
	std::vector<sAnimInfo*> m_Animations; // The animations to update
	
	cAnimationBuilder* m_pAnimBuilder;

	// Boned animation stuff
	std::vector<sBonedAnimation*> m_BonedAnimations;
	std::map<std::string, sBonedAnimation*> map_NameToAnimation;

	glm::vec3 GetAnimationPosition(std::vector<sAnimInfo::sAnimNode>& data, double keyFrameTime);


	bool m_bIsRunning = true;
	float m_TimeScale = 1.0f;

	unsigned int animIDX = 0; // FOR THE ANIMATION PROJECT

	cVAOManager* m_pVAOManager;

	cAnimationManager();
	static cAnimationManager* m_TheOneManager;
};