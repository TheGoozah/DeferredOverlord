#include "stdafx.h"
#include "DeferredMaterial.h"

#define CHECK_AND_BIND(shaderVAR, parseAS, varNAME)\
	if (!shaderVAR)\
	{\
		shaderVAR = GetEffect()->GetVariableByName(#varNAME)->parseAS();\
		if (!shaderVAR->IsValid())\
		{\
			Logger::LogError(L"DeferredMaterial::LoadEffectVariables() > \'"#varNAME"\' variable not found!");\
			shaderVAR = nullptr;\
		}\
	}

#define CHECK_AND_BIND_RAW(shaderVAR, varName)\
if (!shaderVAR)\
{\
	shaderVAR = GetEffect()->GetVariableByName(#varName);\
	if (!shaderVAR->IsValid())\
	{\
		Logger::LogError(L"DeferredMaterial::LoadEffectVariable() > Variable \'#varName\' not found!");\
		shaderVAR = nullptr;\
	}\
}

DeferredMaterial::DeferredMaterial() :
	Material(L"Resources/Effects/Deferred/Deferred_DirectionalLightPass.fx", L"", false)
{}

void DeferredMaterial::LoadEffectVariables()
{
	//CHECK_AND_BIND(m_pLightAccVAR, AsShaderResource, gTextureLightAccumulation);
	CHECK_AND_BIND(m_pDiffuseVAR, AsShaderResource, gTextureDiffuse);
	CHECK_AND_BIND(m_pSpecularVAR, AsShaderResource, gTextureSpecular);
	CHECK_AND_BIND(m_pNormalVAR, AsShaderResource, gTextureNormal);
	CHECK_AND_BIND(m_pDepthVAR, AsShaderResource, gTextureDepth);

	//CHECK_AND_BIND(m_pLightDirVAR, AsVector, gLightDirection);
	CHECK_AND_BIND_RAW(m_pLightVAR, gDirectionalLight);
	CHECK_AND_BIND(m_pEyePosVAR, AsVector, gEyePos);
	CHECK_AND_BIND(m_pViewProjInvVAR, AsMatrix, gMatrixVPinv);
}

void DeferredMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
	//Nothing to do
}
