#include "cVAOManager.h"

#include "../Other Graphics Stuff/OpenGLCommon.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//$(SolutionDir)AssimpFileLoaderHelper\AssimpFileLoaderHelper\include

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>
#include <fstream>


sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
// 	glm::vec3 maxValues;
// 	glm::vec3 minValues;

//	scale = 5.0/maxExtent;		-> 5x5x5
	//float maxExtent;

	return;
}

void AssimpToGLMMat(const aiMatrix4x4& a, glm::mat4& g)
{
	g[0][0] = a.a1; g[0][1] = a.b1; g[0][2] = a.c1; g[0][3] = a.d1;
	g[1][0] = a.a2; g[1][1] = a.b2; g[1][2] = a.c2; g[1][3] = a.d2;
	g[2][0] = a.a3; g[2][1] = a.b3; g[2][2] = a.c3; g[2][3] = a.d3;
	g[3][0] = a.a4; g[3][1] = a.b4; g[3][2] = a.c4; g[3][3] = a.d4;
// 	g[0][0] = a.a1; g[0][1] = a.a2; g[0][2] = a.a3; g[0][3] = a.a4;
// 	g[1][0] = a.b1; g[1][1] = a.b2; g[1][2] = a.b3; g[1][3] = a.b4;
// 	g[2][0] = a.c1; g[2][1] = a.c2; g[2][2] = a.c3; g[2][3] = a.c4;
// 	g[3][0] = a.d1; g[3][1] = a.d2; g[3][2] = a.d3; g[3][3] = a.d4;
}

void AssimpToGLMVec(const aiVector3D& a, glm::vec3& vec)
{
	vec.x = a.x;
	vec.y = a.y;
	vec.z = a.z;
}

void AssimpToGLMQuat(aiQuaternion& a, glm::quat& quat)
{
	quat = glm::quat(a.w, a.x, a.y, a.z);
}

sNode* CreateAnimNode(aiNode* node)
{
	sNode* newNode = new sNode(node->mName.C_Str());
	AssimpToGLMMat(node->mTransformation, newNode->Transformation);
	return newNode;
}


sNode* sModelDrawInfo::GenerateBoneHierarchy(aiNode* assimpNode, const int depth)
{
	sNode* node = CreateAnimNode(assimpNode);

	aiMatrix4x4& transformation = assimpNode->mTransformation;
	aiVector3D position;
	aiQuaternion rotation;
	aiVector3D scaling;
	transformation.Decompose(scaling, rotation, position);

	glm::mat4 glmMatrix;
	AssimpToGLMMat(transformation, glmMatrix);

	glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.f), glm::vec3(position.x, position.y, position.z));

	glm::mat4 RotationMatrix = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z)/* * glm::quat(glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f))*/);


	glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaling.x, scaling.y, scaling.z));

	glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;
	//ModelMatrix = glm::mat4(1.0f);

	NodeNameToIdMap.insert(std::pair<std::string, int>((std::string)(assimpNode->mName.C_Str()), NodeHierarchyTransformations.size()));
	NodeHierarchyTransformations.emplace_back(/*ModelMatrix*/glmMatrix);

	for (unsigned int i = 0; i < assimpNode->mNumChildren; i++)
	{
		node->Children.emplace_back(GenerateBoneHierarchy(assimpNode->mChildren[i], depth + 1));
	}
	return node;
}



void cVAOManager::setBasePath(std::string basePathWithoutSlash)
{
    this->m_basePathWithoutSlash = basePathWithoutSlash;
    return;
}



cVAOManager::cVAOManager()
{
	this->m_pAnimationManager = cAnimationManager::GetInstance();
}

void cVAOManager::Initialize(void)
{

}

bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID,
        bool bIsDynamicBuffer /*=false*/)

