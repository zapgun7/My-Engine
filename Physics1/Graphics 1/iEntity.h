#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>




class iEntity
{
public:
	virtual ~iEntity() {};

	virtual void Update(double dt) = 0;
	virtual void updatePlayerTri(void* tri) = 0;

	virtual glm::vec3 getPosition(void) = 0;
	virtual glm::quat getOrientation(void) = 0;


};