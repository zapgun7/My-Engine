#pragma once

// This is the 'implementation' class for the PIMPL
// or "pointer to implementation" pattern. 
// 
// This one is completely hidden by another class. 
// This is because C++ doesn't have the concept of 'partial classes'
// so we put all the 'guts' of the code, including private stuff, here. 
// 
// This way we can't chage this class all we want and not force a re-compile. 

#include <string>
#include "cAssimpHelper.h"

class cFileLoader_Imp
{
public:
	cFileLoader_Imp();
	~cFileLoader_Imp();

	bool Load3DModelFile(AH::cString filename, AH::cFileLoader::sPostProcessFlags postProcessOptions);
	void SetBasePath(AH::cString basepath_no_end_slash);

	AH::cString getLastError(bool bAndClearErrors = true);


private:
	std::string m_basePath_no_end_slash;

	void m_AppendErrorString(std::string errorString);
	std::string m_LastError;

	// Converts the boolean helper structure into the assimp flags:
	// https://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410a8857a0e30688127a82c7b8939958c6dc
	unsigned int m_loadAssimpPostProcessingFlags(AH::cFileLoader::sPostProcessFlags postProcessOptions);

	unsigned int numberOfModelLoaded;

	// Convert the Assimp Helper cString (exported by DLL) to std::string
	std::string m_cString_to_std_string(AH::cString theString);
	AH::cString m_std_string_to_cString(std::string theString);

};

