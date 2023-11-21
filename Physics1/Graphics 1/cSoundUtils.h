#pragma once

#include <fmod/fmod.hpp>

const char* GetFMODErrorString(FMOD_RESULT result);
void CheckFMODError(FMOD_RESULT result, const char* file, int line);
#define FMOD_ERROR_CHECK(result) CheckFMODError(result, __FILE__, __LINE__)
