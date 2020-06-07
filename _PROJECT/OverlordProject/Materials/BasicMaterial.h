#pragma once
#include "Material.h"

class BasicMaterial final : public Material
{
public:
	BasicMaterial(bool enableTransparency = false);
	~BasicMaterial() = default;

	BasicMaterial(const BasicMaterial& other) = delete;
	BasicMaterial(BasicMaterial&& other) noexcept = delete;
	BasicMaterial& operator=(const BasicMaterial& other) = delete;
	BasicMaterial& operator=(BasicMaterial&& other) noexcept = delete;

	void SetTextureDiffuse(const std::wstring& asset);
	void SetTextureSpecularLevel(const std::wstring& asset);
	void SetTextureNormal(const std::wstring& asset);
	void SetTextureOpacity(const std::wstring& asset);

	void SetColorDiffuse(DirectX::XMFLOAT4 col);
	void SetColorSpecular(DirectX::XMFLOAT4 col);
	void SetColorAmbient(DirectX::XMFLOAT4 col);
	void SetLightDirection(DirectX::XMFLOAT3 dir);

	void SetShininess(float shininess);
	void SetAmbientIntensity(float intensity);
	void SetOpacityLevel(float level);

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

	//Bools
	static ID3DX11EffectScalarVariable* m_pUseDiffuseTextureVAR;
	bool m_UseDiffuseTexture = false;

	static ID3DX11EffectScalarVariable* m_pUseSpecularLevelTextureVAR;
	bool m_UseSpecularLevelTexture = false;

	static ID3DX11EffectScalarVariable* m_pUseNormalTextureVAR;
	bool m_UseNormalTexture = false;

	static ID3DX11EffectScalarVariable* m_pUseOpacityTextureVAR;
	bool m_UseOpacityTexture = false;

	//Textures
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseTextureVAR;
	ID3D11ShaderResourceView* m_pDiffuseTextureSRV = nullptr;

	static ID3DX11EffectShaderResourceVariable* m_pSpecularLevelTextureVAR;
	ID3D11ShaderResourceView* m_pSpecularLevelSRV = nullptr;

	static ID3DX11EffectShaderResourceVariable* m_pNormalTextureVAR;
	ID3D11ShaderResourceView* m_pNormalTextureSRV = nullptr;

	static ID3DX11EffectShaderResourceVariable* m_pOpacityTextureVAR;
	ID3D11ShaderResourceView* m_pOpacityTextureSRV = nullptr;

	//Colors
	static ID3DX11EffectVectorVariable* m_pDiffuseColorVAR;
	DirectX::XMFLOAT4 m_ColorDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	static ID3DX11EffectVectorVariable* m_pSpecularColorVAR; //level
	DirectX::XMFLOAT4 m_ColorSpecular = { .0f, .0f, .0f, 1.0f };

	static ID3DX11EffectVectorVariable* m_pAmbientColorVAR;
	DirectX::XMFLOAT4 m_ColorAmbient = { 0.0f, 0.0f, 0.0f, 1.0f };

	//Misc
	static ID3DX11EffectScalarVariable* m_pShininessVAR;
	float m_Shininess = 15.0f;

	static ID3DX11EffectScalarVariable* m_pOpacityLevelVAR;
	float m_OpacityLevel = 1.0f;

	static ID3DX11EffectScalarVariable* m_pAmbientIntensityVAR;
	float m_AmbientIntensity = 0.3f;

	static ID3DX11EffectVectorVariable* m_pLightDirVAR;
	DirectX::XMFLOAT3 m_LightDirection = { 0.577f, 0.577f, 0.577f };

	static ID3DX11EffectMatrixVariable* m_pMatrixWvpVAR, *m_pMatrixViewInvVAR, *m_pMatrixWorldVAR;
};

