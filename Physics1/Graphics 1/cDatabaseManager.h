#pragma once

#include <string>

class sqlite3;

class cDatabaseManager
{
public:
	static cDatabaseManager* GetInstance(void);
	void Initialize(void);

	int InsertData(int id, float newVal); // For now this is made for one type of data
	float SelectData(int id); // Selects just the TOPSPD value from the requested id
	int UpdateData(int id, float newVal); // updates TOPSPD for the requested id

	float GetTopSpeed(void);

private:

	// Singletone and setup //
	static cDatabaseManager* m_pTheOneDBManager;
	cDatabaseManager();
	~cDatabaseManager();

	int CreateTable(void); // Creates the table used in the game if it does not already exist
	// //////////////////// //

	

	static int SelectCallback(void* NotUsed, int argc, char** argv, char** azColName);

	float m_SavedTopSpd = 0.0f;
	float m_ReturnData = 0.0f;

	const std::string filepath = "../dbdata/data.db";

	sqlite3* db;
};