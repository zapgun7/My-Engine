#include "cShaderManager.h"

#include "../Other Graphics Stuff/OpenGLCommon.h"	// For all the OpenGL calls, etc.

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <fstream>
#include <sstream>		// "string stream"
#include <vector>

#include <algorithm>		// for the std::copy
#include <iterator>			// for the back_inserter

#include <sstream> // For tokenizing strings



int cShaderManager::nextTextureNumber = 22;


cShaderManager::cShaderManager()
	:m_pActiveProgram(nullptr)
{
	// Initialize the map of str -> datatype

	map_Str_to_DType["vec4"] = sULInfo::eDataType::VEC4;
	map_Str_to_DType["mat4"] = sULInfo::eDataType::MAT4;
	map_Str_to_DType["sampler2D"] = sULInfo::eDataType::SAMPLER2D;
	map_Str_to_DType["samplerCube"] = sULInfo::eDataType::SAMPLERCUBE;

	return;
}

cShaderManager::~cShaderManager()
{
	return;
}


bool cShaderManager::useShaderProgram( unsigned int ID )
{
	// Use the number directy... 
	// TODO: Might do a lookup to see if we really have that ID...
	glUseProgram(ID);
	return true;
}

bool cShaderManager::useShaderProgram( std::string friendlyName )
{
// 	std::map< std::string /*name*/, unsigned int /*ID*/ >::iterator 
// 			itShad = this->m_name_to_ID.find(friendlyName);
	std::unordered_map<std::string, cShaderProgram>::iterator itShad = m_mapName_to_Shader.find(friendlyName);

	if ( itShad == this->m_mapName_to_Shader.end() )
	{	// Didn't find it
		// Maybe set glUseProgram(0)....?
		return false;
	}
	glUseProgram(itShad->second.ID);
	m_pActiveProgram = &itShad->second;

	return true;
}

unsigned int cShaderManager::getIDFromFriendlyName( std::string friendlyName )
{
	std::map< std::string /*name*/, unsigned int /*ID*/ >::iterator 
			itShad = this->m_name_to_ID.find(friendlyName);

	if ( itShad == this->m_name_to_ID.end() )
	{	// Didn't find it
		return 0;
	}
	return itShad->second;
}

cShaderManager::cShaderProgram* cShaderManager::pGetShaderProgramFromFriendlyName( std::string friendlyName )
{
	unsigned int shaderID = this->getIDFromFriendlyName(friendlyName);

	// Now get the actual shader
	
		std::map< unsigned int /*ID*/, cShaderProgram >::iterator
			itShad = this->m_ID_to_Shader.find(shaderID);

	if ( itShad == this->m_ID_to_Shader.end() )
	{	// Didn't find it
		return NULL;		// or 0 or nullptr
	}

	cShaderProgram* pShaderIFound = &(itShad->second);

	return pShaderIFound;
}


const unsigned int MAXLINELENGTH = 65536;		// 16x1024

void cShaderManager::setBasePath( std::string basepath )
{
	this->m_basepath = basepath;
	return;
}


// Returns bool if didn't load
bool cShaderManager::m_loadSourceFromFile( cShader &shader )
{
	std::string fullFileName = this->m_basepath + "/" + shader.fileName;

	std::ifstream theFile( fullFileName.c_str() );
	if ( ! theFile.is_open() )
	{
		return false;
	}

	//std::stringstream ssSource;
	//std::string temp;

	//while ( theFile >> temp )
	//{	// Add it to the string stream
	//	ssSource << temp << " ";
	//}

	shader.vecSource.clear();

	char pLineTemp[MAXLINELENGTH] = {0};
	while ( theFile.getline( pLineTemp, MAXLINELENGTH ) )
	{
		std::string tempString(pLineTemp);
		//if ( tempString != "" )
		//{	// Line isn't empty, so add
			shader.vecSource.push_back( tempString );
		//}
	}
	
	theFile.close();
	return true;		// Return the string (from the sstream)
}

