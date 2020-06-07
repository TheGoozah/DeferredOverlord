#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
                                               std::wstring technique)
	: m_IsInitialized(false), 
	  m_pInputLayout(nullptr),
	  m_pInputLayoutSize(0),
	  m_effectFile(std::move(effectFile)),
	  m_InputLayoutID(0),
	  m_RenderIndex(renderIndex),
	  m_pRenderTarget(nullptr),
	  m_pVertexBuffer(nullptr),
	  m_pIndexBuffer(nullptr),
	  m_NumVertices(0),
	  m_NumIndices(0),
	  m_VertexBufferStride(0),
	  m_pEffect(nullptr),
	  m_pTechnique(nullptr),
	  m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (!m_IsInitialized)
	{
		//TODO: complete
		//1. LoadEffect (LoadEffect(...))
		//2. CreateInputLaytout (CreateInputLayout(...))
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		//3. Create RenderTarget (m_pRenderTarget)
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()

		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const std::wstring& effectFile)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(effectFile);

	//TODO: complete
	//Load Effect through ContentManager
	//Check if m_TechniqueName (default constructor parameter) is set
	// If SET > Use this Technique (+ check if valid)
	// If !SET > Use Technique with index 0

	//Call LoadEffectVariables

	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(previousRendertarget);

	//TODO: complete
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	//2. Call UpdateEffectVariables(...)
	//3. Set InputLayout
	//4. Set VertexBuffer
	//5. Set PrimitiveTopology (TRIANGLELIST)
	//6. Draw 

	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	m_NumVertices = 4;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//Create vertex array containing three elements in system memory
	//fill a buffer description to copy the vertexdata into graphics memory
	//create a ID3D10Buffer in graphics memory containing the vertex info
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	m_NumIndices = 6;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	// Create index buffer
}
