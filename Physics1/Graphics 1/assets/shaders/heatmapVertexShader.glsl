// Vertex shader
#version 420

//uniform mat4 MVP;

layout  (std140) uniform Matrices
{
	mat4 matView;
	mat4 matProjection;
};

//uniform mat4 matView;
//uniform mat4 matProjection;
uniform mat4 matModel;			// Later...
uniform mat4 matModel_IT;		// Inverse transpose of the model matrix



//in vec4 vCol;		// was vec3
in vec4 vPos;		// was vec3





void main()
{
	mat4 matMVP = matProjection * matView * matModel;

	gl_Position = matMVP * vec4(vPos.xyz, 1.0);
}
