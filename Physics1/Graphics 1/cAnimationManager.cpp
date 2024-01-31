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


