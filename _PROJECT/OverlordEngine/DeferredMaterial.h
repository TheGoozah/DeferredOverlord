#pragma once
#include "Material.h"

class DeferredMaterial final : public Material
{
public:
	DeferredMaterial();
	~DeferredMaterial() = default;

	DeferredMaterial(const DeferredMaterial& other) = delete;
	DeferredMaterial(DeferredMaterial&& other) noexcept = delete;
	DeferredMaterial& operator=(const DeferredMaterial& other) = delete;
	DeferredMaterial& operator=(DeferredMaterial&& other) noexcept = delete;

	//void SetLightAccumulationSRV(ID3D11ShaderResourceView* pSrv) const { m_pLightAccVAR->SetResource(pSrv); }
	void SetDiffuseSRV(ID3D11ShaderResourceView* pSrv) const { m_pDiffuseVAR->SetResource(pSrv); }
	void SetSpecularSRV(ID3D11ShaderResourceView* pSrv) const { m_pSpecularVAR->SetResource(pSrv); }
	void SetNormalSRV(ID3D11ShaderResourceView* pSrv) const { m_pNormalVAR->SetResource(pSrv); }
	void SetDepthSRV(ID3D11ShaderResourceView* pSrv) const { m_pDepthVAR->SetResource(pSrv); }
	
	//void SetLightDirection(DirectX::XMFLOAT3 lightDir) const { m_pLightDirVAR->SetFloatVector(reinterpret_cast<float*>(&lightDir)); }
	void SetDirectionalLight(const Light& light) const { m_pLightVAR->SetRawValue(&light, 0, sizeof(Light) - 4); }
	void SetViewProjectionInverse(DirectX::XMFLOAT4X4 mat) const { m_pViewProjInvVAR->SetMatrix(reinterpret_cast<float*>(&mat)); }
	void SetEyePosition(DirectX::XMFLOAT3 eyePos) const { m_pEyePosVAR->SetFloatVector(reinterpret_cast<float*>(&eyePos)); }

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	//ID3DX11EffectShaderResourceVariable* m_pLightAccVAR = nullptr;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseVAR = nullptr;
	ID3DX11EffectShaderResourceVariable* m_pSpecularVAR = nullptr;
	ID3DX11EffectShaderResourceVariable* m_pNormalVAR = nullptr;
	ID3DX11EffectShaderResourceVariable* m_pDepthVAR = nullptr;

	//ID3DX11EffectVectorVariable* m_pLightDirVAR = nullptr;
	ID3DX11EffectVariable* m_pLightVAR = nullptr;
	ID3DX11EffectVectorVariable* m_pEyePosVAR = nullptr;
	ID3DX11EffectMatrixVariable* m_pViewProjInvVAR = nullptr;
};

