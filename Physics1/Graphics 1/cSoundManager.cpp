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
#include <iostream>

#include "cSoundUtils.h"


gdpAudioNamespaceBegin

cSoundManager* cSoundManager::m_Instance = nullptr;

cSoundManager::cSoundManager()
	: m_IsInitialized(false)
	, m_Destroying(false)
	, m_Destroyed(false)
	, m_System(nullptr)
{
}


cSoundManager::~cSoundManager()
{
	if (m_IsInitialized && !m_Destroying && !m_Destroyed)
	{
		Destroy();
	}
}

cSoundManager* cSoundManager::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new cSoundManager;
	}
	return m_Instance;
}


void cSoundManager::Initialize()
{

	if (m_IsInitialized)
	{
		printf("[Warning] SoundManager::Initialize: Already initialized\n");
	}

	FMOD_RESULT result;
	result = FMOD::System_Create(&m_System);
	FMOD_ERROR_CHECK(result);

	result = m_System->init(512, FMOD_INIT_3D_RIGHTHANDED, nullptr);
	FMOD_ERROR_CHECK(result);

	m_FilePath = "assets/audio/";

	m_IsInitialized = true;
}

void cSoundManager::Destroy()
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

void cSoundManager::Update()
{
	UpdateAllObjects();
	m_System->update();
}

unsigned int cSoundManager::LoadSound(const char* file)
{
	std::string fullPath = m_FilePath + file;
	int soundId = CheckSoundCache(fullPath.c_str());
	if (soundId != -1)
	{
		return soundId;
	}

	FMOD::Sound* sound = nullptr;
	FMOD_RESULT result = m_System->createSound(fullPath.c_str(), FMOD_3D, nullptr, &sound);
	FMOD_ERROR_CHECK(result);

	m_Sounds.push_back(sound);
	m_SoundCache.insert(std::pair<const char*, unsigned int>(file, m_Sounds.size() - 1)); // Keep the internal name just the file name, no path

	return m_Sounds.size() - 1;
}


int cSoundManager::CreateChannelGroup(const char* name)
{
	FMOD_RESULT result;
	FMOD::ChannelGroup* newChannelGroup;
	result = m_System->createChannelGroup(name, &newChannelGroup);
	FMOD_ERROR_CHECK(result);
	m_ChannelGroups.push_back(newChannelGroup);
	return (m_ChannelGroups.size() - 1);
}


void cSoundManager::PlaySound(unsigned int soundId, unsigned int channelGrpId)
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

void cSoundManager::PlaySound(unsigned int soundId, audio::s3DSound* audioObj, bool isLooping)
{
	FMOD::Sound* sound = m_Sounds[soundId];
	FMOD::Channel* newChannel = nullptr;
	FMOD::ChannelGroup* channelGroup = m_ChannelGroups[0]; // TODO just mess around with one channel group for now
	bool paused = true;

	//channel->setLoopCount(-1);
	FMOD_RESULT result = m_System->playSound(sound, channelGroup, paused, &newChannel);
	FMOD_ERROR_CHECK(result);
	if (isLooping)
	{
		newChannel->setMode(FMOD_LOOP_NORMAL);
		newChannel->setLoopCount(-1);
	}
	else
	{
		newChannel->setMode(FMOD_INIT_NORMAL);
		newChannel->setLoopCount(0);
	}
	newChannel->setVolume(2.0f);
	audioObj->channel = newChannel;

	newChannel->setPaused(false);
}

int cSoundManager::CheckSoundCache(const char* name)
{
	sound_cache_iter it = m_SoundCache.find(name);
	if (it == m_SoundCache.end())
	{
		return -1;
	}
	return it->second;
}

void cSoundManager::SetChannelPan(unsigned int channelId, float value)
{
	m_Channels[channelId]->setPan(value);
}

void cSoundManager::SetChannelPitch(unsigned int channelId, float value)
{
	m_Channels[channelId]->setPitch(value);
}

void cSoundManager::SetChannelVolume(unsigned int channelId, float value)
{
	m_Channels[channelId]->setVolume(value);
}


