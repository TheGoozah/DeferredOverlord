#include "stdafx.h"
#include "DeferredLightDrawer.h"
#include "ContentManager.h"
#include "EffectHelper.h"
#include "MeshFilter.h"
#include "DeferredRenderer.h"
#include "Components.h"
#include "LightManager.h"

inline DirectX::XMMATRIX MatrixAlignVectors(DirectX::XMFLOAT4 v1, DirectX::XMFLOAT4 v2)
{
	DirectX::XMVECTOR vec1 = XMLoadFloat4(&v1);
	DirectX::XMVECTOR vec2 = XMLoadFloat4(&v2);

	float cos_theta = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(vec1), DirectX::XMVector3Normalize(vec2)));
	float angle = acos(cos_theta);

	if (angle == 0.f)
		return DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR axis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vec1, vec2));

	return DirectX::XMMatrixRotationAxis(axis, angle);
}

DeferredLightDrawer::DeferredLightDrawer()
{}

DeferredLightDrawer::~DeferredLightDrawer()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pReadOnlyDepthStencilView);
}

void DeferredLightDrawer::Initialize(ID3D11Device* pDevice)
{
	//Load Effect & Technique
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"Resources/Effects/Deferred/Deferred_LightPass.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (!m_pTechnique->IsValid())
		Logger::LogError(L"DeferredLightDrawer::Initialize > Invalid Technique.");

	//Build InputLayout
	std::vector<ILDescription> ilDescriptions;
	UINT ilSize;
	EffectHelper::BuildInputLayout(pDevice, m_pTechnique, &m_pInputLayout, ilDescriptions, ilSize, m_InputLayoutID);
	
	//Load Mesh & Build VB & IB
	m_pSphereMesh = ContentManager::Load<MeshFilter>(L"Resources/Meshes/UnitSphere.ovm");
	m_pSphereMesh->BuildVertexBuffer(pDevice, m_InputLayoutID, ilSize, ilDescriptions);
	m_pSphereVB = m_pSphereMesh->GetVertexBufferData(m_InputLayoutID).pVertexBuffer;
	
	m_pSphereMesh->BuildIndexBuffer(pDevice);
	m_pSphereIB = m_pSphereMesh->m_pIndexBuffer;

	m_pConeMesh = ContentManager::Load<MeshFilter>(L"Resources/Meshes/UnitCone.ovm");
	m_pConeMesh->BuildVertexBuffer(pDevice, m_InputLayoutID, ilSize, ilDescriptions);
	auto vbData = m_pConeMesh->GetVertexBufferData(m_InputLayoutID);
	m_pConeVB = vbData.pVertexBuffer;
	m_VertexStride = vbData.VertexStride;

	m_pConeMesh->BuildIndexBuffer(pDevice);
	m_pConeIB = m_pConeMesh->m_pIndexBuffer;	
}

void DeferredLightDrawer::DoLightingPass(const GameContext& gameContext, ID3D11RenderTargetView* pDefaultRTV, ID3D11ShaderResourceView* pBufferSRVs[], ID3D11ShaderResourceView* pDepthSRV) const
{
	//Set RenderTarget
	gameContext.pDeviceContext->OMSetRenderTargets(1, &pDefaultRTV, m_pReadOnlyDepthStencilView);

	//Bind SRVs
	m_pEffect->GetVariableByName("gTextureDepth")->AsShaderResource()->SetResource(pDepthSRV);
	m_pEffect->GetVariableByName("gTextureDiffuse")->AsShaderResource()->SetResource(pBufferSRVs[int(DeferredBufferIds::DIFFUSE)]);
	m_pEffect->GetVariableByName("gTextureSpecular")->AsShaderResource()->SetResource(pBufferSRVs[int(DeferredBufferIds::SPECULAR)]);
	m_pEffect->GetVariableByName("gTextureNormal")->AsShaderResource()->SetResource(pBufferSRVs[int(DeferredBufferIds::NORMAL)]);
	m_pEffect->GetVariableByName("gEyePos")->AsVector()->SetFloatVector((float*)(&gameContext.pCamera->GetTransform()->GetWorldPosition()));
	m_pEffect->GetVariableByName("gMatrixViewProjInv")->AsMatrix()->SetMatrix((float*)(&gameContext.pCamera->GetViewProjectionInverse()));
	
	for(auto light: gameContext.pLightManager->m_Lights)
	{
		if (!light.isEnabled)
			continue;

		//Reset Stencil to 1
		gameContext.pDeviceContext->ClearDepthStencilView(m_pReadOnlyDepthStencilView, D3D11_CLEAR_STENCIL, 0.f, 1);

		DrawLight(gameContext, light);
	}
}

void DeferredLightDrawer::DrawLight(const GameContext& gameContext, const Light& light) const
{
	DirectX::XMMATRIX rot = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT3 s = { light.range, light.range, light.range };
	if(light.type == LightType::Spot)
	{
		s.x = s.y = tanf(DirectX::XMConvertToRadians(light.spotLightAngle)) * light.range;
		rot = MatrixAlignVectors({ 0.f, 0.f, 1.f, 0.f }, light.direction);
	}

	//Calculate wvp
	auto trans = DirectX::XMMatrixTranslation(light.position.x, light.position.y, light.position.z);
	auto scale = DirectX::XMMatrixScaling(s.x, s.y, s.z);
	auto world = scale *rot*trans;
	auto wvp = world * XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
	m_pEffect->GetVariableByName("gWorldViewProjection")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&wvp));
	m_pEffect->GetVariableByName("gCurrentLight")->SetRawValue(&light, 0, sizeof(Light) - 4);

	//Set InputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	//Set Vertexbuffer
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, light.type==LightType::Point ? &m_pSphereVB : &m_pConeVB, &m_VertexStride, &offset);

	//Set Indexbuffer
	gameContext.pDeviceContext->IASetIndexBuffer(light.type == LightType::Point ? m_pSphereIB : m_pConeIB, DXGI_FORMAT_R32_UINT, 0);

	//Set Primitive Topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DRAW
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(light.type == LightType::Point ? m_pSphereMesh->m_IndexCount : m_pConeMesh->m_IndexCount, 0, 0);
	}
}

void DeferredLightDrawer::CreateReadOnlyDSV(ID3D11Device* pDevice, ID3D11Texture2D* pResource, DXGI_FORMAT format)
{
	//DEPTHSTENCIL VIEW
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));

	descDSV.Format = format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = D3D11_DSV_READ_ONLY_DEPTH; //DEPTH READ-ONLY!

	auto hr = pDevice->CreateDepthStencilView(pResource, &descDSV, &m_pReadOnlyDepthStencilView);
	Logger::LogHResult(hr, L"DeferredRenderer::CreateReadOnlyDSV");
}
