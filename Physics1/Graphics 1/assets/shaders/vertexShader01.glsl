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

//uniform bool UseBones;
uniform vec4 bUseBones; // yzw empty spaces for future variables
uniform mat4 BoneMatrices[50];

//uniform vec3 modelScale;
//uniform vec3 modelOffset;

//in vec4 vCol;		// was vec3
in vec4 vPos;		// was vec3

in vec4 vNormal;	// NEW for 2023!
in vec4 vTextureCoords; // Keeping this vec2 for now
in ivec4 vBoneIds;
in vec4 vBoneWeights;

//out vec4 colour;
out vec4 vertexWorldPos;	
out vec4 vertexWorldNormal;
out vec2 textureCoords;

// Bones
out vec4 ex_BoneId;

void main()
{
	
	vec4 position = vPos;
	
	
	if (bUseBones.x == 1.0)
	{
		ex_BoneId = vBoneIds;
		
		

		mat4 boneTransform = BoneMatrices[vBoneIds.x] * vBoneWeights.x;
			boneTransform += BoneMatrices[vBoneIds.y] * vBoneWeights.y;
			boneTransform += BoneMatrices[vBoneIds.z] * vBoneWeights.z;
			boneTransform += BoneMatrices[vBoneIds.w] * vBoneWeights.w;
		position = boneTransform * vPos;
	}
	
	
//	gl_Position = MVP * vec4(finalPos, 1.0);
//	gl_Position = MVP * vertModelPosition;
	mat4 matMVP = matProjection * matView * matModel;
	//gl_Position = matMVP * vec4(vPos.xyz, 1.0);
	gl_Position = matMVP * vec4(position.xyz, 1.0);
	//gl_Position = vec4(vPos.xyz, 1.0f);
	// Rotate the normal by the inverse transpose of the model matrix
	// (so that it only is impacted by the rotation, not translation or scale)
	vertexWorldNormal = matModel_IT * vec4(vNormal.xyz, 1.0f);
	vertexWorldNormal.xyz = normalize(vertexWorldNormal.xyz);
	vertexWorldNormal.w = 1.0f;
	
	vertexWorldPos = matModel * vec4( position.xyz, 1.0f);
	
//	colour = vCol;
	
	textureCoords.xy = vTextureCoords.xy;
}
