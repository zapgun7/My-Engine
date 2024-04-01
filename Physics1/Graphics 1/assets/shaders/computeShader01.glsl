
#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(rgba32f, binding = 1) uniform image2D imgInput;

layout(rgba32f, binding = 2) uniform image2D heatMap;

layout(binding = 3) uniform sampler2D noise;

uniform vec4 currTime; // x = time


void SpikyEffect(float noiseSum, ivec2 textelCoords, vec4 value); // Creates unique spikes coming out of the model
void CurlingSpikes(float noiseSum, ivec2 textelCoords, vec4 value);


void main() {
	vec4 value = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//ivec2 textelCoord = ivec2(gl_WorkGroupID.xy);
	ivec2 textelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 stCoord = vec2(gl_GlobalInvocationID.x / 1920.0f, gl_GlobalInvocationID.y / 1080.0f);
	//vec2 smallestSTDelta = vec2(1.0f / 1920.0f, 1.0f / 1080.0f);
	
	//vec4 currentVal = imageLoad(imgInput, textelCoord);
	vec2 noiseOffset = vec2(sin(currTime.x / 50.0f) / 20.0f, currTime.x / 50.0f);
	
	vec4 noiseVal = texture(noise, stCoord + noiseOffset);
	vec4 heatVal = imageLoad(heatMap, textelCoord);
	//currentVal += imageLoad(noise, textelCoord);
	//value.xyz = imageLoad(imgInput, textelCoord).rgb;
	//vec3 currentVal = texture(imgInput, textelCoord).rgb;
	float noiseSum = (noiseVal.x + noiseVal.y + noiseVal.z) - 1.5f; // "half" its max potential
	vec4 currentVal;
	if (heatVal.x == 1.0f)
	{
		noiseSum = sin(tan(tan(noiseSum))) * 10;
		currentVal = imageLoad(imgInput, textelCoord + ivec2(noiseSum * 30));
	}
	else
	{
		currentVal = imageLoad(imgInput, textelCoord);
	}
	
	//value.xyz = currentVal.xyz;
	value = currentVal;
	//value.x = 1.0f;

	imageStore(imgOutput, textelCoord, value);
	//if (heatVal.x == 1.0f)
	//{
		//imageStore(imgOutput, textelCoord - ivec2(noiseSum * 35), value); // Just this is cool, but lets push it
	//	for (int i = 0; i < 5; i++
	//	{
	//		imageStore(imgOutput, textelCoord + ivec2(-noiseSum * 35, -noiseSum * 35), value);
	//		imageStore(imgOutput, textelCoord + ivec2(noiseSum * 35, -noiseSum * 35), value);
	//	}
	//}
	if (heatVal.x == 1.0f)
		//CurlingSpikes(noiseSum, textelCoord, value);
		SpikyEffect(noiseSum, textelCoord, value);

}


void SpikyEffect(float noiseSum, ivec2 textelCoords, vec4 value)
{
// have it come out of 2 diagonal directions
// Only works downwards, since the screen gets printed from top to bottom (I guess?)
	imageStore(imgOutput, textelCoords + ivec2(-noiseSum * 35, -noiseSum * 20), value);
	//imageStore(imgOutput, textelCoords + ivec2(noiseSum * 35, noiseSum * 35), value);
	//imageStore(imgOutput, textelCoords + ivec2(-noiseSum * 35, noiseSum * 35), value);
	imageStore(imgOutput, textelCoords + ivec2(noiseSum * 35, -noiseSum * 20), value);
}
void CurlingSpikes(float noiseSum, ivec2 textelCoords, vec4 value)
{
	for (int i = 1; i < 6; i++)
	{
		int yModifier = int(abs(pow(noiseSum * i, 2))) - textelCoords.x;
		imageStore(imgOutput, textelCoords + ivec2(-noiseSum * 35, -noiseSum * 20 ), value);
	}
}