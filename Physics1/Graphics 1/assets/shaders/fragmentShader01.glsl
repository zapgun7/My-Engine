// Fragment shader
#version 420

//in vec4 colour;
in vec4 vertexWorldPos;			// vertex in "world space"
in vec4 vertexWorldNormal;	
in vec2 textureCoords;

out vec4 outputColour;		// To the frame buffer (aka screen)

/////// Bool Vecs ///////
uniform vec4 bBoneFrag;
uniform vec4 bDontLight_CustomCol;
//uniform bool bUseBonesFrag;
//uniform bool bDoNotLight;		// Really a float (0.0 or not zero)
//uniform bool bUseCustomColor;	// if this is true, then use debugColourRGBA for the colour
//uniform bool bIsCamera;

uniform vec4 bReflect_Refract_fAlpha_NONE;
//uniform bool bUseReflect;
//uniform bool bUseRefract;
//uniform float transparencyAlpha;


uniform vec4 bUseHeightmap_IsSkyBox_UseDiscard_NONE;
//uniform bool bUseHeightMap;
//uniform bool bIsSkyBox;
//uniform bool bUseDiscardMaskTexture;



// Bone Stuff
in vec4 ex_BoneId;




// If true, then passes the colour without calculating lighting

uniform vec4 eyeLocation;


uniform vec4 customColorRGBA;		




uniform sampler2D texture_00;			// 2D meaning x,y or s,t or u,v
uniform sampler2D texture_01;
uniform sampler2D texture_02;
uniform sampler2D texture_03;
uniform sampler2D texture_04;			
uniform sampler2D texture_05;
uniform sampler2D texture_06;
uniform sampler2D texture_07;



uniform sampler2D heightMapSampler;		// Texture unit 20
uniform sampler2D discardSampler;		// Texture unit 21

// Skybox, cubemap, etc.
uniform samplerCube skyBoxTexture;

// For the discard example
uniform sampler2D maskSamplerTexture01;


uniform vec4 screenWidthAndHeight_bIsOffScreen;
//uniform bool bIsOffScreenTextureQuad;


//... and so on
//uniform float textureMixRatio[8];
uniform vec4 textureMixRatio_0_3;		// 1, 0, 0, 0 
uniform vec4 textureMixRatio_4_7;

// uv-offset
//uniform vec3 uv_Offset_Scale;
uniform vec4 uv_Offset_Scale_NONE;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	// Colour of the light (used for diffuse)
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;
const int AMBIENT_LIGHT_TYPE = 3;

const int NUMBEROFLIGHTS = 80;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 70 uniforms


struct sMaterial
{
	//vec4 ambient;
	//vec4 diffuse;
	sampler2D diffuse;
	sampler2D specular;
	vec4 power; // w is shininess
};

uniform sMaterial material;

vec4 calculateLightContrib( vec3 vertexMaterialColor, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );
							
vec4 calulateLightContribNEW( vec3 vertexMaterialColor, vec3 vertexNormal,
							  vec3 vertexWorldPos, vec4 vertexSpecular);

// 2nd pass effects
vec3 getFBOColour();

// Spooky Effects
int getDistToDistortion(vec2 textureCoords);

