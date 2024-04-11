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

/////////////// OLD BONE STUFF ///////////////////

struct sOldBoneInfo
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

/////////////// /// //// ///// ///////////////////


// New Bone
struct sBoneInfo
{
	int id;
	glm::mat4 offset;
};





 
struct sModelDrawInfo
{
	// New Bone Stuff
	std::map<std::string, sBoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	std::map<std::string, sBoneInfo>& GetBoneInfoMap(void) { return m_BoneInfoMap; }
	int& GetBoneCount(void) { return m_BoneCounter; }

	void SetVertexBoneDataToDefault(sVertex_p4t4n4b4w4& vertex)
	{
		vertex.BoneIds.x = -1;
		vertex.BoneIds.y = -1;
		vertex.BoneIds.z = -1;
		vertex.BoneIds.w = -1;
		vertex.BoneWeights.x = 0.0f;
		vertex.BoneWeights.y = 0.0f;
		vertex.BoneWeights.z = 0.0f;
		vertex.BoneWeights.w = 0.0f;
	}





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
	void addMeshesFromNode(aiNode* asmpNde, sNode* node);


	// Animation Stuff
	aiScene* scene;
	std::vector<glm::mat4> NodeHierarchyTransformations;
	std::map<std::string, int> NodeNameToIdMap;

	std::vector<sOldBoneInfo> BoneInfoVec;
	std::map<std::string, int> BoneNameToIdMap;
	sNode* RootNode;

	//std::vector<sCharacterAnimation*> CharacterAnimations;

	glm::mat4 GlobalInverseTransformation;

	std::vector<unsigned int> testVec;


	// Stuff to Pass into the VAO (temporary)
// 	sVertex_p4t4n4b4w4* pTempVertexArray = nullptr;
// 	int currVert = 0;
// 	GLint* pTempIndexArray = nullptr;
// 	int currInd = 0;
// 	/*std::vector<sBoneWeightInfo> boneWeights;*/
// 	sBoneWeightInfo* pTempBoneWeightArray = nullptr;
// 	int currBone = 0;


	// 
	unsigned int getUniqueID(void);
};
