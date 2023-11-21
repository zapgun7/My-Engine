#include "cSceneManagement.h"


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Other Graphics Stuff/cGraphicsMain.h"

#include <iostream>
#include <fstream>
#include <cstdio>

#pragma warning(disable : 4996)

using namespace rapidjson;

cSceneManagement::cSceneManagement()
{

}

cSceneManagement::~cSceneManagement()
{

}

void cSceneManagement::Initialize()
{
	//m_GraphicsMain = cGraphicsMain::getGraphicsMain();
	m_saveFilePath = "../saves/";
	updateAvailableSaves();
	// Maybe should initialize string vec to store available save states?
}

bool cSceneManagement::saveScene(char* fileName, std::vector< cMesh* > MeshVec, cLightManager* Lights)
{
	Document output;
	output.SetObject();

	Value meshes(kArrayType); // Big ol' array to store mesh object data
	Value meshobj(kObjectType); // Each mesh object we add to meshes
	Value string(kObjectType); // Variable to repeatedly use for strings
	Value vec(kArrayType); // Array to hold pos, orientation, etc.
	std::string str;
	glm::vec3 vec3;
	float num;
	bool state;

	for (unsigned int i = 0; i < MeshVec.size(); i++) //Iterate through all meshes
	{
		// Object filename and friendlyname
		str = MeshVec[i]->meshName; // filename
		string.SetString(str.c_str(), str.length(), output.GetAllocator());
		meshobj.AddMember("meshName", string, output.GetAllocator()); // Add meshname
		str = MeshVec[i]->friendlyName; // friendlyname
		string.SetString(str.c_str(), str.length(), output.GetAllocator());
		meshobj.AddMember("friendlyName", string, output.GetAllocator()); // Add friendlyname
		// Pos, orientation(euler)
		vec3 = MeshVec[i]->drawPosition; // Draw position
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		meshobj.AddMember("drawPosition", vec, output.GetAllocator()); // Add drawPosition
		vec.SetArray(); // Clear the vec
		vec3 = MeshVec[i]->eulerOrientation; // Draw orientation
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		meshobj.AddMember("eulerOrientation", vec, output.GetAllocator()); // Add eulerOrientation
		vec.SetArray(); // Clear the vec

		vec3 = MeshVec[i]->wholeObjectDebugColourRGBA; // Custom Color
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		meshobj.AddMember("customColor", vec, output.GetAllocator()); // Add custom color
		vec.SetArray(); // Clear the vec

		// Scale and bools
		num = MeshVec[i]->scale.x; // scale
		meshobj.AddMember("scale", num, output.GetAllocator()); // Add scale
		state = MeshVec[i]->bIsVisible;
		meshobj.AddMember("isVisible", state, output.GetAllocator()); // Add isVisible
		state = MeshVec[i]->bIsWireframe;
		meshobj.AddMember("isWireframe", state, output.GetAllocator()); // Add isWireframe
		state = MeshVec[i]->bDoNotLight;
		meshobj.AddMember("doNotLight", state, output.GetAllocator()); // Add doNotLight
		state = MeshVec[i]->bUseDebugColours;
		meshobj.AddMember("useCustomColor", state, output.GetAllocator()); // Add useCustomColor

		meshes.PushBack(meshobj, output.GetAllocator()); // Add to array of objects
		meshobj.SetObject(); // Clear mesh object for next iteration
	}
	output.AddMember("meshes", meshes, output.GetAllocator()); // Add array of objects to root object


	// Lights time (so much data oh god; just a lotta vec4s)

	// Creating new ones for naming purposes
	Value lights(kArrayType); // Big ol' array to store light data
	Value lightobj(kObjectType); // Each mesh object we add to meshes
	glm::vec4 vec4;



	for (unsigned int i = 0; i < Lights->NUMBER_OF_LIGHTS_IM_USING; i++) // Iterate through all lights (yes even the ones we're not using)
	{
		str = Lights->theLights[i].friendlyName; // friendlyname
		string.SetString(str.c_str(), str.length(), output.GetAllocator());
		lightobj.AddMember("friendlyname", string, output.GetAllocator()); // Add friendlyname
		// Position
		vec4 = Lights->theLights[i].position;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("position", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Diffuse
		vec4 = Lights->theLights[i].diffuse;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("diffuse", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Specular
		vec4 = Lights->theLights[i].specular;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("specular", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Attenuation
		vec4 = Lights->theLights[i].atten;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("atten", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Direction
		vec4 = Lights->theLights[i].direction;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("direction", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Param1
		vec4 = Lights->theLights[i].param1;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("param1", vec, output.GetAllocator());
		vec.SetArray(); // Clear 
		// Param2
		vec4 = Lights->theLights[i].param2;
		vec.PushBack(vec4.x, output.GetAllocator());
		vec.PushBack(vec4.y, output.GetAllocator());
		vec.PushBack(vec4.z, output.GetAllocator());
		vec.PushBack(vec4.w, output.GetAllocator());
		lightobj.AddMember("param2", vec, output.GetAllocator());
		vec.SetArray(); // Clear 


		lights.PushBack(lightobj, output.GetAllocator()); // Add to array of lights
		lightobj.SetObject(); // Clear light object for next iteration
	}
	output.AddMember("lights", lights, output.GetAllocator()); // Add array of objects to root object


	std::ofstream outputFile;
	outputFile.open(m_saveFilePath + fileName + ".json"); // Will open file at this location if existing
														  // Will create a new one if not
	StringBuffer strbuf;
	PrettyWriter<StringBuffer> writer(strbuf); // Formats string... prettily

	output.Accept(writer);

	outputFile << strbuf.GetString();

	outputFile.close();

	addNewSave(fileName);

	return true;
}
// 
// 
//

void cSceneManagement::loadScene(std::string fileName)
{
	cGraphicsMain* graphicsMain = cGraphicsMain::getGraphicsMain(); // Get the one and only graphics main
	std::vector<cMesh*> newMeshVec; // New mesh vector to replace the old
	std::vector<cLight> newLights; // New light data to be copied over to existing light manager

	FILE* fp = fopen((m_saveFilePath + fileName + ".json").c_str(), "rb");
	//char readBuffer[65536];
	char* readBuffer = new char[65536]; // heap, cuz big!
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document input;
	input.ParseStream(is);
	const Value& meshes = input["meshes"]; // Array of mesh objects
	Value::ConstMemberIterator itr; // Variable to iterate over mesh object members
	for (unsigned int i = 0; i < meshes.Size(); i++)
	{
		cMesh* newMesh = new cMesh(); // Add to this before pushing into the vector
		itr = meshes[i].FindMember("meshName");
		newMesh->meshName = itr->value.GetString(); // Set meshname
		itr = meshes[i].FindMember("friendlyName");
		newMesh->friendlyName = itr->value.GetString(); // Set friendlyName
		// Position and Orientation
		itr = meshes[i].FindMember("drawPosition");
		newMesh->drawPosition.x = itr->value[0].GetFloat();
		newMesh->drawPosition.y = itr->value[1].GetFloat();
		newMesh->drawPosition.z = itr->value[2].GetFloat();
		itr = meshes[i].FindMember("eulerOrientation");
		newMesh->eulerOrientation.x = itr->value[0].GetFloat();
		newMesh->eulerOrientation.y = itr->value[1].GetFloat();
		newMesh->eulerOrientation.z = itr->value[2].GetFloat();
		itr = meshes[i].FindMember("customColor");
		newMesh->wholeObjectDebugColourRGBA.x = itr->value[0].GetFloat();
		newMesh->wholeObjectDebugColourRGBA.y = itr->value[1].GetFloat();
		newMesh->wholeObjectDebugColourRGBA.z = itr->value[2].GetFloat();
		// Scale and bools
		//itr = meshes[i].FindMember("scale");
		//newMesh->scale = itr->value.GetFloat();
		itr = meshes[i].FindMember("isVisible");
		newMesh->bIsVisible = itr->value.GetBool();
		itr = meshes[i].FindMember("isWireframe");
		newMesh->bIsWireframe = itr->value.GetBool();
		itr = meshes[i].FindMember("doNotLight");
		newMesh->bDoNotLight = itr->value.GetBool();
		itr = meshes[i].FindMember("useCustomColor");
		newMesh->bUseDebugColours = itr->value.GetBool();

		newMesh->setRotationFromEuler(newMesh->eulerOrientation);

		newMeshVec.push_back(newMesh); // Add complete new mesh to vector
	}

	// Lights

	const Value& lights = input["lights"]; // Array of light objects

	for (unsigned int i = 0; i < lights.Size(); i++)
	{
		cLight newLight;
		itr = lights[i].FindMember("friendlyname");
		newLight.friendlyName = itr->value.GetString(); // Set friendlyname
		// vec4 values
		itr = lights[i].FindMember("position");
		newLight.position.x = itr->value[0].GetFloat();
		newLight.position.y = itr->value[1].GetFloat();
		newLight.position.z = itr->value[2].GetFloat();
		newLight.position.w = itr->value[3].GetFloat();
		itr = lights[i].FindMember("diffuse");
		newLight.diffuse.x = itr->value[0].GetFloat();
		newLight.diffuse.y = itr->value[1].GetFloat();
		newLight.diffuse.z = itr->value[2].GetFloat();
		newLight.diffuse.w = itr->value[3].GetFloat();
		itr = lights[i].FindMember("specular");
		newLight.specular.x = itr->value[0].GetFloat();
		newLight.specular.y = itr->value[1].GetFloat();
		newLight.specular.z = itr->value[2].GetFloat();
		newLight.specular.w = itr->value[3].GetFloat();
		itr = lights[i].FindMember("atten");
		newLight.atten.x = itr->value[0].GetFloat();
		newLight.atten.y = itr->value[1].GetFloat();
		newLight.atten.z = itr->value[2].GetFloat();
		newLight.atten.w = itr->value[3].GetFloat();
		itr = lights[i].FindMember("direction");
		newLight.direction.x = itr->value[0].GetFloat();
		newLight.direction.y = itr->value[1].GetFloat();
		newLight.direction.z = itr->value[2].GetFloat();
		newLight.direction.w = itr->value[3].GetFloat();
		itr = lights[i].FindMember("param1");
		newLight.param1.x = itr->value[0].GetFloat();
		newLight.param1.y = itr->value[1].GetFloat();
		newLight.param1.z = itr->value[2].GetFloat();
		newLight.param1.w = itr->value[3].GetFloat();
		itr = lights[i].FindMember("param2");
		newLight.param2.x = itr->value[0].GetFloat();
		newLight.param2.y = itr->value[1].GetFloat();
		newLight.param2.z = itr->value[2].GetFloat();
		newLight.param2.w = itr->value[3].GetFloat();

		newLights.push_back(newLight); // Add completed light object to array
	}
	fclose(fp);

	//is.Flush();
	//delete fp;   // deleting this breaks subsequent loads
	delete[] readBuffer;
	graphicsMain->switchScene(newMeshVec, newLights);


}

// Refreshes the private vector of available saves
void cSceneManagement::updateAvailableSaves()
{
	std::ifstream savelistFile(m_saveFilePath + "savelist.txt");
	if (!savelistFile.is_open())
	{
		std::cout << "ERROR: Save file not found" << std::endl;
		return;
	}
	std::string line = ""; // Variable to parse each line
	std::vector<std::string> newAvailSaves; // New vector to copy over to the private one
	while (std::getline(savelistFile, line))
	{
		if (line.length() > 0)
			newAvailSaves.push_back(line);
	}

	savelistFile.close();
	m_AvailableSaves = newAvailSaves;

	return;
}

std::vector<std::string> cSceneManagement::getAvailableSaves()
{
	return m_AvailableSaves;
}

// Add new save name if name doesn't already exist
void cSceneManagement::addNewSave(char* newSaveName)
{
	std::ifstream savelistFile(m_saveFilePath + "savelist.txt");
	if (!savelistFile.is_open())
	{
		std::cout << "ERROR: Save file not found" << std::endl;
		return;
	}
	std::string line = ""; // Variable to parse each line
	std::string newFileCont = ""; // Data to store new file contents   !! might not use
	while (std::getline(savelistFile, line))
	{
		if (line == newSaveName)
			return;
	}

	savelistFile.close();

	std::ofstream slFileOut;
	slFileOut.open(m_saveFilePath + "savelist.txt", std::ios_base::app); // Letting output know we're appending
	slFileOut << (std::string(newSaveName) + "\n");
	slFileOut.close();

	updateAvailableSaves();

	return;
}
