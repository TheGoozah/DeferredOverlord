//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SponzaScene.h"
#include "Components.h"
#include "FreeCamera.h"
#include "DebugRenderer.h"
#include "../Materials/BasicMaterial.h"
#include "../Materials/BasicMaterial_Deferred.h"

#define SPONZA_FULL

#pragma region
inline std::wstring Tex(const std::wstring& str)
{
	if (str.empty())
		return str;

	auto res = L"Resources/Textures/Sponza/" + str;
	return res;
}

#define ADD_MODEL(id, modelName, diffuseTex, specularLevelTex, normalTex)\
matDef = new BasicMaterial_Deferred();\
matDef->SetTextureDiffuse(diffuseTex);\
matDef->SetTextureSpecularLevel(specularLevelTex);\
matDef->SetTextureNormal(normalTex);\
matMan->AddMaterial(matDef, ++id);\
modelCmp = new ModelComponent(L"Resources/Meshes/Sponza/"#modelName".ovm");\
modelCmp->SetMaterial(id);\
m_pSponzaRoot->AddComponent(modelCmp);

#define ADD_MODEL_T(id, modelName, diffuseTex, specularLevelTex, normalTex, opacityTex)\
mat = new BasicMaterial(true);\
mat->SetTextureDiffuse(diffuseTex);\
mat->SetTextureSpecularLevel(specularLevelTex);\
mat->SetTextureNormal(normalTex);\
mat->SetTextureOpacity(opacityTex);\
matMan->AddMaterial(mat, ++id);\
modelCmp = new ModelComponent(L"Resources/Meshes/Sponza/"#modelName".ovm");\
modelCmp->SetMaterial(id);\
m_pSponzaRoot->AddComponent(modelCmp);

#pragma endregion HELPER Macros

SponzaScene::SponzaScene():
	GameScene(L"SponzaScene")
{}

void SponzaScene::Initialize()
{
	const auto gameContext = GetGameContext();

	auto matMan = gameContext.pMaterialManager;
	auto matId = -1;

	m_pSponzaRoot = new GameObject();

	BasicMaterial* mat;
	BasicMaterial_Deferred* matDef;
	ModelComponent* modelCmp;

	ADD_MODEL(matId, Sponza_Bricks_a, Tex(L"spnza_bricks_a_diff.tga"), Tex(L"spnza_bricks_a_spec.tga"), Tex(L"spnza_bricks_a_ddn.tga"));
	ADD_MODEL(matId, Sponza_Floor, Tex(L"sponza_floor_a_diff.tga"), Tex(L"sponza_floor_a_spec.tga"), L"");
	ADD_MODEL(matId, Sponza_Arches, Tex(L"sponza_arch_diff.tga"), Tex(L"sponza_arch_spec.tga"), Tex(L"sponza_arch_ddn.tga"));
	ADD_MODEL(matId, Sponza_Ceiling_a, Tex(L"sponza_ceiling_a_diff.tga"), Tex(L"sponza_ceiling_a_spec.tga"), L"");
	ADD_MODEL(matId, Sponza_Column_a, Tex(L"sponza_column_a_diff.tga"), Tex(L"sponza_column_a_spec.tga"), Tex(L"sponza_column_a_ddn.tga"));
	ADD_MODEL(matId, Sponza_Column_b, Tex(L"sponza_column_b_diff.tga"), Tex(L"sponza_column_b_spec.tga"), Tex(L"sponza_column_b_ddn.tga"));
	ADD_MODEL(matId, Sponza_Column_c, Tex(L"sponza_column_c_diff.tga"), Tex(L"sponza_column_c_spec.tga"), Tex(L"sponza_column_c_ddn.tga"));
	ADD_MODEL(matId, Sponza_Curtain_Blue, Tex(L"sponza_curtain_blue_diff.tga"), L"", L"");
	ADD_MODEL(matId, Sponza_Curtain_Red, Tex(L"sponza_curtain_diff.tga"), L"", L"");
	ADD_MODEL(matId, Sponza_Curtain_Green, Tex(L"sponza_curtain_green_diff.tga"), L"", L"");
	
#ifdef SPONZA_FULL
	ADD_MODEL(matId, Sponza_Details, Tex(L"sponza_details_diff.tga"), Tex(L"sponza_details_spec.tga"), L"");
	ADD_MODEL(matId, Sponza_Fabric_Blue, Tex(L"sponza_fabric_blue_diff.tga"), Tex(L"sponza_fabric_spec.tga"), L"");
	ADD_MODEL(matId, Sponza_Fabric_Green, Tex(L"sponza_fabric_green_diff.tga"), Tex(L"sponza_fabric_spec.tga"), L"");
	ADD_MODEL(matId, Sponza_Fabric_Red, Tex(L"sponza_fabric_diff.tga"), Tex(L"sponza_fabric_spec.tga"), L"");
	ADD_MODEL(matId, Sponza_FlagPoles, Tex(L"sponza_flagpole_diff.tga"), Tex(L"sponza_flagpole_spec.tga"), L"");
	ADD_MODEL_T(matId, Sponza_Thorns, Tex(L"sponza_thorn_diff.tga"), Tex(L"sponza_thorn_spec.tga"), Tex(L"sponza_thorn_ddn.tga"), Tex(L"sponza_thorn_mask.tga"));
	ADD_MODEL(matId, Sponza_Lion_Head, Tex(L"lion.tga"), L"", Tex(L"lion_ddn.tga"));
	ADD_MODEL(matId, Sponza_Lion_Plate, Tex(L"background.tga"), L"", Tex(L"background_ddn.tga"));
	ADD_MODEL(matId, Sponza_Roof, Tex(L"sponza_roof_diff.tga"), L"", L"");
	ADD_MODEL(matId, Sponza_RoundVase, Tex(L"vase_round.tga"), Tex(L"vase_round_spec.tga"), Tex(L"vase_round_ddn.tga"));
	ADD_MODEL_T(matId, Sponza_RoundVase_Plant, Tex(L"vase_plant.tga"), Tex(L"vase_plant_spec.tga"),L"", Tex(L"vase_plant_mask.tga"));
	ADD_MODEL(matId, Sponza_Vase, Tex(L"vase_dif.tga"), L"", Tex(L"vase_ddn.tga"));
	ADD_MODEL(matId, Sponza_HangingVase, Tex(L"vase_hanging.tga"), L"",L"");
	ADD_MODEL_T(matId, Sponza_HangingVase_Chains, Tex(L"chain_texture.tga"), L"", Tex(L"chain_texture_ddn.tga"), Tex(L"chain_texture_mask.tga"));
#endif

	m_pSponzaRoot->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	m_pSponzaRoot->GetTransform()->Rotate(0, 90, 0);

	AddChild(m_pSponzaRoot);

	gameContext.pCamera->GetTransform()->Translate(7.18050051f, 18.6554089f, -114.360588f);
	static_cast<FreeCamera*>(gameContext.pCamera->GetGameObject())->SetRotation(-14.0423603f, -2.72134018f);

	gameContext.pLightManager->GetDirectionalLight().intensity = 0.5f;
	gameContext.pLightManager->GetDirectionalLight().isEnabled = true;

	Light light = {};
	light.position = { 0.f,5.f,0.f,1.0f };
	light.direction = { 0.f,0.f,1.f,0.f };
	light.color = { 0.7f,0.f,0.f,1.f };
	light.intensity = 1.0f;
	light.spotLightAngle = 35.f;
	light.range = 150.0f;
	light.type = LightType::Spot;
	gameContext.pLightManager->AddLight(light);

	light = {};
	light.position = { 0.f,20.f,0.f,1.0f };
	light.color = { 0.f,1.f,0.f,1.f };
	light.intensity = 1.f;
	light.range = 30.0f;
	light.type = LightType::Point;
	gameContext.pLightManager->AddLight(light);

	DebugRenderer::ToggleDebugRenderer();
}

void SponzaScene::Update()
{}

void SponzaScene::Draw()
{
	const auto gameContext = GetGameContext();

	DirectX::XMFLOAT4 pos;
	pos.x = sin(gameContext.pGameTime->GetTotal()) * 30.0f;
	pos.y = 5.0f;
	pos.z = cos(gameContext.pGameTime->GetTotal()) * 30.0f;
	pos.w = 1.0f;

	gameContext.pLightManager->GetLight(1).position = pos;
	gameContext.pLightManager->GetLight(1).intensity = randF(0.2f, 1.2f);

	auto camPos = gameContext.pCamera->GetTransform()->GetWorldPosition();
	auto camForward = gameContext.pCamera->GetTransform()->GetForward();
}

void SponzaScene::SceneActivated()
{}

void SponzaScene::SceneDeactivated()
{}
