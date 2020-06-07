#pragma once
#include "Singleton.h"

class OverlordGame;
class DeferredMaterial;
class DeferredLightDrawer;

enum class DeferredBufferIds
{
	LIGHT_ACCUMULATION = 0, //Emissive/Ambient (+ lights)
	DIFFUSE = 1, //Diffuse Color
	SPECULAR = 2, //Specular Color/Level
	NORMAL = 3 //Normals
};

class DeferredRenderer final : public Singleton<DeferredRenderer>
{
public:
	DeferredRenderer() = default;
	~DeferredRenderer();

	DeferredRenderer(const DeferredRenderer& other) = delete;
	DeferredRenderer(DeferredRenderer&& other) noexcept = delete;
	DeferredRenderer& operator=(const DeferredRenderer& other) = delete;
	DeferredRenderer& operator=(DeferredRenderer&& other) noexcept = delete;

	void Begin() const;
	void End(const GameContext& gameContext);

	void InitRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, OverlordGame* pGame);
	ID3D11ShaderResourceView* GetShaderResourceView(DeferredBufferIds id) const { return m_pShaderResourceViewArr[int(id)]; }
	   
private:
	bool m_IsInitialized = false;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	OverlordGame* m_pGame = nullptr;

	DeferredMaterial* m_pDeferredMat = nullptr;
	DeferredLightDrawer* m_pLightPass = nullptr;

	static const int BUFFER_COUNT = 4;
	ID3D11Texture2D* m_pBufferArr[BUFFER_COUNT] = { nullptr };
	ID3D11RenderTargetView* m_pRenderTargetViewArr[BUFFER_COUNT] = { nullptr };
	ID3D11ShaderResourceView* m_pShaderResourceViewArr[BUFFER_COUNT] = { nullptr };

	ID3D11RenderTargetView* m_pDefaultRenderTargetView = nullptr; //FROM GAME
	ID3D11DepthStencilView* m_pDefaultDepthStencilView = nullptr; //FROM GAME
	ID3D11ShaderResourceView* m_pDefaultDepthStencilSRV = nullptr; //FROM GAME

	void CreateBufferAndViews(int width, int height, DXGI_FORMAT format, DeferredBufferIds id);
	void ClearRenderTargets() const;
	void SetRenderTargets() const;
};

