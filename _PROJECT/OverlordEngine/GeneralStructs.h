#pragma once
#include "GameTime.h"
#include "CameraComponent.h"
#include "InputManager.h"
#include "MaterialManager.h"
#include "LightManager.h"

class CameraComponent;

struct GameSettings
{
	GameSettings():
		Window(WindowSettings()),
		DirectX(DirectXSettings())
	{}

#pragma region
	struct WindowSettings
	{
		WindowSettings():
			Width(1280),
			Height(720),
			AspectRatio(Width/static_cast<float>(Height)),
			Title(L"Overlord Engine (DX11)"),
			WindowHandle(nullptr)
		{
		}

		int Width;
		int Height;
		float AspectRatio;
		std::wstring Title;
		HWND WindowHandle;
	}Window;
#pragma endregion WINDOW_SETTINGS

#pragma region
	struct DirectXSettings
	{
		DirectXSettings() :
			pAdapter(nullptr),
			pOutput(nullptr)
		{}

		IDXGIAdapter* pAdapter;
		IDXGIOutput* pOutput;
		bool DeferredRenderingEnabled;
	}DirectX;
#pragma endregion DIRECTX_SETTINGS
};

struct GameContext
{
	GameTime* pGameTime;
	CameraComponent* pCamera;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	InputManager* pInput;
	MaterialManager* pMaterialManager;
	LightManager* pLightManager;
};
