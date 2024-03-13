#include "cPhysics.h"
#include <iostream>


cPhysics* cPhysics::m_pTheOnePhysics = nullptr;

DWORD WINAPI ThreadedIntersectionLoop(LPVOID lpParamater)
{
	sThreadCollisionInfo* myInfo = (sThreadCollisionInfo*)lpParamater;
	float t = 0;
	//float soonestT = FLT_MAX;
	glm::vec3 hn = glm::vec3(0.0f);
	//glm::vec3 soonestHN = hn;


	while (true) // Just loop forever for now, maybe add some bool to check later
	{
// 		if (myInfo->hasWork) // Has work? do work
// 		{

		for (unsigned int triIDX = 0; triIDX < myInfo->arraySize; triIDX++)
		{
			//if (!myInfo->thePhysics->m_TestMovingSphereTriangle(myInfo->theShape, &(myInfo->theTriangles[triIDX]), t, hn))
			if (!cPhysics::m_TestMovingSphereTriangle(myInfo->theShape, &(myInfo->theTriangles[triIDX]), t, hn))
			{
				continue;
			}
			else
			{
				if (t < myInfo->soonestHit)
				{
					myInfo->soonestHit = t;
					myInfo->hn = hn;
				}
			}
		}
		if (myInfo->soonestHit <= 1.0f)
		{
			myInfo->thePhysics->UpdateCollision(myInfo->soonestHit, myInfo->hn);
		}
		myInfo->hasWork = false;
		myInfo->soonestHit = FLT_MAX;
			

		while(!myInfo->hasWork)
		{
			Sleep(5);
			//SleepEx(1, myInfo->hasWork);
		}
	}
}



cPhysics::cPhysics()
{

}

int cPhysics::Initialize(void)
{
	this->m_pTheSoonestCollision = new sPossibleCollision();
	this->m_pReversedObject = new sPhysicsProperties();
	InitializeCriticalSection(&(this->m_CollisionUpdate));

	// Generate Threads and thread info
	//sPhysicsProperties* theShape = nullptr;
	//sPhysicsProperties** theShapePtr = new sPhysicsProperties*[1];


	this->m_ThreadInfos = new sThreadCollisionInfo[NUM_THREADS];
	this->m_ThreadHandles = new HANDLE[NUM_THREADS];
	this->m_ThreadIDs = new DWORD[NUM_THREADS];

	for (unsigned int threadIDX = 0; threadIDX < NUM_THREADS; threadIDX++)
	{
		this->m_ThreadInfos[threadIDX].thePhysics = this->m_pTheOnePhysics;
		this->m_ThreadInfos[threadIDX].theShape = nullptr;

		void* pParams = (void*)(&(this->m_ThreadInfos[threadIDX]));

		this->m_ThreadHandles[threadIDX] = CreateThread(
			NULL,
			0,
			ThreadedIntersectionLoop,
			pParams,
			0,
			&(this->m_ThreadIDs[threadIDX]));
	}

	return 1;
}

cPhysics* cPhysics::GetInstance(void)
{
	if (cPhysics::m_pTheOnePhysics == nullptr)
	{
		cPhysics::m_pTheOnePhysics = new cPhysics();
		cPhysics::m_pTheOnePhysics->Initialize();
	}
	return cPhysics::m_pTheOnePhysics;
}

cPhysics::~cPhysics()
{
}

void cPhysics::setVAOManager(cVAOManager* pTheMeshManager)
{
	this->m_pMeshManager = pTheMeshManager;


	return;
}

void cPhysics::UpdateCollision(float& t, glm::vec3& hn)
{
	while (!TryEnterCriticalSection(&(this->m_CollisionUpdate)))
	{
		Sleep(0);
	}

	if (t < this->m_pTheSoonestCollision->q)
	{
		this->m_pTheSoonestCollision->q = t;
		this->m_pTheSoonestCollision->hitNorm = hn;
	}

	LeaveCriticalSection(&(this->m_CollisionUpdate));
}


void cPhysics::ToggleThreading(void)
{
	this->m_bUseThreading = this->m_bUseThreading ? false : true;
	std::cout << "Threading: " << this->m_bUseThreading << std::endl;
	return;
}

void cPhysics::generateAABBs(std::vector<std::string> models)
{
	std::cout << "Generating AABBs..." << std::endl;

	for (std::vector<std::string>::iterator itModel = models.begin();
		 itModel != models.end();
		 itModel++)
	{
		cAABB* newAABB = new cAABB();
		newAABB->StartMakeTree(*itModel, m_pMeshManager, 2000);
		m_map_ModelAABBs[*itModel] = newAABB;
	}

}


