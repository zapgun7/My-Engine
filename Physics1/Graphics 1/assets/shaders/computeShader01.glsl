
#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(rgba32f, binding = 1) uniform image2D imgInput;

layout(rgba32f, binding = 2) uniform image2D heatMap;

layout(binding = 3) uniform sampler2D noise;

uniform vec4 currTime; // x = time 			 y = mouseX  z = mouseY (overall)


void SpikyEffect(float noiseSum, ivec2 textelCoords, vec4 value); // Creates unique spikes coming out of the model
void CurlingSpikes(float noiseSum, ivec2 textelCoords, vec4 value, vec3 time_mouse);


void main() {
	vec4 value = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//ivec2 textelCoord = ivec2(gl_WorkGroupID.xy);
	ivec2 textelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 stCoord = vec2(gl_GlobalInvocationID.x / 1920.0f, gl_GlobalInvocationID.y / 1080.0f);
	//vec2 smallestSTDelta = vec2(1.0f / 1920.0f, 1.0f / 1080.0f);
	
	
	float mouseXInfluence = currTime.y / 960; // Half resolution
	float mouseYInfluence = currTime.z / 540; // 
	
	// Gradual scroll of the texture
	vec2 noiseOffset = vec2(
							sin(currTime.x / 50.0f) / 20.0f + mouseXInfluence, // Very small horizontal oscilation
							currTime.x / 60.0f - mouseYInfluence               // Slow scroll down (60s full cycle)
							);
	
	
	// Magnify the perlin noise
	float noiseZoom = (sin
						  (currTime.x / 30.0f) // Complete cycle = 30s
						  * 0.3 // Oscilates my a min-max of [-0.3, 0.3]
						  + .9 // Baseline, final oscilation: [1.2, 1.8]
						  ); 
	
	
	vec4 noiseVal = texture(noise, stCoord * noiseZoom + noiseOffset); // Add this for perlin texture scrolling
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
		CurlingSpikes(noiseSum, textelCoord, value, currTime.xyz);
		//SpikyEffect(noiseSum, textelCoord, value);

}


void SpikyEffect(float noiseSum, ivec2 textelCoords, vec4 value)
{
// have it come out of 2 diagonal directions
// Only works downwards, since the screen gets printed from bottom to top (I guess?)

	imageStore(imgOutput, textelCoords + ivec2(-noiseSum * 35, -noiseSum * 20), value);

	imageStore(imgOutput, textelCoords + ivec2(noiseSum * 35, -noiseSum * 20), value);
}
void CurlingSpikes(float noiseSum, ivec2 textelCoords, vec4 value, vec3 time_mouse)
{	
	// Slow amp of 2
	float slowAmp = sin(currTime.x / 20) * 2; 
	
	int spikeCurve = int(pow(
							noiseSum 
							+ sin(cos(pow(slowAmp * 3, 2))) // Slow oscilitaion from erratic change, to slow, back to erratic
							+ sin(time_mouse.x - time_mouse.z / 540 + (textelCoords.t / 300) // Shift based on horizontal coord
							+ (textelCoords.s / 100 + time_mouse.y / 960) // Shift based on vertical coord
							) * 2, 2)); // Multiply all by 1.5 and power to 2
							
	vec4 testVal;
	//if (noiseSum
	
	for (int i = 0; i < 2; i++) // Make it denser
	{
		imageStore(imgOutput, textelCoords + ivec2(-noiseSum * (35 + i), (-noiseSum * (20 + i)) + spikeCurve), value);
		imageStore(imgOutput, textelCoords + ivec2(noiseSum * (35 + i), (-noiseSum * (20 + i)) + spikeCurve), value);
	}
}