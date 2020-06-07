#include "stdafx.h"

#include "ContentManager.h"
#include "EffectLoader.h"
#include "MeshFilterLoader.h"
#include "TextureDataLoader.h"

std::vector<BaseLoader*> ContentManager::m_Loaders = std::vector<BaseLoader*>();
ID3D11Device* ContentManager::m_pDevice = nullptr;
bool ContentManager::m_IsInitialized = false;

void ContentManager::Release()
{
	for(BaseLoader *ldr:m_Loaders)
	{	
		ldr->Unload();
		SafeDelete(ldr);
	}

	m_Loaders.clear();
}

void ContentManager::Initialize(ID3D11Device* pDevice)
{
	if(!m_IsInitialized)
	{
		m_pDevice = pDevice;
		m_IsInitialized = true;
		AddLoader(new EffectLoader());
		AddLoader(new MeshFilterLoader());
		AddLoader(new TextureDataLoader());
	}
}

void ContentManager::AddLoader(BaseLoader* loader)
{ 
	for(BaseLoader *ldr:m_Loaders)
	{	
		if(ldr->GetType()==loader->GetType())
		{
			SafeDelete(loader);
			break;
		}
	}

	m_Loaders.push_back(loader);
	loader->SetDevice(m_pDevice);
}
