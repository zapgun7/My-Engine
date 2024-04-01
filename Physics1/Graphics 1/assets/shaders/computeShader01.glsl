
#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;


void main() {
	vec4 value = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	ivec2 textelCoord = ivec2(gl_GlobalInvocationID.xy);
	
	value.x = float(textelCoord.x) / (gl_NumWorkGroups.x);
	value.y = float(textelCoord.y) / (gl_NumWorkGroups.y);
	
	imageStore(imgOutput, textelCoord, value);

}