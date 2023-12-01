#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "../Physics/iPhysicsMeshTransformAccess.h"


class cMesh : public iPhysicsMeshTransformAccess
{
public:
	cMesh();		// Called on creation   c'tor
	~cMesh();		// Called on deletion   d'tor

	// Texture Stuff

	static const int NUM_TEXTURES = 8;
	std::string textureName[NUM_TEXTURES];
	int textureIdx[NUM_TEXTURES];
	float textureRatios[NUM_TEXTURES];


	///////////////////



	std::string meshName;

	std::string friendlyName;		// "Ground"
	int uniqueID; // This should match the physics object id; so create physics first then take that ID and set this one to it

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
	virtual void setDrawPosition(const glm::vec3& newPosition);
	virtual void setDrawOrientation(const glm::vec3& newOrientation);
	virtual void setDrawOrientation(const glm::quat& newOrientation);
	// ENDOF: iPhysicsMeshTransformAccess interface


	bool bIsVisible;

	bool bUseDebugColours;
	glm::vec4 wholeObjectDebugColourRGBA;

	// Physics properties
	// Physics update this:
	//sPhsyicsProperties* pPhysProps;

	bool bIsWireframe;
	bool bDoNotLight;

	//void Update(double deltaTime);

};

#endif

