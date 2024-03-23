#include "cSoundLuaBrain.h"

#include <iostream>

int lua_PlaySound(lua_State* L);



cSoundLuaBrain* cSoundLuaBrain::m_pTheOneSoundBrain = nullptr;

cSoundLuaBrain* cSoundLuaBrain::GetInstance(void)
{
	if (cSoundLuaBrain::m_pTheOneSoundBrain == nullptr)
	{
		cSoundLuaBrain::m_pTheOneSoundBrain = new cSoundLuaBrain();
		cSoundLuaBrain::m_pTheOneSoundBrain->Initialize();
	}
	return cSoundLuaBrain::m_pTheOneSoundBrain;
}

void cSoundLuaBrain::RunScriptImmediately(std::string script)
{
	int error = luaL_loadstring(this->m_pLuaState, script.c_str());

	if (error != 0) // 0 = no error
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->m_DecodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	error = lua_pcall(this->m_pLuaState,	/* lua state */
		0,	/* nargs: number of arguments pushed onto the lua stack */
		0,	/* nresults: number of results that should be on stack at end*/
		0);	/* errfunc: location, in stack, of error function.
				if 0, results are on top of stack. */

	if (error != 0 /*no error*/)
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_DecodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(this->m_pLuaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
	}

	return;
}

cSoundLuaBrain::cSoundLuaBrain()
{

}

cSoundLuaBrain::~cSoundLuaBrain()
{
	lua_close(this->m_pLuaState);
	return;
}

void cSoundLuaBrain::Initialize(void)
{
	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState);



	// Create command calls in this format:
// 	lua_pushcfunction(this->m_pLuaState, lua_AddSerialMoveObjectCommand);
// 	lua_setglobal(this->m_pLuaState, "AddSerialMove");
	lua_pushcfunction(this->m_pLuaState, lua_PlaySound);
	lua_setglobal(this->m_pLuaState, "PlaySound");


	InitializeCommandCPP();
}

std::string cSoundLuaBrain::m_DecodeLuaErrorToString(int error)
{
	switch (error)
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}

