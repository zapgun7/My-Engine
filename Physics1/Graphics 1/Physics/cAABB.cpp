#include "cAABB.h"




// Getters
inline float cAABB::xMax()
{
	return this->centerPosition.x + this->halfLengths.x;
}

inline float cAABB::xMin()
{
	return this->centerPosition.x - this->halfLengths.x;
}

inline float cAABB::yMax()
{
	return this->centerPosition.y + this->halfLengths.y;
}

inline float cAABB::yMin()
{
	return this->centerPosition.y - this->halfLengths.y;
}

inline float cAABB::zMax()
{
	return this->centerPosition.z + this->halfLengths.z;
}

inline float cAABB::zMin()
{
	return this->centerPosition.z - this->halfLengths.z;
}

inline glm::vec3 cAABB::XYZMax()
{
	return this->centerPosition + this->halfLengths;
}

inline glm::vec3 cAABB::XYZMin()
{
	return this->centerPosition - this->halfLengths;
}

inline glm::vec3 cAABB::XYZcenter()
{
	return this->centerPosition;
	
}

inline float cAABB::Xcenter()
{
	return this->centerPosition.x;
}

inline float cAABB::Ycenter()
{
	return this->centerPosition.y;
}

inline float cAABB::Zcenter()
{
	return this->centerPosition.z;
}


