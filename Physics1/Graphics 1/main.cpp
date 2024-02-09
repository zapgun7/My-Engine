//#include "Other Graphics Stuff/cGraphicsMain.h"
//#include "Physics/cPhysics.h"
//#include "cSoundManager.h"
#include "cEngineController.h"
#include <iostream>
#include <time.h>

// Test Includes
// #include "Physics/cAABB.h"
// #include "Physics/sTriangle.h"
// #include "cVAOManager/cVAOManager.h"

//void testArea();

int main(int argc, char** argv)
{
// 	testArea();
// 	return 0;
	srand(time(NULL));

	cEngineController* engineFace = cEngineController::GetEngineController();
	if (!engineFace->Initialize())
	{
		std::cout << "Something went wrong in the initialization";
		return 0;
	}
	engineFace->Run();
	engineFace->Destroy();




	return 0;
}


// void testArea()
// {
// 
// // 	cAABB* testAABB = new cAABB();
// // 	std::vector<sTriangle_A> testVec;
// // 	std::vector<sTriangle_A> testVecSend;
// // 
// // 	glm::vec3 boxMin(-2);
// // 	glm::vec3 boxMax(2);
// // 
// // 	sTriangle_A tempTri;
// // 	tempTri.vertices[0] = glm::vec3(0, -1, 4);
// // 	tempTri.vertices[1] = glm::vec3(0, -4, 1);
// // 	tempTri.vertices[2] = glm::vec3(0, -4, 4);
// // 	testVecSend.push_back(tempTri);
// // 
// // 	testVec = testAABB->trisInBox(&testVecSend, boxMin, boxMax);
// // 
// // 	std::cout << testVec.size() << std::endl;
// 
// 
// 
// 	//testVec = testAABB->trisInBox()
// 
// 
// 	return;
// }