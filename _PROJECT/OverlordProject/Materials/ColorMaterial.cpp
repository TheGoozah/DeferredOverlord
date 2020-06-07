//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ColorMaterial.h"
#include "GeneralStructs.h"
#include "ModelComponent.h"

ColorMaterial::ColorMaterial(bool enableTransparency) : 
	Material(L"./Resources/Effects/PosNormCol3D.fx", enableTransparency?L"TransparencyTech":L"")
{}

void ColorMaterial::LoadEffectVariables()
{}

void ColorMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
}
