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

bool AddMemberToObject(Value& obj, std::string memberName, void* data, cSceneManagement::eDataType type, Document& output);
bool LoadDataFromMember(const Value& obj, std::string memberName, void* data, cSceneManagement::eDataType type, Document& output);

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


bool AddMemberToObject(Value& obj, std::string memberName, void* data, cSceneManagement::eDataType type, Document& output)
{
	Value memName(memberName.c_str(), output.GetAllocator());

	if (type == cSceneManagement::STRING)
	{
		std::string* strData = (std::string*)data;
		Value string(kObjectType);
		string.SetString(strData->c_str(), static_cast<SizeType>(strData->length()), output.GetAllocator());
		obj.AddMember(memName, string, output.GetAllocator());
	}
	else if (type == cSceneManagement::BOOL)
	{
		bool* boolData = (bool*)data;
		obj.AddMember(memName, *boolData, output.GetAllocator());
	}
	else if (type == cSceneManagement::FLOAT)
	{
		float* fltData = (float*)data;
		obj.AddMember(memName, *fltData, output.GetAllocator());
	}
	else if (type == cSceneManagement::INT)
	{
		int* intData = (int*)data;
		obj.AddMember(memName, *intData, output.GetAllocator());
	}
	else if (type == cSceneManagement::VEC2)
	{
		glm::vec2* vecData = (glm::vec2*)data;
		Value vec(kArrayType);
		vec.PushBack(vecData->x, output.GetAllocator());
		vec.PushBack(vecData->y, output.GetAllocator());
		obj.AddMember(memName, vec, output.GetAllocator());
	}
	else if (type == cSceneManagement::VEC3)
	{
		glm::vec3* vecData = (glm::vec3*)data;
		Value vec(kArrayType);
		vec.PushBack(vecData->x, output.GetAllocator());
		vec.PushBack(vecData->y, output.GetAllocator());
		vec.PushBack(vecData->z, output.GetAllocator());
		obj.AddMember(memName, vec, output.GetAllocator());
	}
	else if (type == cSceneManagement::VEC4)
	{
		glm::vec4* vecData = (glm::vec4*)data;
		Value vec(kArrayType);
		vec.PushBack(vecData->x, output.GetAllocator());
		vec.PushBack(vecData->y, output.GetAllocator());
		vec.PushBack(vecData->z, output.GetAllocator());
		vec.PushBack(vecData->w, output.GetAllocator());
		obj.AddMember(memName, vec, output.GetAllocator());
	}


	return true;
}

