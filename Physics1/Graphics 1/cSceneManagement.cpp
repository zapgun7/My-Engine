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

		*intData = itr->value.GetInt();
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
		// Object filename and friendlyname
// 		str = MeshVec[i]->meshName; // filename
// 		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
// 		meshobj.AddMember("meshName", string, output.GetAllocator()); // Add meshname
		AddMemberToObject(meshobj, "meshName", &MeshVec[i]->meshName, STRING, output);
		

// 		str = MeshVec[i]->friendlyName; // friendlyname
// 		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
// 		meshobj.AddMember("friendlyName", string, output.GetAllocator()); // Add friendlyname
		AddMemberToObject(meshobj, "friendlyName", &MeshVec[i]->friendlyName, STRING, output);

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

		///// MATERIAL SETTING /////
		// Power
// 		vec4 = MeshVec[i]->material.power;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		meshobj.AddMember("mat_pow", vec, output.GetAllocator()); // Material settings
// 		vec.SetArray();
// 
// 		// Diffuse Texture
// 		str = MeshVec[i]->material.diffuseTex;
// 		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
// 		meshobj.AddMember("mat_difftex", string, output.GetAllocator());
		AddMemberToObject(meshobj, "mat_difftex", &MeshVec[i]->material.diffuseTex, STRING, output);
		// Specular Texture
// 		str = MeshVec[i]->material.specularTex;
// 		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
// 		meshobj.AddMember("mat_spectex", string, output.GetAllocator());
		AddMemberToObject(meshobj, "mat_spectex", &MeshVec[i]->material.specularTex, STRING, output);

		// Diffuse Mirror
// 		state = MeshVec[i]->material.isDiffMirrored;
// 		meshobj.AddMember("mat_diffmirr", state, output.GetAllocator());
		AddMemberToObject(meshobj, "mat_diffmirr", &MeshVec[i]->material.isDiffMirrored, BOOL, output);
		// Specular Mirror
// 		state = MeshVec[i]->material.isSpecMirrored;
// 		meshobj.AddMember("mat_specmirr", state, output.GetAllocator());
		AddMemberToObject(meshobj, "mat_specmirr", &MeshVec[i]->material.isSpecMirrored, BOOL, output);

		///// //////// /////// /////

		// Custom Color
// 		vec3 = MeshVec[i]->customColorRGBA; // Custom Color
// 		vec.PushBack(vec3.x, output.GetAllocator());
// 		vec.PushBack(vec3.y, output.GetAllocator());
// 		vec.PushBack(vec3.z, output.GetAllocator());
// 		meshobj.AddMember("customColor", vec, output.GetAllocator()); // Add custom color
// 		vec.SetArray();
		AddMemberToObject(meshobj, "customColor", &MeshVec[i]->customColorRGBA, VEC3, output);

		// UV Offset and scale
// 		vec3 = MeshVec[i]->uv_Offset_Scale;
// 		vec.PushBack(vec3.x, output.GetAllocator());
// 		vec.PushBack(vec3.y, output.GetAllocator());
// 		vec.PushBack(vec3.z, output.GetAllocator());
// 		meshobj.AddMember("uv_OS", vec, output.GetAllocator());
// 		vec.SetArray();
		AddMemberToObject(meshobj, "uv_OS", &MeshVec[i]->uv_Offset_Scale, VEC3, output);
		
		// UV Offset speed
// 		vec2 = MeshVec[i]->uvOffsetSpeed;
// 		vec.PushBack(vec2.x, output.GetAllocator());
// 		vec.PushBack(vec2.y, output.GetAllocator());
// 		meshobj.AddMember("uv_Spd", vec, output.GetAllocator());
// 		vec.SetArray();
		AddMemberToObject(meshobj, "uv_Spd", &MeshVec[i]->uv_Offset_Scale, VEC2, output);

		// Scale and bools
// 		flt = MeshVec[i]->scale.x; // scale
// 		meshobj.AddMember("scale", flt, output.GetAllocator()); // Add scale
		AddMemberToObject(meshobj, "scale", &MeshVec[i]->scale.x, FLOAT, output);
