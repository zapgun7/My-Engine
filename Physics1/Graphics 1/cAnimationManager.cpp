#include "cAnimationManager.h"

#include "Physics/sPhysicsProperties.h"



cAnimationManager* cAnimationManager::m_TheOneManager = NULL;


void GetDeltaLinear(const std::vector<sAnimInfo::sAnimNode*>& keyFrames, const double& dt, glm::vec3& theDelta);





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
			// Start by getting the surrounding key frames to the position before the update
			std::vector< sAnimInfo::sAnimNode* > theTwoKF;
			theTwoKF.reserve(2);

			for (std::vector<sAnimInfo::sAnimNode>::iterator ITvec = currAnim->moveKeyFrames.begin();
				ITvec != currAnim->moveKeyFrames.end(); ITvec++)
			{
				if (ITvec->time > currAnim->timeInAnimation + (dt - moveDT))
				{
					// Found the next keyframe
					ITvec--;
					theTwoKF.push_back(&*ITvec);
					ITvec++;
					theTwoKF.push_back(&*ITvec);
					break;
				}
			}

			
			if (!theTwoKF.empty())
			{
				glm::vec3 deltaMove;
				double timeAfterUpdate = currAnim->timeInAnimation + dt;

				if (timeAfterUpdate > theTwoKF[1]->time) // If this dt update will go past the next keyframe
				{
					GetDeltaLinear(theTwoKF, theTwoKF[1]->time - currAnim->timeInAnimation, deltaMove);
					moveDT -= theTwoKF[1]->time - currAnim->timeInAnimation;
				}
				else
				{
					GetDeltaLinear(theTwoKF, moveDT, deltaMove);
					moveDT = 0;
				}

				currAnim->theObj->position += deltaMove;
			}
			else // This should trigger the loop
			{
				// 2 keyframes not found
				moveDT = 0;
				//currAnim->timeInAnimation = 0;
			}

		}

		

		// Now handle translating, orienting, and scaling

		// Start by checking if we're past or equal to the last keyframe
// 		if (currAnim->timeInAnimation >= currAnim->moveKeyFrames.end()->time)
// 		{
// 			currAnim->loopCount--;
// 			//currAnim->theObj->position
// 			GetDeltaLinear()
// 		}

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
}





//////////////////////// HELPER FUNCTIONS ///////////////////////////
/////////////////////////////////////////////////////////////////////

// Find delta value between last and current update

// Pass in a vec of size 2: previous and next keyframes
// Also pass in the two points in time we'll traverse between
void GetDeltaLinear(const std::vector<sAnimInfo::sAnimNode*>& keyFrames, const double& dt, glm::vec3& theDelta)
{
	double timeBetweenKF = keyFrames[1]->time - keyFrames[0]->time;

	double deltaRatio = dt / timeBetweenKF;

	theDelta = (float)deltaRatio * keyFrames[1]->deltaValue;

	return;
}
