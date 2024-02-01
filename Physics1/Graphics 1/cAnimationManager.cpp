#include "cAnimationManager.h"

#include "Physics/sPhysicsProperties.h"



cAnimationManager* cAnimationManager::m_TheOneManager = NULL;


void GetSurroundingKF(const std::vector<sAnimInfo::sAnimNode>& keyframeVec, int& outIDX, double currTime);
void GetDeltaLinear(const sAnimInfo::sAnimNode* keyFrames, const double& dt, glm::vec3& theDelta);





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

		double moveDT = dt;

		while (moveDT != 0)
		{
			int prevKF;
			GetSurroundingKF(currAnim->moveKeyFrames, prevKF, currAnim->timeInAnimation + (dt - moveDT));

			
			if (prevKF >= 0)
			{
				glm::vec3 deltaMove;
				double timeAfterUpdate = currAnim->timeInAnimation + dt;

				if (timeAfterUpdate > currAnim->moveKeyFrames[prevKF + 1].time) // If this dt update will go past the next keyframe
				{
					GetDeltaLinear(&currAnim->moveKeyFrames[prevKF], currAnim->moveKeyFrames[prevKF + 1].time - currAnim->timeInAnimation, deltaMove);
					moveDT -= currAnim->moveKeyFrames[prevKF + 1].time - currAnim->timeInAnimation;
				}
				else
				{
					GetDeltaLinear(&currAnim->moveKeyFrames[prevKF], moveDT, deltaMove);
					moveDT = 0;
				}

				currAnim->theObj->position += deltaMove;
			}
			else // This should trigger the loop
			{
				// 2 keyframes not found
				moveDT = 0;
				break;
				//currAnim->timeInAnimation = 0;

				// TEMP FOR TESTING LOOPING  /// This should be at the end, after confirming completion of all 3 KF vecs
				if (currAnim->timeInAnimation > currAnim->moveKeyFrames[currAnim->moveKeyFrames.size() - 1].time)
				{
					currAnim->timeInAnimation -= currAnim->moveKeyFrames[currAnim->moveKeyFrames.size() - 1].time;
					currAnim->loopCount--;
				}
			}

		}

		


		// !!!!! Should only increment loop when all 3 keyframe vectors have been exhausted!!!!!


		// This has to be at the end
		currAnim->timeInAnimation += dt;
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
	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.interp_func = sAnimInfo::sAnimNode::LINEAR;
	newNode.time = 0;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(10, 0, 0);
	newNode.time = 1;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, 10, 0);
	newNode.time = 2;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(-10, 0, 0);
	newNode.time = 3;
	newAnime->moveKeyFrames.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, -10, 0);
	newNode.time = 4;
	newAnime->moveKeyFrames.push_back(newNode);

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
