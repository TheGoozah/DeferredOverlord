#include "stdafx.h"
#include "DeferredRenderer.h"
#include "OverlordGame.h"
#include "DeferredMaterial.h"
#include "QuadDrawer.h"
#include "RenderTarget.h"
#include "Components.h"
#include "DeferredLightDrawer.h"

DeferredRenderer::~DeferredRenderer()
{
	for(auto i = 1; i < BUFFER_COUNT; ++i)
	{
		SafeRelease(m_pBufferArr[i]);
		m_pBufferArr[i] = nullptr;

		SafeRelease(m_pRenderTargetViewArr[i]);
		m_pRenderTargetViewArr[i] = nullptr;

		SafeRelease(m_pShaderResourceViewArr[i]);
		m_pShaderResourceViewArr[i] = nullptr;
	}

	SafeDelete(m_pDeferredMat);
	SafeDelete(m_pLightPass);
}

void DeferredRenderer::InitRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, OverlordGame* pGame)
{
	if (m_IsInitialized)
		return;

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	m_pGame = pGame;

	auto w = m_pGame->GetGameSettings().Window.Width;
	auto h = m_pGame->GetGameSettings().Window.Height;

	//Set Default RTV and DSV
	auto rt = m_pGame->GetRenderTarget(); //ENABLE RT SRVs (OverlordGame)
	m_pDefaultRenderTargetView = rt->GetRenderTargetView(); //Owned by OverlordGame class
	m_pDefaultDepthStencilView = rt->GetDepthStencilView(); //Owned by OverlordGame class
	m_pDefaultDepthStencilSRV = rt->GetDepthShaderResourceView(); //Owned by OverlordGame class

	//Create Buffers, RTVs & SRVs
	//CreateBufferAndViews(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, DeferredBufferIds::LIGHT_ACCUMULATION);
	m_pRenderTargetViewArr[int(DeferredBufferIds::LIGHT_ACCUMULATION)] = m_pDefaultRenderTargetView; //Light-Accumulation = Default Rendertarget
	CreateBufferAndViews(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, DeferredBufferIds::DIFFUSE);
	CreateBufferAndViews(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, DeferredBufferIds::SPECULAR);
	CreateBufferAndViews(w, h, DXGI_FORMAT_R32G32B32A32_FLOAT, DeferredBufferIds::NORMAL);
															
	//Create and Intialize DeferredBasePassMaterial
	m_pDeferredMat = new DeferredMaterial();
	m_pDeferredMat->Initialize(m_pDevice); //Normally handled by MaterialManager

	//Initialize DeferredLightDrawer
	m_pLightPass = new DeferredLightDrawer();
	m_pLightPass->Initialize(pDevice);
	m_pLightPass->CreateReadOnlyDSV(pDevice, rt->GetDesc().pDepth, rt->GetDesc().DepthFormat);

	m_IsInitialized = true;
}

void DeferredRenderer::Begin() const
{
	if (!m_IsInitialized)
		Logger::LogError(L"DeferredRenderer::Begin() > Deferred Renderer not Initialized!");

	//1. Clear the G-Buffer
	ClearRenderTargets();

	//2. Bind G-Buffer RTVs
	SetRenderTargets();

	//RENDER SCENE
	//...
	//Fill G-Buffer (BasicEffect_Deferred.fx)
}

