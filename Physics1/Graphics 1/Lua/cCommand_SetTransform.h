#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
//#include "../Other Graphics Stuff/cMesh.h"
#include "../Physics/sPhysicsProperties.h"

#include "iCommand.h"

enum transformState
{
	NOTDONE,
	ISDONE
};


struct initTransformInfo
{
	initTransformInfo()
		:isTranslate(false)
		,transformData(glm::vec3(0))
		,theObj(nullptr)
	{};

	bool isTranslate;
	glm::vec3 transformData;
	sPhysicsProperties* theObj;
};

class cCommand_SetTransform : public iCommand
{
public:
	cCommand_SetTransform();
	virtual ~cCommand_SetTransform();

	virtual void Initialize(void* initStruct);

	// Call every frame
	virtual bool Update(double deltaTime);
	// Also called every frame. Returns true when done
	virtual bool isDone(void);

	virtual bool PreStart(void);
	virtual bool PostEnd(void);

private:

	sPhysicsProperties* m_pThePhysObj;

	glm::vec3 m_TransformData;
	bool m_bIsTranslate;


	transformState m_state;

};
