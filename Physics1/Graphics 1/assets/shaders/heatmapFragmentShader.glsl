
#version 420

out vec4 outColor;

// Very simple shader, we just color any pixel that goes through here, red!
// Maybe we can put this in the regular frag shader, so it discards if behind other objects (would have to make sure to draw that object last)
// For now we have this to perform simple tests
void main()
{
	outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	return;
}