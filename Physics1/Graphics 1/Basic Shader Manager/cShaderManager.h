#ifndef _cShaderManager_HG_
#define _cShaderManager_HG_

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

struct sULInfo
{
	enum eDataType // New data types must be put in the map in the constructor!!!
	{
		VEC4,
		MAT4,
		SAMPLER2D,
		SAMPLERCUBE,
		NA // Return value when we cannot find in map
	};
	eDataType dataType = NA;
	int UL_Location = -1;
	int textureNum = -1;
};



class cShaderManager
{
public:
	class cShader  {
	public:
		cShader();
		~cShader();
		enum eShaderType
		{
			VERTEX_SHADER,
			FRAGMENT_SHADER,
			COMPUTE_SHADER,
			UNKNOWN
		};
		eShaderType shaderType;
		std::string getShaderTypeString(void);

		unsigned int ID;	// or "name" from OpenGL
		std::vector<std::string> vecSource;
		bool bSourceIsMultiLine;
		std::string fileName;
			
	};

	class cShaderProgram {
	public:
		cShaderProgram() : ID(0) {};
		~cShaderProgram() {};
		unsigned int ID;	// ID from OpenGL (calls it a "name")
		std::string friendlyName;	// We give it this name


		std::unordered_map< std::string, sULInfo*> map_UniformName_to_ULInfo;
		
		// Returns -1 (just like OpenGL) if NOT found
		sULInfo* getUniformID_From_Name(std::string name);
		
		// Look up the uniform location and save it.
		//bool LoadUniformLocation(std::string variableName);
		void setULValue(std::string& uniformName, void* val);
	};

	cShaderManager();
	~cShaderManager();

	bool useShaderProgram( unsigned int ID );
	bool useShaderProgram( std::string friendlyName );
	bool createProgramFromFile( std::string friendlyName, 
		                        cShader &vertexShad, 
					            cShader &fragShader );
	bool createComputeProgramFromFile(std::string friendlyName,
								cShader& computeShader);
	void setBasePath( std::string basepath );
	unsigned int getIDFromFriendlyName( std::string friendlyName );

	// Used to load the uniforms. Returns NULL if not found.
	cShaderProgram* GetShaderProgramFromFriendlyName( std::string friendlyName );

	// !! Does not work with arrays[]
	bool tryAddUL(std::string& line, cShaderProgram* program); // Takes line from the shader text, stores UL info if a uniform
	void generateULs(cShaderProgram* program);
	cShaderProgram* getActiveShader(void);

	// Clears last error
	std::string getLastError(void);
private:
	// Returns an empty string if it didn't work
	bool m_loadSourceFromFile( cShader &shader );
	std::string m_basepath;

	bool m_compileShaderFromSource( cShader &shader, std::string &error );
	// returns false if no error
	bool m_wasThereACompileError( unsigned int shaderID, std::string &errorText );
	bool m_wasThereALinkError( unsigned int progID, std::string &errorText );

	std::string m_lastError;

	std::map< unsigned int /*ID*/, cShaderProgram > m_ID_to_Shader;
	std::map< std::string /*name*/, unsigned int /*ID*/ > m_name_to_ID;
	std::unordered_map<std::string, cShaderProgram> m_mapName_to_Shader;

	std::unordered_map<std::string, sULInfo::eDataType> map_Str_to_DType;
	cShaderProgram* m_pActiveProgram;

	// Little Helper Functions
	sULInfo::eDataType getDataTypeFromStr(std::string& str);


	static int nextTextureNumber;
};

#endif