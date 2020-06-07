#include "stdafx.h"
#include "QuadMaterial.h"

QuadMaterial::QuadMaterial() :
	Material(L"Resources/Effects/QuadRenderer.fx", L"", false)
{}

void QuadMaterial::SetTexture(ID3D11ShaderResourceView* pSRV)
{
	GetEffect()->GetVariableByName("gTexture")->AsShaderResource()->SetResource(pSRV);
}

void QuadMaterial::LoadEffectVariables()
{
	//Nothing to do...
	//Using Registers instead
}

void QuadMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	//Nothing to do...
	//EffectVariable (gTexture, register 0) updated in QuadMaterial::SetTexture
}
