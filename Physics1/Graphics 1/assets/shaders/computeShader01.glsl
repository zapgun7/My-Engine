
#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(rgba32f, binding = 1) uniform image2D imgInput;


void main() {
	vec4 value = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//ivec2 textelCoord = ivec2(gl_WorkGroupID.xy);
	ivec2 textelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 stCoord = vec2(gl_GlobalInvocationID.x / 1920.0f, gl_GlobalInvocationID.y / 1080.0f);
	
	vec4 currentVal = imageLoad(imgInput, textelCoord);
	//value.xyz = imageLoad(imgInput, textelCoord).rgb;
	//vec3 currentVal = texture(imgInput, textelCoord).rgb;
	if (textelCoord.x % 5 == 0)
	{
		textelCoord.y += 3;
	}
	if (textelCoord.y % 5 == 0)
	{
		textelCoord.x += 5;
	}
	//value.xyz = currentVal.xyz;
	value = currentVal;
	//value.x = 1.0f;

	imageStore(imgOutput, textelCoord, value);

}