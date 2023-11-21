#pragma once

#include <vector>
#include <string>
#include "Other Graphics Stuff/cMesh.h"            // Need these two, as we're storing these objects
#include "Other Graphics Stuff/cLightManager.h"    //
//#include "Other Graphics Stuff/cGraphicsMain.h"


class cSceneManagement
{

public:
	cSceneManagement();
	~cSceneManagement();

	void Initialize();

	bool saveScene(char* fileName, std::vector< cMesh* > MeshVec, cLightManager* Lights);
	void loadScene(std::string fileName);
	
	void updateAvailableSaves(); // Checks the txt file that has all saves in it

	std::vector<std::string> getAvailableSaves(); // Returns available filenames to load from

private:
	void addNewSave(char* newSaveName); // Tries to add new save string to txt list, doesn't add it if the same name already exists
	std::vector<std::string> m_AvailableSaves; 
	std::string m_saveFilePath;

};