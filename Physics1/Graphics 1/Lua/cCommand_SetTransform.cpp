#include "cCommand_SetTransform.h"
#include <iostream>

cCommand_SetTransform::cCommand_SetTransform()
{

}

cCommand_SetTransform::~cCommand_SetTransform()
{

}


void cCommand_SetTransform::Initialize(void* initStruct)
{
	initTransformInfo* info = (initTransformInfo*)initStruct;

	m_pThePhysObj = info->theObj;
	m_bIsTranslate = info->isTranslate;
	m_TransformData = info->transformData;

	m_state = NOTDONE;

}

bool cCommand_SetTransform::Update(double deltaTime)
{
	if (m_bIsTranslate)
	{
		m_pThePhysObj->position = m_TransformData;
	}
	else
	{
		m_pThePhysObj->setRotationFromEuler(m_TransformData);
	}



	m_state = ISDONE;

	return this->isDone();
}

// Also called every frame. Returns true when done
bool cCommand_SetTransform::isDone(void)
{
	if (m_state == ISDONE)
		return true;
	else
		return false;
}

bool cCommand_SetTransform::PreStart(void)
{
	

	return false;
}

bool cCommand_SetTransform::PostEnd(void)
{

	return false;
}

