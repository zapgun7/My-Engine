#include "cAnimationManager.h"

#include "Physics/sPhysicsProperties.h"





cAnimationManager* cAnimationManager::m_TheOneManager = NULL;


void GetSurroundingKF(const std::vector<sAnimInfo::sAnimNode>& keyframeVec, int& outIDX, double currTime);
void GetDeltaLinear(const sAnimInfo::sAnimNode* keyFrames, const double& dt, glm::vec3& theDelta);
void GetDeltaEaseIn(const sAnimInfo::sAnimNode* keyFrames, const double& oldTime, const double& dt, glm::vec3& theDelta);





cAnimationManager* cAnimationManager::GetInstance(void)
{
	if (!m_TheOneManager)
	{
		m_TheOneManager = new cAnimationManager();
		if (!m_TheOneManager->Initialize())
		{
			m_TheOneManager->Destroy();
			return nullptr;
		}
	}
	return m_TheOneManager;
}

cAnimationManager::cAnimationManager()
{

}

cAnimationManager::~cAnimationManager()
{

}


// Updates all active animations
void cAnimationManager::Update(double dt)
{
	for (sAnimInfo* currAnim: m_Animations) // Go through all animations
	{
		if (currAnim->loopCount == 0) // Inactive or finished animation
			continue;

		double operationDT = dt;

		glm::vec3 deltaValue; // Reused variable in translate, scale, orient
		double funcDT; // Another reused variable // DT to pass to function to get delta of animation type; changes whether proper dt goes past next keyframe or not
		double timeAfterUpdate = currAnim->timeInAnimation + dt; // Variable used in all 3 sections


		while (operationDT != 0)
		{
			if (currAnim->finishedVecs & 1) break;

			// Get the previous (or current if on it) key frame
			int prevKF;
			GetSurroundingKF(currAnim->moveKeyFrames, prevKF, currAnim->timeInAnimation + (dt - operationDT));

			
			if (prevKF >= 0)
			{

				if (timeAfterUpdate > currAnim->moveKeyFrames[prevKF + 1].time) // If this dt update will go past the next keyframe
				{
					funcDT = currAnim->moveKeyFrames[prevKF + 1].time - currAnim->timeInAnimation;
					operationDT -= funcDT;
				}
				else
				{
					funcDT = operationDT;
					operationDT = 0;
				}


				switch (currAnim->moveKeyFrames[prevKF + 1].interp_func)
				{
				case sAnimInfo::sAnimNode::LINEAR:
				{
					GetDeltaLinear(&currAnim->moveKeyFrames[prevKF],
						funcDT,
						deltaValue);
					break;
				}
				case sAnimInfo::sAnimNode::EASEIN:
				{
					GetDeltaEaseIn(&currAnim->moveKeyFrames[prevKF],
						currAnim->timeInAnimation,
						funcDT,
						deltaValue);
					break;
				}
				}

				currAnim->theObj->position += deltaValue;
			}
			else // This should trigger the loop
			{
				// 2 keyframes not found
				currAnim->finishedVecs += 1;
				operationDT = 0;
				break;
			}

		}

		operationDT = dt; // Set variable for scaling

		while (operationDT != 0)
		{
			if (currAnim->finishedVecs & 2) break;

			int prevKF;
			GetSurroundingKF(currAnim->scaleKeyFrames, prevKF, currAnim->timeInAnimation + (dt - operationDT));


			if (prevKF >= 0)
			{

				if (timeAfterUpdate > currAnim->scaleKeyFrames[prevKF + 1].time) // If this dt update will go past the next keyframe
				{
					funcDT = currAnim->scaleKeyFrames[prevKF + 1].time - currAnim->timeInAnimation;
					operationDT -= funcDT;
				}
				else
				{
					funcDT = operationDT;
					operationDT = 0;
				}

				switch (currAnim->scaleKeyFrames[prevKF + 1].interp_func)
				{
				case sAnimInfo::sAnimNode::LINEAR:
				{
					GetDeltaLinear(&currAnim->scaleKeyFrames[prevKF],
						funcDT,
						deltaValue);
					break;
				}
				case sAnimInfo::sAnimNode::EASEIN:
				{
					GetDeltaEaseIn(&currAnim->scaleKeyFrames[prevKF],
						currAnim->timeInAnimation,
						funcDT,
						deltaValue);
					break;
				}
				}

				currAnim->theObj->scale += deltaValue;
			}
			else // This should trigger the loop
			{
				currAnim->finishedVecs += 2;
				operationDT = 0;
				break;
			}
		}

		operationDT = dt; // Set variable for scaling

		while (operationDT != 0)
		{
			if (currAnim->finishedVecs & 4) break;

			int prevKF;
			GetSurroundingKF(currAnim->orientKeyFrames, prevKF, currAnim->timeInAnimation + (dt - operationDT));


			if (prevKF >= 0)
			{

				if (timeAfterUpdate > currAnim->orientKeyFrames[prevKF + 1].time) // If this dt update will go past the next keyframe
				{
					funcDT = currAnim->orientKeyFrames[prevKF + 1].time - currAnim->timeInAnimation;
					operationDT -= funcDT;
				}
				else
				{
					funcDT = operationDT;
					operationDT = 0;
				}

				switch (currAnim->orientKeyFrames[prevKF + 1].interp_func)
				{
				case sAnimInfo::sAnimNode::LINEAR:
				{
					GetDeltaLinear(&currAnim->orientKeyFrames[prevKF],
						funcDT,
						deltaValue);
					break;
				}
				case sAnimInfo::sAnimNode::EASEIN:
				{
					GetDeltaEaseIn(&currAnim->orientKeyFrames[prevKF],
						currAnim->timeInAnimation,
						funcDT,
						deltaValue);
					break;
				}
				}

				//currAnim->theObj->scale += deltaScale;
				glm::quat rotAdjust = glm::quat(glm::radians(deltaValue));
				currAnim->theObj->setRotationFromQuat(currAnim->theObj->get_qOrientation()* rotAdjust);
			}
			else // This should trigger the loop
			{
				currAnim->finishedVecs += 4;
				operationDT = 0;
				break;
			}
		}

		


		// !!!!! Should only increment loop when all 3 keyframe vectors have been exhausted!!!!!

		// This has to be at the end
		currAnim->timeInAnimation += dt;

		if (currAnim->finishedVecs == sAnimInfo::BITMASKMAX)
		{
			currAnim->timeInAnimation = 0;
			currAnim->loopCount--;
			currAnim->finishedVecs = 0;
		}
	}
}

