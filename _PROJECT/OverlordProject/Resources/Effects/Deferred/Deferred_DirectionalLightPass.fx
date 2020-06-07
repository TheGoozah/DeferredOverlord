//Deferred_DirectionalLightPas > Fullscreen Quad Render
#include "Deferred_Helpers.fx"

//VARIABLES
//*********
float4x4 gMatrixVPinv;
float3 gEyePos = float3(0, 0, 0);
Light gDirectionalLight;

//G-BUFFER DATA
//Texture2D gTextureLightAccumulation;
Texture2D gTextureDiffuse;
Texture2D gTextureSpecular;
Texture2D gTextureNormal;
Texture2D gTextureDepth;

SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

//VS & PS IO
//**********
struct VS_INPUT
{
	float3 Position: POSITION;
	float2 TexCoord: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

//STATES
//******
RasterizerState gRasterizerState
{
	FillMode = SOLID;
	CullMode = BACK;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

BlendState gBlendState //Additive Blending (LIGHT-ACCUMULATION + LIGHTING-RESULTS)
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
};

//VERTEX SHADER
//*************
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;

	return output;
}

//PIXEL SHADER
//************
float4 PS(VS_OUTPUT input):SV_TARGET
{
	int2 screenCoord = input.Position.xy;
	int3 loadCoord = int3(screenCoord, 0);

	//Calculate Pixel WorldPosition from Depth value (DepthBuffer)
	float depth = gTextureDepth.Load(loadCoord).r;
	
	//Non-Linear Depth To WorldPosition
	float2 ndcXY = float2(input.TexCoord.x, 1.0f - input.TexCoord.y) * 2.0f - 1.0f;
	float3 ndc = float3(ndcXY, depth);
	float4 P = mul(float4(ndc, 1.0f), gMatrixVPinv);
	P.xyz /= P.w;

	float4 V = normalize(float4(gEyePos, 0.f) - P);
	
	float4 diffuse = gTextureDiffuse.Load(loadCoord);
	float4 specularLevel = gTextureSpecular.Load(loadCoord);
	float shininess = exp2(specularLevel.a * 10.5f);
	float4 N = gTextureNormal.Load(loadCoord);
	float4 L = normalize(-gDirectionalLight.Direction);

	//Material
	Material mat = (Material)0;
	mat.DiffuseColor = diffuse;
	mat.SpecularLevel = specularLevel;
	mat.Shininess = shininess;

	//Do Lighting
	LightingResult result = DoDirectionalLighting(gDirectionalLight, mat, L, V, N);

	//FINAL COLOR
	return (mat.DiffuseColor * result.Diffuse) + (mat.SpecularLevel * result.Specular);
}

//TECHNIQUE
//*********
technique11 Default
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, VS() ));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
};