void DeferredRenderer::End(const GameContext& gameContext)
{
	//1. Geometry Pass finished > Unbind G-Buffer RTVs
	ID3D11RenderTargetView* rTargets[BUFFER_COUNT] = { nullptr };
	m_pDeviceContext->OMSetRenderTargets(BUFFER_COUNT, rTargets, nullptr);

	//2. Restore RenderTarget (GAME RTV) + Disable DephtStencil (needed as SRV)
	m_pDeviceContext->OMSetRenderTargets(1, &m_pDefaultRenderTargetView, nullptr);

	//3. DIRECTIONAL LIGHTING
	auto light = gameContext.pLightManager->GetDirectionalLight();
	if (light.isEnabled)
	{
		//m_pDeferredMat->SetLightAccumulationSRV(m_pShaderResourceViewArr[int(DeferredBufferIds::LIGHT_ACCUMULATION)]);
		m_pDeferredMat->SetDiffuseSRV(m_pShaderResourceViewArr[int(DeferredBufferIds::DIFFUSE)]);
		m_pDeferredMat->SetSpecularSRV(m_pShaderResourceViewArr[int(DeferredBufferIds::SPECULAR)]);
		m_pDeferredMat->SetNormalSRV(m_pShaderResourceViewArr[int(DeferredBufferIds::NORMAL)]);

		m_pDeferredMat->SetDepthSRV(m_pDefaultDepthStencilSRV);

		m_pDeferredMat->SetEyePosition(gameContext.pCamera->GetTransform()->GetWorldPosition());
		m_pDeferredMat->SetViewProjectionInverse(gameContext.pCamera->GetViewProjectionInverse());
		m_pDeferredMat->SetDirectionalLight(light);

		//3.1 Draw Fullscreen Quad [Directional Light affects the entire screen] (using DeferredMaterial > Deferred_DirectionalLightPass.fx)
		QuadDrawer::GetInstance()->Draw(gameContext, m_pDeferredMat);
	}

	//5. Do LightingPass (Point and Spot lights)
	m_pLightPass->DoLightingPass(gameContext, m_pDefaultRenderTargetView, m_pShaderResourceViewArr, m_pDefaultDepthStencilSRV);

	//6. Unbind G-Buffer SRVs & Depth
	ID3D11ShaderResourceView* pSRV[BUFFER_COUNT] = { nullptr };
	m_pDeviceContext->PSSetShaderResources(0, BUFFER_COUNT, pSRV); //Unbind 4 SRVs (Position, Specular, Normal & Depth)

	//7. Restore DepthStencilView
	m_pDeviceContext->OMSetRenderTargets(1, &m_pDefaultRenderTargetView, m_pDefaultDepthStencilView); //BACK TO NORMAL
}

void DeferredRenderer::CreateBufferAndViews(int width, int height, DXGI_FORMAT format, DeferredBufferIds id)
{
	//TEXTURE BUFFER
	//**************
	D3D11_TEXTURE2D_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	//Create Descriptor
	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.MipLevels = 1;
	bufferDesc.ArraySize = 1;
	bufferDesc.Format = format;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	//Create Resource
	auto hRes = m_pDevice->CreateTexture2D(&bufferDesc, nullptr, &m_pBufferArr[int(id)]);
	Logger::LogHResult(hRes, L"DeferredRenderer::CreateBuffer(...)"); //Blocking call on fail

	//RENDER TARGET VIEW
	//******************
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));

	//Create Descriptor
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	//Create Resource
	hRes = m_pDevice->CreateRenderTargetView(m_pBufferArr[int(id)], &rtvDesc, &m_pRenderTargetViewArr[int(id)]);
	Logger::LogHResult(hRes, L"DeferredRenderer::CreateBuffer(...)"); //Blocking call on fail

	//SHADER RESOURCE VIEW
	//********************
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	//Create Descriptor
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hRes = m_pDevice->CreateShaderResourceView(m_pBufferArr[int(id)], &srvDesc, &m_pShaderResourceViewArr[int(id)]);
	Logger::LogHResult(hRes, L"DeferredRenderer::CreateBuffer(...)"); //Blocking call on fail
}

void DeferredRenderer::ClearRenderTargets() const
{
	float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };

	for(auto i = 0; i < BUFFER_COUNT; ++i)
	{
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetViewArr[i], clearColor);
	}

	m_pDeviceContext->ClearDepthStencilView(m_pDefaultDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DeferredRenderer::SetRenderTargets() const
{
	m_pDeviceContext->OMSetRenderTargets(BUFFER_COUNT, m_pRenderTargetViewArr, m_pDefaultDepthStencilView);
}