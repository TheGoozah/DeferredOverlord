#pragma once
#include "GameScene.h"

class SponzaScene final : public GameScene
{
public:
	SponzaScene();
	~SponzaScene() = default;

	SponzaScene(const SponzaScene& other) = delete;
	SponzaScene(SponzaScene&& other) noexcept = delete;
	SponzaScene& operator=(const SponzaScene& other) = delete;
	SponzaScene& operator=(SponzaScene&& other) noexcept = delete;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:
	GameObject* m_pSponzaRoot = nullptr;
};