// Returns empty string if no error
// returns false if no error
bool cShaderManager::m_wasThereACompileError( unsigned int shaderID, 
											  std::string &errorText )
{
	errorText = "";	// No error

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_lastError.append("\n");
		this->m_lastError.append( errorText );


		delete [] pLogText;	// Oops

		return true;	// There WAS an error
	}
	return false; // There WASN'T an error
}

bool cShaderManager::m_wasThereALinkError( unsigned int programID, std::string &errorText )
{
	errorText = "";	// No error

	GLint wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if(wasError == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_lastError.append("\n");
		this->m_lastError.append( errorText );


		delete [] pLogText;	

		// There WAS an error
		return true;	
	}
	
	// There WASN'T an error
	return false; 
}


sULInfo::eDataType cShaderManager::getDataTypeFromStr(std::string& str)
{
	std::unordered_map<std::string, sULInfo::eDataType>::iterator uMapIT = map_Str_to_DType.find(str);

	if (uMapIT == map_Str_to_DType.end())
		return sULInfo::NA;
	
	return uMapIT->second;
}



std::string cShaderManager::getLastError(void)
{
	std::string lastErrorTemp = this->m_lastError;
	this->m_lastError = "";
	return lastErrorTemp;
}

#include <iostream>

bool cShaderManager::m_compileShaderFromSource( cShaderManager::cShader &shader, std::string &error )
{
	error = "";

	const unsigned int MAXLINESIZE = 8 * 1024;	// About 8K PER LINE, which seems excessive

	unsigned int numberOfLines = static_cast<unsigned int>(shader.vecSource.size());

	// This is an array of pointers to strings. aka the lines of source
	char** arraySource = new char*[numberOfLines];
	// Clear array to all zeros (i.e. '\0' or null terminator)
	memset( arraySource, 0, numberOfLines );	

	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
	{
		unsigned int numCharacters = (unsigned int)shader.vecSource[indexLine].length();
		// Create an array of chars for each line
		arraySource[indexLine] = new char[numCharacters + 2];		// For the '\n' and '\0' at end
		memset( arraySource[indexLine], 0, numCharacters + 2 );

		// Copy line of source into array
		for ( unsigned int indexChar = 0; indexChar != shader.vecSource[indexLine].length(); indexChar++ )
		{
			arraySource[indexLine][indexChar] = shader.vecSource[indexLine][indexChar];
		}//for ( unsigned int indexChar = 0...
		
		// At a '\0' at end (just in case)
		arraySource[indexLine][numCharacters + 0] = '\n';
		arraySource[indexLine][numCharacters + 1] = '\0';

		// Or you could use the (unsecure) strcpy (string copy)
//		strcpy( arraySource[indexLine], shader.vecSource[indexLine].c_str() );

		// Or the "secure" version, where you specify the number of chars to copy
//		strcpy_s( arraySource[indexLine], 
//				  strlen( shader.vecSource[indexLine].c_str() + 1 ),	// Number of char in copy + 0 terminator
//				  shader.vecSource[indexLine].c_str() );

	}//for ( unsigned int indexLine = 0...

//******************************************************************************************
//	// Print it out (for debug)
//	std::cout << "Source (from char** array):" << std::endl;
//	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
//	{
//		std::cout << "(" << indexLine << "):\t" << arraySource[indexLine] << std::endl;
//	}
//	std::cout << "END" << std::endl;
//******************************************************************************************

	//const char* tempVertChar = "Hey there;";

	glShaderSource(shader.ID, numberOfLines, arraySource, NULL);
    glCompileShader(shader.ID); 

	// Get rid of the temp source "c" style array
	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
	{	// Delete this line
		delete [] arraySource[indexLine];
	}
	// And delete the original char** array
	delete [] arraySource;

	// Did it work? 
	std::string errorText = "";
	if ( this->m_wasThereACompileError( shader.ID, errorText ) )
	{
		std::stringstream ssError;
		ssError << shader.getShaderTypeString();
		ssError << " compile error: ";
		ssError << errorText;
		error = ssError.str();
		return false;
	}

	return true;
}



