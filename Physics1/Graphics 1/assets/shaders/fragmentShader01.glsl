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



// Stuff for spooky redaction-esque effects
uniform vec4 isSpooky;// x = Generate heat map   y = use heatmap
uniform sampler2D spookyHeatMap;



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




//

uniform sampler2D heightMapSampler;		// Texture unit 20
uniform sampler2D discardSampler;		// Texture unit 21

// Skybox, cubemap, etc.

uniform samplerCube skyBoxTexture;

// For the discard example
uniform sampler2D maskSamplerTexture01;

// FBO
uniform sampler2D textureOffScreen;
//uniform vec2 screenWidthAndHeight;
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

const int NUMBEROFLIGHTS = 80;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 70 uniforms
//... is really:
//uniform vec4 theLights[0].position;
//uniform vec4 theLights[1].position;
//uniform vec4 theLights[2].position;
// etc...


vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );

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


	if (isSpooky.x == 1.0f) // If generating the heatmap
	{
		outputColour = vec4(1.0f, 0.0f, 0.0f, 1.0f); // Pure Red
		return;
	}




if (isSpooky.y == 1.0f) // Spooky heatmap application
{
	int distDist = getDistToDistortion(textureCoords);
	if (distDist != -1)
	{
		float finalSCoord = textureCoords.s;
		float finalTCoord = textureCoords.t;
		
		bool isPos = false;
		if (cos(textureCoords.s * 333 + textureCoords.t * 99) < 0) isPos = true;
		
		if (cos(textureCoords.s * 1000) < 0)
		{
			if (isPos)
				finalSCoord -= 0.001f * distDist;
			else
				finalSCoord += 0.001f * distDist;
		}
		else
		{
			if (isPos)
				finalSCoord += 0.001f * distDist;
			else
				finalSCoord -= 0.001f * distDist;
		}
		if (cos(textureCoords.t * 1000) < 0)
		{
			if (isPos)
				finalTCoord += 0.001f * distDist;
			else
				finalTCoord -= 0.001f * distDist;
		}
		else
		{
			if (isPos)
				finalTCoord -= 0.001f * distDist;
			else
				finalTCoord += 0.001f * distDist;
		}
		
		
		
		
		outputColour = vec4(texture(textureOffScreen, vec2(finalSCoord, finalTCoord)).rgb, 1.0f);
		return;
	}
	else
	{
		outputColour = vec4(texture(textureOffScreen, textureCoords.st).rgb, 1.0f);
		return;
	}
}

if (isSpooky.z == 1.0f)
{
	vec3 spookyTexValue = texture( spookyHeatMap, textureCoords.st ).rgb;
	spookyTexValue.r -= (1.0f * isSpooky.w);
	if (spookyTexValue.r < 0) spookyTexValue.r = 0;
	
	
	outputColour = vec4(spookyTexValue, 1.0f);
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
	
	if ( screenWidthAndHeight_bIsOffScreen.z == 1.0f )
	{
//		outputColour.rgb = vec3(1.0f, 0.0f, 0.0f);
//		vec3 theColour = texture( textureOffScreen, fTextureCoords.st ).rgb;		
//		outputColour.rgb = theColour;						   
//		outputColour.a = 1.0f;
//		return;
//
		// Convert the integer pixel location (10, 15) or (1902, 546)
		// 	into a texture coordinate from 0.0 to 1.0
//		vec2 textCoordsScreen = vec2( gl_FragCoord.x / screenWidthAndHeight_bIsOffScreen.x, 
//		                              gl_FragCoord.y / screenWidthAndHeight_bIsOffScreen.y );
//		vec3 theColour = texture( textureOffScreen, textCoordsScreen.st ).rgb;	
//		vec3 HUDTextureColour = texture( textureHUF, textCoordsScreen.st).rgb;
//
//		outputColour.rgb = theColour * 0.5f + HUDTextureColour.rgb * 0.5f;
//
		// All of these assume we are sampling from the textureOffscreen sampler

		outputColour.rgb = getFBOColour();
//		outputColour.rgb = ChromicAberration(0.5f);
//		outputColour.rgb = BasicBlurScreen();

		// 3 gives this: *** * *** = 7x7= 49
		// 5 gives this: ***** * ***** = 11x11 = 121 samples
//		outputColour.rgb = BlurScreen(25);

		//outputColour.rgb = BlurScreenFaster(25);
		
		outputColour.a = 1.0f;
		return;
	}

	

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
	
	if ( bUseHeightmap_IsSkyBox_UseDiscard_NONE.y == 1.0f )
	{
		//uniform samplerCube skyBoxTexture;
		vec4 skyBoxSampleColour = texture( skyBoxTexture, vertexWorldNormal.xyz ).rgba;
		outputColour.rgb = skyBoxSampleColour.rgb;
		outputColour.a = 1.0f;
		return;
	}
	
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
	
	
	vec4 textureColour;
	

	textureColour = 
		  texture( texture_00, (textureCoords.st + uv_Offset_Scale_NONE.xy) * uv_Offset_Scale_NONE.z ).rgba * textureMixRatio_0_3.x 	
		//+ texture( texture_01, textureCoords.st ).rgba * textureMixRatio_0_3.y
		+ texture( texture_02, textureCoords.st ).rgba * textureMixRatio_0_3.z;
		//+ texture( texture_03, textureCoords.st ).rgba * textureMixRatio_0_3.w;

	// Make the 'vertex colour' the texture colour we sampled...
	vec4 vertexRGBA = textureColour;	

	
	if ( bDontLight_CustomCol.y == 1.0f )
	{	
		vertexRGBA = customColorRGBA;
	}

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
	
	vec4 vertexColourLit = calculateLightContrib( vertexRGBA.rgb, vertexWorldNormal.xyz, 
	                                              vertexWorldPos.xyz, vertexSpecular );
	// *************************************
			
	outputColour.rgb = vertexColourLit.rgb;
	
	// Real gamma correction is a curve, but we'll Rock-n-Roll it here
	outputColour.rgb *= 1.35f;
	
	//outputColour.a = 1.0f;
	outputColour.a = bReflect_Refract_fAlpha_NONE.z;
}

