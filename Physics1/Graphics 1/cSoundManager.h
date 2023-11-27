#pragma once

//#include "SoundManagerPImpl.h"
#include <fmod/fmod.hpp>
#include <vector>
#include <map>
#include "Physics/sPhsyicsProperties.h" // To bind an object to a channel to play sound through


#define gdpAudioNamespaceBegin namespace audio {
#define gdpAudioNamespaceEnd }


gdpAudioNamespaceBegin

enum DSPs
{
	Echo,
	Distortion,
	PitchShift,
	Fader
};

struct s3DSound
{
	FMOD::Channel* channel; // To play 3d sound through
	sPhsyicsProperties* object;

	FMOD::DSP* echo = nullptr;
	FMOD::DSP* distortion = nullptr;
	FMOD::DSP* pitchshift = nullptr;
	FMOD::DSP* fader = nullptr;
};



class cSoundManager
{
public:
	~cSoundManager();

	static cSoundManager* GetInstance();

	// System
	void Initialize();
	void Destroy();
	void Update();

	// Sounds
	unsigned int LoadSound(const char* file);


	int CreateChannelGroup(const char* name);


	void PlaySound(unsigned int soundId, unsigned int channelGrpId);
	void PlaySound(unsigned int soundId, audio::s3DSound* audioObj, bool isLooping);

	// Channels
	void SetChannelPan(unsigned int channelId, float value);
	void SetChannelPitch(unsigned int channelId, float value);
	void SetChannelVolume(unsigned int channelId, float value);

	// ChannelGroups
	void SetChannelGroupPan(unsigned int channelGroupId, float value);
	void SetChannelGroupPitch(unsigned int channelGroupId, float value);
	void SetChannelGroupVolume(unsigned int channelGroupId, float value);

	// DSP
	void AddEchoDSP(audio::s3DSound* audioObj);
	void SetEchoDSP(FMOD::DSP* echoDSP, float delay, float feedback, float drylevel, float wetlevel);
	void AddDistortionDSP(audio::s3DSound* audioObj);
	void SetDistortionDSP(FMOD::DSP* distortionDSP, float distortion);
	void AddPitchshiftDSP(audio::s3DSound* audioObj);
	void SetPitchShiftDSP(FMOD::DSP* pitchshiftDSP, float pitch);
	void AddFaderDSP(audio::s3DSound* audioObj);
	void SetFaderDSP(FMOD::DSP* faderDSP, float gain);


	// Sound Objects
	void AddNewObject(sPhsyicsProperties* phyObj); // Should only need the object to initialize the object
	void UpdateAllObjects(void); // Updates 3d info on all objects in vec
	void PlaySoundFromObject(int id, const char* soundName, bool isLooping);
	void AddDSPToObject(int id, DSPs dsp);
	void SetListenerAttribs(glm::vec3 pos, glm::vec3 vel, glm::vec3 up, glm::vec3 forward);
	void SetDSPFromObject(int id, DSPs dsp, float param1);


	// FMOD -> GLM -> FMOD
	void GLMToFMOD(const glm::vec3& in, FMOD_VECTOR& out);
	void FMODToGLM(const FMOD_VECTOR& in, glm::vec3& out);


	// Occlusion Geometry
	void AddPolygonGeometry(const std::vector<glm::vec3>& vertices);


private:
	cSoundManager();
	static cSoundManager* m_Instance;
	std::string m_FilePath;

	int CheckSoundCache(const char* name);

	bool m_IsInitialized;
	bool m_Destroying;
	bool m_Destroyed;
	FMOD::System* m_System;
	FMOD::Geometry* m_Geometry;


	using sound_obj_vec = std::vector<s3DSound>; // Vector of objects bound to (dynamic) objects
	using sound_vec = std::vector<FMOD::Sound*>;
	using channel_vec = std::vector<FMOD::Channel*>;
	using channelgroup_vec = std::vector<FMOD::ChannelGroup*>;
	using sound_cache = std::map<const char*, unsigned int>;
	using sound_cache_iter = sound_cache::iterator;

	sound_obj_vec m_SoundObjs;
	sound_vec m_Sounds;
	channel_vec m_Channels;
	sound_cache m_SoundCache;

	channelgroup_vec m_ChannelGroups;
};

gdpAudioNamespaceEnd

//#define GetSoundManager() audio::cSoundManager::GetInstance()