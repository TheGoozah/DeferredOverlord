struct Light
{
	float4 Direction;
	float4 Position;
	float4 Color;
	float Intensity;
	float Range;
	float SpotLightAngle;
	int Type;
};

struct Material
{
	float4 DiffuseColor;
	float4 SpecularLevel;
	float Shininess;
};

struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};

//CONVERSION
//**********
float4 DepthToWorldPosition(float depth, float2 screenCoordinate, float2 screenDimension, float4x4 matrixViewProjectionInverse)
{
	float2 normalizedScreenCoord = screenCoordinate / screenDimension;
	float2 ndcXY = float2(normalizedScreenCoord.x, 1.0f - normalizedScreenCoord.y) * 2.0f - 1.0f;
	float4 ndc = float4(ndcXY, depth, 1.0f);
	float4 posW = mul(ndc, matrixViewProjectionInverse);
	return posW / posW.w;
}

float4 DepthToWorldPosition_QUAD(float depth, float2 texCoords, float4x4 matrixViewProjectionInverse)
{
	float2 ndcXY = float2(texCoords.x, 1.0f - texCoords.y) * 2.0f - 1.0f;
	float4 ndc = float4(ndcXY, depth, 1.0f);
	float4 posW = mul(ndc, matrixViewProjectionInverse);
	return posW / posW.w;
}

//LIGHTING CALCULATIONS
//*********************
float4 DoDiffuse(Light light, float4 L, float4 N)
{
	//Lambert Diffuse
	float diffuseStrength = dot(N, L);
	diffuseStrength = saturate(diffuseStrength);

	return light.Color * diffuseStrength;
}

float4 DoSpecular(Light light, Material mat, float4 V, float4 L, float4 N)
{
	//Phong Specular
	float4 reflectedVector = reflect(-L, N);
	float specularStrength = dot(V, reflectedVector);
	specularStrength = saturate(specularStrength);
	specularStrength = pow(specularStrength, mat.Shininess);

	return light.Color * specularStrength;
}

float DoAttenuation(Light light, float distance)
{
	return 1.0f - smoothstep(light.Range * 0.75f, light.Range, distance);
}

float DoSpotCone(Light light, float4 L)
{
	//cos(90 degrees) > 0
	//cos(0 degrees) > 1
	// If the cosine angle of the light's direction 
	// vector and the vector from the light source to the point being 
	// shaded is less than minCos, then the spotlight contribution will be 0.
	float minCos = cos(radians(light.SpotLightAngle));
	// If the cosine angle of the light's direction vector
	// and the vector from the light source to the point being shaded
	// is greater than maxCos, then the spotlight contribution will be 1.
	float maxCos = lerp(minCos, 1, 0.5f);
	float cosAngle = dot(light.Direction, -L);
	// Blend between the maxixmum and minimum cosine angles.
	return smoothstep(minCos, maxCos, cosAngle);
}

//Directional
LightingResult DoDirectionalLighting(Light light, Material mat, float4 L, float4 V, float4 N)
{
	LightingResult result;

	result.Diffuse = DoDiffuse(light, L, N) * light.Intensity;
	result.Specular = DoSpecular(light, mat, L, V, N) * light.Intensity;

	return result;
}

//PointLight
LightingResult DoPointLighting(Light light, Material mat, float4 V, float4 N, float4 P)
{
	LightingResult result;

	float4 L = light.Position - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);

	result.Diffuse = DoDiffuse(light, L, N) *
		attenuation * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) *
		attenuation * light.Intensity;

	return result;
}

//SpotLight
LightingResult DoSpotLighting(Light light, Material mat, float4 V, float4 N, float4 P)
{
	LightingResult result;

	float4 L = light.Position - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);
	float spotIntensity = DoSpotCone(light, L);

	result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * attenuation * spotIntensity * light.Intensity;

	return result;
}