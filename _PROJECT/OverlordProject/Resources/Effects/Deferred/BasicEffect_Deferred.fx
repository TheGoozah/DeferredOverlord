//***************
//GLOBAL MATRICES
//***************
// The World View Projection Matrix
float4x4 gMatrixWorldViewProj : WORLDVIEWPROJECTION;
// The ViewInverse Matrix
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
 	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

//LIGHT
//*****
float3 gLightDirection:DIRECTION = float3(0.577f, 0.577f, 0.577f);

//DIFFUSE
//*******
bool gUseDiffuseTexture = false;
float4 gColorDiffuse = float4(1,1,1,1);
Texture2D gTextureDiffuse;

//SPECULAR
//********
float4 gColorSpecular = float4(1,1,1,1);
Texture2D gTextureSpecularLevel;
bool gUseSpecularLevelTexture = false;
int gShininess = 15;

//AMBIENT
//*******
float4 gColorAmbient = float4(0,0,0,1);
float gAmbientIntensity = 0.0f;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel = false;
bool gUseNormalMapping = false;
Texture2D gTextureNormal;

//OPACITY
//***************
float gOpacityLevel = 1.0f;
bool gUseOpacityTexture = false;
Texture2D gTextureOpacity;

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
	float3 Binormal : BINORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct PS_Output
{
	float4 LightAccumulation : SV_TARGET0;
	float4 Diffuse : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Normal : SV_TARGET3;
};

// The main vertex shader
VS_Output MainVS(VS_Input input) 
{
	
	VS_Output output = (VS_Output)0;
	
	output.Position = mul(float4(input.Position, 1.0), gMatrixWorldViewProj);
	output.WorldPosition = mul(float4(input.Position,1.0), gMatrixWorld);
	output.Normal = normalize(mul(input.Normal, (float3x3)gMatrixWorld));
	output.Tangent = normalize(mul(input.Tangent, (float3x3)gMatrixWorld));

	//Calculate Binormal
	float3 binormal = cross(input.Tangent, input.Normal);
	binormal = normalize(binormal);
	if (gFlipGreenChannel)
	{
		binormal = -binormal;
	}

	output.Binormal = binormal;
	output.TexCoord = input.TexCoord;
	
	return output;
}

// The main pixel shader
PS_Output MainPS(VS_Output input) 
{
	
	PS_Output output = (PS_Output)0;

	//DIFFUSE
	//*******
	float4 diffuse = gColorDiffuse;
	if (gUseDiffuseTexture)
	{
		diffuse *= gTextureDiffuse.Sample(gTextureSampler, input.TexCoord);
	}

	//SET DIFFUSE
	output.Diffuse = diffuse;

	//ALPHA
	//*****
	float alpha = diffuse.a;
	if (gUseOpacityTexture)
	{
		alpha = gTextureOpacity.Sample(gTextureSampler, input.TexCoord).r;
	}

	clip(alpha - 0.1f); //ALPHA < 0.1 => CLIPPED

	//AMBIENT
	//*******
	float4 ambient = gColorAmbient;
	//CHEAP AMBIENT
	ambient *= diffuse;
	ambient *= gAmbientIntensity;

	//SET LIGHT_ACCU
	output.LightAccumulation = ambient;

	//NORMAL
	//******
	float3 normal;
	if (gUseNormalMapping)
	{
		float3x3 TBN = float3x3(normalize(input.Tangent),
								normalize(input.Binormal),
								normalize(input.Normal));

		normal = gTextureNormal.Sample(gTextureSampler, input.TexCoord).xyz;
		normal = normal * 2.0f - 1.0f;
		normal = mul(normal, TBN);
	}
	else
	{
		normal = input.Normal;
	}

	//SET NORMAL
	output.Normal = float4(normalize(normal), 0); //SET TO RT3

	//SPECULAR
	//********
	float3 specularLevel = gColorSpecular.xyz;
	if (gUseSpecularLevelTexture)
	{
		specularLevel = gTextureSpecularLevel.Sample(gTextureSampler, input.TexCoord);
	}

	float shininess = log2(gShininess) / 10.5f;

	//SET SPECULAR
	output.Specular = float4(specularLevel, shininess); //SET TO RT2

	return output;
}

// Default Technique
technique10 WithoutAlphaBlending 
{
	pass p0 {
		SetDepthStencilState(gDepthState, 0);
		SetRasterizerState(gRasterizerState);
		SetBlendState(gDisableBlending,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

