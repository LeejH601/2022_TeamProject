#include "Map.h"
#include "Object.h"
#include "ModelManager.h"
#include "..\Global\Locator.h"
#include "..\Object\Terrain.h"
#include "..\Shader\TerrainShader.h"

void CMap::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	DXGI_FORMAT pdxgiObjectRtvFormats[7] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32_FLOAT };

	m_pTerrainShader = std::make_unique<CSplatTerrainShader>();
	m_pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 7, pdxgiObjectRtvFormats, DXGI_FORMAT_D32_FLOAT, 0);
	m_pTerrainShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 13);
	m_pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// Terrain »ý¼º
	XMFLOAT3 xmf3Scale(1.0f, 1.0f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = std::make_unique<CSplatTerrain>(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, _T("Terrain/terrainNew.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color, m_pTerrainShader.get());
}

void CMap::UpdateObjectArticulation()
{
	for (int i = 0; i < m_pMapObjects.size(); ++i) {
		m_pMapObjects[i]->updateArticulationMatrix();
	}
}

void CMap::Update(float fTimeElapsed)
{
	for (int i = 0; i < m_pMapObjects.size(); ++i) {
		m_pMapObjects[i]->Update(fTimeElapsed);
	}
}

void CMap::RenderTerrain(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pTerrainShader->Render(pd3dCommandList, 0);
	m_pTerrain->Render(pd3dCommandList, true);
}

void CMap::RenderMapObjects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_pMapObjects.size(); ++i) {
		m_pMapObjects[i]->Render(pd3dCommandList, true);
	}
}

void CMap::LoadSceneFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	char pstrToken[256] = { '\0' };

	int nGameObjects;

	UINT nRead;

	::ReadStringFromFile(pInFile, pstrToken);

	std::wstring TextString = L"Loading Scene";
	OutputDebugString(TextString.c_str());

	try
	{
		if (!strcmp(pstrToken, "<GameObjects>:"))
		{
			int nObjects = ReadIntegerFromFile(pInFile);
			for (int i = 0; i < nObjects; ++i) {
				ReadStringFromFile(pInFile, pstrToken);
				nRead = ReadStringFromFile(pInFile, pstrToken);
				pstrToken[nRead] = '\0';

				std::string objName = pstrToken;
				TextString.assign(objName.begin(), objName.end());
				TextString += L"Loading.....\n";
				OutputDebugString(TextString.c_str());

				if (pstrToken[0] == '\0')
					break;

				float buffer[16];
				std::string objPath{ "Object/Scene/" };
				objPath += static_cast<std::string>(pstrToken) + ".bin";

				FILE* objFile = NULL;
				::fopen_s(&objFile, objPath.data(), "rb");
				::rewind(objFile);


				nRead = (UINT)::fread(&buffer, sizeof(float), 16, pInFile);
				XMFLOAT4X4 xmfWorld = {
					buffer[0],buffer[1],buffer[2],buffer[3],
					buffer[4],buffer[5],buffer[6],buffer[7],
					buffer[8],buffer[9],buffer[10],buffer[11],
					buffer[12],buffer[13],buffer[14],buffer[15]
				};

				int nSkinnedMeshes;
				std::unique_ptr<CMapObject> pMapObject = std::make_unique<CMapObject>();

				pMapObject->LoadObject(pd3dDevice, pd3dCommandList, objFile);

				pMapObject->m_xmf4x4Transform = xmfWorld;

				std::string name{ pstrToken };
				if (name.find("Tree") != std::string::npos || name.find("Rock") != std::string::npos)
				{
					if (name.find("Tree") != std::string::npos)
					{
						pMapObject->SetObjType(MAP_OBJ_TYPE::TREE);
						pMapObject->PrepareBoundingBox(pd3dDevice, pd3dCommandList);
						pMapObject->AddPhysicsScene(xmfWorld);
					}
					else
						pMapObject->SetObjType(MAP_OBJ_TYPE::ROCK);
				}

				pMapObject->UpdateTransform(NULL);

				m_pMapObjects.push_back(std::move(pMapObject));

				fclose(objFile);
			}
		}
	}
	catch (const std::exception&)
	{

	}
	
	fclose(pInFile);
}
