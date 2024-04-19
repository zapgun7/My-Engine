#include "cSoundManager.h"

#include <iostream>
#include <fstream>




cSoundManager* cSoundManager::m_pTheOnlyManager = nullptr;


cSoundManager* cSoundManager::GetInstance(void)
{
	if (cSoundManager::m_pTheOnlyManager == nullptr)
	{
		cSoundManager::m_pTheOnlyManager = new cSoundManager();
		if (!cSoundManager::m_pTheOnlyManager->Initialize())
		{
			printf("There was an error initializing the cSoundManager\n");
			return nullptr;
		}
	}
	return cSoundManager::m_pTheOnlyManager;
}


void cSoundManager::Destroy(void)
{
	FMOD_RESULT result;

	std::map<std::string, FMOD::Sound*>::iterator mapIT = this->m_mapNameToSound.begin();
	while (mapIT != this->m_mapNameToSound.end())
	{
		result = mapIT->second->release();
		if (result != FMOD_OK)
		{
			printf("Failed to release sound: %s\n", mapIT->first.c_str());
		}
		mapIT++;
	}

	result = this->m_pSystem->close();
	if (result != FMOD_OK)
	{
		printf("Failed to close the FMOD system\n");
	}

	result = this->m_pSystem->release();
	if (result != FMOD_OK)
	{
		printf("Failed to release the FMOD system\n");
	}

	return;
}

void cSoundManager::Update(double dt)
{
	// Will put other stuff here later, stuff scaling off of delta time...



	FMOD_RESULT result;
	result = this->m_pSystem->update();
	if (result != FMOD_OK)
	{
		printf("Failure updating the FMOD system; Destroying FMOD instance...\n");
		Destroy();
	}
}

int cSoundManager::Initialize(void)
{
	const std::string audioText = "audios.txt";

	FMOD_RESULT result;
	result = FMOD::System_Create(&this->m_pSystem);

	if (result != FMOD_OK)
	{
		printf("Failed to create the FMOD System\n");
		return 0;
	}

	result = this->m_pSystem->init(5, FMOD_INIT_NORMAL, nullptr);   /// Prob will want to change this to a 3D setup later
	if (result != FMOD_OK)
	{
		printf("Failed to initialize the FMOD system\n");
		// Cleanup what we just made
		result = this->m_pSystem->close();
		if (result != FMOD_OK)
		{
			printf("Failed to close the FMOD system lol\n");
		}
		return 0;
	}


	if (!LoadFiles(audioText))
	{
		printf("Failed to load audio files\n");
		return 0;
	}

	std::string heartBeatName = "heartBeat";
	FMOD::Sound* sound = FindSoundByName(heartBeatName);

	result = this->m_pSystem->playSound(sound, 0, false, &this->m_HeartBeatChannel);
	if (result != FMOD_OK)
	{
		printf("Failed to play a sound\n");
	}
	this->m_HeartBeatChannel->setMode(FMOD_LOOP_NORMAL);
	this->m_HeartBeatChannel->setVolume(0.0f);

	std::string droneBeatName = "drone";
	sound = FindSoundByName(droneBeatName);

	result = this->m_pSystem->playSound(sound, 0, false, &this->m_DroneChannel);
	if (result != FMOD_OK)
	{
		printf("Failed to play a sound\n");
	}
	this->m_DroneChannel->setMode(FMOD_LOOP_NORMAL);
	this->m_DroneChannel->setVolume(0.0f);


	return 1;
}

// Removes the .mp3, .wav or whatever
void removeExt(std::string& fileName)
{
	int indexOfDot = 0;
	for (int strIdx = fileName.length() - 1; strIdx > -1; strIdx--)
	{
		if ((char)fileName[strIdx] == '.') // Dumb setup but it prob works?
		{
			indexOfDot = strIdx;
			break;
		}
	}

	fileName = fileName.substr(0, indexOfDot);
	return;
}


int cSoundManager::LoadFiles(std::string fileName)
{
	std::string audioFilesPath = "assets/audio/";

	std::ifstream audioReadFile((audioFilesPath + fileName).c_str());
	if (!audioReadFile.is_open())
	{
		printf("Failed to open the audio file\n");
		return 0;
	}
	
	std::string fName, fileFullPath;
	FMOD_RESULT result;
	FMOD::Sound* tempSound;

	while (audioReadFile >> fName) // TODO add more details in the txt we read from, so we have SAMPLE and STREAM types of audio (shorter and longer audio files respectively)
	{
		fileFullPath = audioFilesPath + fName;
		result = this->m_pSystem->createSound(fileFullPath.c_str(), FMOD_CREATESAMPLE, 0, &tempSound); // We have to store each pointer generated here for reference later

		if (result != FMOD_OK)
		{
			printf("Failed to load audio: %s\n", fName.c_str());
			continue;
		}

		// Successfully loaded sound, make it so we can access later
		
		removeExt(fName);
		m_mapNameToSound[fName] = tempSound;
		m_vecStoredAudio.push_back(fName);
	}
	audioReadFile.close();

	return 1;
}

FMOD::Channel* cSoundManager::StartSound(FMOD::Sound* sound)
{
	if (sound == nullptr) return nullptr;

	FMOD_RESULT result;
	result = this->m_pSystem->playSound(sound, 0, false, &this->m_pChannel);
	if (result != FMOD_OK)
	{
		printf("Failed to play a sound\n");
		return nullptr;
	}
	return this->m_pChannel;
}

FMOD::Channel* cSoundManager::StartSound(std::string& soundName)
{
	return StartSound(FindSoundByName(soundName));
}

void cSoundManager::setHeartBeatVol(float vol)
{
	if (vol > 1.0f) vol = 1.0f; // Prevent any unanticipated eardrum blowouts
	m_HeartBeatChannel->setVolume(vol);
	return;
}

void cSoundManager::setDroneVolume(float vol)
{
	if (vol > 1.0f) vol = 1.0f; // Prevent any unanticipated eardrum blowouts
	m_DroneChannel->setVolume(vol);
	return;
}

FMOD::Sound* cSoundManager::FindSoundByName(std::string& name)
{
	std::map<std::string, FMOD::Sound*>::iterator itSound = this->m_mapNameToSound.find(name);

	if (itSound == this->m_mapNameToSound.end())
	{
		printf("Failed to find sound\n");
		return nullptr;
	}

	return itSound->second;
}

