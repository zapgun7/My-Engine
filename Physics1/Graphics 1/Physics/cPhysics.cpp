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


void cPhysics::AddShape(sPhsyicsProperties* pNewShape)
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

std::vector< sPhsyicsProperties* > cPhysics::getPhysicsVec(void)
{
	return m_vec_pPhysicalProps;
}

void cPhysics::setGraphics(void)
{
	m_pGraphicsMain = cGraphicsMain::getGraphicsMain();
}

void cPhysics::setShapePos(glm::vec3 newPos, unsigned int ID)
{
	for (sPhsyicsProperties* physObj : m_vec_pPhysicalProps)
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
	for (sPhsyicsProperties* physObj : m_vec_pPhysicalProps)
	{
		if (physObj->getUniqueID() == ID)
		{
			physObj->setRotationFromEuler(newOri);
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


bool cPhysics::m_CheckExistingCollision(sPhsyicsProperties* objB, sPhsyicsProperties* objA)
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

sPhsyicsProperties* cPhysics::findShapeByUniqueID(unsigned int uniqueIDtoFind)
{
	for (sPhsyicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
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

sPhsyicsProperties* cPhysics::findShapeByFriendlyName(std::string friendlyNameToFind)
{
	for (sPhsyicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
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
std::vector<sPhsyicsProperties*> cPhysics::findShapesByType(sPhsyicsProperties::eShape shapeType)
{
	std::vector<sPhsyicsProperties*> vecShapesFound;
	for (sPhsyicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
	{
		if (pCurrentShape->shapeType == shapeType)
		{
			vecShapesFound.push_back(pCurrentShape);
		}
	}
	return vecShapesFound;
}

// Returns false if none found
bool cPhysics::findShapesByType(sPhsyicsProperties::eShape shapeType, std::vector<sPhsyicsProperties*>& vecShapes)
{
	for (sPhsyicsProperties* pCurrentShape : this->m_vec_pPhysicalProps)
	{
		if (pCurrentShape->shapeType == shapeType)
		{
			vecShapes.push_back(pCurrentShape);
		}
	}
	return ( ! vecShapes.empty() );
}



