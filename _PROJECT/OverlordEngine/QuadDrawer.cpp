#include "stdafx.h"
#include "QuadDrawer.h"
#include "OverlordGame.h"
#include "Material.h"
#include "QuadMaterial.h"

QuadDrawer::QuadDrawer()
{}

QuadDrawer::~QuadDrawer()
{
	SafeRelease(m_pVertexBuffer);
	SafeDelete(m_pDefaultMaterial);
}

void QuadDrawer::Initialize(ID3D11Device* pDevice)
{
	if (m_IsInitialized)
		return;

	//Retrieve Screen Dimensions
	auto windowSettings = OverlordGame::GetGameSettings().Window;
	m_ScreenWidth = float(windowSettings.Width);
	m_ScreenHeight = float(windowSettings.Height);
	m_HalfScreenWidth = m_ScreenWidth * 0.5f;
	m_HalfScreenHeight = m_ScreenHeight * 0.5f;

	//Fill VertexArray (VertexPosTex only!)
	VertexPosTex vertices[8]; //2 Quads
	
	//Quad 0 (CUSTOM_QUAD) > Used to render custom sized quads (Dynamic) [debugging purposes]
	auto i = CUSTOM_QUAD * 4;
	vertices[i++] = VertexPosTex(DirectX::XMFLOAT3(-1.f, 1.f, 0.f), DirectX::XMFLOAT2(0.f, 0.f));
	vertices[i++] = VertexPosTex(DirectX::XMFLOAT3(1.f, 1.f, 0.f), DirectX::XMFLOAT2(1.f, 0.f));
	vertices[i++] = VertexPosTex(DirectX::XMFLOAT3(-1.f, -1.f, 0.f), DirectX::XMFLOAT2(0.f, 1.f));
	vertices[i] = VertexPosTex(DirectX::XMFLOAT3(1.f, -1.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f));

	//Quad 1 (FULLSCREEN_QUAD) > Used only to render fullscreen (Immutable)
	i = FULLSCREEN_QUAD * 4;
	vertices[i++] = VertexPosTex(DirectX::XMFLOAT3(-1.f, 1.f, 0.f), DirectX::XMFLOAT2(0.f, 0.f));
	vertices[i++] = VertexPosTex(DirectX::XMFLOAT3(1.f, 1.f, 0.f), DirectX::XMFLOAT2(1.f, 0.f));
	vertices[i++] = VertexPosTex(DirectX::XMFLOAT3(-1.f, -1.f, 0.f), DirectX::XMFLOAT2(0.f, 1.f));
	vertices[i] = VertexPosTex(DirectX::XMFLOAT3(1.f, -1.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f));

	//Vertexbuffer (dynamic)
	D3D11_BUFFER_DESC vertexBuffDesc;
	vertexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.ByteWidth = sizeof(VertexPosTex) * 8;
	vertexBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	vertexBuffDesc.Usage =  D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &vertices[0];

	auto hRes = pDevice->CreateBuffer(&vertexBuffDesc, &initData, &m_pVertexBuffer);
	Logger::LogHResult(hRes, L"QuadDrawer::Initialize(...) - CreateBuffer");

	//Create Default Material (used to render a texture directly) [Material class Updated!]
	m_pDefaultMaterial = new QuadMaterial();
	m_pDefaultMaterial->Initialize(pDevice);

	m_IsInitialized = true;
}

//SRV DRAW
void QuadDrawer::Draw(const GameContext& gameContext, ID3D11ShaderResourceView* pSRV, QuadRect dim, bool useRect)
{
	//TODO: DEBUG EFFECTS11 FRAMEWORK (> EffectPass::Apply resets PSShaderResources...)
	m_pDefaultMaterial->SetTexture(pSRV);
	//gameContext.pDeviceContext->PSSetShaderResources(0, 1, &pSRV); //BIND SRV (Set T-Register 0)
	Draw(gameContext, m_pDefaultMaterial, dim, useRect);

	//ID3D11ShaderResourceView* pEmptySRV[] = { nullptr };
	//gameContext.pDeviceContext->PSSetShaderResources(0, 1, pEmptySRV); //UNBIND SRV (Clear T-Register 0)
}

//CUSTOM MATERIAL DRAW (WITH FIXED INPUTLAYOUT - VertexPosTex)
void QuadDrawer::Draw(const GameContext& gameContext, Material* pMaterial, QuadRect dim, bool useRect)
{
	auto quadMode = useRect? CUSTOM_QUAD :FULLSCREEN_QUAD;
	auto pDeviceContext = gameContext.pDeviceContext;

	if(useRect && m_PreviousDimensions!=dim) //Only update the vertexbuffer when needed!
	{
		//CUSTOM QUAD
		m_PreviousDimensions = dim; //Set PrevDim (Prevents updating the vertexbuffer with the same values...)

		//Update vertexbuffer (first quad only (index 0))
		//Screenspace Coordinates > Clipspace Coordinates (NDC)
		float ndcX = ((dim.x - m_HalfScreenWidth) / m_ScreenWidth) * 2.0f;
		float ndcY = ((m_HalfScreenHeight - dim.y) / m_ScreenHeight) *2.0f;
		float ndcWidth = (dim.width / m_ScreenWidth) * 2.0f;
		float ndcHeight = (dim.height / m_ScreenHeight) * 2.0f;

		//Update affected vertexbuffer data
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); //Prepare for writing

		VertexPosTex* pVertices = static_cast<VertexPosTex*>(mappedResource.pData);
		pVertices[CUSTOM_QUAD].Position = { ndcX, ndcY, 0.f };
		pVertices[CUSTOM_QUAD + 1].Position = { ndcX + ndcWidth, ndcY, 0.f };
		pVertices[CUSTOM_QUAD + 2].Position = { ndcX, ndcY - ndcHeight, 0.f };
		pVertices[CUSTOM_QUAD + 3].Position = { ndcX + ndcWidth, ndcY - ndcHeight, 0.f };

		pDeviceContext->Unmap(m_pVertexBuffer, 0);
	}

	//Setup Pipeline
	//INPUT LAYOUT
	pDeviceContext->IASetInputLayout(pMaterial->GetInputLayout());

	//VERTEXBUFFER
	UINT offset = 0;
	UINT stride = sizeof(VertexPosTex);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//PRIM. TOPOLOGY
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Call Material Variable Update
	pMaterial->SetEffectVariables(gameContext, nullptr);

	//DRAWCALL
	auto pTechnique = pMaterial->GetTechnique();
	D3DX11_TECHNIQUE_DESC techDesc;
	pTechnique->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(4, quadMode * 4); //Only draw one of the stored quads, based on quadMode
		//pDeviceContext->Draw(4, 0); > CUSTOM_QUAD
		//pDeviceContext->Draw(4, 4); > FULLSCREEN_QUAD
	}
}