{
	// Load the model from file
	// (We do this here, since if we can't load it, there's 
	//	no point in doing anything else, right?)

	drawInfo.meshName = fileName;

    std::string fileAndPath = this->m_basePathWithoutSlash + "/" + fileName;

	if (fileName == "Padoru_v1-4.dae")//"Adventurer Aland@Idle.FBX")
	{
		if (!this->m_LoadTheFileAnimModel(fileAndPath, drawInfo, shaderProgramID))
		{
			return false;
		};
		//m_pAnimationManager;
		return true;
	}

    if ( ! this->m_LoadTheFile(fileAndPath, drawInfo) )
    {
        return false;
    };


	// ***********************************************************
	// TODO: Load the model from file

	// ***********************************************************
	// 
	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]

    // This is updated for bIsDynamicBuffer so:
    // * if true, then it's GL_DYNAMIC_DRAW
    // * if false, then it's GL_STATIC_DRAW
    // 
    // Honestly, it's not that Big Of A Deal in that you can still update 
    //  a buffer if it's set to STATIC, but in theory this will take longer.
    // Does it really take longer? Who knows?
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  (bIsDynamicBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ) );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");	// program
	//GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;

	GLint vTextureCoords_location = glGetAttribLocation(shaderProgramID, "vTextureCoords");

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);	    // vPos
	glVertexAttribPointer( vpos_location, 4,		// vPos
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVertex), 
						   ( void* ) offsetof(sVertex, x));

// 	glEnableVertexAttribArray(vcol_location);	    // vCol
// 	glVertexAttribPointer( vcol_location, 4,		// vCol
// 						   GL_FLOAT, GL_FALSE,
//                           sizeof(sVertex),
// 						   ( void* ) offsetof(sVertex, r));

	glEnableVertexAttribArray(vNormal_location);	// vNormal
	glVertexAttribPointer(vNormal_location, 4,		// vNormal
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, nx));


	glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
	glVertexAttribPointer(vTextureCoords_location, 4,		// in vec2 vTextureCoords;
		GL_FLOAT, GL_FALSE,
		sizeof(sVertex),
		(void*)offsetof(sVertex, u));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	//glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vNormal_location);
	glDisableVertexAttribArray(vTextureCoords_location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}



