#include "stdafx.h"
#include "MeshFilterLoader.h"
#include "BinaryReader.h"
#include "EffectHelper.h"

#define OVM_vMAJOR 1
#define OVM_vMINOR 1

MeshFilter* MeshFilterLoader::LoadContent(const std::wstring& assetFile)
{
	auto binReader = new BinaryReader();
	binReader->Open(assetFile);

	if(!binReader->Exists())
		return nullptr;

	//READ OVM FILE
	const int versionMajor = binReader->Read<char>();
	const int versionMinor = binReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::Load() > Wrong OVM version\n\tFile: \"%s\" \n\tExpected version %i.%i, not %i.%i.", assetFile.c_str(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);
		delete binReader;
		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	auto pMesh = new MeshFilter();

	for (;;)
	{
		const auto meshDataType = static_cast<MeshDataType>(binReader->Read<char>());
		if(meshDataType == MeshDataType::END)
			break;

		const auto dataOffset = binReader->Read<unsigned int>();

		switch(meshDataType)
		{
		case MeshDataType::HEADER:
			{
				pMesh->m_MeshName = binReader->ReadString();
				vertexCount = binReader->Read<unsigned int>();
				indexCount = binReader->Read<unsigned int>();

				pMesh->m_VertexCount = vertexCount;
				pMesh->m_IndexCount = indexCount;
			}
			break;
		case MeshDataType::POSITIONS:
			{
				pMesh->m_HasElement |= ILSemantic::POSITION;

				for(unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 pos;
					pos.x = binReader->Read<float>();
					pos.y = binReader->Read<float>();
					pos.z = binReader->Read<float>();
					pMesh->m_Positions.push_back(pos);
				}
			}
			break;
		case MeshDataType::INDICES:
			{
				for(unsigned int i = 0; i<indexCount; ++i)
				{
					pMesh->m_Indices.push_back(binReader->Read<DWORD>());
				}
			}
			break;
		case MeshDataType::NORMALS:
			{
				pMesh->m_HasElement |= ILSemantic::NORMAL;

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 normal;
					normal.x = binReader->Read<float>();
					normal.y = binReader->Read<float>();
					normal.z = binReader->Read<float>();
					pMesh->m_Normals.push_back(normal);
				}
			}
			break;
		case MeshDataType::TANGENTS:
			{
				pMesh->m_HasElement |= ILSemantic::TANGENT;

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 tangent;
					tangent.x = binReader->Read<float>();
					tangent.y = binReader->Read<float>();
					tangent.z = binReader->Read<float>();
					pMesh->m_Tangents.push_back(tangent);
				}
			}
			break;
		case MeshDataType::BINORMALS:
			{
				pMesh->m_HasElement |= ILSemantic::BINORMAL;

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT3 binormal;
					binormal.x = binReader->Read<float>();
					binormal.y = binReader->Read<float>();
					binormal.z = binReader->Read<float>();
					pMesh->m_Binormals.push_back(binormal);
				}
			}
			break;
		case MeshDataType::TEXCOORDS:
			{
				pMesh->m_HasElement |= ILSemantic::TEXCOORD;

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT2 tc;
					tc.x = binReader->Read<float>();
					tc.y = binReader->Read<float>();
					pMesh->m_TexCoords.push_back(tc);
				}
			}
			break;
		case MeshDataType::COLORS:
			{
				pMesh->m_HasElement |= ILSemantic::COLOR;

				for (unsigned int i = 0; i<vertexCount; ++i)
				{
				 DirectX::XMFLOAT4 color;
					color.x = binReader->Read<float>();
					color.y = binReader->Read<float>();
					color.z = binReader->Read<float>();
					color.w = binReader->Read<float>();
					pMesh->m_Colors.push_back(color);
				}
			}
			break;
		default:
			binReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	delete binReader;

	return pMesh;
}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}
