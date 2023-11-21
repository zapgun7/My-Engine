#include "cSoundManager.h"
#pragma once
#include <Windows.h>
#ifdef PlaySound
#undef PlaySound
#endif
#include <memory>
#include <assert.h>
#include <vector>
#include <map>
#include <string>
#include <array>
#include <conio.h>
#include <fmod/fmod.hpp>

#include "cSoundUtils.h"


gdpAudioNamespaceBegin

SoundManager* SoundManager::m_Instance = nullptr;

SoundManager::SoundManager()
	: m_IsInitialized(false)
	, m_Destroying(false)
	, m_Destroyed(false)
	, m_System(nullptr)
{
}

SoundManager::~SoundManager()
{
	if (m_IsInitialized && !m_Destroying && !m_Destroyed)
	{
		Destroy();
	}
}

SoundManager* SoundManager::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new SoundManager;
	}
	return m_Instance;
}


void SoundManager::Initialize()
{

	if (m_IsInitialized)
	{
		printf("[Warning] SoundManager::Initialize: Already initialized\n");
	}

	FMOD_RESULT result;
	result = FMOD::System_Create(&m_System);
	FMOD_ERROR_CHECK(result);

	result = m_System->init(512, FMOD_INIT_NORMAL, nullptr);
	FMOD_ERROR_CHECK(result);

	m_IsInitialized = true;
}

void SoundManager::Destroy()
{

	if (!m_IsInitialized)
	{
		printf("[Warning] SoundManager::Destroy: Not initialized\n");
		return;
	}

	m_Destroying = true;

	for (int i = 0; i < m_Sounds.size(); i++)
	{
		m_Sounds[i]->release();
	}

	for (int i = 0; i < m_Channels.size(); i++)
	{
		m_Channels[i]->stop();
	}

	for (int i = 0; i < m_ChannelGroups.size(); i++)
	{
		m_ChannelGroups[i]->release();
	}

	m_System->close();
	m_System->release();

	m_IsInitialized = false;
	m_Destroyed = true;
}

void SoundManager::Update()
{
	m_System->update();
}

unsigned int SoundManager::LoadSound(const char* file)
{
	int soundId = CheckSoundCache(file);
	if (soundId != -1)
	{
		return soundId;
	}

	FMOD::Sound* sound = nullptr;
	FMOD_RESULT result = m_System->createSound(file, FMOD_DEFAULT, nullptr, &sound);
	FMOD_ERROR_CHECK(result);

	m_Sounds.push_back(sound);
	m_SoundCache.insert(std::pair<const char*, unsigned int>(file, m_Sounds.size() - 1));

	return m_Sounds.size() - 1;
}


int SoundManager::CreateChannelGroup(const char* name)
{
	FMOD_RESULT result;
	FMOD::ChannelGroup* newChannelGroup;
	result = m_System->createChannelGroup(name, &newChannelGroup);
	FMOD_ERROR_CHECK(result);
	m_ChannelGroups.push_back(newChannelGroup);
	return (m_ChannelGroups.size() - 1);
}


void SoundManager::PlaySound(unsigned int soundId, unsigned int channelGrpId)
{
	assert(soundId < m_Sounds.size());
	assert(channelGrpId < m_ChannelGroups.size());

	FMOD::Sound* sound = m_Sounds[soundId];
	FMOD::Channel* channel = nullptr;
	FMOD::ChannelGroup* channelGroup = m_ChannelGroups[channelGrpId];
	bool paused = true;


	FMOD_RESULT result = m_System->playSound(sound, channelGroup, paused, &channel);
	FMOD_ERROR_CHECK(result);

	channel->setPaused(false);
	FMOD_ERROR_CHECK(result);
}

int SoundManager::CheckSoundCache(const char* name)
{
	sound_cache_iter it = m_SoundCache.find(name);
	if (it == m_SoundCache.end())
	{
		return -1;
	}
	return it->second;
}

void SoundManager::SetChannelPan(unsigned int channelId, float value)
{
	m_Channels[channelId]->setPan(value);
}

void SoundManager::SetChannelPitch(unsigned int channelId, float value)
{
	m_Channels[channelId]->setPitch(value);
}

void SoundManager::SetChannelVolume(unsigned int channelId, float value)
{
	m_Channels[channelId]->setVolume(value);
}


// Group Pan
void SoundManager::SetChannelGroupPan(unsigned int channelGroupId, float value)
{
	assert(channelGroupId < m_ChannelGroups.size());
	FMOD::ChannelGroup* theChannelGroup = m_ChannelGroups[channelGroupId];
	FMOD_RESULT result;

	result = theChannelGroup->setPan(value);
	FMOD_ERROR_CHECK(result);
}


// Group Pitch
void SoundManager::SetChannelGroupPitch(unsigned int channelGroupId, float value)
{
	assert(channelGroupId < m_ChannelGroups.size());
	FMOD::ChannelGroup* theChannelGroup = m_ChannelGroups[channelGroupId];
	FMOD_RESULT result;

	result = theChannelGroup->setPitch(value);
	FMOD_ERROR_CHECK(result);
}


// Group Volume
void SoundManager::SetChannelGroupVolume(unsigned int channelGroupId, float value)
{
	assert(channelGroupId < m_ChannelGroups.size());
	FMOD::ChannelGroup* theChannelGroup = m_ChannelGroups[channelGroupId];
	FMOD_RESULT result;

	result = theChannelGroup->setVolume(value);
	FMOD_ERROR_CHECK(result);
}


gdpAudioNamespaceEnd