// 		state = MeshVec[i]->bIsVisible;
// 		meshobj.AddMember("isVisible", state, output.GetAllocator()); // Add isVisible
		AddMemberToObject(meshobj, "isVisible", &MeshVec[i]->bIsVisible, BOOL, output);
// 		state = MeshVec[i]->bIsWireframe;
// 		meshobj.AddMember("isWireframe", state, output.GetAllocator()); // Add isWireframe
		AddMemberToObject(meshobj, "isWireframe", &MeshVec[i]->bIsWireframe, BOOL, output);
// 		state = MeshVec[i]->bDoNotLight;
// 		meshobj.AddMember("doNotLight", state, output.GetAllocator()); // Add doNotLight
		AddMemberToObject(meshobj, "doNotLight", &MeshVec[i]->bDoNotLight, BOOL, output);
// 		state = MeshVec[i]->bUseCustomColors;
// 		meshobj.AddMember("useCustomColor", state, output.GetAllocator()); // Add useCustomColor
		AddMemberToObject(meshobj, "useCustomColor", &MeshVec[i]->bUseCustomColors, BOOL, output);

		// Transparency Alpha
// 		flt = MeshVec[i]->transparencyAlpha;
// 		meshobj.AddMember("tAlph", flt, output.GetAllocator());
		AddMemberToObject(meshobj, "tAlph", &MeshVec[i]->transparencyAlpha, FLOAT, output);
		// Discard Mask
// 		state = MeshVec[i]->bUseDiscardMaskTex;
// 		meshobj.AddMember("dMask", state, output.GetAllocator());
		AddMemberToObject(meshobj, "dMask", &MeshVec[i]->bUseDiscardMaskTex, BOOL, output);
		// Reflect/Refract
// 		state = MeshVec[i]->bUseReflect;
// 		meshobj.AddMember("reflect", state, output.GetAllocator());
		AddMemberToObject(meshobj, "reflect", &MeshVec[i]->bUseReflect, BOOL, output);
// 		state = MeshVec[i]->bUseRefract;
// 		meshobj.AddMember("refract", state, output.GetAllocator());
		AddMemberToObject(meshobj, "refract", &MeshVec[i]->bUseRefract, BOOL, output);

		// The id that will bind it to the physics obj
// 		num = MeshVec[i]->uniqueID;
// 		meshobj.AddMember("ID", num, output.GetAllocator());
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
// 		str = PhysVec[i]->friendlyName; // friendlyname
// 		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
// 		physobj.AddMember("friendlyName", string, output.GetAllocator()); // Add friendlyname
		AddMemberToObject(physobj, "friendlyName", &PhysVec[i]->friendlyName, STRING, output);

		// Shape Type
// 		num = PhysVec[i]->shapeType;
// 		physobj.AddMember("shape_t", num, output.GetAllocator());
		AddMemberToObject(physobj, "shape_t", &PhysVec[i]->shapeType, INT, output);

		// Position
// 		vec3 = PhysVec[i]->position;
// 		vec.PushBack(vec3.x, output.GetAllocator());
// 		vec.PushBack(vec3.y, output.GetAllocator());
// 		vec.PushBack(vec3.z, output.GetAllocator());
// 		physobj.AddMember("pos", vec, output.GetAllocator());
// 		vec.SetArray();
		AddMemberToObject(physobj, "pos", &PhysVec[i]->position, VEC3, output);


		// Euler Orientation
 		vec3 = PhysVec[i]->get_eOrientation();
// 		vec.PushBack(vec3.x, output.GetAllocator());
// 		vec.PushBack(vec3.y, output.GetAllocator());
// 		vec.PushBack(vec3.z, output.GetAllocator());
// 		physobj.AddMember("ori", vec, output.GetAllocator());
// 		vec.SetArray();
		AddMemberToObject(physobj, "ori", &vec3, VEC3, output);

		// Unique ID
		num = PhysVec[i]->getUniqueID();
		//physobj.AddMember("ID", num, output.GetAllocator());
		AddMemberToObject(physobj, "ID", &num, VEC3, output);

		// Inverse Mass