bool cShaderManager::createProgramFromFile( 
	        std::string friendlyName,
			cShader &vertexShad, 
			cShader &fragShader )
{
	std::string errorText = "";


	// Shader loading happening before vertex buffer array
	vertexShad.ID = glCreateShader(GL_VERTEX_SHADER);
	vertexShad.shaderType = cShader::VERTEX_SHADER;
	//  char* vertex_shader_text = "wewherlkherlkh";
	// Load some text from a file...
	if ( ! this->m_loadSourceFromFile( vertexShad ) )
	{
		return false;
	}//if ( ! this->m_loadSourceFromFile(...

	errorText = "";
	if ( ! this->m_compileShaderFromSource( vertexShad, errorText ) )
	{
		this->m_lastError = errorText;
		return false;
	}//if ( this->m_compileShaderFromSource(...



    fragShader.ID = glCreateShader(GL_FRAGMENT_SHADER);
	fragShader.shaderType = cShader::FRAGMENT_SHADER;
	if ( ! this->m_loadSourceFromFile( fragShader ) )
	{
		return false;
	}//if ( ! this->m_loadSourceFromFile(...

	if ( ! this->m_compileShaderFromSource( fragShader, errorText ) )
	{
		this->m_lastError = errorText;
		return false;
	}//if ( this->m_compileShaderFromSource(...


	cShaderProgram curProgram;
    curProgram.ID = glCreateProgram();

    glAttachShader(curProgram.ID, vertexShad.ID);
    glAttachShader(curProgram.ID, fragShader.ID);
    glLinkProgram(curProgram.ID);

	// Was there a link error? 
	errorText = "";
	if ( this->m_wasThereALinkError( curProgram.ID, errorText ) )
	{
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// At this point, shaders are compiled and linked into a program

	curProgram.friendlyName = friendlyName;


	// Load up UL locations using the vec source
	for (unsigned int i = 0; i < vertexShad.vecSource.size(); i++)
	{
		if (!tryAddUL(vertexShad.vecSource[i], &curProgram)) break;
	}
	for (unsigned int i = 0; i < fragShader.vecSource.size(); i++)
	{
		if (!tryAddUL(fragShader.vecSource[i], &curProgram)) break;
	}

	generateULs(&curProgram);



	// Add the shader to the map
	this->m_ID_to_Shader[curProgram.ID] = curProgram;
	// Save to other map, too
	this->m_name_to_ID[curProgram.friendlyName] = curProgram.ID;

	this->m_mapName_to_Shader[curProgram.friendlyName] = curProgram;


	return true;
}


bool cShaderManager::createComputeProgramFromFile(std::string friendlyName, cShader& computeShader)
{
	std::string errorText = "";


	// Shader loading happening before vertex buffer array
	computeShader.ID = glCreateShader(GL_COMPUTE_SHADER);
	computeShader.shaderType = cShader::COMPUTE_SHADER;
	//  char* vertex_shader_text = "wewherlkherlkh";
	// Load some text from a file...
	if (!this->m_loadSourceFromFile(computeShader))
	{
		return false;
	}//if ( ! this->m_loadSourceFromFile(...

	errorText = "";
	if (!this->m_compileShaderFromSource(computeShader, errorText))
	{
		this->m_lastError = errorText;
		return false;
	}//if ( this->m_compileShaderFromSource(...


	cShaderProgram curProgram;
	curProgram.ID = glCreateProgram();

	glAttachShader(curProgram.ID, computeShader.ID);
	glLinkProgram(curProgram.ID);

	// Was there a link error? 
	errorText = "";
	if (this->m_wasThereALinkError(curProgram.ID, errorText))
	{
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// At this point, shaders are compiled and linked into a program

	curProgram.friendlyName = friendlyName;


	// Load up UL locations using the vec source
	for (unsigned int i = 0; i < computeShader.vecSource.size(); i++)
	{
		if (!tryAddUL(computeShader.vecSource[i], &curProgram)) break;
	}
	for (unsigned int i = 0; i < computeShader.vecSource.size(); i++)
	{
		if (!tryAddUL(computeShader.vecSource[i], &curProgram)) break;
	}

	generateULs(&curProgram);



	// Add the shader to the map
	this->m_ID_to_Shader[curProgram.ID] = curProgram;
	// Save to other map, too
	this->m_name_to_ID[curProgram.friendlyName] = curProgram.ID;

	this->m_mapName_to_Shader[curProgram.friendlyName] = curProgram;


	return true;
}

// Returns false when it hits the void main{    (so no more uniforms)
bool cShaderManager::tryAddUL(std::string& line, cShaderProgram* program) // !!! Does not work with arrays[]
{
	std::stringstream stream(line);
	std::string token;

	bool isUniform = false;

	getline(stream, token, ' ');
	if (token != "uniform")
	{
		if (token == "//EOU") return false; // Right before the main now, no more uniforms
		return true;
	}

	sULInfo* newinfo = new sULInfo();

	// Uniform value found
	getline(stream, token, ' ');

	newinfo->dataType = getDataTypeFromStr(token);

	if (newinfo->dataType == sULInfo::NA)
	{
		delete newinfo;
		std::cerr << "Failed to read uniform data type" << std::endl;
		return true;
	}
	else if (newinfo->dataType == sULInfo::SAMPLER2D)
	{
		newinfo->textureNum = cShaderManager::nextTextureNumber++;
	}

	getline(stream, token, ' ');

	int semicolonIDX = -1;
	bool isArray = false;
	for (int i = 1; i < token.length(); i++)
	{
		if (token[i] == ';')
		{
			semicolonIDX = i;
			break;
		}
		else if (token[i] == '[')
		{
			semicolonIDX = i;
			isArray = true;
			break;
		}
	}
	//token = token.substr(0, semicolonIDX);

	token = isArray ? token.substr(0, semicolonIDX + 1) : token.substr(0, semicolonIDX);

	if (isArray)
	{
		token += "0]";
	}

	program->map_UniformName_to_ULInfo[token] = newinfo;

	return true;
	
}

void cShaderManager::generateULs(cShaderProgram* program)
{
	for (std::unordered_map<std::string, sULInfo*>::iterator mapIT = program->map_UniformName_to_ULInfo.begin();
		mapIT != program->map_UniformName_to_ULInfo.end();
		mapIT++)
	{
		sULInfo* currUL = mapIT->second;
		currUL->UL_Location = glGetUniformLocation(program->ID, (mapIT->first).c_str());
	}
}

cShaderManager::cShaderProgram* cShaderManager::getActiveShader(void)
{
	return m_pActiveProgram;
}

void cShaderManager::cShaderProgram::setULValue(std::string& uniformName, void* val)
{
	sULInfo* ULInfo = getUniformID_From_Name(uniformName);

	if (ULInfo == nullptr)
	{
		std::cerr << "ERROR: Could not find Uniform by name: " << uniformName << std::endl;
		return;
	}

	switch (ULInfo->dataType)
	{
	case sULInfo::VEC4:
	{
		glm::vec4* valueVec = (glm::vec4*)val;
		glUniform4f(ULInfo->UL_Location, valueVec->x, valueVec->y, valueVec->z, valueVec->w);
		break;
	}
	case sULInfo::MAT4:
	{
		glm::mat4* valueMat = (glm::mat4*)val;
		glUniformMatrix4fv(ULInfo->UL_Location, 1, GL_FALSE, glm::value_ptr(*valueMat));
		break;
	}
	case sULInfo::SAMPLER2D:
	{
		GLint textureUnitNumber = ULInfo->textureNum;
		GLuint* Texture03 = (GLuint*)val;//m_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
		glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
		glBindTexture(GL_TEXTURE_2D, *Texture03);
		//GLint texture_03_UL = glGetUniformLocation(shaderProgramID, "texture_03");
		glUniform1i(ULInfo->UL_Location, textureUnitNumber);
		break;
	}
	case sULInfo::SAMPLERCUBE:
		// Might use this later when we use different skyboxes in one instance
		break;
	}
}
