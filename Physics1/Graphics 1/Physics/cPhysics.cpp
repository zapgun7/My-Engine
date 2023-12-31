#include "cPhysics.h"


cPhysics::cPhysics()
{
	
}

cPhysics::~cPhysics()
{
}

void cPhysics::setVAOManager(cVAOManager* pTheMeshManager)
{
	this->m_pMeshManager = pTheMeshManager;
	return;
}


void cPhysics::generateAABBs(std::vector<std::string> models)
{
	// Temp test area
	// float m_ClosestPtSegmentSegment(glm::vec3 p1, glm::vec3 q1, glm::vec3 p2, glm::vec3 q2, float& s, float& t, glm::vec3& c1, glm::vec3& c2)

	float returnVal, s, t;
	glm::vec3 c1, c2;

	glm::vec3 p1(0, -1, 0);
	glm::vec3 q1(0, 1, 0);

	glm::vec3 p2(-1, 0, 1);
	glm::vec3 q2(1, 0, 1);

	returnVal = m_ClosestPtSegmentSegment(p1, q1, p2, q2, s, t, c1, c2);


	printf("Distance %.3f", returnVal);


	for (std::vector<std::string>::iterator itModel = models.begin();
		 itModel != models.end();
		 itModel++)
	{
		cAABB* newAABB = new cAABB();
		newAABB->StartMakeTree(*itModel, m_pMeshManager, 25);
		m_map_ModelAABBs[*itModel] = newAABB;
	}
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



