#version 420


in vec4 vPos;		// was vec3



void main()
{
	//TexCoords = vTextureCoords.xy;
	//gl_Position = vec4(vPos.xyz, 1.0f);
	//gl_Position = matMVP * vec4(vPos.xyz, 1.0);
	
	gl_Position = vec4(vPos.xyz * 2.0f, 1.0f); // 2x is perfect for fitting (at least my) screen ratio
}