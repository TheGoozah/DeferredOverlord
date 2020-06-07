#pragma once
#include "Material.h"

class ColorMaterial final: public Material
{
public:
	ColorMaterial(bool enableTransparency = false);
	~ColorMaterial() = default;

	ColorMaterial(const ColorMaterial& other) = delete;
	ColorMaterial(ColorMaterial&& other) noexcept = delete;
	ColorMaterial& operator=(const ColorMaterial& other) = delete;
	ColorMaterial& operator=(ColorMaterial&& other) noexcept = delete;

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);
};

