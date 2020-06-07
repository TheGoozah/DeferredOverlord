//**************//
// BASIC EFFECT //
//**************//

/*
	- Diffuse Color/Texture
	- Specular Color
	- SpecularLevel Texture (Blinn)
	- Specular Intensity (Shininess)
	- NormalMap Texture
	- Ambient Color
	- Ambient Intensity [0-1]
	- Opacity Texture/Value [0-1]
*/

//GLOBAL MATRICES
//***************
// The World View Projection Matrix
float4x4 gMatrixWorldViewProj : WORLDVIEWPROJECTION;
// The ViewInverse Matrix - the third row contains the camera position!
float4x4 gMatrixViewInv : VIEWINVERSE;
// The World Matrix
float4x4 gMatrixWorld : WORLD;

//STATES
//******
RasterizerState gRasterizerState 
{ 
	FillMode = SOLID;
	CullMode = BACK; 
};

BlendState gEnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

BlendState gDisableBlending 
{     
	BlendEnable[0] = FALSE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState gDepthState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

//SAMPLER STATES
//**************
SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = ANISOTROPIC;
 	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

//LIGHT
//*****
float3 gLightDirection:DIRECTION
<
	string UIName = "Light Direction";
	string Object = "TargetLight";
> = float3(0.577f, 0.577f, 0.577f);

//DIFFUSE
//*******
bool gUseDiffuseTexture
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Bool";
> = false;

float4 gColorDiffuse
<
	string UIName = "Diffuse Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

Texture2D gTextureDiffuse
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Texture";
>;

//SPECULAR
//********
float4 gColorSpecular
<
	string UIName = "Specular Color";
	string UIWidget = "Color";
> = float4(1,1,1,1);

Texture2D gTextureSpecularLevel
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Texture";
>;

bool gUseSpecularLevelTexture
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Bool";
> = false;

int gShininess<
	string UIName = "Shininess";
	string UIWidget = "Slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.1f;
> = 15;

//AMBIENT
//*******
float4 gColorAmbient
<
	string UIName = "Ambient Color";
	string UIWidget = "Color";
> = float4(0,0,0,1);

float gAmbientIntensity
<
	string UIName = "Ambient Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
>  = 0.0f;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel
<
	string UIName = "Flip Green Channel";
	string UIWidget = "Bool";
> = false;

bool gUseNormalMapping
<
	string UIName = "Normal Mapping";
	string UIWidget = "Bool";
> = false;

Texture2D gTextureNormal
<
	string UIName = "Normal Texture";
	string UIWidget = "Texture";
>;

//OPACITY
//***************
float gOpacityLevel<
	string UIName = "Opacity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
>  = 1.0f;

bool gUseOpacityTexture
<
	string UIName = "Opacity Map";
	string UIWidget = "Bool";
> = false;

Texture2D gTextureOpacity
<
	string UIName = "Opacity Map";
	string UIWidget = "Texture";
>;

//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: COLOR0;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

float3 CalculateSpecular(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 hn = normalize(viewDirection + gLightDirection);
	float specularStrength = dot(-normal, hn);
	specularStrength = saturate(specularStrength);
	specularStrength = pow(specularStrength,gShininess);
	float3 specColor =  gColorSpecular * specularStrength;
	
	//Use a Texture to control the specular level?
	if(gUseSpecularLevelTexture)
		specColor*= gTextureSpecularLevel.Sample(gTextureSampler, texCoord).r;
		
	return specColor;
}

float3 CalculateNormal(float3 tangent, float3 normal, float2 texCoord)
{
	float3 newNormal = normal;
	
	if(gUseNormalMapping)
	{
		float3 binormal = cross(tangent, normal);
		binormal = normalize(binormal);
		if (gFlipGreenChannel)
		{
			binormal = -binormal;
		}
		float3x3 localAxis = float3x3(tangent, binormal, normal);
	
		float3 sampledNormal = 2.0f * gTextureNormal.Sample(gTextureSampler,texCoord)-1.0f;
		newNormal =  mul(sampledNormal, localAxis);
	}
	
	return newNormal;
}

float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
	float3 diffColor = gColorDiffuse;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(-normal, gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);

	diffColor *= diffuseStrength;
	
	//Use a texture to define the diffuse color?
	if(gUseDiffuseTexture)
		diffColor *= gTextureDiffuse.Sample(gTextureSampler, texCoord);
		
	return diffColor;
}

float CalculateOpacity(float2 texCoord)
{
	float opacity = gOpacityLevel;
	if (gUseOpacityTexture)
		opacity *= gTextureOpacity.Sample(gTextureSampler, texCoord).r;
	
	return opacity;
}

// The main vertex shader
VS_Output MainVS(VS_Input input) {
	
	VS_Output output = (VS_Output)0;
	
	output.Position = mul(float4(input.Position, 1.0), gMatrixWorldViewProj);
	output.WorldPosition = mul(float4(input.Position,1.0), gMatrixWorld);
	output.Normal = normalize(mul(input.Normal, (float3x3)gMatrixWorld));
	output.Tangent = normalize(mul(input.Tangent, (float3x3)gMatrixWorld));
	output.TexCoord = input.TexCoord;
	
	return output;
}

// The main pixel shader
float4 MainPS(VS_Output input) : SV_TARGET {
	// NORMALIZE
	input.Normal = normalize(input.Normal);
	input.Tangent = normalize(input.Tangent);
	
	float3 viewDirection = normalize(input.WorldPosition.xyz - gMatrixViewInv[3].xyz);
	
	//NORMAL
	float3 newNormal = CalculateNormal(input.Tangent, input.Normal, input.TexCoord);
		
	//SPECULAR
	float3 specColor = CalculateSpecular(viewDirection, newNormal, input.TexCoord);
		
	//DIFFUSE
	float3 diffColor = CalculateDiffuse(newNormal, input.TexCoord);
		
	//AMBIENT
	float3 ambientColor = gColorAmbient * gAmbientIntensity;
		
	//FINAL COLOR CALCULATION
	float3 finalColor = diffColor + specColor + ambientColor;
	
	//OPACITY
	float opacity = CalculateOpacity(input.TexCoord);
	
	return float4(finalColor,opacity);
}

// Default Technique
technique10 WithAlphaBlending {
	pass p0 {
		SetDepthStencilState(gDepthState, 0);
		SetRasterizerState(gRasterizerState);
		SetBlendState(gEnableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

// Default Technique
technique10 WithoutAlphaBlending {
	pass p0 {
		SetDepthStencilState(gDepthState, 0);
		SetRasterizerState(gRasterizerState);
		SetBlendState(gDisableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

