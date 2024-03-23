#pragma once


#include <fmod/fmod.hpp>
#include <fmod/fmod.h>
#include <string>
#include <map>
#include <vector>




class cSoundManager
{
public:
	static cSoundManager* GetInstance(void);
	void Destroy(void);
	void Update(double dt);
	FMOD::Channel* StartSound(std::string& soundName);


private:

	// Singleton & setup //
	static cSoundManager* m_pTheOnlyManager;
	int Initialize(void);

	int LoadFiles(std::string fileName); // Reads audio files from text (fileName)
	// ///////////////// //

	FMOD::Channel* StartSound(FMOD::Sound* sound); 
	FMOD::Sound* FindSoundByName(std::string& name);


	FMOD::System* m_pSystem = nullptr;
	FMOD::Channel* m_pChannel = nullptr;

	std::map<std::string, FMOD::Sound*> m_mapNameToSound;
	std::vector<std::string> m_vecStoredAudio; // Just nice to have for now I
};