//EOU
void main()
{	
	if (bBoneFrag.x == 1.0f)
	{
		vec3 color = vec3(1);
		if (int(ex_BoneId.x) % 6 == 0)
		{
			color = vec3(1, 0, 0);
		}
		if (int(ex_BoneId.x) % 6 == 1)
		{
			color = vec3(1, 1, 0);
		}
		if (int(ex_BoneId.x) % 6 == 2)
		{
			color = vec3(0, 1, 0);
		}
		if (int(ex_BoneId.x) % 6 == 3)
		{
			color = vec3(1, 0, 1);
		}
		if (int(ex_BoneId.x) % 6 == 4)
		{
			color = vec3(0, 0, 1);
		}
		if (int(ex_BoneId.x) % 6 == 5)
		{
			color = vec3(0, 1, 1);
		}

		outputColour = vec4(color, 1);
		
		return;
	}






//	gl_FragColor = vec4(color, 1.0);

//	if ( bUseHeightmap_IsSkyBox_UseDiscard_NONE.x )
//	{
//		outputColour.rgba = vec4(1.0f, 0.0f, 0.0f, 1.0f);
//		return;
//	}

	// Discard the water
//	if ( bUseHeightmap_IsSkyBox_UseDiscard_NONE.x )
//	{
//		// Range of 0 to 1
//		float height = texture( heightMapSampler, textureCoords.st ).r;
//		
//		if ( height <= 0.005f )
//		{
//			discard;
//		}
//	}

	
	// Discard
	if ( bUseHeightmap_IsSkyBox_UseDiscard_NONE.z == 1.0f )
	{
		vec3 maskValues = texture( maskSamplerTexture01, (textureCoords.st + uv_Offset_Scale_NONE.xy) * uv_Offset_Scale_NONE.z ).rgb;
		float maskValue = maskValues.r + maskValues.g + maskValues.b; // Ensure it is pure black that gets removed
		// If "black" then discard
		if ( maskValue < 0.1f )
		{
			discard;
//			outputColour.rgba = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
//			return;
		}	
	}
	
	// Skybox
	if ( bUseHeightmap_IsSkyBox_UseDiscard_NONE.y == 1.0f )
	{
		//uniform samplerCube skyBoxTexture;
		vec4 skyBoxSampleColour = texture( skyBoxTexture, vertexWorldNormal.xyz ).rgba;
		outputColour.rgb = skyBoxSampleColour.rgb;
		outputColour.a = 1.0f;
		//gl_FragDepth = 0.999f;
		return;
	}
	
	// Reflect
	if (bReflect_Refract_fAlpha_NONE.x == 1.0f)
	{
		//vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);
		//vec3 reflectAngle = reflect( eyeVector, vertexWorldNormal.xyz);
		//vec4 skyBoxSampleColour = texture( skyBoxTexture, reflectAngle.xyz ).rgba;
		//outputColour.rgb = skyBoxSampleColour.rgb;
		//outputColour.a = bReflect_Refract_fAlpha_NONE.z;
		//return;
		vec4 textureColour = texture( texture_00, (textureCoords.st + uv_Offset_Scale_NONE.xy) * uv_Offset_Scale_NONE.z ).rgba * textureMixRatio_0_3.x ;
		float reflectRatio = 1 - textureMixRatio_0_3.x;
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);
		vec3 reflectAngle = reflect( eyeVector, vertexWorldNormal.xyz);
		vec4 skyBoxSampleColour = texture( skyBoxTexture, reflectAngle.xyz ).rgba * reflectRatio;
		outputColour.rgb = skyBoxSampleColour.rgb + textureColour.rgb;
		outputColour.a = bReflect_Refract_fAlpha_NONE.z;
		return;
	}
	// Refract
	else if (bReflect_Refract_fAlpha_NONE.y == 1.0f)
	{
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);
		vec3 reflectAngle = refract( eyeVector, vertexWorldNormal.xyz, 1.0);  //1.0 gives good results
		vec4 skyBoxSampleColour = texture( skyBoxTexture, reflectAngle.xyz ).rgba;
		outputColour.rgb = skyBoxSampleColour.rgb;
		outputColour.a = bReflect_Refract_fAlpha_NONE.z;
		return;
	}
	
	
	// Reflect
	// Reflects based on the eye position
	 //genType reflect( genType IncidentAngle, genType Nnormal);
	//vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);
	//vec3 reflectAngle = reflect( eyeVector, vertexWorldNormal.xyz);
	//vec3 reflectAngle = refract( eyeVector, vertexWorldNormal.xyz, 	1.0);
	
	//vec4 skyBoxSampleColour = texture( skyBoxTexture, reflectAngle.xyz ).rgba;
	
	//outputColour.rgb = skyBoxSampleColour.rgb;
	//outputColour.a = 1.0f;
	//return;
	
	

	vec4 textureColour = 
		  texture( texture_00, (textureCoords.st + uv_Offset_Scale_NONE.xy) * uv_Offset_Scale_NONE.z ).rgba * textureMixRatio_0_3.x 	
		//+ texture( texture_01, textureCoords.st ).rgba * textureMixRatio_0_3.y
		+ texture( texture_02, textureCoords.st ).rgba * textureMixRatio_0_3.z;
		//+ texture( texture_03, textureCoords.st ).rgba * textureMixRatio_0_3.w;

	// Make the 'vertex colour' the texture colour we sampled...
	
	vec4 diffuseColor = texture(material.diffuse, textureCoords.st * uv_Offset_Scale_NONE.z).rgba;
	
	//vec4 vertexRGBA = textureColour;	
	vec4 vertexRGBA = diffuseColor;

	
	// Custom Color
	if ( bDontLight_CustomCol.y == 1.0f )
	{	
		vertexRGBA = customColorRGBA;
	}
	// Do not light
	if ( bDontLight_CustomCol.x == 1.0f )
	{
		outputColour = vertexRGBA;
		outputColour.a = bReflect_Refract_fAlpha_NONE.z;
		return;
	}
	
	// *************************************
	// Hard code the specular (for now)
	vec4 vertexSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// xyzw or rgba or stuw
	// RGB is the specular highglight colour (usually white or the colour of the light)
	// 4th value is the specular POWER (STARTS at 1, and goes to 1000000s)
	
	//vec3 bumpMap = normalize(texture( texture_01, textureCoords.st ).rgb * 2.0f - vec3(1.0f)); // Should only have to add this to vertexWorldNormal in a way
	//bumpMap -= (bumpMap/2);
	
	//bumpMap.xyz = normalize(bumpMap.xyz);
	//bumpMap *= textureMixRatio_0_3.y;
	//bumpMap.xyz += vertexWorldNormal.xyz;
	//bumpMap.xyz = normalize(bumpMap.xyz);
	
	//vec4 vertexColourLit = calculateLightContrib( vertexRGBA.rgb, bumpMap, 
	//                                              vertexWorldPos.xyz, vertexSpecular );
	
	//vec4 vertexColorLit = calculateLightContrib( vertexRGBA.rgb, vertexWorldNormal.xyz, 
	//                                              vertexWorldPos.xyz, vertexSpecular );
	vec4 vertexColorLit = calulateLightContribNEW( vertexRGBA.rgb, vertexWorldNormal.xyz, 
	                                               vertexWorldPos.xyz, vertexSpecular );
	// *************************************
			
	outputColour.rgb = vertexColorLit.rgb;
	
	// Real gamma correction is a curve, but we'll Rock-n-Roll it here
	//outputColour.rgb *= 1.35f;
	
	//outputColour.a = 1.0f;
	outputColour.a = bReflect_Refract_fAlpha_NONE.z;
}


