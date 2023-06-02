#include "Texture.h"
#include "Terrain.h"
#include "../Shader/TerrainShader.h"
#include "../Global/Locator.h"

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
	XMStoreFloat3(&m_xmf3Scale, XMVectorScale(XMLoadFloat3(&m_xmf3Scale), 0.38819f*2));
	m_xmf3Scale.y = xmf3Scale.y;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, m_xmf3Scale);

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
			pHeightMapGridMesh = new CSplatGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, m_xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(std::static_pointer_cast<CMesh>(std::shared_ptr<CSplatGridMesh>(pHeightMapGridMesh)));

		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_xmf4x4ScaleNormalMatrix;

	UINT ncbElementBytes = ((sizeof(XMFLOAT4X4) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_d3dcbScaleNormalMatrix = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_d3dcbScaleNormalMatrix->Map(0, NULL, (void**)&m_xmf4x4ScaleNormalMatrix);

	std::shared_ptr<CTexture> pTerrainTextures = std::make_shared<CTexture>(13, RESOURCE_TEXTURE2D, 0, 1);

	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Grass.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Sand.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Pebbles_Sand.dds", RESOURCE_TEXTURE2D, 2);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Rockwall.dds", RESOURCE_TEXTURE2D, 3);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/BaseGrass_normals.dds", RESOURCE_TEXTURE2D, 4);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/BaseGrass_normals.dds", RESOURCE_TEXTURE2D, 5);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Pebbles_normals.dds", RESOURCE_TEXTURE2D, 6);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Rockwall_Normal.dds", RESOURCE_TEXTURE2D, 7);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/BaseGrass_Heights.dds", RESOURCE_TEXTURE2D, 8);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/BaseGrass_Heights.dds", RESOURCE_TEXTURE2D, 9);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Pebbles_Heights.dds", RESOURCE_TEXTURE2D, 10);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Rockwall_Heights.dds", RESOURCE_TEXTURE2D, 11);
	pTerrainTextures->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Alphatexture.dds", RESOURCE_TEXTURE2D, 12);

	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pTerrainTextures.get(), 0, 6);

	std::shared_ptr<CMaterial> pTerrainMaterial = std::make_shared<CMaterial>();

	pTerrainMaterial->SetTexture(pTerrainTextures);

	m_nMaterials = 1;
	m_ppMaterials.resize(m_nMaterials);

	SetMaterial(0, pTerrainMaterial);
}

CSplatTerrain::~CSplatTerrain()
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

void CSplatTerrain::SetRigidStatic()
{
	physx::PxPhysics* pPhysics = Locator.GetPxPhysics();

	physx::PxHeightFieldDesc Desc;
	Desc.format = physx::PxHeightFieldFormat::eS16_TM;
	Desc.nbColumns =  m_nLength/* * m_xmf3Scale.z*/;
	Desc.nbRows = m_nWidth/* * m_xmf3Scale.x*/;

	WORD* Pixels = m_pHeightMapImage->GetRawImagePixels();
	physx::PxReal minHeight = PX_MAX_F32;
	physx::PxReal maxHeight = -PX_MAX_F32;
	for (int z = 0; z < Desc.nbColumns; z++)
	{
		//tessflag = z % 2 == 0 ? false : true;
		for (int x = 0; x < Desc.nbRows; x++)
		{
			minHeight = physx::PxMin(minHeight, (float)(m_pHeightMapImage->GetRawImagePixel(x, z)));
			maxHeight = physx::PxMax(maxHeight, (float)(m_pHeightMapImage->GetRawImagePixel(x, z)));
			/*TCHAR pstrDebug[256] = { 0 };
			_stprintf_s(pstrDebug, 256, _T("height = %f\n"), m_pHeightMapImage->GetHeight(x, z));
			OutputDebugString(pstrDebug);*/
		}
	}

	physx::PxReal deltaHeight = maxHeight - minHeight;

	physx::PxHeightFieldSample* hfSamples = new physx::PxHeightFieldSample[Desc.nbColumns * Desc.nbRows];

	//physx::PxReal* buffer = new physx::PxReal[Desc.nbColumns * Desc.nbRows];
	physx::PxReal quantization = (physx::PxReal)0x0100;

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	int i = 0;
	bool tessflag = false;
	for (int z = 0; z < Desc.nbColumns; z++)
	{
		//tessflag = z % 2 == 0 ? false : true;
		for (int x = 0; x < Desc.nbRows; x++, i++)
		{
			physx::PxI16 height;
			//height = physx::PxI16(quantization * ((float)(m_pHeightMapImage->GetRawImagePixel(x, z) - minHeight) / deltaHeight));
			//height = (int)(((float)(m_pHeightMapImage->GetRawImagePixel(x, z) - minHeight) / deltaHeight)*100);
			height = physx::PxI16((m_pHeightMapImage->GetRawImagePixel(x, z)));

			physx::PxHeightFieldSample& smp = hfSamples[(x * Desc.nbColumns) + z];
			smp.height = height;
			if (tessflag)
				smp.setTessFlag();
			//buffer[i] = physx::PxReal(GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z));
			
		}
	}

	Desc.samples.data = hfSamples;
	Desc.samples.stride = sizeof(physx::PxHeightFieldSample);


	physx::PxHeightFieldGeometry HeightGeometry;
	HeightGeometry.rowScale = m_xmf3Scale.x;
	HeightGeometry.columnScale = m_xmf3Scale.z;
	HeightGeometry.heightScale = m_xmf3Scale.y;
	HeightGeometry.heightField = PxCreateHeightField(Desc);

	delete[] hfSamples;

	/*XMFLOAT4X4 matrix = Matrix4x4::Identity();
	matrix._41 = -800.0f;
	matrix._42 = -300.0f;
	matrix._43 = -800.0f;*/
	/*XMFLOAT4X4 directxToPhysx; XMStoreFloat4x4(&directxToPhysx, XMMatrixScaling(1.0f, 1.0f, -1.0f) * XMMatrixRotationX(XMConvertToRadians(90.0f)));
	XMFLOAT3 _pos{-800.0f, -600.0f, -800.0f - m_nLength * m_xmf3Scale.z };
	_pos = Vector3::TransformCoord(_pos, directxToPhysx);*/

	//physx::PxVec3 pos = convertToPhysXCoordSystem(matrix).column3.getXYZ();

	physx::PxTransform transform(physx::PxVec3(86.4804, -46.8876 - 46.8876 * 0.38819 + 6.5f, -183.7856));

	physx::PxMaterial* material = pPhysics->createMaterial(0.7, 0.5, 0.0);
	physx::PxRigidStatic* plane = pPhysics->createRigidStatic(transform);


	physx::PxShape* planeShape = pPhysics->createShape(HeightGeometry, *material);
	/*planeShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	planeShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);*/
	plane->attachShape(*planeShape);


	Rigid = plane;

	Locator.GetPxScene()->addActor(*Rigid);
}

CTerrainInputStream::CTerrainInputStream(physx::PxHeightFieldDesc& Desc, BYTE* bytes)
{
	HeightFiledDesc = Desc;
	m_pRawImagePixels = bytes;
}

CTerrainInputStream::~CTerrainInputStream()
{
}

uint32_t CTerrainInputStream::read(void* dest, uint32_t count)
{
	uint32_t cnt = count;
	if (HeightFiledDesc.nbColumns * HeightFiledDesc.nbRows <= count)
		cnt = HeightFiledDesc.nbColumns * HeightFiledDesc.nbRows - 1;

	memcpy(dest, m_pRawImagePixels, cnt);

	return cnt;
}
