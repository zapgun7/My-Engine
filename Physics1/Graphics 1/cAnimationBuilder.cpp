#include "cAnimationBuilder.h"
#include "cAnimationManager.h" // for the sAnimInfo

#include "Other Graphics Stuff/OpenGLCommon.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp>

#include <vector>


cAnimationBuilder::cAnimationBuilder()
{
	// Nothing
}

cAnimationBuilder::~cAnimationBuilder()
{

}

sAnimInfo* cAnimationBuilder::MakeAnimation(AnimType type)
{
	sAnimInfo* newAnim = new sAnimInfo();
	newAnim->loopCount = -1; // Infinitely animate
	sAnimInfo::sAnimNode nodeToAdd, startNode; // Default value is what key frame 0 should be

	switch (type)
	{
	case BOUNCEBALL:
	{
// 		const float BOUNCEHEIGHT = 30;
// 		const float HORIZONTALAMP = 50;
// 		const float SQUISHAMP = 0.8f;
// 		const double TIMEAMP = 4.0; // Time
// 
// 
// 		// Start by setting up first placeholder key frame
// 		std::vector< sAnimInfo::sAnimNode > moveVec;
// 		std::vector< sAnimInfo::sAnimNode > scaleVec;
// 		std::vector< sAnimInfo::sAnimNode > oriVec;
// 		moveVec.push_back(startNode);
// 		scaleVec.push_back(startNode);
// 		oriVec.push_back(startNode);
// 
// 
// 		// Start with movement: horizontal move vec and vertical move vec
// 		
// 		// LINEAR HORIZONTAL
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::LINEAR;
// 		nodeToAdd.time = TIMEAMP;
// 		nodeToAdd.value = glm::vec3(-HORIZONTALAMP, 0, 0); // Move right
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 2;
// 		nodeToAdd.value = glm::vec3(HORIZONTALAMP, 0, 0); // Move left
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 3;
// 		nodeToAdd.value = glm::vec3(HORIZONTALAMP, 0, 0); // Move left
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 4;
// 		nodeToAdd.value = glm::vec3(-HORIZONTALAMP, 0, 0); // Move right
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 5;
// 		nodeToAdd.value = glm::vec3(-HORIZONTALAMP, 0, 0); // Move right
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 6;
// 		nodeToAdd.value = glm::vec3(HORIZONTALAMP, 0, 0); // Move left
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 7;
// 		nodeToAdd.value = glm::vec3(HORIZONTALAMP, 0, 0); // Move left
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 8;
// 		nodeToAdd.value = glm::vec3(-HORIZONTALAMP, 0, 0); // Move right
// 		moveVec.push_back(nodeToAdd);
// 		
// 
// 		// HORIZONTAL PUSH
// 		newAnim->moveKeyFrames.push_back(moveVec);
// 
// 
// 		moveVec.clear();
// 		moveVec.push_back(startNode);
// 
// 		// LINEAR VERTICAL
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::LINEAR;
// 		nodeToAdd.time = TIMEAMP * 0.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0); // Down
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0); // Up
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 1.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0); // Down
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 2;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0); // Up
// 		moveVec.push_back(nodeToAdd);
// 
// 		// EASE IN
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEIN;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 2.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::SINE;
// 
// 		nodeToAdd.time = TIMEAMP * 3;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 3.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::SINE;
// 
// 		nodeToAdd.time = TIMEAMP * 4;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 
// 		// EASE OUT
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::SINE;
// 
// 		nodeToAdd.time = TIMEAMP * 4.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 5;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::SINE;
// 
// 		nodeToAdd.time = TIMEAMP * 5.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 6;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		
// 		// EASE IN OUT
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEINOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 6.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		//nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 7;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		//nodeToAdd.interp_spec = sAnimInfo::sAnimNode::SINE;
// 
// 		nodeToAdd.time = TIMEAMP * 7.5;
// 		nodeToAdd.value = glm::vec3(0, -BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 		//nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 8;
// 		nodeToAdd.value = glm::vec3(0, BOUNCEHEIGHT, 0);
// 		moveVec.push_back(nodeToAdd);
// 
// 
// 		// VERTICAL PUSH
// 		newAnim->moveKeyFrames.push_back(moveVec);
// 
// 
// 
// 		// Now scaling
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::LINEAR;
// 		nodeToAdd.time = TIMEAMP / 2;
// 		nodeToAdd.value = glm::vec3(0, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP;
// 		nodeToAdd.value = glm::vec3(-SQUISHAMP, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 1.5;
// 		nodeToAdd.value = glm::vec3(SQUISHAMP, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 2;
// 		nodeToAdd.value = glm::vec3(0, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEIN;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::EXPO;
// 
// 		nodeToAdd.time = TIMEAMP * 2.5;
// 		nodeToAdd.value = glm::vec3(0, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUART;
// 
// 		nodeToAdd.time = TIMEAMP * 3;
// 		nodeToAdd.value = glm::vec3(-SQUISHAMP, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 3.5;
// 		nodeToAdd.value = glm::vec3(SQUISHAMP, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		/*nodeToAdd.interp_spec = sAnimInfo::sAnimNode::BACK;*/
// 
// 		nodeToAdd.time = TIMEAMP * 4;
// 		nodeToAdd.value = glm::vec3(0, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 
// 		nodeToAdd.time = TIMEAMP * 4.5;
// 		nodeToAdd.value = glm::vec3(0, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 5;
// 		nodeToAdd.value = glm::vec3(-SQUISHAMP, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 5.5;
// 		nodeToAdd.value = glm::vec3(SQUISHAMP, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 6;
// 		nodeToAdd.value = glm::vec3(0, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEINOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::ELASTIC;
// 
// 		nodeToAdd.time = TIMEAMP * 6.5;
// 		nodeToAdd.value = glm::vec3(0, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 7;
// 		nodeToAdd.value = glm::vec3(-SQUISHAMP, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 7.5;
// 		nodeToAdd.value = glm::vec3(SQUISHAMP, -SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.time = TIMEAMP * 8;
// 		nodeToAdd.value = glm::vec3(0, SQUISHAMP, 0);
// 		scaleVec.push_back(nodeToAdd);
// 
// 
// 		newAnim->scaleKeyFrames.push_back(scaleVec);
// 

		return newAnim;
	}
	case FLYSHIP:
	{
// 		const float TIMEAMP = 2.0f;
// 		const float MOVEAMP = 50.0f;
// 
// 
// 
// 
// 		// Start by setting up first placeholder key frame
// 		std::vector< sAnimInfo::sAnimNode > moveVec;
// 		std::vector< sAnimInfo::sAnimNode > scaleVec;
// 		std::vector< sAnimInfo::sAnimNode > oriVec;
// 		moveVec.push_back(startNode);
// 		scaleVec.push_back(startNode);
// 		oriVec.push_back(startNode);
// 
// 
// 		// Will have two move vecs, one for x and one for z 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::LINEAR;
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 2;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 3;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 4;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEIN;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 5;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 6;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 7;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 8;
// 		moveVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 9;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 10;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 11;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 12;
// 		moveVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEINOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 13;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 14;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 15;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(-MOVEAMP, 0, 0);
// 		nodeToAdd.time = TIMEAMP * 16;
// 		moveVec.push_back(nodeToAdd);
// 
// 
// 
// 
// 		newAnim->moveKeyFrames.push_back(moveVec);
// 
// 
// 
// 		moveVec.clear();
// 		moveVec.push_back(startNode);
// 
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::LINEAR;
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 2;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 3;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 4;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEIN;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 5;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 6;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 7;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 8;
// 		moveVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 9;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 10;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 11;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 12;
// 		moveVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEINOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 13;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, -MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 14;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 15;
// 		moveVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, 0, MOVEAMP);
// 		nodeToAdd.time = TIMEAMP * 16;
// 		moveVec.push_back(nodeToAdd);
// 
// 		newAnim->moveKeyFrames.push_back(moveVec);
// 
// 
// 
// 
// 		// ORIENTATION
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::LINEAR;
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 2;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 3;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 4;
// 		oriVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEIN;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::CUBIC;
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 5;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 6;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 7;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 8;
// 		oriVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::CUBIC;
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 9;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 10;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 11;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 12;
// 		oriVec.push_back(nodeToAdd);
// 
// 
// 		nodeToAdd.interp_func = sAnimInfo::sAnimNode::EASEINOUT;
// 		nodeToAdd.interp_spec = sAnimInfo::sAnimNode::QUAD;
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 13;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 14;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 15;
// 		oriVec.push_back(nodeToAdd);
// 
// 		nodeToAdd.value = glm::vec3(0, -90, 0);
// 		nodeToAdd.time = TIMEAMP * 16;
// 		oriVec.push_back(nodeToAdd);
// 
// 
// 		newAnim->orientKeyFrames.push_back(oriVec);
// 
// 

		return newAnim;
	}
	}

	// Invalid AnimType
	delete newAnim;
	return nullptr;
}

