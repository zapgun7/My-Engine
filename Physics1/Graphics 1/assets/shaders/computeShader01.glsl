
#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(rgba32f, binding = 1) uniform image2D imgInput;

//layout(rgba32f, binding = 2) uniform image2D heatMap;
layout(binding = 2) uniform sampler2D heatMap;

layout(binding = 3) uniform sampler2D noise;

layout ( std140, binding = 4 ) buffer Bl
{
	vec4 Bool;
};

uniform vec4 currTime; // x = time 			 y = mouseX  z = mouseY (overall) ,, w = blur cycle


void SpikyEffect(float noiseSum, ivec2 textelCoords, vec4 value); // Creates unique spikes coming out of the model
void CurlingSpikes(float noiseSum, ivec2 textelCoords, vec4 value, vec3 time_mouse, float effectIntensity, vec4 originalPixel);
float getBlurredHeatMap(vec2 stCoord , int cycle);


void main() {
	vec4 value = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//ivec2 textelCoord = ivec2(gl_WorkGroupID.xy);
	ivec2 textelCoord = ivec2(gl_GlobalInvocationID.xy);
	
	// /////// ////
	
	
	
	
	vec2 stCoord = vec2(gl_GlobalInvocationID.x / 1920.0f, gl_GlobalInvocationID.y / 1080.0f);
	
	
	float mouseXInfluence = currTime.y / 1920; // Half resolution
	float mouseYInfluence = currTime.z / 1080; // 
	ivec2 mouseInflVec = ivec2(mouseXInfluence, mouseYInfluence);
	
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
	
	
	vec4 noiseVal = texture(noise, stCoord  // Noise vec, current uv-coord
							* noiseZoom // "zoom" on texture
							+ noiseOffset); // Offset (scrolling)
							//- vec2(mouseXInfluence, mouseYInfluence)); // Have this effect not be moved around by player movemnt (counter offset)
							
	//float heatVal = texture(heatMap, stCoord).r;// imageLoad(heatMap, textelCoord).r;
	float heatVal = getBlurredHeatMap(stCoord,
									  int(currTime.w)); // Cycle count
	vec4 inputVal = imageLoad(imgInput, textelCoord); // What the current pixel is without effects
	
	if (heatVal > 0.95f) Bool.x = 1.0f;
	
	// Testing Zone
	//value = vec4(  (imageLoad(imgInput, textelCoord).rgb + texture(heatMap, stCoord).rgb), 1.0f  );
	//imageStore (imgOutput, textelCoord, value);
	//return;
	// End of Testing Zone
	
	
	//currentVal += imageLoad(noise, textelCoord);
	//value.xyz = imageLoad(imgInput, textelCoord).rgb;
	//vec3 currentVal = texture(imgInput, textelCoord).rgb;
	float noiseSum = (noiseVal.x + noiseVal.y + noiseVal.z) - 1.5f; // "half" its max potential
	vec4 currentVal;
	if (heatVal > 0.0f)
	{
		noiseSum = sin(tan(tan(noiseSum))) * 10 * heatVal;
		currentVal = imageLoad(imgInput, textelCoord  - mouseInflVec + ivec2(noiseSum * 30 * heatVal.x)) * (heatVal.x) // The warped pixel * heatmap intensity
					 + inputVal * (1.0f - heatVal); // Remainder of heatmap intensity given to original pixel
	}
	else
	{
		currentVal = inputVal;//imageLoad(imgInput, textelCoord); // No change
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
	if (heatVal > 0.0f)
		CurlingSpikes(noiseSum, textelCoord + mouseInflVec, value, currTime.xyz, heatVal, inputVal);
		//SpikyEffect(noiseSum, textelCoord, value);

}

void SpikyEffect(float noiseSum, ivec2 textelCoords, vec4 value)
{
// have it come out of 2 diagonal directions
// Only works downwards, since the screen gets printed from bottom to top (I guess?)

	imageStore(imgOutput, textelCoords + ivec2(-noiseSum * 35, -noiseSum * 20), value);

	imageStore(imgOutput, textelCoords + ivec2(noiseSum * 35, -noiseSum * 20), value);
}
void CurlingSpikes(float noiseSum, ivec2 textelCoords, vec4 value, vec3 time_mouse, float effectIntensity, vec4 originalPixel)
{	
	// Slow amp of 2
	float slowAmp = sin(currTime.x / 20) * 2; 
	
	// Taper the intensity of the effect; make it seem more etherial, less physical
	effectIntensity = pow(effectIntensity *0.8f, 2);
	
	int spikeCurve = int(pow(
							noiseSum 
							+ sin(cos(pow(slowAmp * 3, 2))) // Slow oscilitaion from erratic change, to slow, back to erratic
							+ sin(time_mouse.x - time_mouse.z / 1080 + (textelCoords.t / 300) // Shift based on horizontal coord
							+ (textelCoords.s / 100 + time_mouse.y / 1920) // Shift based on vertical coord
							) * 2, 2)); // Multiply all by 1.5 and power to 2
							
	vec4 testVal;
	//if (noiseSum
	
	for (int i = 0; i < 2; i++) // Make it denser
	{
		imageStore(imgOutput, textelCoords + ivec2(-noiseSum * (35 + i), (-noiseSum * (20 + i)) + spikeCurve), value * effectIntensity + originalPixel * (1.0f - effectIntensity));
		imageStore(imgOutput, textelCoords + ivec2(noiseSum * (35 + i), (-noiseSum * (20 + i)) + spikeCurve), value * effectIntensity + originalPixel * (1.0f - effectIntensity));
	}
}





//////////////////////////////////////
////////// THE BLUR MINES ////////////
//////////////////////////////////////




// This function takes the current uv-coord and a cycle (algorithm for those handled by CPU)
// Each cycle has a different set of points relative to the target pixel to grab
// Gets average of target pixel + 2 offset pixels
// This should create some noise while also providing good blurring effect assuming the fps is high enough
float getBlurredHeatMap(vec2 stCoord, int cycle)
{
	vec2 minMove = vec2(1.0f / 1920, 1.0f / 1080.0f);
	
	// Template: Direction (implies opposite too) r1 (radius of 1; r2 = radius of 2)
	// 								^^ only have to label half
	
	switch(cycle)
	{
	case 0: // Left-Up
		stCoord += vec2(-minMove.s, minMove.t); 
		break;
	case 1: // Up 
		stCoord += vec2(0.0f, minMove.t); 
		break;
	case 2: // Right-Up
		stCoord += vec2(minMove.s, minMove.t);
		break;
	case 3: // Right 
		stCoord += vec2(minMove.s, 0.0f);
		break;
	
	
	case 4: // Left-Left-Up
		stCoord += vec2(-minMove.s * 2.0f, minMove.t); 
		break;
	case 5: // Left-Up-Up
		stCoord += vec2(-minMove.s, minMove.t * 2.0f); 
		break;
	case 6: // Up-Up
	
		stCoord += vec2(0.0f, minMove.t * 2.0f); 
		break;
	case 7: // Right-Up-Up
		stCoord += vec2(minMove.s, minMove.t * 2.0f); 
		break;
	case 8: // Right-Right-Up
		stCoord += vec2(minMove.s * 2.0f, minMove.t); 
		break;
	case 9: // Right-Right
		stCoord += vec2(minMove.s * 2.0f, 0.0f); 
		break;
	
	
	case 10: // Left-Left-Left-Up
		stCoord += vec2(-minMove.s * 3.0f, minMove.t); 
		break;
	case 11: // Left-Left-Up-Up
		stCoord += vec2(-minMove.s * 2.0f, minMove.t * 2.0f); 
		break;
	case 12: // Left-Up-Up-Up
		stCoord += vec2(-minMove.s, minMove.t * 3.0f); 
		break;
	case 13: // Up-Up-Up
		stCoord += vec2(0.0f , minMove.t * 3.0f); 
		break;
	case 14: // Right-Up-Up-Up
		stCoord += vec2(minMove.s, minMove.t * 3.0f); 
		break;
	case 15: // Right-Right-Up-Up
		stCoord += vec2(minMove.s * 2.0f, minMove.t * 2.0f); 
		break;
	case 16: // Right-Right-Right-Up
		stCoord += vec2(minMove.s * 3.0f, minMove.t); 
		break;
	case 17: // Right-Right-Right
		stCoord += vec2(minMove.s * 3.0f, 0.0f); 
		break;
	
	
	case 18: // Left-Left-Left-Up-Up
		stCoord += vec2(-minMove.s * 3.0f, minMove.t * 2.0f); 
		break;
	case 19: // L-L-U-U-U
		stCoord += vec2(-minMove.s * 2.0f, minMove.t * 3.0f); 
		break;
	case 20: // U-U-U-U
		stCoord += vec2(0.0f, minMove.t * 4.0f); 
		break;
	case 21: // R-R-U-U-U
		stCoord += vec2(minMove.s * 2.0f, minMove.t * 3.0f); 
		break;
	case 22: // R-R-R-U-U
		stCoord += vec2(minMove.s * 3.0f, minMove.t * 2.0f); 
		break;
	case 23: // R-R-R-R
		stCoord += vec2(minMove.s * 4.0f, 0.0f); 
		break;
	
	
	
	case 24: // L-L-L-L-U
		stCoord += vec2(-minMove.s * 4.0f, minMove.t); 
		break;
	case 25: // L-L-L-L-U-U
		stCoord += vec2(-minMove.s * 4.0f, minMove.t * 2.0f); 
		break;
	case 26: // LLL-UUU
		stCoord += vec2(-minMove.s * 3.0f, minMove.t * 3.0f); 
		break;
	case 27: // LL-UUUU
		stCoord += vec2(-minMove.s * 2.0f, minMove.t * 4.0f); 
		break;
	case 28: // L-UUUU
		stCoord += vec2(-minMove.s , minMove.t * 4.0f); 
		break;
	case 29: // U5
		stCoord += vec2(0.0f, minMove.t * 5.0f); 
		break;
	case 30: // R-U4
		stCoord += vec2(minMove.s, minMove.t * 4.0f); 
		break;
	case 31: // R2-U4
		stCoord += vec2(minMove.s * 2.0f, minMove.t * 4.0f); 
		break;
	case 32: // R3-U3
		stCoord += vec2(minMove.s * 3.0f, minMove.t * 3.0f); 
		break;
	case 33: // R4-U2
		stCoord += vec2(minMove.s * 4.0f, minMove.t * 2.0f); 
		break;
	case 34: // R4-U1
		stCoord += vec2(minMove.s * 4.0f, minMove.t); 
		break;
	case 35: // R5
		stCoord += vec2(minMove.s * 5.0f, 0.0f); 
		break;
	
	case 36: // L5-U1
		stCoord += vec2(-minMove.s * 5.0f, minMove.t); 
		break;
	case 37: // L4-U3
		stCoord += vec2(-minMove.s * 4.0f, minMove.t * 3.0f); 
		break;
	case 38: // L3-U4
		stCoord += vec2(-minMove.s * 3.0f, minMove.t * 4.0f); 
		break;
	case 39: // L1-U5
		stCoord += vec2(-minMove.s, minMove.t * 5.0f); 
		break;
	case 40: // R1-U5
		stCoord += vec2(minMove.s , minMove.t * 5.0f); 
		break;
	case 41: // R3-U4
		stCoord += vec2(minMove.s * 3.0f, minMove.t * 4.0f); 
		break;
	case 42: // R4-U3
		stCoord += vec2(minMove.s * 4.0f, minMove.t * 3.0f); 
		break;
	case 43: // R5-U1
		stCoord += vec2(minMove.s * 5.0f, minMove.t); 
		break;
	
	}// End of switch
	
	
	float offsetScale = 1.0f;
	if (cycle < 4)
	{
		offsetScale = 0.6f;
	}
	else if (cycle < 10)
	{
		offsetScale = 0.5f;
	}
	else if (cycle < 18)
	{
		offsetScale = 0.4f;
	}
	else if (cycle < 24)
	{
		offsetScale = 0.3f;
	}
	else// if (cycle < 36)
	{
		offsetScale = 0.2f;
	}
	
	
	
	// Target Pixel
	float outputVal = texture(heatMap, stCoord).r * 1.5f;
	
	// Offset Pixels
	outputVal += texture(heatMap, vec2(stCoord.s + minMove.s, stCoord.t  + minMove.t)).r * offsetScale;
	outputVal += texture(heatMap, vec2(stCoord.s - minMove.s, stCoord.t  - minMove.t)).r * offsetScale;
	
	
	outputVal /= (1.5f + offsetScale * 2.0f); 

	return outputVal;
}