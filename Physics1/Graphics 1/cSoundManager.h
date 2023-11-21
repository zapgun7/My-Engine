#pragma once

//#include "SoundManagerPImpl.h"
#include <fmod/fmod.hpp>
#include <vector>
#include <map>


#define gdpAudioNamespaceBegin namespace audio {
#define gdpAudioNamespaceEnd }


gdpAudioNamespaceBegin

class SoundManager
{
public:
	~SoundManager();

	static SoundManager* GetInstance();

	// System
	void Initialize();
	void Destroy();
	void Update();

	// Sounds
	unsigned int LoadSound(const char* file);


	int CreateChannelGroup(const char* name);


	void PlaySound(unsigned int channelId, unsigned int channelGrpId);

	// Channels
	void SetChannelPan(unsigned int channelId, float value);
	void SetChannelPitch(unsigned int channelId, float value);
	void SetChannelVolume(unsigned int channelId, float value);

	// ChannelGroups
	void SetChannelGroupPan(unsigned int channelGroupId, float value);
	void SetChannelGroupPitch(unsigned int channelGroupId, float value);
	void SetChannelGroupVolume(unsigned int channelGroupId, float value);

	// DSP

private:
	SoundManager();
	static SoundManager* m_Instance;

	int CheckSoundCache(const char* name);

	bool m_IsInitialized;
	bool m_Destroying;
	bool m_Destroyed;
	FMOD::System* m_System;

	using sound_vec = std::vector<FMOD::Sound*>;
	using channel_vec = std::vector<FMOD::Channel*>;
	using channelgroup_vec = std::vector<FMOD::ChannelGroup*>;
	using sound_cache = std::map<const char*, unsigned int>;
	using sound_cache_iter = sound_cache::iterator;

	sound_vec m_Sounds;
	channel_vec m_Channels;
	sound_cache m_SoundCache;

	channelgroup_vec m_ChannelGroups;
};

gdpAudioNamespaceEnd

#define GetSoundManager() audio::SoundManager::GetInstance()