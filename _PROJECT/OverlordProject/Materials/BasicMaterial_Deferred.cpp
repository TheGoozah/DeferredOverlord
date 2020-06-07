//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "BasicMaterial_Deferred.h"
#include "GeneralStructs.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "Components.h"

ID3DX11EffectScalarVariable* BasicMaterial_Deferred::m_pUseDiffuseTextureVAR = nullptr;
ID3DX11EffectScalarVariable* BasicMaterial_Deferred::m_pUseSpecularLevelTextureVAR = nullptr;
ID3DX11EffectScalarVariable* BasicMaterial_Deferred::m_pUseNormalTextureVAR = nullptr;
ID3DX11EffectScalarVariable* BasicMaterial_Deferred::m_pUseOpacityTextureVAR = nullptr;
ID3DX11EffectShaderResourceVariable* BasicMaterial_Deferred::m_pDiffuseTextureVAR = nullptr;
ID3DX11EffectShaderResourceVariable* BasicMaterial_Deferred::m_pSpecularLevelTextureVAR = nullptr;
ID3DX11EffectShaderResourceVariable* BasicMaterial_Deferred::m_pNormalTextureVAR = nullptr;
ID3DX11EffectShaderResourceVariable* BasicMaterial_Deferred::m_pOpacityTextureVAR = nullptr;
ID3DX11EffectVectorVariable* BasicMaterial_Deferred::m_pDiffuseColorVAR = nullptr;
ID3DX11EffectVectorVariable* BasicMaterial_Deferred::m_pSpecularColorVAR = nullptr;
ID3DX11EffectVectorVariable* BasicMaterial_Deferred::m_pAmbientColorVAR = nullptr;
ID3DX11EffectScalarVariable* BasicMaterial_Deferred::m_pShininessVAR = nullptr;
ID3DX11EffectScalarVariable* BasicMaterial_Deferred::m_pAmbientIntensityVAR = nullptr;
ID3DX11EffectScalarVariable* BasicMaterial_Deferred::m_pOpacityLevelVAR = nullptr;
ID3DX11EffectVectorVariable* BasicMaterial_Deferred::m_pLightDirVAR = nullptr;
ID3DX11EffectMatrixVariable* BasicMaterial_Deferred::m_pMatrixWvpVAR = nullptr;
ID3DX11EffectMatrixVariable* BasicMaterial_Deferred::m_pMatrixViewInvVAR = nullptr;
ID3DX11EffectMatrixVariable* BasicMaterial_Deferred::m_pMatrixWorldVAR = nullptr;

