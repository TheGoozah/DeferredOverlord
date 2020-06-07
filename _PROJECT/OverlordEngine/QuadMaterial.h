#pragma once
#include "Material.h"

class QuadMaterial: public Material
{
public:
	QuadMaterial();
	~QuadMaterial() = default;

	QuadMaterial(const QuadMaterial& other) = delete;
	QuadMaterial(QuadMaterial&& other) noexcept = delete;
	QuadMaterial& operator=(const QuadMaterial& other) = delete;
	QuadMaterial& operator=(QuadMaterial&& other) noexcept = delete;

	void SetTexture(ID3D11ShaderResourceView* pSRV);

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;
};

