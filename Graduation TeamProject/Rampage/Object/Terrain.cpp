#include "Texture.h"
#include "Terrain.h"
#include "../Shader/TerrainShader.h"
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, CShader* pTerrainShader) : CGameObject(1)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//std::shared_ptr<CMaterial> pMaterial = std::make_shared<CMaterial>();
	//m_nMeshes = cxBlocks * czBlocks;
	m_pMesh = std::make_shared<CMesh>();
	//for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(std::shared_ptr<CHeightMapGridMesh>(pHeightMapGridMesh));

		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	std::shared_ptr<CTexture> pTerrainTexture = std::make_shared<CTexture>(3, RESOURCE_TEXTURE2D, 0, 1);

	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Detail_Texture_7.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/HeightMap(Alpha).dds", RESOURCE_TEXTURE2D, 2);

	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pTerrainTexture.get(), 0, 2);


	std::shared_ptr<CMaterial> pTerrainMaterial = std::make_shared<CMaterial>();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	//pTerrainMaterial->SetShader(pTerrainShader);
	SetMaterial(0, pTerrainMaterial);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

CSplatTerrain::CSplatTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, CShader* pTerrainShader)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//std::shared_ptr<CMaterial> pMaterial = std::make_shared<CMaterial>();
	//m_nMeshes = cxBlocks * czBlocks;
	m_pMesh = std::make_shared<CMesh>();
	//for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CSplatGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CSplatGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(std::static_pointer_cast<CMesh>(std::shared_ptr<CSplatGridMesh>(pHeightMapGridMesh)));

		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	std::shared_ptr<CTexture> pTerrainTextures = std::make_shared<CTexture>(9, RESOURCE_TEXTURE2D, 0, 1);

	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Grass.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Sand.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Pebbles_Sand.dds", RESOURCE_TEXTURE2D, 2);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Rockwall.dds", RESOURCE_TEXTURE2D, 3);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/BaseGrass_normals.dds", RESOURCE_TEXTURE2D, 4);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/BaseGrass_normals.dds", RESOURCE_TEXTURE2D, 5);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Pebbles_normals.dds", RESOURCE_TEXTURE2D, 6);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Rockwall_Normal.dds", RESOURCE_TEXTURE2D, 7);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Alphatexture.dds", RESOURCE_TEXTURE2D, 8);

	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pTerrainTextures.get(), 0, 6);

	std::shared_ptr<CMaterial> pTerrainMaterial = std::make_shared<CMaterial>();

	pTerrainMaterial->SetTexture(pTerrainTextures);

	m_nMaterials = 1;
	m_ppMaterials.resize(m_nMaterials);

	SetMaterial(0, pTerrainMaterial);
}

CSplatTerrain::~CSplatTerrain()
{
}