#define CHECK_AND_BIND(shaderVAR, parseAS, varNAME)\
	if (!shaderVAR)\
	{\
		shaderVAR = GetEffect()->GetVariableByName(#varNAME)->parseAS();\
		if (!shaderVAR->IsValid())\
		{\
			Logger::LogError(L"BasicMaterial_Deferred::LoadEffectVariables() > \'"#varNAME"\' variable not found!");\
			shaderVAR = nullptr;\
		}\
	}

#define SET_TEXTURE(srv, useSrv, asset, origin)\
if (asset.empty()) {\
	srv = nullptr;\
	useSrv = false;\
	return;\
}\
\
srv = ContentManager::Load<TextureData>(asset)->GetShaderResourceView();\
useSrv = true;

BasicMaterial_Deferred::BasicMaterial_Deferred(bool enableTransparency):
	Material(L"Resources/Effects/Deferred/BasicEffect_Deferred.fx",L"", false)
{
	SetTransparency(enableTransparency);
}

void BasicMaterial_Deferred::LoadEffectVariables()
{
	CHECK_AND_BIND(m_pUseDiffuseTextureVAR, AsScalar, gUseDiffuseTexture)
	CHECK_AND_BIND(m_pUseSpecularLevelTextureVAR, AsScalar, gUseSpecularLevelTexture)
	CHECK_AND_BIND(m_pUseNormalTextureVAR, AsScalar, gUseNormalMapping)
	CHECK_AND_BIND(m_pUseOpacityTextureVAR, AsScalar, gUseOpacityTexture)

	CHECK_AND_BIND(m_pDiffuseTextureVAR, AsShaderResource, gTextureDiffuse)
	CHECK_AND_BIND(m_pSpecularLevelTextureVAR, AsShaderResource, gTextureSpecularLevel)
	CHECK_AND_BIND(m_pNormalTextureVAR, AsShaderResource, gTextureNormal)
	CHECK_AND_BIND(m_pOpacityTextureVAR, AsShaderResource, gTextureOpacity)

	CHECK_AND_BIND(m_pDiffuseColorVAR, AsVector, gColorDiffuse)
	CHECK_AND_BIND(m_pSpecularColorVAR, AsVector, gColorSpecular)
	CHECK_AND_BIND(m_pAmbientColorVAR, AsVector, gColorAmbient)

	CHECK_AND_BIND(m_pShininessVAR, AsScalar, gShininess)
	CHECK_AND_BIND(m_pAmbientIntensityVAR, AsScalar, gAmbientIntensity)
	CHECK_AND_BIND(m_pLightDirVAR, AsVector, gLightDirection)
	CHECK_AND_BIND(m_pOpacityLevelVAR, AsScalar, gOpacityLevel)

	CHECK_AND_BIND(m_pMatrixWvpVAR, AsMatrix, gMatrixWorldViewProj)
	CHECK_AND_BIND(m_pMatrixViewInvVAR, AsMatrix, gMatrixViewInv)
	CHECK_AND_BIND(m_pMatrixWorldVAR, AsMatrix, gMatrixWorld)
}

#pragma region
void BasicMaterial_Deferred::SetTextureDiffuse(const std::wstring& asset)
{
	SET_TEXTURE(m_pDiffuseTextureSRV, m_UseDiffuseTexture, asset, __func__)
}

void BasicMaterial_Deferred::SetTextureSpecularLevel(const std::wstring& asset)
{
	SET_TEXTURE(m_pSpecularLevelSRV, m_UseSpecularLevelTexture, asset, __func__)
}

void BasicMaterial_Deferred::SetTextureNormal(const std::wstring& asset)
{
	SET_TEXTURE(m_pNormalTextureSRV, m_UseNormalTexture, asset, __func__)
}

void BasicMaterial_Deferred::SetTextureOpacity(const std::wstring& asset)
{
	SET_TEXTURE(m_pOpacityTextureSRV, m_UseOpacityTexture, asset, __func__)
}

void BasicMaterial_Deferred::SetColorDiffuse(DirectX::XMFLOAT4 col)
{
	m_ColorDiffuse = col;
}

void BasicMaterial_Deferred::SetColorSpecular(DirectX::XMFLOAT4 col)
{
	m_ColorSpecular = col;
}

void BasicMaterial_Deferred::SetColorAmbient(DirectX::XMFLOAT4 col)
{
	m_ColorAmbient = col;
}

void BasicMaterial_Deferred::SetShininess(float shininess)
{
	m_Shininess = shininess;
}

void BasicMaterial_Deferred::SetAmbientIntensity(float intensity)
{
	m_AmbientIntensity = intensity;
}

void BasicMaterial_Deferred::SetOpacityLevel(float level)
{
	m_OpacityLevel = level;
}

void BasicMaterial_Deferred::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	m_LightDirection = dir;
}
#pragma endregion SETTERS

void BasicMaterial_Deferred::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
	auto view = XMLoadFloat4x4(&gameContext.pCamera->GetView());
	auto projection = XMLoadFloat4x4(&gameContext.pCamera->GetProjection());

	m_pMatrixWorldVAR->SetMatrix(reinterpret_cast<float*>(&world));
	DirectX::XMMATRIX wvp = world*view*projection;
	m_pMatrixWvpVAR->SetMatrix(reinterpret_cast<const float*>(&(wvp)));
	auto viewInv = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
	m_pMatrixViewInvVAR->SetMatrix(reinterpret_cast<float*>(&viewInv));

	m_pUseDiffuseTextureVAR->SetBool(m_UseDiffuseTexture);
	m_pUseSpecularLevelTextureVAR->SetBool(m_UseSpecularLevelTexture);
	m_pUseNormalTextureVAR->SetBool(m_UseNormalTexture);
	m_pUseOpacityTextureVAR->SetBool(m_UseOpacityTexture);

	if (m_UseDiffuseTexture)
		m_pDiffuseTextureVAR->SetResource(m_pDiffuseTextureSRV);

	if (m_UseSpecularLevelTexture)
		m_pSpecularLevelTextureVAR->SetResource(m_pSpecularLevelSRV);

	if (m_UseNormalTexture)
		m_pNormalTextureVAR->SetResource(m_pNormalTextureSRV);

	if (m_UseOpacityTexture)
		m_pOpacityTextureVAR->SetResource(m_pOpacityTextureSRV);

	m_pDiffuseColorVAR->SetFloatVector(reinterpret_cast<float*>(&m_ColorDiffuse));
	m_pAmbientColorVAR->SetFloatVector(reinterpret_cast<float*>(&m_ColorAmbient));
	m_pSpecularColorVAR->SetFloatVector(reinterpret_cast<float*>(&m_ColorSpecular));
	m_pLightDirVAR->SetFloatVector(reinterpret_cast<float*>(&m_LightDirection));

	m_pShininessVAR->SetFloat(m_Shininess);
	m_pAmbientIntensityVAR->SetFloat(m_AmbientIntensity);
	m_pOpacityLevelVAR->SetFloat(m_OpacityLevel);
}