// 		flt = PhysVec[i]->inverse_mass;
// 		physobj.AddMember("inv_mass", flt, output.GetAllocator());
		AddMemberToObject(physobj, "inv_mass", &PhysVec[i]->inverse_mass, FLOAT, output);

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
// 		str = Lights->theLights[i].friendlyName; // friendlyname
// 		string.SetString(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), output.GetAllocator());
// 		lightobj.AddMember("friendlyname", string, output.GetAllocator()); // Add friendlyname
		AddMemberToObject(lightobj, "friendlyname", &Lights->theLights[i].friendlyName, STRING, output);
		// Position
// 		vec4 = Lights->theLights[i].position;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		lightobj.AddMember("position", vec, output.GetAllocator());
// 		vec.SetArray(); // Clear 
		AddMemberToObject(lightobj, "position", &Lights->theLights[i].position, VEC4, output);
		// Diffuse
// 		vec4 = Lights->theLights[i].diffuse;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		lightobj.AddMember("diffuse", vec, output.GetAllocator());
// 		vec.SetArray(); // Clear 
		AddMemberToObject(lightobj, "diffuse", &Lights->theLights[i].diffuse, VEC4, output);
		// Specular
// 		vec4 = Lights->theLights[i].specular;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		lightobj.AddMember("specular", vec, output.GetAllocator());
// 		vec.SetArray(); // Clear 
		AddMemberToObject(lightobj, "specular", &Lights->theLights[i].specular, VEC4, output);
		// Attenuation
// 		vec4 = Lights->theLights[i].atten;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		lightobj.AddMember("atten", vec, output.GetAllocator());
// 		vec.SetArray(); // Clear 
		AddMemberToObject(lightobj, "atten", &Lights->theLights[i].atten, VEC4, output);
		// Direction
// 		vec4 = Lights->theLights[i].direction;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		lightobj.AddMember("direction", vec, output.GetAllocator());
// 		vec.SetArray(); // Clear 
		AddMemberToObject(lightobj, "direction", &Lights->theLights[i].direction, VEC4, output);
		// Param1
// 		vec4 = Lights->theLights[i].param1;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		lightobj.AddMember("param1", vec, output.GetAllocator());
// 		vec.SetArray(); // Clear 
		AddMemberToObject(lightobj, "param1", &Lights->theLights[i].param1, VEC4, output);
		// Param2
// 		vec4 = Lights->theLights[i].param2;
// 		vec.PushBack(vec4.x, output.GetAllocator());
// 		vec.PushBack(vec4.y, output.GetAllocator());
// 		vec.PushBack(vec4.z, output.GetAllocator());
// 		vec.PushBack(vec4.w, output.GetAllocator());
// 		lightobj.AddMember("param2", vec, output.GetAllocator());
// 		vec.SetArray(); // Clear 
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
// 		itr = meshes[i].FindMember("meshName");
//  		newMesh->meshName = itr->value.GetString(); // Set meshname
		LoadDataFromMember(meshes[i], "meshName", &newMesh->meshName, STRING, input);

// 		itr = meshes[i].FindMember("friendlyName");
// 		newMesh->friendlyName = itr->value.GetString(); // Set friendlyName
		LoadDataFromMember(meshes[i], "friendlyName", &newMesh->friendlyName, STRING, input);

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


// 		// Position and Orientation
// 		itr = meshes[i].FindMember("drawPosition");
// 		newMesh->drawPosition.x = itr->value[0].GetFloat();
// 		newMesh->drawPosition.y = itr->value[1].GetFloat();
// 		newMesh->drawPosition.z = itr->value[2].GetFloat();
// 		itr = meshes[i].FindMember("eulerOrientation");
// 		newMesh->eulerOrientation.x = itr->value[0].GetFloat();
// 		newMesh->eulerOrientation.y = itr->value[1].GetFloat();
// 		newMesh->eulerOrientation.z = itr->value[2].GetFloat();


		///// MATERIALS INFO /////
