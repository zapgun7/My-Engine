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
	m_pEngineController = cEngineController::GetEngineController();
	// Maybe should initialize string vec to store available save states?
}

bool cSceneManagement::saveScene(char* fileName, std::vector< cMesh* > MeshVec, cLightManager* Lights, std::vector< sPhysicsProperties* > PhysVec)
{
	Document output;
	output.SetObject();

	Value meshes(kArrayType); // Big ol' array to store mesh object data
	Value meshobj(kObjectType); // Each mesh object we add to meshes
	Value string(kObjectType); // Variable to repeatedly use for strings
	Value vec(kArrayType); // Array to hold pos, orientation, etc.
	std::string str;
	glm::vec3 vec3;
	glm::vec2 vec2;
	int num;
	float flt;
	bool state;

	for (unsigned int i = 0; i < MeshVec.size(); i++) //Iterate through all meshes
	{
		// Object filename and friendlyname
		str = MeshVec[i]->meshName; // filename
		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
		meshobj.AddMember("meshName", string, output.GetAllocator()); // Add meshname

		str = MeshVec[i]->friendlyName; // friendlyname
		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
		meshobj.AddMember("friendlyName", string, output.GetAllocator()); // Add friendlyname

		// Texture Info: name array, ratio array
		for (unsigned int e = 0; e < cMesh::NUM_TEXTURES; e++)
		{
			string.SetString(MeshVec[i]->textureName[e].c_str(), static_cast<rapidjson::SizeType>(MeshVec[i]->textureName[e].length()), output.GetAllocator());
			vec.PushBack(string, output.GetAllocator());
		}
		meshobj.AddMember("texNames", vec, output.GetAllocator()); // Add texture names
		vec.SetArray(); // Clear array

		for (unsigned int e = 0; e < cMesh::NUM_TEXTURES; e++)
			vec.PushBack(MeshVec[i]->textureRatios[e], output.GetAllocator());
		meshobj.AddMember("texRatios", vec, output.GetAllocator()); // Add texture names
		vec.SetArray();

		// Pos, orientation(euler)
// 		vec3 = MeshVec[i]->drawPosition; // Draw position
// 		vec.PushBack(vec3.x, output.GetAllocator());
// 		vec.PushBack(vec3.y, output.GetAllocator());
// 		vec.PushBack(vec3.z, output.GetAllocator());
// 		meshobj.AddMember("drawPosition", vec, output.GetAllocator()); // Add drawPosition
// 		vec.SetArray(); // Clear the vec
// 		vec3 = MeshVec[i]->eulerOrientation; // Draw orientation
// 		vec.PushBack(vec3.x, output.GetAllocator());
// 		vec.PushBack(vec3.y, output.GetAllocator());
// 		vec.PushBack(vec3.z, output.GetAllocator());
// 		meshobj.AddMember("eulerOrientation", vec, output.GetAllocator()); // Add eulerOrientation
// 		vec.SetArray(); // Clear the vec

		//////// Now add all the vec2's and 3's //////

		// Custom Color
		vec3 = MeshVec[i]->customColorRGBA; // Custom Color
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		meshobj.AddMember("customColor", vec, output.GetAllocator()); // Add custom color
		vec.SetArray();

		// UV Offset and scale
		vec3 = MeshVec[i]->uv_Offset_Scale;
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		meshobj.AddMember("uv_OS", vec, output.GetAllocator());
		vec.SetArray();
		
		// UV Offset speed
		vec2 = MeshVec[i]->uvOffsetSpeed;
		vec.PushBack(vec2.x, output.GetAllocator());
		vec.PushBack(vec2.y, output.GetAllocator());
		meshobj.AddMember("uv_Spd", vec, output.GetAllocator());
		vec.SetArray();

		// Scale and bools
		flt = MeshVec[i]->scale.x; // scale
		meshobj.AddMember("scale", flt, output.GetAllocator()); // Add scale
		state = MeshVec[i]->bIsVisible;
		meshobj.AddMember("isVisible", state, output.GetAllocator()); // Add isVisible
		state = MeshVec[i]->bIsWireframe;
		meshobj.AddMember("isWireframe", state, output.GetAllocator()); // Add isWireframe
		state = MeshVec[i]->bDoNotLight;
		meshobj.AddMember("doNotLight", state, output.GetAllocator()); // Add doNotLight
		state = MeshVec[i]->bUseCustomColors;
		meshobj.AddMember("useCustomColor", state, output.GetAllocator()); // Add useCustomColor

		// Transparency Alpha
		flt = MeshVec[i]->transparencyAlpha;
		meshobj.AddMember("tAlph", flt, output.GetAllocator());
		// Discard Mask
		state = MeshVec[i]->bUseDiscardMaskTex;
		meshobj.AddMember("dMask", state, output.GetAllocator());
		// Reflect/Refract
		state = MeshVec[i]->bUseReflect;
		meshobj.AddMember("reflect", state, output.GetAllocator());
		state = MeshVec[i]->bUseRefract;
		meshobj.AddMember("refract", state, output.GetAllocator());

		// The id that will bind it to the physics obj
		num = MeshVec[i]->uniqueID;
		meshobj.AddMember("ID", num, output.GetAllocator());


		meshes.PushBack(meshobj, output.GetAllocator()); // Add to array of objects
		meshobj.SetObject(); // Clear mesh object for next iteration
	}
	output.AddMember("meshes", meshes, output.GetAllocator()); // Add array of objects to root object


	// Physics data
	Value phys(kArrayType);
	Value physobj(kObjectType);

	for (unsigned int i = 0; i < PhysVec.size(); i++)
	{
		// Friendly Name
		str = PhysVec[i]->friendlyName; // friendlyname
		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
		physobj.AddMember("friendlyName", string, output.GetAllocator()); // Add friendlyname

		// Shape Type
		num = PhysVec[i]->shapeType;
		physobj.AddMember("shape_t", num, output.GetAllocator());

		// Position
		vec3 = PhysVec[i]->position;
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		physobj.AddMember("pos", vec, output.GetAllocator());
		vec.SetArray();

		// Euler Orientation
		vec3 = PhysVec[i]->get_eOrientation();
		vec.PushBack(vec3.x, output.GetAllocator());
		vec.PushBack(vec3.y, output.GetAllocator());
		vec.PushBack(vec3.z, output.GetAllocator());
		physobj.AddMember("ori", vec, output.GetAllocator());
		vec.SetArray();

		// Unique ID
		num = PhysVec[i]->getUniqueID();
		physobj.AddMember("ID", num, output.GetAllocator());

		// Inverse Mass
		flt = PhysVec[i]->inverse_mass;
		physobj.AddMember("inv_mass", flt, output.GetAllocator());

		phys.PushBack(physobj, output.GetAllocator());
		physobj.SetObject();

		

	}
	output.AddMember("phys", phys, output.GetAllocator());



	// Lights time (so much data oh god; just a lotta vec4s)

	// Creating new ones for naming purposes
	Value lights(kArrayType); // Big ol' array to store light data
	Value lightobj(kObjectType); // Each mesh object we add to meshes
	glm::vec4 vec4;



	for (unsigned int i = 0; i < Lights->NUMBER_OF_LIGHTS_IM_USING; i++) // Iterate through all lights (yes even the ones we're not using)
	{
		str = Lights->theLights[i].friendlyName; // friendlyname
		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
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
	std::vector<sPhysicsProperties*> newPhysVec;

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

		// Texture Names
		itr = meshes[i].FindMember("texNames");
		for (unsigned int e = 0; e < itr->value.Size(); e++)
		{
			newMesh->textureName[e] = itr->value[e].GetString();
		}
		// Texture Ratios
		itr = meshes[i].FindMember("texRatios");
		for (unsigned int e = 0; e < itr->value.Size(); e++)
		{
			newMesh->textureRatios[e] = itr->value[e].GetFloat();
		}


// 		// Position and Orientation
// 		itr = meshes[i].FindMember("drawPosition");
// 		newMesh->drawPosition.x = itr->value[0].GetFloat();
// 		newMesh->drawPosition.y = itr->value[1].GetFloat();
// 		newMesh->drawPosition.z = itr->value[2].GetFloat();
// 		itr = meshes[i].FindMember("eulerOrientation");
// 		newMesh->eulerOrientation.x = itr->value[0].GetFloat();
// 		newMesh->eulerOrientation.y = itr->value[1].GetFloat();
// 		newMesh->eulerOrientation.z = itr->value[2].GetFloat();
		itr = meshes[i].FindMember("customColor");
		newMesh->customColorRGBA.x = itr->value[0].GetFloat();
		newMesh->customColorRGBA.y = itr->value[1].GetFloat();
		newMesh->customColorRGBA.z = itr->value[2].GetFloat();

		// UV offset and scale
		itr = meshes[i].FindMember("uv_OS");
		newMesh->uv_Offset_Scale.x = itr->value[0].GetFloat();
		newMesh->uv_Offset_Scale.y = itr->value[1].GetFloat();
		newMesh->uv_Offset_Scale.z = itr->value[2].GetFloat();

		// UV Offset speed
		itr = meshes[i].FindMember("uv_Spd");
		newMesh->uvOffsetSpeed.x = itr->value[0].GetFloat();
		newMesh->uvOffsetSpeed.y = itr->value[1].GetFloat();


		// Scale and bools
		itr = meshes[i].FindMember("scale");
		newMesh->scale = glm::vec3(itr->value.GetFloat());
		itr = meshes[i].FindMember("isVisible");
		newMesh->bIsVisible = itr->value.GetBool();
		itr = meshes[i].FindMember("isWireframe");
		newMesh->bIsWireframe = itr->value.GetBool();
		itr = meshes[i].FindMember("doNotLight");
		newMesh->bDoNotLight = itr->value.GetBool();
		itr = meshes[i].FindMember("useCustomColor");
		newMesh->bUseCustomColors = itr->value.GetBool();

		// Transparency Alpha
		itr = meshes[i].FindMember("tAlph");
		newMesh->transparencyAlpha = itr->value.GetFloat();

		// Discard Mask
		itr = meshes[i].FindMember("dMask");
		newMesh->bUseDiscardMaskTex = itr->value.GetBool();

		// Reflect/Refract
		itr = meshes[i].FindMember("reflect");
		newMesh->bUseReflect = itr->value.GetBool();
		itr = meshes[i].FindMember("refract");
		newMesh->bUseRefract = itr->value.GetBool();

		// Unique ID from last instance
		itr = meshes[i].FindMember("ID");
		newMesh->uniqueID = itr->value.GetInt();



		//newMesh->setRotationFromEuler(newMesh->eulerOrientation);

		newMeshVec.push_back(newMesh); // Add complete new mesh to vector
	}
	
	// Physics

	const Value& phys = input["phys"]; // Array of physics objects

	for (unsigned int i = 0; i < phys.Size(); i++)
	{
		sPhysicsProperties* newPhys = new sPhysicsProperties();
		newPhys->inverse_mass = -1.0f; // !!! JUST A TEMP BANDAID

		// Friendly Name
		itr = phys[i].FindMember("friendlyName");
		newPhys->friendlyName = itr->value.GetString();

		// Shape Type
		itr = phys[i].FindMember("shape_t");
		newPhys->shapeType = (sPhysicsProperties::eShape)itr->value.GetInt();

		// Position
		itr = phys[i].FindMember("pos");
		newPhys->position.x = itr->value[0].GetFloat();
		newPhys->position.y = itr->value[1].GetFloat();
		newPhys->position.z = itr->value[2].GetFloat();

		// Orientation
		itr = phys[i].FindMember("ori");
		newPhys->setRotationFromEuler(glm::vec3(itr->value[0].GetFloat(), itr->value[1].GetFloat(), itr->value[2].GetFloat()));

		itr = phys[i].FindMember("ID");
		int num = itr->value.GetInt();
		for (unsigned int e = 0; e < newMeshVec.size(); e++) // Bind related mesh to physics object: ID and set associated mesh
		{
			if (newMeshVec[e]->uniqueID == num)
			{
				newMeshVec[e]->uniqueID = newPhys->getUniqueID();
				newPhys->pTheAssociatedMesh = newMeshVec[e];

				// We also properly set the shape here
				switch (newPhys->shapeType)
				{
				case(sPhysicsProperties::SPHERE):
				{
					newPhys->setShape(new sPhysicsProperties::sSphere(1.0f)); // TODO set this to proper radius
					break;
				}
				case(sPhysicsProperties::MESH_OF_TRIANGLES_INDIRECT):
				{
					newPhys->setShape(new sPhysicsProperties::sMeshOfTriangles_Indirect(newPhys->pTheAssociatedMesh->getMeshName()));
					break;
				}
				}

				break;
			}
		}

		if (phys[i].HasMember("inv_mass"))
		{
			itr = phys[i].FindMember("inv_mass");
			float flt = itr->value.GetFloat();
			newPhys->inverse_mass = flt;
		}

		newPhysVec.push_back(newPhys);
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
	//graphicsMain->switchScene(newMeshVec, newLights);
	m_pEngineController->resetScene(newMeshVec, newLights, newPhysVec);

	return;
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
