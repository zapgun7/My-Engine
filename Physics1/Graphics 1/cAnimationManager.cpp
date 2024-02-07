#include "cAnimationManager.h"

#include "Physics/sPhysicsProperties.h"





cAnimationManager* cAnimationManager::m_TheOneManager = NULL;


void GetSurroundingKF(const std::vector<sAnimInfo::sAnimNode>& keyframeVec, int& outIDX, double currTime);
void GetDeltaLinear(const sAnimInfo::sAnimNode* keyFrames, const double& dt, glm::vec3& theDelta);
void GetDeltaEaseIn(const sAnimInfo::sAnimNode* keyFrames, const double& oldTime, const double& dt, glm::vec3& theDelta);
void GetDeltaEaseOut(const sAnimInfo::sAnimNode* keyFrames, const double& oldTime, const double& dt, glm::vec3& theDelta);
void GetDeltaEaseInOut(const sAnimInfo::sAnimNode* keyFrames, const double& oldTime, const double& dt, glm::vec3& theDelta);





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
	if (!m_bIsRunning) return;
	dt *= m_TimeScale;


	for (sAnimInfo* currAnim : m_Animations) // Go through all animations
	{
		if (currAnim->loopCount == 0) // Inactive or finished animation
			continue;

		double operationDT = dt;

		glm::vec3 deltaValue; // Reused variable in translate, scale, orient
		double funcDT; // Another reused variable // DT to pass to function to get delta of animation type; changes whether proper dt goes past next keyframe or not
		double timeAfterUpdate = currAnim->timeInAnimation + dt; // Variable used in all 3 sections


		//while (operationDT != 0)
		//{
			//if (currAnim->finishedVecs & 1) break;
		if (~currAnim->finishedVecs & 1)
		{
			unsigned int finishedMoveParts = 0;

			// Get the previous (or current if on it) key frame
			int prevKF;
			for (std::vector<sAnimInfo::sAnimNode> currAnimSegment : currAnim->moveKeyFrames) // Loop through each movement animation
			{
				operationDT = dt;
				while (operationDT != 0)
				{
					//GetSurroundingKF(currAnim->moveKeyFrames, prevKF, currAnim->timeInAnimation + (dt - operationDT));
					GetSurroundingKF(currAnimSegment, prevKF, currAnim->timeInAnimation + (dt - operationDT));


					if (prevKF >= 0)
					{

						if (timeAfterUpdate > /*currAnim->moveKeyFrames*/currAnimSegment[prevKF + 1].time) // If this dt update will go past the next keyframe
						{
							funcDT = /*currAnim->moveKeyFrames*/currAnimSegment[prevKF + 1].time - currAnim->timeInAnimation;
							operationDT -= funcDT;
						}
						else
						{
							funcDT = operationDT;
							operationDT = 0;
						}


						switch (/*currAnim->moveKeyFrames*/currAnimSegment[prevKF + 1].interp_func)
						{
						case sAnimInfo::sAnimNode::LINEAR:
						{
							GetDeltaLinear(&/*currAnim->moveKeyFrames*/currAnimSegment[prevKF],
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEIN:
						{
							GetDeltaEaseIn(&/*currAnim->moveKeyFrames*/currAnimSegment[prevKF],
								currAnim->timeInAnimation,
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEOUT:
						{
							GetDeltaEaseOut(&/*currAnim->moveKeyFrames*/currAnimSegment[prevKF],
								currAnim->timeInAnimation,
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEINOUT:
						{
							GetDeltaEaseInOut(&/*currAnim->moveKeyFrames*/currAnimSegment[prevKF],
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
						//currAnim->finishedVecs += 1;
						operationDT = 0;
						finishedMoveParts++;
						break;
					}
				}
			}

			if (finishedMoveParts == currAnim->moveKeyFrames.size()) // If all move parts have finished, we can skip this whole section in future updates
			{
				currAnim->finishedVecs += 1;
			}
		} // if (currAnim->finishedVecs & 1) break;
		//}

		//operationDT = dt; // Set variable for scaling

		// 		while (operationDT != 0)
		// 		{
		if (~currAnim->finishedVecs & 2)
		{

			unsigned int finishedScaleParts = 0;

			int prevKF;
			for (std::vector<sAnimInfo::sAnimNode> currAnimSegment : currAnim->scaleKeyFrames)
			{
				operationDT = dt;
				while (operationDT != 0)
				{
					//GetSurroundingKF(currAnim->scaleKeyFrames, prevKF, currAnim->timeInAnimation + (dt - operationDT));
					GetSurroundingKF(currAnimSegment, prevKF, currAnim->timeInAnimation + (dt - operationDT));


					if (prevKF >= 0)
					{

						if (timeAfterUpdate > /*currAnim->scaleKeyFrames*/currAnimSegment[prevKF + 1].time) // If this dt update will go past the next keyframe
						{
							funcDT = /*currAnim->scaleKeyFrames*/currAnimSegment[prevKF + 1].time - currAnim->timeInAnimation;
							operationDT -= funcDT;
						}
						else
						{
							funcDT = operationDT;
							operationDT = 0;
						}

						switch (/*currAnim->scaleKeyFrames*/currAnimSegment[prevKF + 1].interp_func)
						{
						case sAnimInfo::sAnimNode::LINEAR:
						{
							GetDeltaLinear(&/*currAnim->scaleKeyFrames*/currAnimSegment[prevKF],
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEIN:
						{
							GetDeltaEaseIn(&/*currAnim->scaleKeyFrames*/currAnimSegment[prevKF],
								currAnim->timeInAnimation,
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEOUT:
						{
							GetDeltaEaseOut(&/*currAnim->scaleKeyFrames*/currAnimSegment[prevKF],
								currAnim->timeInAnimation,
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEINOUT:
						{
							GetDeltaEaseInOut(&/*currAnim->scaleKeyFrames*/currAnimSegment[prevKF],
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
						//currAnim->finishedVecs += 2;
						operationDT = 0;
						finishedScaleParts++;
						break;
					}
				}
			}

			if (finishedScaleParts == currAnim->scaleKeyFrames.size())
			{
				currAnim->finishedVecs += 2;
			}
		}
		//}

		//operationDT = dt; // Set variable for orientation

		//while (operationDT != 0)
		//{
			//if (currAnim->finishedVecs & 4) break;
		if (~currAnim->finishedVecs & 4)
		{
			unsigned int finishedOrientParts = 0;


			int prevKF;
			for (std::vector<sAnimInfo::sAnimNode> currAnimSegment : currAnim->orientKeyFrames)
			{
				operationDT = dt;
				while (operationDT != 0)
				{

					//GetSurroundingKF(currAnim->orientKeyFrames, prevKF, currAnim->timeInAnimation + (dt - operationDT));
					GetSurroundingKF(currAnimSegment, prevKF, currAnim->timeInAnimation + (dt - operationDT));


					if (prevKF >= 0)
					{

						if (timeAfterUpdate > /*currAnim->orientKeyFrames*/currAnimSegment[prevKF + 1].time) // If this dt update will go past the next keyframe
						{
							funcDT = /*currAnim->orientKeyFrames*/currAnimSegment[prevKF + 1].time - currAnim->timeInAnimation;
							operationDT -= funcDT;
						}
						else
						{
							funcDT = operationDT;
							operationDT = 0;
						}

						switch (/*currAnim->orientKeyFrames*/currAnimSegment[prevKF + 1].interp_func)
						{
						case sAnimInfo::sAnimNode::LINEAR:
						{
							GetDeltaLinear(&/*currAnim->orientKeyFrames*/currAnimSegment[prevKF],
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEIN:
						{
							GetDeltaEaseIn(&/*currAnim->orientKeyFrames*/currAnimSegment[prevKF],
								currAnim->timeInAnimation,
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEOUT:
						{
							GetDeltaEaseOut(&/*currAnim->orientKeyFrames*/currAnimSegment[prevKF],
								currAnim->timeInAnimation,
								funcDT,
								deltaValue);
							break;
						}
						case sAnimInfo::sAnimNode::EASEINOUT:
						{
							GetDeltaEaseInOut(&/*currAnim->orientKeyFrames*/currAnimSegment[prevKF],
								currAnim->timeInAnimation,
								funcDT,
								deltaValue);
							break;
						}
						}

						//currAnim->theObj->scale += deltaScale;
						glm::quat rotAdjust = glm::quat(glm::radians(deltaValue));
						currAnim->theObj->setRotationFromQuat(currAnim->theObj->get_qOrientation() * rotAdjust);
					}
					else // This should trigger the loop
					{
						//currAnim->finishedVecs += 4;
						operationDT = 0;
						finishedOrientParts++;
						break;
					}
				}
			}

			if (finishedOrientParts == currAnim->orientKeyFrames.size())
			{
				currAnim->finishedVecs += 4;
			}
		}
		//}

		

		//printf("X: %f, Y: %f, Z: %f\n", currAnim->theObj->scale.x, currAnim->theObj->scale.y, currAnim->theObj->scale.z);
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
	m_pAnimBuilder = new cAnimationBuilder();


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
	// Lazy setup for animation project

	if (theObj->getUniqueID() == 2) // ball
	{
		sAnimInfo* newAnim = m_pAnimBuilder->MakeAnimation(BOUNCEBALL);
		newAnim->theObj = theObj;
		m_Animations.push_back(newAnim);
		return;
	}
	else if (theObj->getUniqueID() == 3) // ship
	{
		sAnimInfo* newAnim = m_pAnimBuilder->MakeAnimation(FLYSHIP);
		newAnim->theObj = theObj;
		m_Animations.push_back(newAnim);
		return;
	}

	return;





	// Everything below is old testing, might use later :)

	sAnimInfo* newAnime = new sAnimInfo();

	newAnime->theObj = theObj;
	newAnime->loopCount = 1;
	
	std::vector<sAnimInfo::sAnimNode> vecToAdd;
	sAnimInfo::sAnimNode newNode;

	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.interp_func = sAnimInfo::sAnimNode::EASEINOUT;
	newNode.interp_spec = sAnimInfo::sAnimNode::SINE;
	newNode.time = 0;

	vecToAdd.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, 5, 0);
	newNode.time = 3;
	vecToAdd.push_back(newNode);

	newNode.deltaValue = glm::vec3(0, -5, 0);
	newNode.time = 6;
	vecToAdd.push_back(newNode);

// 	newNode.deltaValue = glm::vec3(-20, 0, 0);
// 	newNode.time = 6;
// 	vecToAdd.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(0, -20, 0);
// 	newNode.time = 8;
// 	vecToAdd.push_back(newNode);

	newAnime->moveKeyFrames.push_back(vecToAdd);


	vecToAdd.clear();
// 
// 	newNode.deltaValue = glm::vec3(0, 0, 0);
// 	newNode.time = 0;
// 
// 	vecToAdd.push_back(newNode);
// 
// 
// 	newNode.interp_func = sAnimInfo::sAnimNode::EASEOUT;
// 	newNode.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 	newNode.deltaValue = glm::vec3(0, 30, 0);
// 	newNode.time = 0.5;
// 	vecToAdd.push_back(newNode);
// 
// 	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;
// 	newNode.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 	newNode.deltaValue = glm::vec3(0, -30, 0);
// 	newNode.time = 1;
// 	vecToAdd.push_back(newNode);
// 
// 	newAnime->moveKeyFrames.push_back(vecToAdd);




	// SCALE NODES

	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.time = 0;
	vecToAdd.push_back(newNode);


	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;
	newNode.interp_spec = sAnimInfo::sAnimNode::BACK;

	newNode.deltaValue = glm::vec3(0, 1.5, 0);
	newNode.time = 2;
	vecToAdd.push_back(newNode);

	newNode.interp_spec = sAnimInfo::sAnimNode::QUINT;
	newNode.interp_func = sAnimInfo::sAnimNode::EASEOUT;
	newNode.deltaValue = glm::vec3(0, .5, 0);
	newNode.time = 3;
	vecToAdd.push_back(newNode);

	newNode.interp_func = sAnimInfo::sAnimNode::EASEOUT;
	newNode.interp_spec = sAnimInfo::sAnimNode::ELASTIC;
	newNode.deltaValue = glm::vec3(0, -2.5, 0);
	newNode.time = 4.5;
	vecToAdd.push_back(newNode);

	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;
	newNode.interp_spec = sAnimInfo::sAnimNode::BACK;
	newNode.deltaValue = glm::vec3(0, .5, 0);
	newNode.time = 6;
	vecToAdd.push_back(newNode);

	newAnime->scaleKeyFrames.push_back(vecToAdd);


	vecToAdd.clear();


	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.time = 0;
	vecToAdd.push_back(newNode);


	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;
	newNode.interp_spec = sAnimInfo::sAnimNode::BACK;

	newNode.deltaValue = glm::vec3(-.25, 0, -.25);
	newNode.time = 2;
	vecToAdd.push_back(newNode);


	newNode.interp_spec = sAnimInfo::sAnimNode::QUINT;
	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;
	newNode.deltaValue = glm::vec3(-.05, 0, -.05);
	newNode.time = 2.3;
	vecToAdd.push_back(newNode);


	newNode.deltaValue = glm::vec3(-.05, 0, -.05);
	newNode.time = 2.6;
	vecToAdd.push_back(newNode);


	newNode.deltaValue = glm::vec3(-.05, 0, -.05);
	newNode.time = 2.9;
	vecToAdd.push_back(newNode);






	newNode.interp_spec = sAnimInfo::sAnimNode::QUINT;
	newNode.interp_func = sAnimInfo::sAnimNode::EASEOUT;
	newNode.deltaValue = glm::vec3(-.15, 0, -.15);
	newNode.time = 3;
	vecToAdd.push_back(newNode);

	newNode.interp_func = sAnimInfo::sAnimNode::EASEOUT;
	newNode.interp_spec = sAnimInfo::sAnimNode::ELASTIC;
	newNode.deltaValue = glm::vec3(1.45, 0, 1.45);
	newNode.time = 4.5;
	vecToAdd.push_back(newNode);

	newNode.interp_func = sAnimInfo::sAnimNode::EASEIN;
	newNode.interp_spec = sAnimInfo::sAnimNode::BACK;
	newNode.deltaValue = glm::vec3(-1, 0, -1);
	newNode.time = 6;
	vecToAdd.push_back(newNode);

	newAnime->scaleKeyFrames.push_back(vecToAdd);

// 	newNode.interp_func = sAnimInfo::sAnimNode::EASEINOUT;
// 
// 	newNode.deltaValue = glm::vec3(0, 0, 0);
// 	newNode.time = 0;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(4, 4, 4);
// 	newNode.time = 2;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(-4, -4, -4);
// 	newNode.time = 4;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(4, 4, 4);
// 	newNode.time = 6;
// 	newAnime->scaleKeyFrames.push_back(newNode);
// 
// 	newNode.deltaValue = glm::vec3(-4, -4, -4);
// 	newNode.time = 8;
// 	newAnime->scaleKeyFrames.push_back(newNode);



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


	vecToAdd.clear();


	// ORIENTATION 
	newNode.interp_func = sAnimInfo::sAnimNode::EASEINOUT;


	newNode.deltaValue = glm::vec3(0, 0, 0);
	newNode.time = 0;
	vecToAdd.push_back(newNode);

	newNode.interp_func = sAnimInfo::sAnimNode::LINEAR;
	newNode.deltaValue = glm::vec3(0, 360, 0);
	newNode.time = 6;
	vecToAdd.push_back(newNode);
	//newAnime->orientKeyFrames.push_back(vecToAdd);

// 	newNode.deltaValue = glm::vec3(0, 360, 0);
// 	newNode.time = 2.5;
	//newAnime->orientKeyFrames.push_back(newNode);

// 	newNode.deltaValue = glm::vec3(0, -360, 0);
// 	newNode.time = 5.0;
	//newAnime->orientKeyFrames.push_back(newNode);


	this->m_Animations.push_back(newAnime);
}

void cAnimationManager::setTimescale(float newTS)
{
	m_TimeScale = newTS;
	return;
}

void cAnimationManager::toggleRunning(void)
{
	if (m_bIsRunning) m_bIsRunning = false;
	else m_bIsRunning = true;

	return;
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
	}
	case sAnimInfo::sAnimNode::QUAD:
	{
		oldRatio = glm::pow(oldRatio, 2);
		newRatio = glm::pow(newRatio, 2);
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::CUBIC:
	{
		oldRatio = glm::pow(oldRatio, 3);
		newRatio = glm::pow(newRatio, 3);
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUART:
	{
		oldRatio = glm::pow(oldRatio, 4); 
		newRatio = glm::pow(newRatio, 4);
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUINT:
	{
		oldRatio = glm::pow(oldRatio, 5);
		newRatio = glm::pow(newRatio, 5);
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::EXPO:
	{
		oldRatio = oldRatio ==  0 ? 0 : glm::pow(2, 10 * oldRatio - 10);
		newRatio = newRatio == 0 ? 0 : glm::pow(2, 10 * newRatio - 10);
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::CIRC:
	{
		oldRatio = 1 - sqrt(1 - glm::pow(oldRatio, 2));
		newRatio = 1 - sqrt(1 - glm::pow(newRatio, 2));
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::BACK:
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1;

		oldRatio = c3 * oldRatio * oldRatio * oldRatio - c1 * oldRatio * oldRatio;
		newRatio = c3 * newRatio * newRatio * newRatio - c1 * newRatio * newRatio;
		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::ELASTIC:
	{
		const float c4 = (2 * glm::pi<float>() / 3);

		oldRatio = oldRatio == 0 
			? 0 
			: oldRatio == 1
			? 1
			: -glm::pow(2, 10 * oldRatio - 10) * sin((oldRatio * 10 - 10.75) * c4);
		newRatio = newRatio == 0
			? 0
			: newRatio == 1
			? 1
			: -glm::pow(2, 10 * newRatio - 10) * sin((newRatio * 10 - 10.75) * c4);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::BOUNCE:
	{
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		oldRatio = 1 - oldRatio;
		newRatio = 1 - newRatio;

		if (oldRatio < 1 / d1) {
			oldRatio =  n1 * oldRatio * oldRatio;
		}
		else if (oldRatio < 2 / d1) {
			oldRatio = n1 * (oldRatio -= 1.5 / d1) * oldRatio + 0.75;
		}
		else if (oldRatio < 2.5 / d1) {
			oldRatio = n1 * (oldRatio -= 2.25 / d1) * oldRatio + 0.9375;
		}
		else {
			oldRatio = n1 * (oldRatio -= 2.625 / d1) * oldRatio + 0.984375;
		}

		if (newRatio < 1 / d1) {
			newRatio = n1 * newRatio * newRatio;
		}
		else if (newRatio < 2 / d1) {
			newRatio = n1 * (newRatio -= 1.5 / d1) * newRatio + 0.75;
		}
		else if (newRatio < 2.5 / d1) {
			newRatio = n1 * (newRatio -= 2.25 / d1) * newRatio + 0.9375;
		}
		else {
			newRatio = n1 * (newRatio -= 2.625 / d1) * newRatio + 0.984375;
		}

		oldRatio = 1 - oldRatio;
		newRatio = 1 - newRatio;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	}
}

void GetDeltaEaseOut(const sAnimInfo::sAnimNode* keyFrames, const double& oldTime, const double& dt, glm::vec3& theDelta)
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
		oldRatio = sin((oldRatio * glm::pi<float>()) / 2);
		newRatio = sin((newRatio * glm::pi<float>()) / 2);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUAD:
	{
		oldRatio = 1 - (1 - oldRatio) * (1 - oldRatio);
		newRatio = 1 - (1 - newRatio) * (1 - newRatio);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::CUBIC:
	{
		oldRatio = 1 - glm::pow(1 - oldRatio, 3);
		newRatio = 1 - glm::pow(1 - newRatio, 3);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUART:
	{
		oldRatio = 1 - glm::pow(1 - oldRatio, 4);
		newRatio = 1 - glm::pow(1 - newRatio, 4);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUINT:
	{
		oldRatio = 1 - glm::pow(1 - oldRatio, 5);
		newRatio = 1 - glm::pow(1 - newRatio, 5);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::EXPO:
	{
		oldRatio = oldRatio ==  1 ? 1 : 1 - glm::pow(2, -10 * oldRatio);
		newRatio = newRatio == 1 ? 1 : 1 - glm::pow(2, -10 * newRatio);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::CIRC:
	{
		oldRatio = sqrt(1 - glm::pow(oldRatio - 1, 2));
		newRatio = sqrt(1 - glm::pow(newRatio - 1, 2));

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::BACK:
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1;

		oldRatio = 1 + c3 * glm::pow(oldRatio - 1, 3) + c1 * glm::pow(oldRatio - 1, 2);
		newRatio = 1 + c3 * glm::pow(newRatio - 1, 3) + c1 * glm::pow(newRatio - 1, 2);

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::ELASTIC:
	{
		const float c4 = (2 * glm::pi<float>()) / 3;

		oldRatio = oldRatio ==  0
			? 0
			: oldRatio == 1
			? 1
			: glm::pow(2, -10 * oldRatio) * sin((oldRatio * 10 - 0.75) * c4) + 1;

		newRatio = newRatio == 0
			? 0
			: newRatio == 1
			? 1
			: glm::pow(2, -10 * newRatio) * sin((newRatio * 10 - 0.75) * c4) + 1;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::BOUNCE:
	{
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if (oldRatio < 1 / d1) {
			oldRatio = n1 * oldRatio * oldRatio;
		}
		else if (oldRatio < 2 / d1) {
			oldRatio = n1 * (oldRatio -= 1.5 / d1) * oldRatio + 0.75;
		}
		else if (oldRatio < 2.5 / d1) {
			oldRatio = n1 * (oldRatio -= 2.25 / d1) * oldRatio + 0.9375;
		}
		else {
			oldRatio = n1 * (oldRatio -= 2.625 / d1) * oldRatio + 0.984375;
		}

		if (newRatio < 1 / d1) {
			newRatio = n1 * newRatio * newRatio;
		}
		else if (newRatio < 2 / d1) {
			newRatio = n1 * (newRatio -= 1.5 / d1) * newRatio + 0.75;
		}
		else if (newRatio < 2.5 / d1) {
			newRatio = n1 * (newRatio -= 2.25 / d1) * newRatio + 0.9375;
		}
		else {
			newRatio = n1 * (newRatio -= 2.625 / d1) * newRatio + 0.984375;
		}

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	}
}

void GetDeltaEaseInOut(const sAnimInfo::sAnimNode* keyFrames, const double& oldTime, const double& dt, glm::vec3& theDelta)
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
		oldRatio = -(cos(glm::pi<float>() * oldRatio) - 1) / 2;
		newRatio = -(cos(glm::pi<float>() * newRatio) - 1) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUAD:
	{
		oldRatio = oldRatio < 0.5 ? 2 * oldRatio * oldRatio : 1 - glm::pow(-2 * oldRatio + 2, 2) / 2;
		newRatio = newRatio < 0.5 ? 2 * newRatio * newRatio : 1 - glm::pow(-2 * newRatio + 2, 2) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::CUBIC:
	{
		oldRatio = oldRatio < 0.5 ? 4 * oldRatio * oldRatio * oldRatio : 1 - glm::pow(-2 * oldRatio + 2, 3) / 2;
		newRatio = newRatio < 0.5 ? 4 * newRatio * newRatio * newRatio : 1 - glm::pow(-2 * newRatio + 2, 3) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUART:
	{
		oldRatio = oldRatio < 0.5 ? 8 * oldRatio * oldRatio * oldRatio * oldRatio : 1 - glm::pow(-2 * oldRatio + 2, 4) / 2;
		newRatio = newRatio < 0.5 ? 8 * newRatio * newRatio * newRatio * newRatio : 1 - glm::pow(-2 * newRatio + 2, 4) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::QUINT:
	{
		oldRatio = oldRatio < 0.5 ? 16 * oldRatio * oldRatio * oldRatio * oldRatio * oldRatio : 1 - glm::pow(-2 * oldRatio + 2, 5) / 2;
		newRatio = newRatio < 0.5 ? 16 * newRatio * newRatio * newRatio * newRatio * newRatio : 1 - glm::pow(-2 * newRatio + 2, 5) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::EXPO:
	{
		oldRatio = oldRatio == 0
			? 0
			: oldRatio == 1
			? 1
			: oldRatio < 0.5 ? glm::pow(2, 20 * oldRatio - 10) / 2
			: (2 - glm::pow(2, -20 * oldRatio + 10)) / 2;

		newRatio = newRatio == 0
			? 0
			: newRatio == 1
			? 1
			: newRatio < 0.5 ? glm::pow(2, 20 * newRatio - 10) / 2
			: (2 - glm::pow(2, -20 * newRatio + 10)) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::CIRC:
	{
		oldRatio = oldRatio < 0.5
			? (1 - sqrt(1 - pow(2 * oldRatio, 2))) / 2
			: (sqrt(1 - pow(-2 * oldRatio + 2, 2)) + 1) / 2;

		newRatio = newRatio < 0.5
			? (1 - sqrt(1 - pow(2 * newRatio, 2))) / 2
			: (sqrt(1 - pow(-2 * newRatio + 2, 2)) + 1) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::BACK:
	{
		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		oldRatio = oldRatio < 0.5
			? (glm::pow(2 * oldRatio, 2) * ((c2 + 1) * 2 * oldRatio - c2)) / 2
			: (glm::pow(2 * oldRatio - 2, 2) * ((c2 + 1) * (oldRatio * 2 - 2) + c2) + 2) / 2;

		newRatio = newRatio < 0.5
			? (glm::pow(2 * newRatio, 2) * ((c2 + 1) * 2 * newRatio - c2)) / 2
			: (glm::pow(2 * newRatio - 2, 2) * ((c2 + 1) * (newRatio * 2 - 2) + c2) + 2) / 2;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::ELASTIC:
	{
		const float c5 = (2 * glm::pi<float>()) / 4.5;

		oldRatio = oldRatio == 0
			? 0
			: oldRatio == 1
			? 1
			: oldRatio < 0.5
			? -(glm::pow(2, 20 * oldRatio - 10) * sin((20 * oldRatio - 11.125) * c5)) / 2
			: (glm::pow(2, -20 * oldRatio + 10) * sin((20 * oldRatio - 11.125) * c5)) / 2 + 1;

		newRatio = newRatio == 0
			? 0
			: newRatio == 1
			? 1
			: newRatio < 0.5
			? -(glm::pow(2, 20 * newRatio - 10) * sin((20 * newRatio - 11.125) * c5)) / 2
			: (glm::pow(2, -20 * newRatio + 10) * sin((20 * newRatio - 11.125) * c5)) / 2 + 1;

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	case sAnimInfo::sAnimNode::BOUNCE:
	{
		const float n1 = 7.5625f;
		const float d1 = 2.75f;


		if (oldRatio < 0.5f)
		{
			oldRatio = 1 - 2 * oldRatio;

			if (oldRatio < 1 / d1) {
				oldRatio = n1 * oldRatio * oldRatio;
			}
			else if (oldRatio < 2 / d1) {
				oldRatio = n1 * (oldRatio -= 1.5 / d1) * oldRatio + 0.75;
			}
			else if (oldRatio < 2.5 / d1) {
				oldRatio = n1 * (oldRatio -= 2.25 / d1) * oldRatio + 0.9375;
			}
			else {
				oldRatio = n1 * (oldRatio -= 2.625 / d1) * oldRatio + 0.984375;
			}

			oldRatio = (1 - oldRatio) / 2;
		}
		else
		{
			oldRatio = 2 * oldRatio - 1;

			if (oldRatio < 1 / d1) {
				oldRatio = n1 * oldRatio * oldRatio;
			}
			else if (oldRatio < 2 / d1) {
				oldRatio = n1 * (oldRatio -= 1.5 / d1) * oldRatio + 0.75;
			}
			else if (oldRatio < 2.5 / d1) {
				oldRatio = n1 * (oldRatio -= 2.25 / d1) * oldRatio + 0.9375;
			}
			else {
				oldRatio = n1 * (oldRatio -= 2.625 / d1) * oldRatio + 0.984375;
			}

			oldRatio = (1 + oldRatio) / 2;
		}

		if (newRatio < 0.5f)
		{
			newRatio = 1 - 2 * newRatio;

			if (newRatio < 1 / d1) {
				newRatio = n1 * newRatio * newRatio;
			}
			else if (newRatio < 2 / d1) {
				newRatio = n1 * (newRatio -= 1.5 / d1) * newRatio + 0.75;
			}
			else if (newRatio < 2.5 / d1) {
				newRatio = n1 * (newRatio -= 2.25 / d1) * newRatio + 0.9375;
			}
			else {
				newRatio = n1 * (newRatio -= 2.625 / d1) * newRatio + 0.984375;
			}

			newRatio = (1 - newRatio) / 2;
		}
		else
		{
			newRatio = 2 * newRatio - 1;

			if (newRatio < 1 / d1) {
				newRatio = n1 * newRatio * newRatio;
			}
			else if (newRatio < 2 / d1) {
				newRatio = n1 * (newRatio -= 1.5 / d1) * newRatio + 0.75;
			}
			else if (newRatio < 2.5 / d1) {
				newRatio = n1 * (newRatio -= 2.25 / d1) * newRatio + 0.9375;
			}
			else {
				newRatio = n1 * (newRatio -= 2.625 / d1) * newRatio + 0.984375;
			}

			newRatio = (1 + newRatio) / 2;
		}

		theDelta = keyFrames[1].deltaValue * newRatio - keyFrames[1].deltaValue * oldRatio;

		return;
	}
	}
}