bool cVAOManager::m_LoadTheFile(std::string theFileName, sModelDrawInfo& drawInfo)
{

	//std::string filePath = m_basePathWithoutSlash + "/" + theFileName;

    //sPostProcessFlags loadFlags; // Maybe'll use this later
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(theFileName,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

		//#AI_SCENE_FLAGS_NON_VERBOSE_FORMAT;

	//std::cout << scene->mMeshes[0]->mVertices[0].x << std::endl;

   // m_pAssLoader->Load3DModelFile(theFileName.c_str());

    // !!!!! ASSUMING WE ONLY HAVE ONE MESH PER MODEL FOR NOW !!!!!
    drawInfo.numberOfVertices = scene->mMeshes[0]->mNumVertices;
    drawInfo.numberOfTriangles = scene->mMeshes[0]->mNumFaces;
    drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;



	glm::vec3 maxXYZ(scene->mMeshes[0]->mVertices[0].x, scene->mMeshes[0]->mVertices[0].y, scene->mMeshes[0]->mVertices[0].z);
	glm::vec3 minXYZ = maxXYZ;

	drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
	for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
	{
		// Vertices
        drawInfo.pVertices[vertIndex].x = scene->mMeshes[0]->mVertices[vertIndex].x;
		drawInfo.pVertices[vertIndex].y = scene->mMeshes[0]->mVertices[vertIndex].y;
		drawInfo.pVertices[vertIndex].z = scene->mMeshes[0]->mVertices[vertIndex].z;
		drawInfo.pVertices[vertIndex].w = 1.0f;

		// Quick min/max check
		if (minXYZ.x > drawInfo.pVertices[vertIndex].x) minXYZ.x = drawInfo.pVertices[vertIndex].x;
		else if (maxXYZ.x < drawInfo.pVertices[vertIndex].x) maxXYZ.x = drawInfo.pVertices[vertIndex].x;

		if (minXYZ.y > drawInfo.pVertices[vertIndex].y) minXYZ.y = drawInfo.pVertices[vertIndex].y;
		else if (maxXYZ.y < drawInfo.pVertices[vertIndex].y) maxXYZ.y = drawInfo.pVertices[vertIndex].y;

		if (minXYZ.z > drawInfo.pVertices[vertIndex].z) minXYZ.z = drawInfo.pVertices[vertIndex].z;
		else if (maxXYZ.z < drawInfo.pVertices[vertIndex].z) maxXYZ.z = drawInfo.pVertices[vertIndex].z;


		// Normals
        drawInfo.pVertices[vertIndex].nx = scene->mMeshes[0]->mNormals[vertIndex].x;
		drawInfo.pVertices[vertIndex].ny = scene->mMeshes[0]->mNormals[vertIndex].y;
		drawInfo.pVertices[vertIndex].nz = scene->mMeshes[0]->mNormals[vertIndex].z;
		drawInfo.pVertices[vertIndex].nw = 1.0f;

		// UV Coords
// 		drawInfo.pVertices[vertIndex].u = scene->mMeshes[0]->mTextureCoords[vertIndex]->x;
// 		drawInfo.pVertices[vertIndex].v = scene->mMeshes[0]->mTextureCoords[vertIndex]->y;

// 		if (scene->mMeshes[0]->HasTextureCoords(vertIndex))
// 		{
		drawInfo.pVertices[vertIndex].u = scene->mMeshes[0]->mTextureCoords[0][vertIndex].x;
		drawInfo.pVertices[vertIndex].v = scene->mMeshes[0]->mTextureCoords[0][vertIndex].y;
		drawInfo.pVertices[vertIndex].s = scene->mMeshes[0]->mTextureCoords[0][vertIndex].x;//0.0f;
		drawInfo.pVertices[vertIndex].t = scene->mMeshes[0]->mTextureCoords[0][vertIndex].y;//0.0f;
// 		}
// 		else
// 		{
// 			drawInfo.pVertices[vertIndex].u = 0.0f;
// 			drawInfo.pVertices[vertIndex].v = 0.0f;
// 		}


//         if (scene->mMeshes[0]->HasVertexColors(vertIndex))
//         {
//             drawInfo.pVertices[vertIndex].r = scene->mMeshes[0]->mColors[0][vertIndex].r;
//             drawInfo.pVertices[vertIndex].g = scene->mMeshes[0]->mColors[0][vertIndex].g;
//             drawInfo.pVertices[vertIndex].b = scene->mMeshes[0]->mColors[0][vertIndex].b;
//             drawInfo.pVertices[vertIndex].a = scene->mMeshes[0]->mColors[0][vertIndex].a;
//         }
//         else
//         {
//             drawInfo.pVertices[vertIndex].r = 0.5;
//             drawInfo.pVertices[vertIndex].g = 0.5;
//             drawInfo.pVertices[vertIndex].b = 0.5;
//             drawInfo.pVertices[vertIndex].a = 255;
//         }
	}

	// Set the calculated min/max
	drawInfo.maxExtents_XYZ = maxXYZ;
	drawInfo.minExtents_XYZ = minXYZ;


	// Allocate an array for all the indices (which is 3x the number of triangles)
	// Element array is an 1D array of integers
	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	unsigned int elementIndex = 0;
	for (unsigned int triIndex = 0; triIndex != drawInfo.numberOfTriangles; triIndex++)
	{
		// 3 1582 1581 2063 
		drawInfo.pIndices[elementIndex + 0] = scene->mMeshes[0]->mFaces[triIndex].mIndices[0];
		drawInfo.pIndices[elementIndex + 1] = scene->mMeshes[0]->mFaces[triIndex].mIndices[1];
		drawInfo.pIndices[elementIndex + 2] = scene->mMeshes[0]->mFaces[triIndex].mIndices[2];

		elementIndex += 3;      // Next "triangle"
	}



	return true;
}

struct sBoneWeightInfo {
	sBoneWeightInfo() {
		m_BoneId[0] = 0;
		m_BoneId[1] = 0;
		m_BoneId[2] = 0;
		m_BoneId[3] = 0;
		m_Weight[0] = 0.f;
		m_Weight[1] = 0.f;
		m_Weight[2] = 0.f;
		m_Weight[3] = 0.f;
	}
	int m_BoneId[4];
	float m_Weight[4];
};


bool cVAOManager::m_LoadTheFileAnimModel(std::string theFileName, sModelDrawInfo& drawInfo, unsigned int shaderID)
{
	Assimp::Importer m_AssimpImporter;
	drawInfo.scene = (aiScene*)m_AssimpImporter.ReadFile(theFileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);//aiProcess_GenNormals);

	// Load animations (when I get my animation system to intermingle with bones n' stuff)
	sBonedAnimation* characterAnimation = new sBonedAnimation();
	if (drawInfo.scene->mNumAnimations > 0)
	{
		//sBonedAnimation* characterAnimation = new sBonedAnimation();
		aiAnimation* animation = drawInfo.scene->mAnimations[0];

		characterAnimation->name = animation->mName.C_Str();
		characterAnimation->Duration = animation->mDuration;
		characterAnimation->TicksPerSecond = animation->mTicksPerSecond;

		for (unsigned int i = 0; i < animation->mNumChannels; i++)
		{
			aiNodeAnim* assimpNodeAnim = animation->mChannels[i];
			sAnimInfo* animInfo = new sAnimInfo();
			animInfo->name = assimpNodeAnim->mNodeName.C_Str();
			characterAnimation->map_NameToAnimationData.insert(std::pair<std::string, sAnimInfo*>(animInfo->name, animInfo));

			for (unsigned int e = 0; e < assimpNodeAnim->mNumPositionKeys; e++)
			{
// 				aiVectorKey& p = assimpNodeAnim->mPositionKeys[e];
// 				animInfo->mveKeyFrames.emplace_back(sAnimInfo::sAnimNode(glm::vec3(p.mValue.x, p.mValue.y, p.mValue.z), p.mTime));
				aiVector3D aiPosition = assimpNodeAnim->mPositionKeys[e].mValue;
				float timeStamp = assimpNodeAnim->mPositionKeys[e].mTime;
				sAnimInfo::sKeyPosition data;
				AssimpToGLMVec(aiPosition, data.position);
				data.timeStamp = timeStamp;
				animInfo->m_Positions.push_back(data);
			}
			for (unsigned int e = 0; e < assimpNodeAnim->mNumScalingKeys; e++)
			{
// 				aiVectorKey& s = assimpNodeAnim->mScalingKeys[e];
// 				animInfo->sclKeyFrames.emplace_back(sAnimInfo::sAnimNode(glm::vec3(s.mValue.x, s.mValue.y, s.mValue.z), s.mTime));
				aiVector3D aiScale = assimpNodeAnim->mScalingKeys[e].mValue;
				float timeStamp = assimpNodeAnim->mPositionKeys[e].mTime;
				sAnimInfo::sKeyScale data;
				AssimpToGLMVec(aiScale, data.scale);
				data.timeStamp = timeStamp;
				animInfo->m_Scales.push_back(data);
			}
			for (unsigned int e = 0; e < assimpNodeAnim->mNumRotationKeys; e++)
			{
// 				aiQuatKey& q = assimpNodeAnim->mRotationKeys[e];
// 				glm::quat tempQuat = glm::quat(q.mValue.w, q.mValue.x, q.mValue.y, q.mValue.z);
// 				animInfo->oriKeyFrames.emplace_back(sAnimInfo::sAnimNode(glm::eulerAngles(tempQuat), q.mTime)); // !!!  For now use euler (Until confirmed it works)
				aiQuaternion aiOrientation = assimpNodeAnim->mRotationKeys[e].mValue;
				float timeStamp = assimpNodeAnim->mRotationKeys[e].mTime;
				sAnimInfo::sKeyRotation data;
				data.timeStamp = timeStamp;
				AssimpToGLMQuat(aiOrientation, data.orientation);
				animInfo->m_Orientations.push_back(data);
			}
			animInfo->posSize = assimpNodeAnim->mNumPositionKeys;
			animInfo->oriSize = assimpNodeAnim->mNumRotationKeys;
			animInfo->sclSize = assimpNodeAnim->mNumScalingKeys;
			characterAnimation->BoneAnimations.emplace_back(animInfo);
		}
		
	}





	// Generate bones
	drawInfo.RootNode = drawInfo.GenerateBoneHierarchy(drawInfo.scene->mRootNode);
	characterAnimation->rootNode = drawInfo.RootNode;
	drawInfo.GlobalInverseTransformation = glm::inverse(drawInfo.RootNode->Transformation);

	drawInfo.numberOfVertices = 0;
	drawInfo.numberOfTriangles = 0;
	for (int meshidx = 0; meshidx < drawInfo.scene->mNumMeshes; meshidx++)
	{
		aiMesh* mesh = drawInfo.scene->mMeshes[meshidx];
		drawInfo.numberOfVertices += mesh->mNumVertices;
		drawInfo.numberOfTriangles += mesh->mNumFaces;
	}

	std::vector<sBoneWeightInfo> boneWeights;
	boneWeights.resize(drawInfo.numberOfVertices);
	int boneCounter = 0;

	//aiMesh* mesh = drawInfo.scene->mMeshes[0];

	for (int meshidx = 0; meshidx < drawInfo.scene->mNumMeshes; meshidx++)
	{
		aiMesh* mesh = drawInfo.scene->mMeshes[meshidx];
		if (mesh->HasBones())
		{
			//boneWeights.resize(mesh->mNumVertices);
			unsigned int numBones = mesh->mNumBones;
			for (unsigned int boneIdx = 0; boneIdx < numBones; boneIdx++)
			{
				unsigned int BoneIndex = 0;
				aiBone* bone = mesh->mBones[boneIdx];

				std::string name(bone->mName.C_Str(), bone->mName.length);
				// drawInfo.BoneNameToIdMap.insert(std::pair<std::string, int>(name, drawInfo.BoneInfoVec.size()));

				if (drawInfo.BoneNameToIdMap.find(name) == drawInfo.BoneNameToIdMap.end())
				{
					BoneIndex = boneCounter;
					boneCounter++;
					sOldBoneInfo info;
					drawInfo.BoneInfoVec.push_back(info);
				}
				else
				{
					BoneIndex = drawInfo.BoneNameToIdMap[name];
				}

				drawInfo.BoneNameToIdMap[name] = BoneIndex;
				
				AssimpToGLMMat(bone->mOffsetMatrix, drawInfo.BoneInfoVec[BoneIndex].BoneOffset);


				// Store offset matrices
				//sOldBoneInfo info;
				//AssimpToGLM(bone->mOffsetMatrix, info.BoneOffset);
				//info.BoneOffset = glm::mat4(1.0f);
				//drawInfo.BoneInfoVec.emplace_back(info);


				printf("\n-----------\n");
				printf("Bone: %s\n", name.c_str());
				printf("Number of weights: %d\n", bone->mNumWeights);

				for (unsigned int weightIdx = 0; weightIdx < bone->mNumWeights; weightIdx++)
				{
					aiVertexWeight& vertexWeight = bone->mWeights[weightIdx];
					// BoneId		:	boneIdx
					// Vertexid		:	vertexWeight.mVertexId
					// Weight		:	vertexWeight.mWeight

					sBoneWeightInfo& boneInfo = boneWeights[vertexWeight.mVertexId];
					for (int infoIdx = 0; infoIdx < 4; infoIdx++)
					{
						if (boneInfo.m_Weight[infoIdx] == 0.0f)
						{
							boneInfo.m_BoneId[infoIdx] = boneIdx;
							boneInfo.m_Weight[infoIdx] = vertexWeight.mWeight;
							break;
						}
					}
				}
			}
		}
	}
	characterAnimation->theModel = drawInfo;
	m_pAnimationManager->AddBonedAnimation(characterAnimation, characterAnimation->name);

	// Now the regular model loading stuff

	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;
	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	sVertex_p4t4n4b4w4* pTempVertArray = new sVertex_p4t4n4b4w4[drawInfo.numberOfIndices * 2];
	GLint* pIndexArrayLocal = new GLint[drawInfo.numberOfIndices * 2];
	int count = 0;
	int index = 0;

	int indexIdx = 0;

	unsigned int vertArrayIndex = 0;
	for (int meshidx = 0; meshidx < drawInfo.scene->mNumMeshes; meshidx++)
	{
		aiMesh* mesh = drawInfo.scene->mMeshes[meshidx];
		for (unsigned int faceIdx = 0; faceIdx != mesh->mNumFaces; faceIdx++)
		{
			aiFace face = mesh->mFaces[faceIdx];

			for (int idx = 0; idx != 3; idx++)
			{
				unsigned int index = face.mIndices[idx];

				drawInfo.pIndices[indexIdx++] = index;

				aiVector3D position = mesh->mVertices[index];
				pTempVertArray[vertArrayIndex].Pos.x = position.x;
				pTempVertArray[vertArrayIndex].Pos.y = position.y;
				pTempVertArray[vertArrayIndex].Pos.z = position.z;
				pTempVertArray[vertArrayIndex].Pos.w = 1.0f;

				aiVector3D normal = mesh->mNormals[index];
				pTempVertArray[vertArrayIndex].Normal.x = normal.x;
				pTempVertArray[vertArrayIndex].Normal.y = normal.y;
				pTempVertArray[vertArrayIndex].Normal.z = normal.z;
				pTempVertArray[vertArrayIndex].Normal.w = 1.0f;


				if (mesh->HasTextureCoords(0))
				{
					aiVector3D textureCoord = mesh->mTextureCoords[0][index];
					pTempVertArray[vertArrayIndex].TexUVx2.x = textureCoord.x;
					pTempVertArray[vertArrayIndex].TexUVx2.y = textureCoord.y;
					pTempVertArray[vertArrayIndex].TexUVx2.z = textureCoord.z;
					pTempVertArray[vertArrayIndex].TexUVx2.w = 1.0f;
				}
				else
				{
					pTempVertArray[vertArrayIndex].TexUVx2.x = 0.0f;
					pTempVertArray[vertArrayIndex].TexUVx2.y = 0.0f;
					pTempVertArray[vertArrayIndex].TexUVx2.z = 0.0f;
					pTempVertArray[vertArrayIndex].TexUVx2.w = 0.0f;
				}

				if (mesh->HasBones())
				{
					sBoneWeightInfo& boneInfo = boneWeights[index];

					pTempVertArray[vertArrayIndex].BoneIds.x = boneInfo.m_BoneId[0];
					pTempVertArray[vertArrayIndex].BoneIds.y = boneInfo.m_BoneId[1];
					pTempVertArray[vertArrayIndex].BoneIds.z = boneInfo.m_BoneId[2];
					pTempVertArray[vertArrayIndex].BoneIds.w = boneInfo.m_BoneId[3];

					pTempVertArray[vertArrayIndex].BoneWeights.x = boneInfo.m_Weight[0];
					pTempVertArray[vertArrayIndex].BoneWeights.y = boneInfo.m_Weight[1];
					pTempVertArray[vertArrayIndex].BoneWeights.z = boneInfo.m_Weight[2];
					pTempVertArray[vertArrayIndex].BoneWeights.w = boneInfo.m_Weight[3];

					float weight = boneInfo.m_Weight[0] + boneInfo.m_Weight[1] + boneInfo.m_Weight[2] + boneInfo.m_Weight[3];
					if (weight != 1.0f)
					{
						int breakhere = 0;
					}
				}

				pIndexArrayLocal[vertArrayIndex] = vertArrayIndex;

				vertArrayIndex++;
			}
		}
	}



	// Now to load into VAO

	glGenVertexArrays(1, &(drawInfo.VAO_ID));
	glBindVertexArray(drawInfo.VAO_ID);

	// Index Info
	glGenBuffers(1, &(drawInfo.IndexBufferID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	unsigned int sizeOfIndexArrayInBytes = drawInfo.numberOfIndices * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndexArrayInBytes, pIndexArrayLocal, GL_STATIC_DRAW);


	// Vertex Info
	glGenBuffers(1, &(drawInfo.VertexBufferID));
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);

	unsigned int totalVertBufferSizeBYTES = drawInfo.numberOfVertices * sizeof(sVertex_p4t4n4b4w4);
	glBufferData(GL_ARRAY_BUFFER, totalVertBufferSizeBYTES, pTempVertArray, GL_STATIC_DRAW);


	// Set offset of vertex data
	GLint vPos_UL = glGetAttribLocation(shaderID, "vPos");
	GLint vNorm_UL = glGetAttribLocation(shaderID, "vNormal");
	GLint vTexCoords_UL = glGetAttribLocation(shaderID, "vTextureCoords");
	GLint vBoneIds_UL = glGetAttribLocation(shaderID, "vBoneIds");
	GLint vBoneWeights_UL = glGetAttribLocation(shaderID, "vBoneWeights");

	unsigned int bytesInOneVertex = sizeof(sVertex_p4t4n4b4w4);

// 	sFloat4 BoneWeights;
// 	sFloat4 BoneIds;


	// Position
	glEnableVertexAttribArray(vPos_UL);
	glVertexAttribPointer(vPos_UL, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (void*)offsetof(sVertex_p4t4n4b4w4, Pos));

	// Normal
	glEnableVertexAttribArray(vNorm_UL);
	glVertexAttribPointer(vNorm_UL, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (void*)offsetof(sVertex_p4t4n4b4w4, Normal));

	// Texture
	glEnableVertexAttribArray(vTexCoords_UL);
	glVertexAttribPointer(vTexCoords_UL, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (void*)offsetof(sVertex_p4t4n4b4w4, TexUVx2));

	// Bone Weights
	glEnableVertexAttribArray(vBoneWeights_UL);
	glVertexAttribPointer(vBoneWeights_UL, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (void*)offsetof(sVertex_p4t4n4b4w4, BoneWeights));

	// Bone Ids
	glEnableVertexAttribArray(vBoneIds_UL);
	//glVertexAttribPointer(vBoneIds_UL, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (void*)offsetof(sVertex_p4t4n4b4w4, BoneIds));
	glVertexAttribIPointer(vBoneIds_UL, 4, GL_INT, bytesInOneVertex, (void*)offsetof(sVertex_p4t4n4b4w4, BoneIds));



	// Now set VAO to zero
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vPos_UL);
	glDisableVertexAttribArray(vNorm_UL);
	glDisableVertexAttribArray(vTexCoords_UL);
	glDisableVertexAttribArray(vBoneWeights_UL);
	glDisableVertexAttribArray(vBoneIds_UL);

	this->m_map_ModelName_to_VAOID[drawInfo.meshName] = drawInfo;

	
	return true;
}

