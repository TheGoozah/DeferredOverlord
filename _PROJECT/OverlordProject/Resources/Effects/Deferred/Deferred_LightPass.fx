#include "Deferred_Helpers.fx"

//VARIABLES
//*********
float4x4 gWorldViewProjection : WORLDVIEWPROJECTION;
float4x4 gMatrixViewProjInv;
float3 gEyePos = float3(0, 0, 0);
Light gCurrentLight;

//G-BUFFER DATA
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

//FIRST PASS STATES
//*****************

DepthStencilState gDepthStencilState_FIRST
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
	DepthFunc = GREATER; //If the source data is greater than the destination data, the comparison passes

	StencilEnable = TRUE;
	FrontFaceStencilFunc = ALWAYS; //Always pass the comparison
	FrontFaceStencilPass = DECR_SAT; //Decrement the stencil value by 1, and clamp the result.
};

RasterizerState gRasterizerState_FIRST
{
	CullMode = BACK;
	FillMode = SOLID;
};

BlendState gBlendState_FIRST
{
	BlendEnable[0] = false;
};


//SECOND PASS STATES
//******************
DepthStencilState gDepthStencilState_SECOND
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
	DepthFunc = GREATER_EQUAL;  //If the source data is greater than or equal to the destination data, the comparison passes.

	StencilEnable = TRUE;
	BackFaceStencilFunc = EQUAL; //If the source data is equal to the destination data, the comparison passes
	BackFaceStencilPass = KEEP; //Keep the existing stencil data
};

RasterizerState gRasterizerState_SECOND
{
	CullMode = FRONT;
	FillMode = SOLID;
	DepthClipEnable = FALSE;
};

BlendState gBlendState_SECOND
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
};

//VS-PS IO
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

//VERTEX SHADER
//*************
VS_OUTPUT VS(float3 position:POSITION, float2 texCoord:TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Position = mul(float4(position,1), gWorldViewProjection);
	output.TexCoord = texCoord;

	return output;
}

//PIXEL SHADER
//************
[earlydepthstencil] //Execute Depth&Stencil tests before PS execution (not during as usual)
float4 PS(VS_OUTPUT input):SV_TARGET
{
	int2 screenCoord = input.Position.xy;

	//World Position (From Depth)
	float depth = gTextureDepth.Load(int3(screenCoord, 0)).r;
	float4 P = DepthToWorldPosition(depth, screenCoord, float2(1280.0f, 720.0f), gMatrixViewProjInv);

	//View Direction
	float4 V = normalize(float4(gEyePos,0) - P);

	float4 diffuse = gTextureDiffuse.Load(int3(screenCoord, 0));
	float4 specularLevel = gTextureSpecular.Load(int3(screenCoord, 0));
	float shininess = exp2(specularLevel.a * 10.5f);
	float4 N = gTextureNormal.Load(int3(screenCoord, 0));

	//Material (Pixel Properties)
	Material mat = (Material)0;
	mat.DiffuseColor = diffuse;
	mat.SpecularLevel = specularLevel;
	mat.Shininess = shininess;

	//Do Lighting (Point=0, Spot=1)
	LightingResult result = (LightingResult)0;
	if (gCurrentLight.Type == 0)
	{
		result = DoPointLighting(gCurrentLight, mat, V, N, P);
	}
	else
	{
		result = DoSpotLighting(gCurrentLight, mat, V, N, P);
	}

	return (mat.DiffuseColor * result.Diffuse) + (mat.SpecularLevel * result.Specular);
}

technique11 Default
{
	//PASS ONE
	pass one
	{
		SetRasterizerState(gRasterizerState_FIRST);
		SetDepthStencilState(gDepthStencilState_FIRST, 1);
		SetBlendState(gBlendState_FIRST, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader ( vs_4_0, VS() ));
		SetGeometryShader( NULL );
		SetPixelShader( NULL );
	}

	//PASS TWO
	pass two
	{
		SetRasterizerState(gRasterizerState_SECOND);
		SetDepthStencilState(gDepthStencilState_SECOND, 1);
		SetBlendState(gBlendState_SECOND, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader (vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader (ps_4_0, PS()));
	}
}