vec4 calulateLightContribNEW ( vec3 vertexMaterialColor, vec3 vertexNormal,
							   vec3 vertexWorldPos, vec4 vertexSpecular)
{
	vec4 finalObjectColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	for (int index = 0; index < NUMBEROFLIGHTS; index++)
	{
		// Skip if light is off
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		int lightType = int(theLights[index].param1.x);
		
		if (lightType == AMBIENT_LIGHT_TYPE)
		{
			//float ambientStr = 0.1f;
			vec3 ambient = theLights[index].diffuse.xyz * theLights[index].diffuse.w; // w is power
			
			finalObjectColor.rgb += ambient * vertexMaterialColor.xyz;
			continue;
		}
		
		
		// Vars needed below
		float diffPower = 0.0f;
		float attenuation = 1.0f;
		vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
		vec3 norm = normalize(vertexNormal);
		vec3 lightDir = vec3(0.0f, 0.0f, 0.0f);
		
		// Spotlight Vars
		float intensity = 1.0f;
		
		// Specular variables
		float spec = 0.0f;
		vec3 specular = vec3(0.0f, 0.0f, 0.0f);
		vec3 specMapPower = texture(material.specular, textureCoords.st * uv_Offset_Scale_NONE.z).rgb;
		vec3 viewDir = normalize(eyeLocation.xyz - vertexWorldPos);
		vec3 halfwayDir = vec3(0.0f, 0.0f, 0.0f);
		
		
		if (lightType == DIRECTIONAL_LIGHT_TYPE)
		{
			lightDir = normalize(theLights[index].direction.xyz);
			
		}
		else if (lightType == POINT_LIGHT_TYPE)
		{
			lightDir = vertexWorldPos.xyz - theLights[index].position.xyz;
			float lightDist = length(lightDir);
			lightDir = normalize(lightDir);
			attenuation = 1.0 / (theLights[index].atten.x + theLights[index].atten.y * lightDist +
							     theLights[index].atten.z * (lightDist * lightDist));
		}
		else if (lightType == SPOT_LIGHT_TYPE) 
		{
			lightDir = vertexWorldPos.xyz - theLights[index].position.xyz;
			lightDir = normalize(lightDir);
			
		    float theta = dot(lightDir, normalize(theLights[index].direction.xyz));
			
			if (theta > theLights[index].param1.z) // If within the outer ring
			{
				// This area is lit
				float lightDist = length(lightDir);
				attenuation = 1.0 / (theLights[index].atten.x + theLights[index].atten.y * lightDist +
									 theLights[index].atten.z * (lightDist * lightDist));
									 
				float epsilon = theLights[index].param1.y - theLights[index].param1.z;
				intensity = clamp((theta - theLights[index].param1.z) / epsilon, 0.0, 1.0);
			}
		    else
				continue;
		   
		}
		
		
		diffPower = max(dot(norm, -lightDir), 0.0f);
		diffuse = theLights[index].diffuse.xyz * (diffPower) * theLights[index].diffuse.w;
		
		halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(viewDir, halfwayDir), 0.0f), material.power.w); // material.power.w is shininess
		specular = (spec * specMapPower) * theLights[index].specular.xyz * theLights[index].specular.w;
		
		//vec3 lightDir = vertexWorldPos.xyz - theLights[index].position.xyz;
		//float distFromLight = length(lightDir);
		//lightDir = normalize(lightDir);
		
		//float diffPower = max(dot(norm, -lightDir), 0.0f);
		
		//vec3 diffuse = theLights[index].diffuse.xyz * (diffPower);
		
		
		// Now specular
		//float specStr = 0.5f; // For now hard coding it
		
		
		//vec3 reflectDir = reflect(-lightDir, norm);
		
		
		//float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), material.power.w); // 32 is shininess (should take from mesh object data)
		//vec3 specular = (spec * specMapPower) * theLights[index].specular.xyz;
		
		
		
		diffuse  *= attenuation * intensity;
		specular *= attenuation * intensity;
		
		specular = max(specular, 0.0f);
		
		finalObjectColor.xyz += (diffuse + specular) * vertexMaterialColor.xyz;
	
	}

	finalObjectColor.a = 1.0f;
	
	return finalObjectColor;
}


vec4 calculateLightContrib( vec3 vertexMaterialColor, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColor = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
			lightContrib *= dotProduct;		
			
			finalObjectColor.rgb += (vertexMaterialColor.rgb * theLights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			//return finalObjectColor;		
			continue;
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColor.rgb += (vertexMaterialColor.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}//for(intindex=0...
	
	finalObjectColor.a = 1.0f;
	
	return finalObjectColor;
}


//	// For now, just trust Michael
//	// Very basic directional shader
//	vec3 lightContrib = directionalLightColour * directionalLight_Direction_power.w;
//	// 
//	// Get the dot product of the light and normalize
//	float dotProduct = dot( -directionalLight_Direction_power.xyz,  
//							vertexWorldNormal.xyz );	
//	// Clamp this to a positive number
//	dotProduct = max( 0.0f, dotProduct );		// 0 to 1		
//	
//	lightContrib *= dotProduct;		