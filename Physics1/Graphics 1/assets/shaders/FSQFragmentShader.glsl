#version 420


uniform vec4 screenWidth_Height; // 2x empty floats

out vec4 outputColour;

uniform sampler2D FSQTex;

void main()
{
	//vec3 texCol = texture(FSQTex, TexCoords).rgb;
	vec2 textureCoords = vec2( gl_FragCoord.x / screenWidth_Height.x, 
							   gl_FragCoord.y / screenWidth_Height.y );
	vec3 texCol = texture(FSQTex, textureCoords).rgb;
	outputColour.rgb = texCol;
	outputColour.a = 1.0f;
	return;
}