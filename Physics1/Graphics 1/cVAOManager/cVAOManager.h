#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>
//#include "../AssimpFileLoaderHelper/AssimpFileLoaderHelper/cAssimpHelper.h"

// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVertex
{
//	float x, y, z;		
//	float r, g, b;
	float x, y, z, w;		// w 'cause math
	float r, g, b, a;
	float nx, ny, nz, nw;	// Won't use nw
};


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
};


class cVAOManager
{
public:

	bool LoadModelIntoVAO(std::string fileName, 
						  sModelDrawInfo &drawInfo, 
						  unsigned int shaderProgramID,
						  bool bIsDynamicBuffer = false);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename,
								 sModelDrawInfo &drawInfo);

	std::string getLastError(bool bAndClear = true);

	void setBasePath(std::string basePathWithoutSlash);

	// Here you have to be a little careful about what you pass. 
	// It will use the vertex and element (triangle) buffer IDs 
	//	to update - i.e. OVERWRITE - the buffers with whatever
	//	you've got in the two pointers. 
	// You can get this information by calling FindDrawInfoByModelName()
	//	and using the drawInfo that is returned. Note the sDrawInfo is a
	//	COPY of the structure in the map, but the pointers to the 
	//	vertex and element buffers are the ONE AND ONLY COPIES that
	//	exist on the CPU side. So if you overwrite them, they're gone.
	bool UpdateVAOBuffers(std::string fileName,
						  sModelDrawInfo& updatedDrawInfo,
						  unsigned int shaderProgramID);


private:

	bool m_LoadTheFile(std::string theFileName, sModelDrawInfo& drawInfo);

	std::map< std::string /*model name*/,
		      sModelDrawInfo /* info needed to draw*/ >
		m_map_ModelName_to_VAOID;

	std::string m_basePathWithoutSlash;
	//AH::cFileLoader* m_pAssLoader; // Assimp file loader object

};

#endif	// _cVAOManager_HG_
