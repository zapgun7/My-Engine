#include "cAnimationManager.h"

#include "Physics/sPhysicsProperties.h"

cAnimationManager* cAnimationManager::m_TheOneManager = NULL;


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

		currAnim->timeInAnimation += dt;

		// Now handle translating, orienting, and scaling

		// Start be checking if we're past or equal to the last keyframe
		if (currAnim->timeInAnimation >= currAnim->moveKeyFrames.end()->time)
		{
			currAnim->loopCount--;
			//currAnim->theObj->position
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
}





//////////////////////// HELPER FUNCTIONS ///////////////////////////
/////////////////////////////////////////////////////////////////////

// Find delta value between last and current update

// Pass in a vec of size 2: previous and next keyframes
// Also pass in the two points in time we'll traverse between
void GetDeltaLinear(const std::vector<sAnimInfo::sAnimNode*>& keyFrames, const double& oldT, const double& dt, glm::vec3& theDelta)
{
	double timeBetweenKF = keyFrames[1]->time - keyFrames[0]->time;

	double deltaRatio = dt / timeBetweenKF;

	theDelta = (float)deltaRatio * keyFrames[1]->deltaValue;

	return;
}