// Group Pan
void cSoundManager::SetChannelGroupPan(unsigned int channelGroupId, float value)
{
	assert(channelGroupId < m_ChannelGroups.size());
	FMOD::ChannelGroup* theChannelGroup = m_ChannelGroups[channelGroupId];
	FMOD_RESULT result;

	result = theChannelGroup->setPan(value);
	FMOD_ERROR_CHECK(result);
}


// Group Pitch
void cSoundManager::SetChannelGroupPitch(unsigned int channelGroupId, float value)
{
	assert(channelGroupId < m_ChannelGroups.size());
	FMOD::ChannelGroup* theChannelGroup = m_ChannelGroups[channelGroupId];
	FMOD_RESULT result;

	result = theChannelGroup->setPitch(value);
	FMOD_ERROR_CHECK(result);
}


// Group Volume
void cSoundManager::SetChannelGroupVolume(unsigned int channelGroupId, float value)
{
	assert(channelGroupId < m_ChannelGroups.size());
	FMOD::ChannelGroup* theChannelGroup = m_ChannelGroups[channelGroupId];
	FMOD_RESULT result;

	result = theChannelGroup->setVolume(value);
	FMOD_ERROR_CHECK(result);
}

void cSoundManager::AddNewObject(sPhsyicsProperties* phyObj)
{
	if (!m_IsInitialized)
		return;

	audio::s3DSound newSoundObject;
	
	FMOD::Channel* channel = nullptr;
	


	newSoundObject.channel = channel;
	newSoundObject.object = phyObj;


	m_SoundObjs.push_back(newSoundObject);
}

void cSoundManager::UpdateAllObjects(void)
{
	if (!m_IsInitialized)
		return;

	for (unsigned int i = 0; i < m_SoundObjs.size(); i++)
	{
		sPhsyicsProperties* obj = m_SoundObjs[i].object;

		FMOD_VECTOR FMODpos;
		FMOD_VECTOR FMODvel;
		GLMToFMOD(obj->position, FMODpos);
		GLMToFMOD(obj->velocity, FMODvel);

		//std::cout << FMODpos.y << std::endl;

		m_SoundObjs[i].channel->set3DAttributes(&FMODpos, &FMODvel);

		FMOD_VECTOR testPos;
		FMOD_VECTOR testVel;

		m_SoundObjs[i].channel->get3DAttributes(&testPos, &testVel);
		std::cout << testVel.y << std::endl;
	}
}
void cSoundManager::GLMToFMOD(const glm::vec3& in, FMOD_VECTOR& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}
void cSoundManager::FMODToGLM(const FMOD_VECTOR& in, glm::vec3& out)
{
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
}

void cSoundManager::PlaySoundFromObject(int id, const char* soundName, bool isLooping)
{
	for (unsigned int i = 0; i < m_SoundObjs.size(); i++) // !!! TODO lazy way to do it, should create map with physobj id -> struct
	{
		if (id == m_SoundObjs[i].object->getUniqueID())
		{
			int soundID = CheckSoundCache(soundName);
			if (soundID == -1)
			{
				std::cout << "Sound not found" << std::endl;
				return;
			}

			PlaySound(soundID, &m_SoundObjs[i], isLooping);

			return;
		}
	}
}

void cSoundManager::SetListenerAttribs(glm::vec3 pos, glm::vec3 vel, glm::vec3 up, glm::vec3 forward)
{
	if (!m_IsInitialized)
		return;

	FMOD_VECTOR FMODpos;
	FMOD_VECTOR FMODvel;
	FMOD_VECTOR FMODup;
	FMOD_VECTOR FMODfor;

	GLMToFMOD(pos, FMODpos);
	GLMToFMOD(vel, FMODvel);
	GLMToFMOD(up, FMODup);
	GLMToFMOD(forward, FMODfor);

	m_System->set3DListenerAttributes(0, &FMODpos, &FMODvel, &FMODfor, &FMODup);
	return;
}

gdpAudioNamespaceEnd