vec3 getFBOColour()
{
	vec2 textCoordsScreen = vec2( gl_FragCoord.x / screenWidthAndHeight_bIsOffScreen.x, 
	                              gl_FragCoord.y / screenWidthAndHeight_bIsOffScreen.y );
	vec3 theColour = texture( textureOffScreen, textCoordsScreen.st ).rgb;	
	
	return theColour;
}


int getDistToDistortion(vec2 textureCoords)
{
	//if (texture(spookyHeatMap, vec2(textureCoords.s + (50.0f / 1920.0f), textureCoords.t)).r > 0.0f) return 1;
	//return -1;
	
	
	
	//vec3 spookValue = texture(spookyHeatMap, textureCoords.st).rgb;
	//if (spookValue.r > 0) return 0;
	
//	int distOut = 0;
	// Now scan out to find distance to effect, (max 10)
	for(int i = 1; i < 20; i++)
	{
		float posOffsetS = i / screenWidthAndHeight_bIsOffScreen.x;
		float posOffsetT = i / screenWidthAndHeight_bIsOffScreen.y;
//		distOut++;
		// Diagonal 1
		if ( texture(spookyHeatMap, vec2(textureCoords.s - posOffsetS, textureCoords.t - posOffsetT)).r > 0) return i;
		// Diagonal 2
		if ( texture(spookyHeatMap, vec2(textureCoords.s + posOffsetS, textureCoords.t - posOffsetT)).r > 0) return i;
		// Diagonal 3
		if ( texture(spookyHeatMap, vec2(textureCoords.s - posOffsetS, textureCoords.t + posOffsetT)).r > 0) return i;
		// Diagonal 4
		if ( texture(spookyHeatMap, vec2(textureCoords.s + posOffsetS, textureCoords.t + posOffsetT)).r > 0) return i;
		
		// Cardinal 1
		if ( texture(spookyHeatMap, vec2(textureCoords.s, textureCoords.t + posOffsetT)).r > 0) return i;
		// Cardinal 2
		if ( texture(spookyHeatMap, vec2(textureCoords.s, textureCoords.t - posOffsetT)).r > 0) return i;
		// Cardinal 3
		if ( texture(spookyHeatMap, vec2(textureCoords.s + posOffsetS, textureCoords.t)).r > 0) return i;
		// Cardinal 4
		if ( texture(spookyHeatMap, vec2(textureCoords.s - posOffsetS, textureCoords.t)).r > 0) return i;
	}
	
	return -1; // Not in range
}


vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
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
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			//return finalObjectColour;		
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
		
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}//for(intindex=0...
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
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