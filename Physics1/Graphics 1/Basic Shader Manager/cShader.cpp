#include "cShaderManager.h"
#include "../Other Graphics Stuff/OpenGLCommon.h"	// glfw.h, etc. (openGL.h)

cShaderManager::cShader::cShader()
{
	this->ID = 0;
	this->shaderType = cShader::UNKNOWN;
	return;
}

cShaderManager::cShader::~cShader() 
{
	return;
}

std::string cShaderManager::cShader::getShaderTypeString(void)
{
	switch ( this->shaderType )
	{
	case cShader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case cShader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case cShader::COMPUTE_SHADER:
		return "COMPUTE_SHADER";
		break;
	case cShader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}


//// TODO: For the students to do, because it's FUN, FUN, FUN
//std::map< std::string /*name of uniform variable*/,
//		    unsigned int /* uniform location ID */ > 
//						mapUniformName_to_UniformLocation;

// Look up the uniform inside the shader, then save it, if it finds it
// bool cShaderManager::cShaderProgram::LoadUniformLocation(std::string variableName)
// {
// 	// 
// 	GLint uniLocation = glGetUniformLocation(this->ID, 
// 											 variableName.c_str() );
// 	// Did it find it (not -1)
// 	if ( uniLocation == -1 )
// 	{	// Nope.
// 		return false;
// 	}
// 	// Save it
// 	this->map_UniformName_to_ULInfo[variableName.c_str()] = uniLocation;
// 
// 	return true;	
// }

// Look up the uniform location and save it.
sULInfo* cShaderManager::cShaderProgram::getUniformID_From_Name(std::string name)
{
	std::unordered_map< std::string, sULInfo* >::iterator itUniform = this->map_UniformName_to_ULInfo.find(name);

	if ( itUniform == this->map_UniformName_to_ULInfo.end() )
	{
		return nullptr;		// OpenGL uniform not found value
	}

	return itUniform->second;		// second if the "int" value
}
