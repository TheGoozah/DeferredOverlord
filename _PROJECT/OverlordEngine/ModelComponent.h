#pragma once
#include "BaseComponent.h"

class MeshFilter;
class Material;
class ModelAnimator;

class ModelComponent : public BaseComponent
{
	std::wstring m_AssetFile;
	MeshFilter* m_pMeshFilter = nullptr;
	Material* m_pMaterial = nullptr;
	unsigned int m_MaterialId = 0;
	bool m_MaterialSet = false;

	void UpdateMaterial(const GameContext& gameContext);

public:
	ModelComponent(const ModelComponent& other) = delete;
	ModelComponent(ModelComponent&& other) noexcept = delete;
	ModelComponent& operator=(const ModelComponent& other) = delete;
	ModelComponent& operator=(ModelComponent&& other) noexcept = delete;

	ModelComponent(std::wstring  assetFile);
	virtual ~ModelComponent();

	void SetMaterial(UINT materialId);

protected:
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void Initialize(const GameContext& gameContext) override;
};