bool cVAOManager::m_LoadTheBonedModel(std::string theFileName, sModelDrawInfo& drawInfo, unsigned int shaderID)
{
	Assimp::Importer assimpImporter;
	drawInfo.scene = (aiScene*)assimpImporter.ReadFile(theFileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	aiMesh* mesh = drawInfo.scene->mMeshes[0];




	return true;
}

bool cVAOManager::UpdateVAOBuffers(std::string fileName,
                      sModelDrawInfo& updatedDrawInfo,
                      unsigned int shaderProgramID)
{
    // This exists? 
    sModelDrawInfo updatedDrawInfo_TEMP;
    if ( ! this->FindDrawInfoByModelName(fileName,  updatedDrawInfo_TEMP) )
    {
        // Didn't find this buffer
        return false;
    }


    glBindBuffer(GL_ARRAY_BUFFER, updatedDrawInfo.VertexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ARRAY_BUFFER,
//                 sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
//                 (GLvoid*)updatedDrawInfo.pVertices,					
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 
                    0,  // Offset
                    sizeof(sVertex) * updatedDrawInfo.numberOfVertices,	
                    (GLvoid*)updatedDrawInfo.pVertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, updatedDrawInfo.IndexBufferID);

// Original call to create and copy the initial data:
//     
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
//                 sizeof(unsigned int) * updatedDrawInfo.numberOfIndices,
//                 (GLvoid*)updatedDrawInfo.pIndices,
//                 GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,			
                    0,  // Offset
                    sizeof(unsigned int) * updatedDrawInfo.numberOfIndices,
                    (GLvoid*)updatedDrawInfo.pIndices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    return true;
}


bool cVAOManager::UpdateBoneShit(std::string fileName, sModelDrawInfo& updatedDrawInfo, unsigned int shaderProgramID)
{
	// This exists? 
// 	sModelDrawInfo updatedDrawInfo_TEMP;
// 	if (!this->FindDrawInfoByModelName(fileName, updatedDrawInfo_TEMP))
// 	{
// 		// Didn't find this buffer
// 		return false;
// 	}

	//updatedDrawInfo_TEMP.BoneInfoVec = updatedDrawInfo.BoneInfoVec;


	std::map< std::string /*model name*/,
		sModelDrawInfo /* info needed to draw*/ >::iterator
		itDrawInfo = this->m_map_ModelName_to_VAOID.find(fileName);

	// Find it? 
	if (itDrawInfo == this->m_map_ModelName_to_VAOID.end())
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	itDrawInfo->second.BoneInfoVec = updatedDrawInfo.BoneInfoVec;
}

// Prob will never use this, more efficient to scan for min and max when loading in the vertices! (only realized after making this oh well)
void sModelDrawInfo::calcExtents()
{
	glm::vec3 maxXYZ (this->pVertices[0].x, this->pVertices[0].y, this->pVertices[0].z);
	glm::vec3 minXYZ = maxXYZ;

	for (unsigned int i = 1; i < this->numberOfVertices; i++)
	{
		if (minXYZ.x > this->pVertices[i].x) minXYZ.x = this->pVertices[i].x;
		else if (maxXYZ.x < this->pVertices[i].x) maxXYZ.x = this->pVertices[i].x;

		if (minXYZ.y > this->pVertices[i].y) minXYZ.y = this->pVertices[i].y;
		else if (maxXYZ.y < this->pVertices[i].y) maxXYZ.y = this->pVertices[i].y;

		if (minXYZ.z > this->pVertices[i].z) minXYZ.z = this->pVertices[i].z;
		else if (maxXYZ.z < this->pVertices[i].z) maxXYZ.z = this->pVertices[i].z;
	}

	this->maxExtents_XYZ = maxXYZ;
	this->minExtents_XYZ = minXYZ;
}
