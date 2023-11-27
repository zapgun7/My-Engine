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
	glm::vec3 maxValues;
	glm::vec3 minValues;

//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;

	return;
}


void cVAOManager::setBasePath(std::string basePathWithoutSlash)
{
    this->m_basePathWithoutSlash = basePathWithoutSlash;
    return;
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
	GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");	// program;
	GLint vNormal_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;

	GLint vTextureCoords_location = glGetAttribLocation(shaderProgramID, "vTextureCoords");

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);	    // vPos
	glVertexAttribPointer( vpos_location, 4,		// vPos
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVertex), 
						   ( void* ) offsetof(sVertex, x));

	glEnableVertexAttribArray(vcol_location);	    // vCol
	glVertexAttribPointer( vcol_location, 4,		// vCol
						   GL_FLOAT, GL_FALSE,
                          sizeof(sVertex),
						   ( void* ) offsetof(sVertex, r));

	glEnableVertexAttribArray(vNormal_location);	// vNormal
	glVertexAttribPointer(vNormal_location, 4,		// vNormal
						   GL_FLOAT, GL_FALSE,
                           sizeof(sVertex),
						   ( void* ) offsetof(sVertex, nx));


	glEnableVertexAttribArray(vTextureCoords_location);	    // vTextureCoords;
	glVertexAttribPointer(vTextureCoords_location, 2,		// in vec2 vTextureCoords;
		GL_FLOAT, GL_FALSE,
		sizeof(sVertex),
		(void*)offsetof(sVertex, u));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	glDisableVertexAttribArray(vcol_location);
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




	drawInfo.pVertices = new sVertex[drawInfo.numberOfVertices];
	for (unsigned int vertIndex = 0; vertIndex != drawInfo.numberOfVertices; vertIndex++)
	{
		// Vertices
        drawInfo.pVertices[vertIndex].x = scene->mMeshes[0]->mVertices[vertIndex].x;
		drawInfo.pVertices[vertIndex].y = scene->mMeshes[0]->mVertices[vertIndex].y;
		drawInfo.pVertices[vertIndex].z = scene->mMeshes[0]->mVertices[vertIndex].z;
		drawInfo.pVertices[vertIndex].w = 1.0f;

		// Normals
        drawInfo.pVertices[vertIndex].nx = scene->mMeshes[0]->mNormals[vertIndex].x;
		drawInfo.pVertices[vertIndex].ny = scene->mMeshes[0]->mNormals[vertIndex].y;
		drawInfo.pVertices[vertIndex].nz = scene->mMeshes[0]->mNormals[vertIndex].z;
		drawInfo.pVertices[vertIndex].nw = 1.0f;

		// UV Coords
// 		drawInfo.pVertices[vertIndex].u = scene->mMeshes[0]->mTextureCoords[vertIndex]->x;
// 		drawInfo.pVertices[vertIndex].v = scene->mMeshes[0]->mTextureCoords[vertIndex]->y;
		drawInfo.pVertices[vertIndex].u = scene->mMeshes[0]->mTextureCoords[0][vertIndex].x;
		drawInfo.pVertices[vertIndex].v = scene->mMeshes[0]->mTextureCoords[0][vertIndex].y;


        if (scene->mMeshes[0]->HasVertexColors(vertIndex))
        {
            drawInfo.pVertices[vertIndex].r = scene->mMeshes[0]->mColors[0][vertIndex].r;
            drawInfo.pVertices[vertIndex].g = scene->mMeshes[0]->mColors[0][vertIndex].g;
            drawInfo.pVertices[vertIndex].b = scene->mMeshes[0]->mColors[0][vertIndex].b;
            drawInfo.pVertices[vertIndex].a = scene->mMeshes[0]->mColors[0][vertIndex].a;
        }
        else
        {
            drawInfo.pVertices[vertIndex].r = 150;
            drawInfo.pVertices[vertIndex].g = 150;
            drawInfo.pVertices[vertIndex].b = 150;
            drawInfo.pVertices[vertIndex].a = 255;
        }
	}

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
