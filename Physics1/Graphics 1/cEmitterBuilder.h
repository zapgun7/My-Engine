#pragma once

// This will take a string or enum or whatever and build presets for an emitter (i.e. smoke, sparks, water splashing

// This loads the bare minimum of the effect, other parameters are set elsewhere (density, reach, etc.)

struct sEmitterInfo;

enum EmitterPreset
{
	DEFAULT
};

class cEmitterBuilder 
{
public:
	static void makeEmitter(EmitterPreset preset, sEmitterInfo* info);
};