bool LoadDataFromMember(const Value& obj, std::string memberName, void* data, cSceneManagement::eDataType type, Document& output)
{
	Value memName(memberName.c_str(), output.GetAllocator());
	Value::ConstMemberIterator itr;

	if (type == cSceneManagement::STRING)
	{
		std::string* strData = (std::string*)data;
		itr = obj.FindMember(memName);
		if (itr == obj.MemberEnd()) return false;

		*strData = itr->value.GetString();
	}
	else if (type == cSceneManagement::BOOL)
	{
		bool* boolData = (bool*)data;
		itr = obj.FindMember(memName);
		if (itr == obj.MemberEnd()) return false;

		*boolData = itr->value.GetBool();
	}
	else if (type == cSceneManagement::FLOAT)
	{
		float* fltData = (float*)data;
		itr = obj.FindMember(memName);
		if (itr == obj.MemberEnd()) return false;
		
		*fltData = itr->value.GetFloat();
	}
	else if (type == cSceneManagement::INT)
	{
		int* intData = (int*)data;
		itr = obj.FindMember(memName);
		if (itr == obj.MemberEnd()) return false;

		*intData = itr->value.GetInt64();
	}
	else if (type == cSceneManagement::VEC2)
	{
		glm::vec2* vecData = (glm::vec2*)data;
		itr = obj.FindMember(memName);
		if (itr == obj.MemberEnd()) return false;

		vecData->x = itr->value[0].GetFloat();
		vecData->y = itr->value[1].GetFloat();
	}
	else if (type == cSceneManagement::VEC3)
	{
		glm::vec3* vecData = (glm::vec3*)data;
		itr = obj.FindMember(memName);
		if (itr == obj.MemberEnd()) return false;

		vecData->x = itr->value[0].GetFloat();
		vecData->y = itr->value[1].GetFloat();
		vecData->z = itr->value[2].GetFloat();
	}
	else if (type == cSceneManagement::VEC4)
	{
		glm::vec4* vecData = (glm::vec4*)data;
		itr = obj.FindMember(memName);
		if (itr == obj.MemberEnd()) return false;

		vecData->x = itr->value[0].GetFloat();
		vecData->y = itr->value[1].GetFloat();
		vecData->z = itr->value[2].GetFloat();
		vecData->w = itr->value[3].GetFloat();
	}

	return true;
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
	glm::vec4 vec4;
	glm::vec3 vec3;
	glm::vec2 vec2;
	int num;
	float flt;
	bool state;

	for (unsigned int i = 0; i < MeshVec.size(); i++) //Iterate through all meshes
	{
		AddMemberToObject(meshobj, "meshName", &MeshVec[i]->meshName, STRING, output);
		
		AddMemberToObject(meshobj, "friendlyName", &MeshVec[i]->friendlyName, STRING, output);

		AddMemberToObject(meshobj, "isNav", &MeshVec[i]->isNavMesh, BOOL, output);

		// Texture Info: name array, ratio array
// 		for (unsigned int e = 0; e < cMesh::NUM_TEXTURES; e++)
// 		{
// 			string.SetString(MeshVec[i]->textureName[e].c_str(), static_cast<rapidjson::SizeType>(MeshVec[i]->textureName[e].length()), output.GetAllocator());
// 			vec.PushBack(string, output.GetAllocator());
// 		}
// 		meshobj.AddMember("texNames", vec, output.GetAllocator()); // Add texture names
// 		vec.SetArray(); // Clear array
// 
// 		for (unsigned int e = 0; e < cMesh::NUM_TEXTURES; e++)
// 			vec.PushBack(MeshVec[i]->textureRatios[e], output.GetAllocator());
// 		meshobj.AddMember("texRatios", vec, output.GetAllocator()); // Add texture names
// 		vec.SetArray();


		/// MATERIAL ///
		AddMemberToObject(meshobj, "mat_difftex", &MeshVec[i]->material.diffuseTex, STRING, output);
		AddMemberToObject(meshobj, "mat_spectex", &MeshVec[i]->material.specularTex, STRING, output);
		AddMemberToObject(meshobj, "mat_diffmirr", &MeshVec[i]->material.isDiffMirrored, BOOL, output);
		AddMemberToObject(meshobj, "mat_specmirr", &MeshVec[i]->material.isSpecMirrored, BOOL, output);

		AddMemberToObject(meshobj, "diffUV_data", &MeshVec[i]->material.diffuv_Offset_Scale, VEC3, output);
		AddMemberToObject(meshobj, "specUV_data", &MeshVec[i]->material.specuv_Offset_Scale, VEC3, output);

		AddMemberToObject(meshobj, "mat_power", &MeshVec[i]->material.power, VEC4, output);
		///// //////// /////// /////

		AddMemberToObject(meshobj, "customColor", &MeshVec[i]->customColorRGBA, VEC3, output);
		//AddMemberToObject(meshobj, "uv_OS", &MeshVec[i]->uv_Offset_Scale, VEC3, output);
		//AddMemberToObject(meshobj, "uv_Spd", &MeshVec[i]->uv_Offset_Scale, VEC2, output);
		

		AddMemberToObject(meshobj, "scale", &MeshVec[i]->scale, VEC3, output);
		AddMemberToObject(meshobj, "isVisible", &MeshVec[i]->bIsVisible, BOOL, output);
		AddMemberToObject(meshobj, "isWireframe", &MeshVec[i]->bIsWireframe, BOOL, output);
		AddMemberToObject(meshobj, "doNotLight", &MeshVec[i]->bDoNotLight, BOOL, output);
		AddMemberToObject(meshobj, "useCustomColor", &MeshVec[i]->bUseCustomColors, BOOL, output);

		// Transparency Alpha
		AddMemberToObject(meshobj, "tAlph", &MeshVec[i]->transparencyAlpha, FLOAT, output);
		// Discard Mask
		AddMemberToObject(meshobj, "dMask", &MeshVec[i]->bUseDiscardMaskTex, BOOL, output);
		// Reflect/Refract
		AddMemberToObject(meshobj, "reflect", &MeshVec[i]->bUseReflect, BOOL, output);
		AddMemberToObject(meshobj, "refract", &MeshVec[i]->bUseRefract, BOOL, output);

		// The id that will bind it to the physics obj
		AddMemberToObject(meshobj, "ID", &MeshVec[i]->uniqueID, INT, output);


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
		AddMemberToObject(physobj, "friendlyName", &PhysVec[i]->friendlyName, STRING, output);

		// Shape Type
		AddMemberToObject(physobj, "shape_t", &PhysVec[i]->shapeType, INT, output);

		// Position
		AddMemberToObject(physobj, "pos", &PhysVec[i]->position, VEC3, output);


		// Euler Orientation
 		vec3 = PhysVec[i]->get_eOrientation();
		AddMemberToObject(physobj, "ori", &vec3, VEC3, output);

		// Unique ID
		num = PhysVec[i]->getUniqueID();
		AddMemberToObject(physobj, "ID", &num, INT, output);

		// Inverse Mass
		AddMemberToObject(physobj, "inv_mass", &PhysVec[i]->inverse_mass, FLOAT, output);


		if (PhysVec[i]->shapeType == sPhysicsProperties::HITBOX)
		{
			sPhysicsProperties::sHitBox* hb = (sPhysicsProperties::sHitBox*)PhysVec[i]->pShape;
			AddMemberToObject(physobj, "srcPos", &hb->srcCentre, VEC3, output);
			AddMemberToObject(physobj, "dstPos", &hb->dstCentre, VEC3, output);
			AddMemberToObject(physobj, "boxscle", &hb->scale, FLOAT, output);
		}

		// Add this physics object to the file
		phys.PushBack(physobj, output.GetAllocator());
		physobj.SetObject();
	}
	output.AddMember("phys", phys, output.GetAllocator());



	// Lights time (so much data oh god; just a lotta vec4s)

	// Creating new ones for naming purposes
	Value lights(kArrayType); // Big ol' array to store light data
	Value lightobj(kObjectType); // Each mesh object we add to meshes



	for (unsigned int i = 0; i < Lights->NUMBER_OF_LIGHTS_IM_USING; i++) // Iterate through all lights (yes even the ones we're not using)
	{
		AddMemberToObject(lightobj, "friendlyname", &Lights->theLights[i].friendlyName, STRING, output);

		// Position
		AddMemberToObject(lightobj, "position", &Lights->theLights[i].position, VEC4, output);
		// Diffuse
		AddMemberToObject(lightobj, "diffuse", &Lights->theLights[i].diffuse, VEC4, output);
		// Specular
		AddMemberToObject(lightobj, "specular", &Lights->theLights[i].specular, VEC4, output);
		// Attenuation
		AddMemberToObject(lightobj, "atten", &Lights->theLights[i].atten, VEC4, output);
		// Direction
		AddMemberToObject(lightobj, "direction", &Lights->theLights[i].direction, VEC4, output);
		// Param1
		AddMemberToObject(lightobj, "param1", &Lights->theLights[i].param1, VEC4, output);
		// Param2
		AddMemberToObject(lightobj, "param2", &Lights->theLights[i].param2, VEC4, output);


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

		LoadDataFromMember(meshes[i], "meshName", &newMesh->meshName, STRING, input);
		LoadDataFromMember(meshes[i], "friendlyName", &newMesh->friendlyName, STRING, input);

		LoadDataFromMember(meshes[i], "isNav", &newMesh->isNavMesh, BOOL, input);

		// Texture Names
// 		itr = meshes[i].FindMember("texNames");
// 		for (unsigned int e = 0; e < itr->value.Size(); e++)
// 		{
// 			newMesh->textureName[e] = itr->value[e].GetString();
// 		}
// 		// Texture Ratios
// 		itr = meshes[i].FindMember("texRatios");
// 		for (unsigned int e = 0; e < itr->value.Size(); e++)
// 		{
// 			newMesh->textureRatios[e] = itr->value[e].GetFloat();
// 		}


		LoadDataFromMember(meshes[i], "mat_power", &newMesh->material.power, VEC4, input);
		
		// Diffuse/Specular Texture Names
		if (!LoadDataFromMember(meshes[i], "mat_difftex", &newMesh->material.diffuseTex, STRING, input))
		{
			//LoadDataFromMember(meshes[i], "mat_difftex", &newMesh->material.diffuseTex, STRING, input); // Load from the old texture array
		}

		LoadDataFromMember(meshes[i], "mat_spectex", &newMesh->material.specularTex, STRING, input);

		// Is Mirrored
		LoadDataFromMember(meshes[i], "mat_diffmirr", &newMesh->material.isDiffMirrored, BOOL, input);
		LoadDataFromMember(meshes[i], "mat_specmirr", &newMesh->material.isSpecMirrored, BOOL, input);

		//AddMemberToObject(meshobj, "mat_power", &MeshVec[i]->material.power, VEC4, output);
		LoadDataFromMember(meshes[i], "mat_power", &newMesh->material.power, VEC4, input);

		///// ///////// //// /////



		LoadDataFromMember(meshes[i], "customColor", &newMesh->customColorRGBA, VEC3, input);

		// UV offset and scale
		//LoadDataFromMember(meshes[i], "uv_OS", &newMesh->uv_Offset_Scale, VEC3, input);
		LoadDataFromMember(meshes[i], "diffUV_data", &newMesh->material.diffuv_Offset_Scale, VEC3, input);
		LoadDataFromMember(meshes[i], "specUV_data", &newMesh->material.specuv_Offset_Scale, VEC3, input);
// 		AddMemberToObject(meshobj, "diffUV_data", &MeshVec[i]->material.diffuv_Offset_Scale, VEC3, output);
// 		AddMemberToObject(meshobj, "specUV_data", &MeshVec[i]->material.specuv_Offset_Scale, VEC3, output);

		// UV Offset speed
		//LoadDataFromMember(meshes[i], "uv_Spd", &newMesh->uvOffsetSpeed, VEC2, input);


		// Scale and bools
		LoadDataFromMember(meshes[i], "scale", &newMesh->scale, VEC3, input);
		LoadDataFromMember(meshes[i], "isVisible", &newMesh->bIsVisible, BOOL, input);
		LoadDataFromMember(meshes[i], "isWireframe", &newMesh->bIsWireframe, BOOL, input);
		LoadDataFromMember(meshes[i], "doNotLight", &newMesh->bDoNotLight, BOOL, input);
		LoadDataFromMember(meshes[i], "useCustomColor", &newMesh->bUseCustomColors, BOOL, input);

		// Transparency Alpha
		LoadDataFromMember(meshes[i], "tAlph", &newMesh->transparencyAlpha, FLOAT, input);

		// Discard Mask
		LoadDataFromMember(meshes[i], "dMask", &newMesh->transparencyAlpha, BOOL, input);

		// Reflect/Refract
		LoadDataFromMember(meshes[i], "reflect", &newMesh->bUseReflect, BOOL, input);
		LoadDataFromMember(meshes[i], "refract", &newMesh->bUseRefract, BOOL, input);

		// Unique ID from last instance
		LoadDataFromMember(meshes[i], "ID", &newMesh->uniqueID, INT, input);



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
		LoadDataFromMember(phys[i], "friendlyName", &newPhys->friendlyName, STRING, input);

		// Shape Type
		int temp;
		if (LoadDataFromMember(phys[i], "shape_t", &temp, INT, input))
			newPhys->shapeType = (sPhysicsProperties::eShape)temp;

		// Position
		LoadDataFromMember(phys[i], "pos", &newPhys->position, VEC3, input);

		// Orientation
		glm::vec3 tempVec;
		if (LoadDataFromMember(phys[i], "ori", &tempVec, VEC3, input))
		{
			newPhys->setRotationFromEuler(tempVec);
		}

		LoadDataFromMember(phys[i], "inv_mass", &newPhys->inverse_mass, FLOAT, input);


		//itr = phys[i].FindMember("ID");
		int num;// = itr->value.GetInt();
		LoadDataFromMember(phys[i], "ID", &num, INT, input);

		if (newPhys->shapeType != sPhysicsProperties::HITBOX)
		{
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
		}


		if (newPhys->shapeType == sPhysicsProperties::HITBOX)
		{

			sPhysicsProperties::sHitBox* hb = new sPhysicsProperties::sHitBox();
			newPhys->setShape(hb);
			LoadDataFromMember(phys[i], "srcPos", &hb->srcCentre, VEC3, input);
			LoadDataFromMember(phys[i], "dstPos", &hb->dstCentre, VEC3, input);
			LoadDataFromMember(phys[i], "boxscle", &hb->scale, FLOAT, input);
		}



		// Add completed physics object to the array
		newPhysVec.push_back(newPhys);
	}


	// Lights

	const Value& lights = input["lights"]; // Array of light objects

	for (unsigned int i = 0; i < lights.Size(); i++)
	{
		cLight newLight;
// 		itr = lights[i].FindMember("friendlyname");
// 		newLight.friendlyName = itr->value.GetString(); // Set friendlyname
		LoadDataFromMember(lights[i], "friendlyname", &newLight.friendlyName, STRING, input);
		// Position
		LoadDataFromMember(lights[i], "position", &newLight.position, VEC4, input);
		// Diffuse
		LoadDataFromMember(lights[i], "diffuse", &newLight.diffuse, VEC4, input);
		// Specular
		LoadDataFromMember(lights[i], "specular", &newLight.specular, VEC4, input);
		// Attenuation
		LoadDataFromMember(lights[i], "atten", &newLight.atten, VEC4, input);
		// Direction
		LoadDataFromMember(lights[i], "direction", &newLight.direction, VEC4, input);
		// Param 1
		LoadDataFromMember(lights[i], "param1", &newLight.param1, VEC4, input);
		// Param 2
		LoadDataFromMember(lights[i], "param2", &newLight.param2, VEC4, input);

		// Add completed light to the array
		newLights.push_back(newLight); 
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
		std::cerr << "ERROR: Save file not found" << std::endl;
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
