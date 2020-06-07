#pragma once
struct Light;
class MeshFilter;

class DeferredLightDrawer final
{
public:
	DeferredLightDrawer();
	~DeferredLightDrawer();

	DeferredLightDrawer(const DeferredLightDrawer& other) = delete;
	DeferredLightDrawer(DeferredLightDrawer&& other) noexcept = delete;
	DeferredLightDrawer& operator=(const DeferredLightDrawer& other) = delete;
	DeferredLightDrawer& operator=(DeferredLightDrawer&& other) noexcept = delete;

	void Initialize(ID3D11Device* pDevice);
	void CreateReadOnlyDSV(ID3D11Device* pDevice, ID3D11Texture2D* pResource, DXGI_FORMAT format);

	void DoLightingPass(const GameContext& gameContext, ID3D11RenderTargetView* pDefaultRTV, ID3D11ShaderResourceView* gBuffer[], ID3D11ShaderResourceView* depth) const;

private:
	MeshFilter* m_pSphereMesh = nullptr;
	MeshFilter* m_pConeMesh = nullptr;
	ID3DX11Effect* m_pEffect = nullptr;
	ID3DX11EffectTechnique* m_pTechnique = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3D11Buffer* m_pSphereVB = nullptr, *m_pSphereIB = nullptr;
	ID3D11Buffer* m_pConeVB = nullptr, *m_pConeIB = nullptr;
	UINT m_InputLayoutID = 0, m_VertexStride = 0;

	ID3D11DepthStencilView* m_pReadOnlyDepthStencilView = nullptr;

	void DrawLight(const GameContext& gameContext, const Light& light) const;
};

