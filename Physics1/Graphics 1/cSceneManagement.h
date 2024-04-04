#pragma once

#include <vector>
#include <string>
#include "Other Graphics Stuff/cMesh.h"            // Need these three, as we're storing these objects
#include "Other Graphics Stuff/cLightManager.h"    //
#include "Physics/sPhysicsProperties.h"
#include "cEngineController.h"
//#include "Other Graphics Stuff/cGraphicsMain.h"


class cSceneManagement
{

public:
	enum eDataType
	{
		STRING,
		BOOL,
		VEC2,
		VEC3,
		VEC4,
		FLOAT,
		INT
	};

	cSceneManagement();
	~cSceneManagement();

	void Initialize();

	bool saveScene(char* fileName, std::vector< cMesh* > MeshVec, cLightManager* Lights, std::vector< sPhysicsProperties* > PhysVec);
	void loadScene(std::string fileName);
	
	void updateAvailableSaves(); // Checks the txt file that has all saves in it

	std::vector<std::string> getAvailableSaves(); // Returns available filenames to load from

private:

	void addNewSave(char* newSaveName); // Tries to add new save string to txt list, doesn't add it if the same name already exists

	std::vector<std::string> m_AvailableSaves; 
	std::string m_saveFilePath;
	cEngineController* m_pEngineController;

};