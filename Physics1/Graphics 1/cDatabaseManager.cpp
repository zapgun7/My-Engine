#include "cDatabaseManager.h"


#include <sqlite/sqlite3.h>
#include <stdio.h>
#include <iostream>




cDatabaseManager* cDatabaseManager::m_pTheOneDBManager = nullptr;

DWORD WINAPI ThreadedDBCalls(LPVOID lpParamater)
{
	cDatabaseManager::sThreadDBInfo* myInfo = (cDatabaseManager::sThreadDBInfo*)lpParamater;

	while (true)
	{
		myInfo->theManager->SelectData(0); // Update stored variable
		if (myInfo->hasReset)
		{
			myInfo->theManager->UpdateData(0, 0.0f);
			myInfo->hasReset = false;
		}
		else if (myInfo->dbNewVal > myInfo->theManager->GetTopSpeed())
		{
			myInfo->theManager->UpdateData(0, myInfo->dbNewVal);
		}


		Sleep(100);
	}

	return 0;
}





cDatabaseManager* cDatabaseManager::GetInstance(void)
{
	if (cDatabaseManager::m_pTheOneDBManager == nullptr)
	{
		cDatabaseManager::m_pTheOneDBManager = new cDatabaseManager();
		cDatabaseManager::m_pTheOneDBManager->Initialize();
	}
	return cDatabaseManager::m_pTheOneDBManager;
}

void cDatabaseManager::Initialize(void)
{
	// This will open the file it expects to be in the filepath, otherwise creates a new one

	//sqlite3* db;

	int rc = sqlite3_open(filepath.c_str(), &db);

	if (rc)
	{
		std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}
	else
	{
		std::cout << "Database opened successfully!" << std::endl;
	}


	//sqlite3_close(db);

	CreateTable();
	InsertData(0, 0.0f); // Initialize player data
	m_SavedTopSpd = SelectData(0);
	//SelectData(0);
	//UpdateData(0, 4.6f);
	//SelectData(0);


	// Set up the thread that will make calls to the DB
	this->m_ThreadInfos = new sThreadDBInfo();
	this->m_ThreadHandles = new HANDLE();
	this->m_ThreadIDs = new DWORD();

	
// 	this->m_ThreadInfos[threadIDX].thePhysics = this->m_pTheOnePhysics;
// 	this->m_ThreadInfos[threadIDX].theShape = nullptr;

	this->m_ThreadInfos->dbNewVal = m_SavedTopSpd;
	this->m_ThreadInfos->theManager = this;

	//this->m_ThreadInfos[threadIDX].ID = threadIDX;

	void* pParams = (void*)(this->m_ThreadInfos);

	*this->m_ThreadHandles = CreateThread(
		NULL,
		0,
		ThreadedDBCalls,
		pParams,
		0,
		(this->m_ThreadIDs));
	
}



cDatabaseManager::cDatabaseManager()
{

}

cDatabaseManager::~cDatabaseManager()
{
	sqlite3_close(db);
}

int cDatabaseManager::CreateTable(void)
{
	sqlite3* db;

	std::string sql = "CREATE TABLE IF NOT EXISTS STATS("
		"ID INTEGER PRIMARY KEY, "
		"TOPSPD      REAL NOT NULL,"
		"UNIQUE(ID));";

	try
	{
		int exit = 0;
		exit = sqlite3_open(filepath.c_str(), &db);

		char* messageError;
		exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messageError);

		if (exit != SQLITE_OK)
		{
			std::cerr << "Error creating table" << std::endl;
			sqlite3_free(messageError);
		}
		else
			std::cout << "Table created successfully" << std::endl;
		//sqlite3_close(db);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Some error in the midst of creating the table" << std::endl;
	}

	return 1;
}

int cDatabaseManager::InsertData(int id, float newVal)
{
	sqlite3* db;
	char* messageError;

	int exit = sqlite3_open(filepath.c_str(), &db);

	std::string sql("INSERT INTO STATS(ID, TOPSPD) VALUES(" + std::to_string(id) + ", " + std::to_string(newVal) + ");"); 

	exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK)
	{
		std::cerr << "Error inserting" << std::endl;
		sqlite3_free(messageError);
	}
	else
		std::cout << "Insert was successful" << std::endl;

	//sqlite3_close(db);
	return 1;
}

float cDatabaseManager::SelectData(int id)
{
	sqlite3* db;

	int exit = sqlite3_open(filepath.c_str(), &db);

	std::string sql = "SELECT TOPSPD FROM STATS WHERE ID=" + std::to_string(id) + ";";

	sqlite3_exec(db, sql.c_str(), SelectCallback, NULL, NULL);

	//sqlite3_close(db);



	return m_SavedTopSpd;
}

int cDatabaseManager::UpdateData(int id, float newVal)
{
	sqlite3* db;
	char* messageError;

	int exit = sqlite3_open(filepath.c_str(), &db);

	std::string sql = "UPDATE STATS SET TOPSPD=" + std::to_string(newVal) + " WHERE ID=" + std::to_string(id) + ";";

	exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK)
	{
		std::cerr << "Error updating" << std::endl;
		sqlite3_free(messageError);
	}
	else
		this->m_SavedTopSpd = newVal;
// 		std::cout << "Successfully updated data" << std::endl;

	return 1;
}

float cDatabaseManager::GetTopSpeed(void)
{
	return m_SavedTopSpd;
}

int cDatabaseManager::SelectCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
	std::string output = "";

// 	for (int i = 0; i < argc; i++)
// 	{
// 		std::cout << azColName[i] << ": " << argv[i] << std::endl;
// 	}

	//cDatabaseManager::m_pTheOneDBManager->m_ReturnData = std::stof(argv[0]);
	cDatabaseManager::m_pTheOneDBManager->m_SavedTopSpd = std::stof(argv[0]);

	//std::cout << std::endl;

	return 1;
}

void cDatabaseManager::UpdateTopSpeed(float newSpeed)
{
	this->m_ThreadInfos->dbNewVal = newSpeed;
	return;
}

void cDatabaseManager::ResetData(void)
{
	this->m_SavedTopSpd = 0.0f;
	this->m_ThreadInfos->dbNewVal = 0.0f;
	this->m_ThreadInfos->hasReset = true;
	return;
}