// 		itr = meshes[i].FindMember("mat_power");
// 		if (itr != meshes[i].MemberEnd()) // Does this work like this?
// 		{
// 			newMesh->material.power.x = itr->value[0].GetFloat();
// 			newMesh->material.power.y = itr->value[1].GetFloat();
// 			newMesh->material.power.z = itr->value[2].GetFloat();
// 			newMesh->material.power.w = itr->value[3].GetFloat();
// 		}
		LoadDataFromMember(meshes[i], "mat_power", &newMesh->material.power, VEC4, input);
		
		// Diffuse/Specular Texture Names
// 		itr = meshes[i].FindMember("mat_difftex");
// 		if (itr != meshes[i].MemberEnd())
// 		{
// 			newMesh->material.diffuseTex = itr->value.GetString();
// 		}
		if (!LoadDataFromMember(meshes[i], "mat_difftex", &newMesh->material.diffuseTex, STRING, input))
		{
			LoadDataFromMember(meshes[i], "mat_difftex", &newMesh->material.diffuseTex, STRING, input)
		}
// 		itr = meshes[i].FindMember("mat_spectex");
// 		if (itr != meshes[i].MemberEnd())
// 		{
// 			newMesh->material.specularTex = itr->value.GetString();
// 		}
		LoadDataFromMember(meshes[i], "mat_spectex", &newMesh->material.specularTex, STRING, input);

		// Is Mirrored
// 		itr = meshes[i].FindMember("mat_diffmirr");
// 		if (itr != meshes[i].MemberEnd())
// 		{
// 			newMesh->material.isDiffMirrored = itr->value.GetBool();
// 		}
		LoadDataFromMember(meshes[i], "mat_diffmirr", &newMesh->material.isDiffMirrored, BOOL, input);
// 		itr = meshes[i].FindMember("mat_specmirr");
// 		if (itr != meshes[i].MemberEnd())
// 		{
// 			newMesh->material.isSpecMirrored = itr->value.GetBool();
// 		}
		LoadDataFromMember(meshes[i], "mat_specmirr", &newMesh->material.isSpecMirrored, BOOL, input);

		///// ///////// //// /////


// 		itr = meshes[i].FindMember("customColor");
// 		newMesh->customColorRGBA.x = itr->value[0].GetFloat();
// 		newMesh->customColorRGBA.y = itr->value[1].GetFloat();
// 		newMesh->customColorRGBA.z = itr->value[2].GetFloat();
		LoadDataFromMember(meshes[i], "customColor", &newMesh->customColorRGBA, VEC3, input);

		// UV offset and scale
// 		itr = meshes[i].FindMember("uv_OS");
// 		newMesh->uv_Offset_Scale.x = itr->value[0].GetFloat();
// 		newMesh->uv_Offset_Scale.y = itr->value[1].GetFloat();
// 		newMesh->uv_Offset_Scale.z = itr->value[2].GetFloat();
		LoadDataFromMember(meshes[i], "uv_OS", &newMesh->uv_Offset_Scale, VEC3, input);

		// UV Offset speed
// 		itr = meshes[i].FindMember("uv_Spd");
// 		newMesh->uvOffsetSpeed.x = itr->value[0].GetFloat();
// 		newMesh->uvOffsetSpeed.y = itr->value[1].GetFloat();
		LoadDataFromMember(meshes[i], "uv_Spd", &newMesh->uvOffsetSpeed, VEC2, input);


		// Scale and bools
// 		itr = meshes[i].FindMember("scale");
// 		newMesh->scale = glm::vec3(itr->value.GetFloat());
		LoadDataFromMember(meshes[i], "scale", &newMesh->scale.x, FLOAT, input);
