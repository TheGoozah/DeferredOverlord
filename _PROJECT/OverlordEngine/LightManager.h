#pragma once
#include "Singleton.h"
#include "DeferredLightDrawer.h"

enum class LightType : uint32_t
{
	Point = 0,
	Spot = 1
};

//__declspec(align(16)) struct Light //(with 24B padding)
struct Light
{
	DirectX::XMFLOAT4 direction; //16B
	DirectX::XMFLOAT4 position; //16B
	DirectX::XMFLOAT4 color; //16B
	float intensity; //4B
	float range; //4B
	float spotLightAngle; //4B
	LightType type; //4B
	//4 * 16B (To Shader)

	bool isEnabled; //4B (Discard)

	Light() :
		direction({ 0.f, 1.f, 0.f, 1.f }),
		position({ 0.f, 0.f, 0.f, 0.f }),
		color({ 1.f, 1.f, 1.f, 1.f }),
		intensity(1.f),
		range(50.f),
		spotLightAngle(45.f),
		type(LightType::Point),
		isEnabled(true){}
};

class LightManager final
{
public:
	LightManager();
	~LightManager();

	LightManager(const LightManager& other) = delete;
	LightManager(LightManager&& other) noexcept = delete;
	LightManager& operator=(const LightManager& other) = delete;
	LightManager& operator=(LightManager&& other) noexcept = delete;

	int AddLight(const Light& l) { m_Lights.push_back(l); return m_Lights.size() - 1; }
	Light& GetLight(int id) { return m_Lights[id]; }
	Light& GetDirectionalLight() { return m_DirectionalLight; }

private:
	friend class DeferredLightDrawer;

	std::vector<Light> m_Lights;
	Light m_DirectionalLight = {};
};