bool cAnimationManager::Initialize(void)
{
	


	return true;
}

bool cAnimationManager::Destroy(void)
{
	if (!m_TheOneManager) // No instance exists, cannot destroy
		return false;



	return true;
}





void cAnimationManager::AddAnimationObj(sPhysicsProperties* theObj)
{
	sAnimInfo* newAnime = new sAnimInfo();

	newAnime->theObj = theObj;
	newAnime->loopCount = 3;
	
	sAnimInfo::sAnimNode newNode;
// 	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;
	newNode.time = 0;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(20, 0, 0);
	newNode.time = 2;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, 20, 0);
	newNode.time = 4;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(-20, 0, 0);
	newNode.time = 6;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, -20, 0);
	newNode.time = 8;
	newAnime->moveKeyFrames.push_back(newNode);


	// SCALE NODES
	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;

	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.time = 0;
	newAnime->scaleKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(4, 4, 4);
	newNode.time = 3;
	newAnime->scaleKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(-4, -4, -4);
	newNode.time = 6;
	newAnime->scaleKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(4, 4, 4);
	newNode.time = 9;
	newAnime->scaleKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(-4, -4, -4);
	newNode.time = 12;
	newAnime->scaleKeyFrames.push_back(newNode);



// 	newNode.deltaValue = glm::vec3(0, 1.0f, 0);
// 	newNode.time = .5;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, -2.0f, 0);
// 	newNode.time = 1;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, 2.0f, 0);
// 	newNode.time = 1.5;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, -2.0f, 0);
// 	newNode.time = 2;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, 2.0f, 0);
// 	newNode.time = 2.5;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, -2.0f, 0);
// 	newNode.time = 3;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, 2.0f, 0);
// 	newNode.time = 3.5;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, -1.0f, 0);
// 	newNode.time = 4;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(2, 0, 2);
// 	newNode.time = 5;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(-2, 0, -2);
// 	newNode.time = 6;
// 	newAnime->scaleKeyFrames.push_back(newNode);



	// ORIENTATION 
	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;


	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.time = 0;
	newAnime->orientKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, 360, 0);
	newNode.time = 2.5;
	newAnime->orientKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, -360, 0);
	newNode.time = 5.0;
	newAnime->orientKeyFrames.push_back(newNode);


	this->m_Animations.push_back(newAnime);
}

//////////////////////// HELPER FUNCTIONS ///////////////////////////
/////////////////////////////////////////////////////////////////////

// Find delta value between last and current update


void GetSurroundingKF(const std::vector<sAnimInfo::sAnimNode>& keyframeVec, int& outIDX/*std::vector<sAnimInfo::sAnimNode*>& outVec*/, double currTime)
{
	for (unsigned int i = 0; i < keyframeVec.size(); i++)
	{
		if (keyframeVec[i].time > currTime)
		{
// 			outVec->push_back(&(keyframeVec[i - 1]));
// 			outVec.push_back();
			outIDX = i - 1;

			return;
		}
	}
	outIDX = -1; // Did not find
	return;
}

// Pass in a vec of size 2: previous and next keyframes
// Also pass in the two points in time we'll traverse between
void GetDeltaLinear(const sAnimInfo::sAnimNode* keyFrames/*const std::vector<sAnimInfo::sAnimNode*>& keyFrames*/, const double& dt, glm::vec3& theDelta)
{
	double timeBetweenKF = keyFrames[1].time - keyFrames[0].time;

	double deltaRatio = dt / timeBetweenKF;

	theDelta = (float)deltaRatio * keyFrames[1].deltaValue;

	return;
}

void GetDeltaEaseIn(const sAnimInfo::sAnimNode* keyFrames, const double& oldTime, const double& dt, glm::vec3& theDelta)
{
	// Time between the 2 key frames
	double deltaFrameTime = keyFrames[1].time - keyFrames[0].time;

	// Time from the previous frame
	double frameTime = oldTime - keyFrames[0].time; 


	float oldRatio = frameTime / deltaFrameTime;
	float newRatio = (frameTime + dt) / deltaFrameTime;

	// Now we calculate the value at old and new, and set the difference to theDelta
	switch (keyFrames[1].interp_spec)
	{
	case sAnimInfo::sAnimNode::SINE:
	{
		oldRatio = 1 - cos((oldRatio * glm::pi<float>()) / 2);
		newRatio = 1 - cos((newRatio * glm::pi<float>()) / 2);
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
		break; // Prob don't need this break
	}
	}
}