// 		itr = meshes[i].FindMember("isVisible");
// 		newMesh->bIsVisible = itr->value.GetBool();
		LoadDataFromMember(meshes[i], "isVisible", &newMesh->bIsVisible, BOOL, input);
// 		itr = meshes[i].FindMember("isWireframe");
// 		newMesh->bIsWireframe = itr->value.GetBool();
		LoadDataFromMember(meshes[i], "isWireframe", &newMesh->bIsWireframe, BOOL, input);
// 		itr = meshes[i].FindMember("doNotLight");
// 		newMesh->bDoNotLight = itr->value.GetBool();
		LoadDataFromMember(meshes[i], "doNotLight", &newMesh->bDoNotLight, BOOL, input);
// 		itr = meshes[i].FindMember("useCustomColor");
// 		newMesh->bUseCustomColors = itr->value.GetBool();
		LoadDataFromMember(meshes[i], "useCustomColor", &newMesh->bUseCustomColors, BOOL, input);

		// Transparency Alpha
// 		itr = meshes[i].FindMember("tAlph");
// 		newMesh->transparencyAlpha = itr->value.GetFloat();
		LoadDataFromMember(meshes[i], "tAlph", &newMesh->transparencyAlpha, FLOAT, input);

		// Discard Mask
// 		itr = meshes[i].FindMember("dMask");
// 		newMesh->bUseDiscardMaskTex = itr->value.GetBool();
		LoadDataFromMember(meshes[i], "dMask", &newMesh->transparencyAlpha, BOOL, input);

		// Reflect/Refract
// 		itr = meshes[i].FindMember("reflect");
// 		newMesh->bUseReflect = itr->value.GetBool();
		LoadDataFromMember(meshes[i], "reflect", &newMesh->bUseReflect, BOOL, input);
// 		itr = meshes[i].FindMember("refract");
// 		newMesh->bUseRefract = itr->value.GetBool();
		LoadDataFromMember(meshes[i], "refract", &newMesh->bUseRefract, BOOL, input);

		// Unique ID from last instance
// 		itr = meshes[i].FindMember("ID");
// 		newMesh->uniqueID = itr->value.GetInt();
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
// 		itr = phys[i].FindMember("friendlyName");
// 		newPhys->friendlyName = itr->value.GetString();
		LoadDataFromMember(phys[i], "friendlyName", &newPhys->friendlyName, STRING, input);

		// Shape Type
		int temp;
// 		itr = phys[i].FindMember("shape_t");
// 		newPhys->shapeType = (sPhysicsProperties::eShape)itr->value.GetInt();
		if (LoadDataFromMember(phys[i], "shape_t", &temp, INT, input))
			newPhys->shapeType = (sPhysicsProperties::eShape)temp;

		// Position
// 		itr = phys[i].FindMember("pos");
// 		newPhys->position.x = itr->value[0].GetFloat();
// 		newPhys->position.y = itr->value[1].GetFloat();
// 		newPhys->position.z = itr->value[2].GetFloat();
		LoadDataFromMember(phys[i], "pos", &newPhys->position, VEC3, input);

		// Orientation
		glm::vec3 tempVec;
// 		itr = phys[i].FindMember("ori");
// 		newPhys->setRotationFromEuler(glm::vec3(itr->value[0].GetFloat(), itr->value[1].GetFloat(), itr->value[2].GetFloat()));
		if (LoadDataFromMember(phys[i], "ori", &tempVec, VEC3, input))
		{
			newPhys->setRotationFromEuler(tempVec);
		}


		LoadDataFromMember(phys[i], "inv_mass", &newPhys->inverse_mass, FLOAT, input);


		//itr = phys[i].FindMember("ID");
		int num = itr->value.GetInt();
		LoadDataFromMember(phys[i], "ID", &num, INT, input);

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

// 		if (phys[i].HasMember("inv_mass"))
// 		{
// 			itr = phys[i].FindMember("inv_mass");
// 			float flt = itr->value.GetFloat();
// 			newPhys->inverse_mass = flt;
// 			
// 		}

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