cSoftBodyVerlet* cPhysics::CreateVerlet(void)
{
	cSoftBodyVerlet* blobSoftBody = new cSoftBodyVerlet();
	blobSoftBody->acceleration = glm::vec3(0.0f, -20.0f, 0.0f);
	//blobSoftBody->tightnessFactor = 0.0005f;
	blobSoftBody->tightnessFactor = 0.01f;
	blobSoftBody->iterations = 1;

	sModelDrawInfo blobBodyObjectDrawingInfo;

	if (m_pMeshManager->FindDrawInfoByModelName("sphere.ply", blobBodyObjectDrawingInfo))
	{
		glm::mat4 matTransform = glm::mat4(1.0f);

		matTransform = glm::translate(matTransform, glm::vec3(0.0f, 5.0f, 0.0f));

		// 		matTransform = glm::rotate(matTransform, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		// 		matTransform = glm::rotate(matTransform, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		matTransform = glm::scale(matTransform, glm::vec3(4.0f)); // 4 radius

		blobSoftBody->CreateSoftBody(blobBodyObjectDrawingInfo, matTransform);

		//newSoftBody->CreateRandomBracing(10, 6.0f, 0.0000001f, 0.5f);
		//blobSoftBody->CreateRandomBracing(80, 7.6f, 0.003f, 1.0f);
		blobSoftBody->CreateRandomBracing(80, 7.6f, 0.005f, 1.0f);


		m_VerletObjs.push_back(blobSoftBody);

		
	}

	// Now to create a platform

	cSoftBodyVerlet* plat1SoftBody = new cSoftBodyVerlet();
	plat1SoftBody->acceleration = glm::vec3(0.0f, -20.0f, 0.0f);
	plat1SoftBody->tightnessFactor = 1.0f;
	plat1SoftBody->iterations = 1;

	sModelDrawInfo plat1BodyObjectDrawingInfo;
	if (m_pMeshManager->FindDrawInfoByModelName("plat.ply", plat1BodyObjectDrawingInfo))
	{
		glm::mat4 matTransform = glm::mat4(1.0f);

		matTransform = glm::translate(matTransform, glm::vec3(0.0f, 10.0f, 0.0f));

		// 		matTransform = glm::rotate(matTransform, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		// 		matTransform = glm::rotate(matTransform, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		matTransform = glm::scale(matTransform, glm::vec3(4.0f)); // Plat, so w/l = 4 

		plat1SoftBody->CreateSoftBody(plat1BodyObjectDrawingInfo, matTransform);

		//newSoftBody->CreateRandomBracing(10, 6.0f, 0.0000001f, 0.5f);
		plat1SoftBody->CreateRandomBracing(10, 8.1f, 1.0f, 0.0f);

		plat1SoftBody->BuildPlatform();


		m_VerletObjs.push_back(plat1SoftBody);

		blobSoftBody->vec_pCollisionObjects.push_back(plat1SoftBody);

		//return plat1SoftBody;
	}

	return blobSoftBody;
	//return plat1SoftBody;


	return nullptr;
}

cAABB* cPhysics::findAABBByModelName(std::string modelName)
{
	std::map< std::string,
		cAABB* >::iterator
		itAABB = this->m_map_ModelAABBs.find(modelName);

	// Find it? 
	if (itAABB == this->m_map_ModelAABBs.end())
	{
		// Nope
		return nullptr;
	}

	return itAABB->second;
}

void cPhysics::AddShape(sPhysicsProperties* pNewShape)
{
	this->m_vec_pPhysicalProps.push_back(pNewShape);

	// Does this have an associated mesh?
	if (pNewShape->pTheAssociatedMesh)
	{
		// Update the drawing positions from the physics information
		pNewShape->pTheAssociatedMesh->setDrawPosition(pNewShape->position);
		//pNewShape->pTheAssociatedMesh->setDrawOrientation(pNewShape->orientation);
		pNewShape->pTheAssociatedMesh->setDrawOrientation(pNewShape->get_qOrientation());
	}

	return;
}

void cPhysics::DeleteShape(int shapeID)
{
	for (unsigned int i = 0; i < m_vec_pPhysicalProps.size(); i++)
	{
		if (m_vec_pPhysicalProps[i]->getUniqueID() == shapeID)
		{
			delete m_vec_pPhysicalProps[i]; // Prob breaks it
			m_vec_pPhysicalProps.erase(m_vec_pPhysicalProps.begin() + i);
			return;
		}
	}
}

std::vector< sPhysicsProperties* > cPhysics::getPhysicsVec(void)
{
	return m_vec_pPhysicalProps;
}

