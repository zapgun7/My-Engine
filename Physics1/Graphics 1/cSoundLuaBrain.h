#ifndef _cSoundLuaBrain_HG_
#define _cSoundLuaBrain_HG_

extern "C" {
#include <Lua5.4.6/lua.h>
#include <Lua5.4.6/lauxlib.h>
#include <Lua5.4.6/lualib.h>
}

#include <string>

class cSoundLuaBrain
{
public:
	static cSoundLuaBrain* GetInstance(void);

	void RunScriptImmediately(std::string script);



private:

	// Singleton and creation //
	cSoundLuaBrain();
	~cSoundLuaBrain();
	static cSoundLuaBrain* m_pTheOneSoundBrain;
	void Initialize(void);
	void InitializeCommandCPP(void);
	// ////////////////////// //

	lua_State* m_pLuaState;

	std::string m_DecodeLuaErrorToString(int error);
};






#endif