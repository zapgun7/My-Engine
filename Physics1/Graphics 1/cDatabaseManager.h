#pragma once

#include <string>

#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

class sqlite3;



class cDatabaseManager ///////////// TODO put database calls on a thread
{
public:
	struct sThreadDBInfo
	{
		cDatabaseManager* theManager = nullptr;
		float dbNewVal = 0.0f;
		bool hasReset = false;
	};


	static cDatabaseManager* GetInstance(void);
	void Initialize(void);

	int InsertData(int id, float newVal); // For now this is made for one type of data
	float SelectData(int id); // Selects just the TOPSPD value from the requested id
	int UpdateData(int id, float newVal); // updates TOPSPD for the requested id

	float GetTopSpeed(void);
	void UpdateTopSpeed(float newSpeed);
	void ResetData(void);

private:

	// Singletone and setup //
	static cDatabaseManager* m_pTheOneDBManager;
	cDatabaseManager();
	~cDatabaseManager();

	int CreateTable(void); // Creates the table used in the game if it does not already exist
	// //////////////////// //


	// Thread Info //
	DWORD* m_ThreadIDs;
	HANDLE* m_ThreadHandles;
	sThreadDBInfo* m_ThreadInfos;
	// /////////// //
	

	static int SelectCallback(void* NotUsed, int argc, char** argv, char** azColName);

	float m_SavedTopSpd = 0.0f;
	float m_ReturnData = 0.0f;

	const std::string filepath = "../dbdata/data.db";

	sqlite3* db;
};