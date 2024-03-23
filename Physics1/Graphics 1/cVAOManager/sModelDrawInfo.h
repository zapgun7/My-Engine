#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <map>
#include <vector>

struct aiScene;
struct aiNode;


// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVertex
{
	float x, y, z, w;		// vPos
	//float r, g, b, a;		// vCol
	float nx, ny, nz, nw;	// vNormal
	float u, v, s, t;		// vTextureCoords    // making them vec4 to match up with bone land
};

struct sFloat4
{
	float x, y, z, w;
};

struct sInt4
{
	int x, y, z, w;
};

struct sVertex_p4t4n4b4w4
{
	sFloat4 Pos;
	sFloat4 Normal;
	sFloat4 TexUVx2;
	sFloat4 BoneWeights;
	//sFloat4 BoneIds;
	sInt4 BoneIds;
};

struct sBoneInfo
{
	glm::mat4 BoneOffset;// = glm::mat4(1.0f);				// Offset from the parent bone/node
	glm::mat4 FinalTransformation = glm::mat4(1.0f);		// Calculated transformation used for rendering
	glm::mat4 GlobalTransformation = glm::mat4(1.0f);		// used for the bone hierarchy transformation calculations when animating
};

// Connection Node for hierarchy
struct sNode
{
	sNode(const std::string& name) : Name(name) { }
	std::string Name;
	glm::mat4 Transformation;// = glm::mat4(1.0f);
	std::vector<sNode*> Children;
};

// Animation Node
// struct sNodeAnim
// {
// 	sNodeAnim(const std::string& name) : Name(name) { }
// 	std::string Name;
// 	std::vector<PositionKeyFrame> m_PositionKeyFrames;
// 	std::vector<ScaleKeyFrame> m_ScaleKeyFrames;
// 	std::vector<RotationKeyFrame> m_RotationKeyFrames;
// };

// struct sCharacterAnimation
// {
// 	std::string Name;
// 	double TicksPerSecond;
// 	double Duration;
// 	sNode* RootNode;
// 	std::vector<NodeAnim*> Channels;
// };

struct sModelDrawInfo
{
	sModelDrawInfo();

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVertex* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;

	glm::vec3 maxExtents_XYZ;	// bounding box maximums
	glm::vec3 minExtents_XYZ;	// bounding box minimums
	glm::vec3 deltaExtents_XYZ;	// bounding box dimensions
	float maxExtent;

	void calcExtents(void);

	sNode* GenerateBoneHierarchy(aiNode* node, const int depth = 0);


	// Animation Stuff
	aiScene* scene;
	std::vector<glm::mat4> NodeHierarchyTransformations;
	std::map<std::string, int> NodeNameToIdMap;

	std::vector<sBoneInfo> BoneInfoVec;
	std::map<std::string, int> BoneNameToIdMap;
	sNode* RootNode;

	//std::vector<sCharacterAnimation*> CharacterAnimations;

	glm::mat4 GlobalInverseTransformation;


	// 
	unsigned int getUniqueID(void);
};
