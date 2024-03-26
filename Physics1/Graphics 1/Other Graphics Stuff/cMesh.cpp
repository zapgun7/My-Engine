#include "cMesh.h"
#include <iostream>			// For KillAllHumans() cout

#ifdef _DEBUG
//#include "cGlobal.h"	//cDebugRenderer* 

#endif


// Constructor: Called on creation   c'tor
cMesh::cMesh()
{
	this->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->eulerOrientation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->setRotationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));
	this->scale = glm::vec3(1.0f, 1.0f, 1.0f);

	this->bIsWireframe = false;
	this->bDoNotLight = false;

	this->bIsVisible = true;

	this->bUseCustomColors = false;
	this->customColorRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	this->transparencyAlpha = 1.0f;
	this->bUseDiscardMaskTex = false;
	this->isSkybox = false;

	this->bUseReflect = false;
	this->bUseRefract = false;

	this->uv_Offset_Scale = glm::vec3(0.0f, 0.0f, 1.0f);
	this->uvOffsetSpeed = glm::vec2(0);

	
	// These all add up to 1.0f
	textureRatios[0] = 1.0f;
	textureRatios[1] = 0.0f;
	textureRatios[2] = 0.0f;
	textureRatios[3] = 0.0f;
	textureRatios[4] = 0.0f;
	textureRatios[5] = 0.0f;
	textureRatios[6] = 0.0f;
	textureRatios[7] = 0.0f;

	for (unsigned int i = 0; i < cMesh::NUM_TEXTURES; i++)
	{
		textureIdx[i] = 0;
	}


}

// Destructor: Called on deletion   d'tor
cMesh::~cMesh()
{

}

void cMesh::setRotationFromEuler(glm::vec3 newEulerAngleXYZ)
{
	eulerOrientation = newEulerAngleXYZ;
	this->m_qOrientation = glm::quat(glm::radians(newEulerAngleXYZ)); // Pass in the radians version
	
}

// void cMesh::setDrawOrientation(const glm::vec3& newOrientation)
// {
// 	this->setRotationFromEuler(newOrientation);
// 	return;
// }

void cMesh::adjustRotationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust)
{
	eulerOrientation += EulerAngleXYZ_Adjust;
	glm::quat qChange = glm::quat(glm::radians(EulerAngleXYZ_Adjust));
	this->m_qOrientation *= qChange;
}

glm::quat cMesh::get_qOrientation(void)
{
	return m_qOrientation;
}

glm::vec3 cMesh::getEulerOrientation(void)
{
	return eulerOrientation;
	// Weird outcomes, so we just return stored euler version
	//return glm::eulerAngles(this->get_qOrientation());
}

void cMesh::setUniformDrawScale(float scale)
{
	this->scale.x = this->scale.y = this->scale.z = scale;
	return;
}

// STARTOF: From: iPhysicsMeshTransformAccess interface
glm::vec3 cMesh::getDrawPosition(void)
{
	return this->drawPosition;
}

void cMesh::setDrawPosition(const glm::vec3& newPosition)
{
	this->drawPosition = newPosition;
	return;
}

glm::vec3 cMesh::getDrawOrientation(void)
{
	//	return this->drawOrientation;

		// Be a little careful because quaterion to Euler can return unexpected things
	return glm::eulerAngles(this->get_qOrientation());
}

glm::vec3 cMesh::getDrawScale(void)
{
	return scale;
}

std::string cMesh::getMeshName(void)
{
	return meshName;
}

void cMesh::setDrawOrientation(const glm::vec3& newOrientation)
{
	//	this->drawOrientation = newOrientation;

	this->setRotationFromEuler(newOrientation);

	return;
}

void cMesh::setDrawOrientation(const glm::quat& newOrientation)
{
	//	this->drawOrientation = newOrientation;

	this->m_qOrientation = newOrientation;
	this->eulerOrientation = glm::degrees(glm::eulerAngles(newOrientation));

	return;
}// ENDOF: iPhysicsMeshTransformAccess interface

void cMesh::setDrawScale(const glm::vec3& newScale)
{
	scale = newScale;
	return;
}


// void cMesh::Update(double deltaTime)
// {
// #ifdef _DEBUG
// //	::g_pDebugRenderer->AddSphere();
// #endif
// 	return;
// }