#include "cSoundLuaBrain.h"

#include "cSoundManager.h"


cSoundManager* g_SoundManager;



void cSoundLuaBrain::InitializeCommandCPP()
{
	g_SoundManager = cSoundManager::GetInstance();
}



int lua_PlaySound(lua_State* L)
{
	// For now will only pass a string for the sound name

	//bool testBool(lua_toboolean(L, 1));
	float soundPitch(lua_tonumber(L, 1));
	std::string soundName(lua_tostring(L, 2));

	FMOD::Channel* temp = g_SoundManager->StartSound(soundName); // Putting in pitch setting here, though def not the right spot to do it
	temp->setPitch(soundPitch);
	temp->setVolume(0.5f);

	return 1;
}