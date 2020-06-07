#include "stdafx.h"
#include "Material.h"
#include "ContentManager.h"
#include "CameraComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

Material::Material(std::wstring effectFile, std::wstring technique, bool autoBindMatrices) :
	m_effectFile(std::move(effectFile)), 
	m_pEffect(nullptr),
	m_TechniqueName(std::move(technique)),
	m_pTechnique(nullptr),
	m_pWorldMatrixVariable(nullptr),
	m_pViewMatrixVariable(nullptr),
	m_pViewInverseMatrixVariable(nullptr),
	m_pWvpMatrixVariable(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_InputLayoutID(0),
	m_IsInitialized(false),
	m_AutoBindMatrices(autoBindMatrices)
{}

Material::~Material()
{
	SafeRelease(m_pInputLayout);
	m_pInputLayoutDescriptions.clear();
}

void Material::Initialize(ID3D11Device* pDevice)
{
	if (!m_IsInitialized)
	{
		auto pos = m_effectFile.rfind('.', m_effectFile.length());
		if (pos != std::string::npos)
		{
			std::wstring const extension = m_effectFile.substr(pos + 1, m_effectFile.length() - pos);
			if (extension == L"fx")
			{
				LoadEffect(pDevice);
				m_IsInitialized = true;
			}
			else if (extension == L"fxc")
			{
				Logger::LogInfo(L"Loading precompiled shader");
				LoadCompiledEffect(pDevice);
				m_IsInitialized = true;
			}
		}
	}
}

bool Material::LoadEffect(ID3D11Device* pDevice)
{
	//Load Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(m_effectFile);

	if (!m_TechniqueName.empty())
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string techString = converter.to_bytes(m_TechniqueName);
		m_pTechnique = m_pEffect->GetTechniqueByName(techString.c_str());
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}

	//Build InputLayout
	EffectHelper::BuildInputLayout(pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions,
	                               m_pInputLayoutSize, m_InputLayoutID);

	if (m_AutoBindMatrices)
	{
		auto effectVar = m_pEffect->GetVariableBySemantic("World");
		m_pWorldMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
		effectVar = m_pEffect->GetVariableBySemantic("View");
		m_pViewMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
		effectVar = m_pEffect->GetVariableBySemantic("ViewInverse");
		m_pViewInverseMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
		effectVar = m_pEffect->GetVariableBySemantic("WorldViewProjection");
		m_pWvpMatrixVariable = (effectVar->IsValid()) ? effectVar->AsMatrix() : nullptr;
	}

	LoadEffectVariables();

	return true;
}

bool Material::LoadCompiledEffect(ID3D11Device* pDevice)
{
	ID3DX11Effect* pEffect;
	D3DX11CreateEffectFromFile(m_effectFile.c_str(),
		0, pDevice, &pEffect);
	return pEffect;
}

void Material::SetEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_IsInitialized)
	{
		if (m_AutoBindMatrices)
		{
			auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
			auto view = XMLoadFloat4x4(&gameContext.pCamera->GetView());
			const auto projection = XMLoadFloat4x4(&gameContext.pCamera->GetProjection());

			if (m_pWorldMatrixVariable)
				m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));

			if (m_pViewMatrixVariable)
				m_pViewMatrixVariable->SetMatrix(reinterpret_cast<float*>(&view));

			if (m_pWvpMatrixVariable)
			{
				auto wvp = world * view * projection;
				m_pWvpMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&(wvp)));
			}

			if (m_pViewInverseMatrixVariable)
			{
				auto viewInv = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
				m_pViewInverseMatrixVariable->SetMatrix(reinterpret_cast<float*>(&viewInv));
			}
		}

		UpdateEffectVariables(gameContext, pModelComponent);
	}
}