bool cPhysics::GetKickNorm(glm::vec3 pos, glm::vec3 lookDir, float length, glm::vec3& retNorm)
{

	// m_ClosestPtLineSegTriangle()

	sPossibleCollision theCollision;
	bool didCollide = false;

	for (sPhysicsProperties* currObj : m_vec_pPhysicalProps) // Iterate through all mesh_indirects
	{
		sPossibleCollision newCollision;
		if (currObj->shapeType != sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT) continue;

		// This is a tri-mesh indirect!
		if (this->m_LineSegment_TriMeshIndirect_IntersectionTest(pos, lookDir, length, currObj, newCollision))
		{
			if (newCollision.q < theCollision.q)
				theCollision = newCollision;
			didCollide = true;
		}
	}


	if (didCollide)
	{
		retNorm = theCollision.hitNorm;
		return 1;
	}

	return 0;

}

void cPhysics::getPhysObj(int objID, sPhysicsProperties* theObj)
{
	for (unsigned int i = 0; i < m_vec_pPhysicalProps.size(); i++)
	{
		if (m_vec_pPhysicalProps[i]->getUniqueID() == objID)
		{
			theObj = m_vec_pPhysicalProps[i];
			return;
		}
	}
}

// void cPhysics::setGraphics(void)
// {
// 	m_pGraphicsMain = cGraphicsMain::getGraphicsMain();
// }

void cPhysics::setShapePos(glm::vec3 newPos, unsigned int ID)
{
	for (sPhysicsProperties* physObj : m_vec_pPhysicalProps)
	{
		if (physObj->getUniqueID() == ID)
		{
			physObj->position = newPos;
			physObj->oldPosition = newPos; // So no wacky physics stuff happens
			return;
		}
	}
}

void cPhysics::setShapeOri(glm::vec3 newOri, unsigned int ID) // Passed in ori is in euler
{
	for (sPhysicsProperties* physObj : m_vec_pPhysicalProps)
	{
		if (physObj->getUniqueID() == ID)
		{
			physObj->setRotationFromEuler(newOri);
			return;
		}
	}
}

void cPhysics::setShapePori(glm::vec3 newPos, glm::vec3 newOri, unsigned int ID)
{
	for (sPhysicsProperties* physObj : m_vec_pPhysicalProps)
	{
		if (physObj->getUniqueID() == ID)
		{
			physObj->setRotationFromEuler(newOri);
			physObj->setPosition(newPos);
			return;
		}
	}
}

void cPhysics::setPhysicsRunningState(bool isRunning)
{
	m_IsRunning = isRunning;
	return;
}

void cPhysics::deleteAllObjects(void)
{
	while(m_vec_pPhysicalProps.size() > 0)
	{
		delete m_vec_pPhysicalProps[0];
		m_vec_pPhysicalProps.erase(m_vec_pPhysicalProps.begin());
	}
}


void cPhysics::switchScenes(std::vector<sPhysicsProperties*> newPhysVec)
{
	deleteAllObjects();
	m_vec_pPhysicalProps = newPhysVec;
	return;
}

bool cPhysics::m_CheckExistingCollision(sPhysicsProperties* objB, sPhysicsProperties* objA)
{
	for (unsigned int i = 0; i < m_vecCollisionsThisFrame.size(); i++)
	{
		if ((m_vecCollisionsThisFrame[i].pObjectA == objA) && (m_vecCollisionsThisFrame[i].pObjectB == objB))
		{
			return true;
		}
	}
	return false;
}

sPhysicsProperties* cPhysics::findShapeByUniqueID(unsigned int uniqueIDtoFind)
{
	for (sPhysicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
	{
		if ( pCurrentShape->getUniqueID() == uniqueIDtoFind )
		{
			// Found it
			return pCurrentShape;
		}
	}
	// Didn't find it
	return NULL;
}

sPhysicsProperties* cPhysics::findShapeByFriendlyName(std::string friendlyNameToFind)
{
	for (sPhysicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
	{
		if (pCurrentShape->friendlyName == friendlyNameToFind)
		{
			// Found it
			return pCurrentShape;
		}
	}
	// Didn't find it
	return NULL;
}

// Vector is empty if none found
std::vector<sPhysicsProperties*> cPhysics::findShapesByType(sPhysicsProperties::eShape shapeType)
{
	std::vector<sPhysicsProperties*> vecShapesFound;
	for (sPhysicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
	{
		if (pCurrentShape->shapeType == shapeType)
		{
			vecShapesFound.push_back(pCurrentShape);
		}
	}
	return vecShapesFound;
}

// Returns false if none found
bool cPhysics::findShapesByType(sPhysicsProperties::eShape shapeType, std::vector<sPhysicsProperties*>& vecShapes)
{
	for (sPhysicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
	{
		if (pCurrentShape->shapeType == shapeType)
		{
			vecShapes.push_back(pCurrentShape);
		}
	}
	return ( ! vecShapes.empty() );
}



