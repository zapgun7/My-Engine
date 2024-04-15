#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "../Physics/iPhysicsMeshTransformAccess.h"


struct sMaterial
{
	sMaterial() : power(1.0f, 1.0f, 1.0f, 32.0f), diffuseTex(""), specularTex("defSpec.bmp") {};
	std::string diffuseTex;
	bool isDiffMirrored = false;
	glm::vec3 diffuv_Offset_Scale = glm::vec3(0.0f, 0.0f, 1.0f);

	std::string specularTex;
	bool isSpecMirrored = false;
	glm::vec3 specuv_Offset_Scale = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec4 power;
};

class cMesh : public iPhysicsMeshTransformAccess
{
public:
	cMesh();		// Called on creation   c'tor
	~cMesh();		// Called on deletion   d'tor


	std::string meshName;

	std::string friendlyName;

	bool bIsVisible;
	bool isDoubleSided = false;

	bool bUseCustomColors;
	glm::vec4 customColorRGBA;

	float transparencyAlpha;
	bool bUseDiscardMaskTex;
	bool isSkybox;

	bool bIsWireframe;
	bool bDoNotLight;

	bool bUseReflect;
	bool bUseRefract;

	bool isSpooky = false; // Spooky shader effect

	// These two will update at the draw call based off deltaTime
	 
	//glm::vec4 diffusePow_specularPow; // TODO: Something to add to the shader
	sMaterial material;


	int uniqueID; // This should match the physics object id; so create physics first then take that ID and set this one to it



	bool isNavMesh = false;
	 

	// Draw loop uses this: 
	glm::vec3 drawPosition;				
	glm::vec3 eulerOrientation;
	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ);
	//void setDrawOrientation(const glm::vec3& newOrientation);

	void adjustRotationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust);
	glm::quat get_qOrientation(void);
	glm::vec3 getEulerOrientation(void); // Returns euler orientation

private:
	glm::quat m_qOrientation; // Hiding this
public:
	glm::vec3 scale;

	void setUniformDrawScale(float scale);

	// STARTOF: From: iPhysicsMeshTransformAccess interface
	virtual glm::vec3 getDrawPosition(void);
	virtual glm::vec3 getDrawOrientation(void);
	virtual glm::vec3 getDrawScale(void);
	virtual std::string getMeshName(void);
	virtual void setDrawPosition(const glm::vec3& newPosition);
	virtual void setDrawOrientation(const glm::vec3& newOrientation);
	virtual void setDrawOrientation(const glm::quat& newOrientation);
	virtual void setDrawScale(const glm::vec3& newScale);
	// ENDOF: iPhysicsMeshTransformAccess interface


	

};

#endif

