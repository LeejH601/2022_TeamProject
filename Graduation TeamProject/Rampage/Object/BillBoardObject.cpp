#include <math.h>
#include "../Object/Texture.h"
#include "BillBoardObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"


CBillBoardObject::CBillBoardObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize, bool bBillBoard)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetMesh(std::make_shared<CSpriteMesh>(pd3dDevice, pd3dCommandList, fSize, bBillBoard));
}

CBillBoardObject::~CBillBoardObject()
{
}

void CBillBoardObject::Animate(float fTimeElapsed)
{
	if (m_bEnable) {
		CGameObject::Animate(fTimeElapsed);
	}
}

void CBillBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (m_bEnable)
	{
		CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);

	}
}

void CBillBoardObject::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

bool& CBillBoardObject::GetEnable()
{
	return m_bEnable;
}

void CBillBoardObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_FRAMEWORK_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbFrameworkInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	m_pd3dcbFrameworkInfo->Map(0, NULL, (void**)&m_pcbMappedFrameworkInfo);
}

void CBillBoardObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);

	m_pcbMappedFrameworkInfo->m_fCurrentTime = fCurrentTime;
	m_pcbMappedFrameworkInfo->m_fElapsedTime = fElapsedTime;
	//m_pcbMappedFrameworkInfo->m_fSpeed = 10.f;
	//m_pcbMappedFrameworkInfo->m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);
	//m_pcbMappedFrameworkInfo->m_nMaxFlareType2Particles = 15 * 1.5f;
	m_pcbMappedFrameworkInfo->m_xmf3Color = m_xmf3Color;
	//m_pcbMappedFrameworkInfo->m_nParticleType = 0;
	//m_pcbMappedFrameworkInfo->m_fLifeTime = m_fLifeTime;
	m_pcbMappedFrameworkInfo->m_fSize = m_fSize;

	if (m_bStart)
		m_bStart = !m_bStart;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbFrameworkInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dGpuVirtualAddress);

	int m_nType = 100;
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_nType, 32); // 16

	m_xmf4x4Texture._11 = m_iTextureIndex;
	XMFLOAT4X4 xmfTexture;
	XMStoreFloat4x4(&xmfTexture, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Texture)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &m_xmf4x4Texture, 16);

}


CMultiSpriteObject::CMultiSpriteObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nRows, int nCols, float fSize, bool bBillBoard, float fSpeed) : CBillBoardObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize, bBillBoard)
{
	m_fSpeed = fSpeed;
}

CMultiSpriteObject::~CMultiSpriteObject()
{
}

void CMultiSpriteObject::AnimateRowColumn(float fTimeElapsed)
{
	if (fTimeElapsed >= 0.0f)
	{
		m_fAccumulatedTime += fTimeElapsed;

		float fraction = std::fmodf(m_fAccumulatedTime / m_fLifeTime, 1.0f);
		interval = 1.0f / (m_iTotalRow * m_iTotalCol);

		m_iCurrentCol = (int)(fraction / (interval * m_iTotalRow));
		float remainvalue = (fraction / (interval * m_iTotalRow)) - m_iCurrentCol;
		m_iCurrentRow = (int)(remainvalue * m_iTotalRow);

		if (m_fAccumulatedTime > m_fLifeTime)
			m_bEnable = false;
	}
	m_xmf4x4World._11 = 1.0f / float(m_iTotalRow);
	m_xmf4x4World._22 = 1.0f / float(m_iTotalCol);
	m_xmf4x4World._31 = float(m_iCurrentRow) / float(m_iTotalRow);
	m_xmf4x4World._32 = float(m_iCurrentCol) / float(m_iTotalCol);
}

void CMultiSpriteObject::SetEnable(bool bEnable)
{
	CBillBoardObject::SetEnable(bEnable);
	m_iCurrentRow = 0;
	m_iCurrentCol = 0;
	m_fAccumulatedTime = 0.0f;
}


void CMultiSpriteObject::SetTotalRowColumn(int iTotalRow, int iTotalColumn)
{
	m_iTotalRow = iTotalRow;
	m_iTotalCol = iTotalColumn;
}

void CMultiSpriteObject::SetColor(XMFLOAT3 fColor)
{
	m_xmf3Color = fColor;
}

void CMultiSpriteObject::SetSize(XMFLOAT2 fSize)
{
	m_fSize = fSize;
}

void CMultiSpriteObject::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CMultiSpriteObject::SetStartAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;
	if (m_ppMaterials.data())
	{
		m_ppMaterials[0]->m_nType = (int)(fAlpha * 100); // 0.0f ~1.f -> 0 ~ 100 (uint로 전달하기 위해 변경, 이후 쉐이더에서 float로 변환)
	}
}

void CMultiSpriteObject::SetLifeTime(float fLifeTime)
{
	m_fLifeTime = fLifeTime;
}

void CMultiSpriteObject::SetStart(bool bStart)
{
	m_bStart = bStart;
}

bool& CMultiSpriteObject::GetAnimation()
{
	return m_bAnimation;
}

void CMultiSpriteObject::Animate(float fTimeElapsed)
{
	if (m_bEnable) {
		CBillBoardObject::Animate(fTimeElapsed);
	}
}


void CMultiSpriteObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		// CGameObject의 정보를 넘길 버퍼가 있고, 해당 버퍼에 대한 CPU 포인터가 있으면 UpdateShaderVariables 함수를 호출한다.
		CGameObject::UpdateShaderVariables(pd3dCommandList);
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader)
					m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, 0);

				if (b_UseTexture)
					m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);

			}
		}

		// 여기서 메쉬의 렌더를 한다.
		m_pMesh->OnPreRender(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList, 0);
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}

CTerrainSpriteObject::CTerrainSpriteObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nRows, int nCols, float fSize, float fSpeed) : CMultiSpriteObject(iTextureIndex, pd3dDevice, pd3dCommandList, nRows, nCols, fSize, false, fSpeed)
{
}

CTerrainSpriteObject::~CTerrainSpriteObject()
{
}

void CTerrainSpriteObject::Animate(CHeightMapTerrain* pTerrain, float fTimeElapsed)
{
	if (!m_bEnable)
		return;

	m_fTime -= fTimeElapsed * m_fSpeed;

	if (m_fTime < 0.f)
		SetEnable(false);

	if (pTerrain)
	{
		XMFLOAT3 xmf3Scale = pTerrain->GetScale();
		XMFLOAT3 xmf3TerrainSpritePosition = GetPosition();
		int z = (int)(xmf3TerrainSpritePosition.z / xmf3Scale.z);
		bool bReverseQuad = ((z % 2) != 0);
		// 86.4804, -46.8876 - 46.8876 * 0.38819 + 6.5f, -183.7856

		float fHeight = pTerrain->GetHeight(xmf3TerrainSpritePosition.x - 86.4804f, xmf3TerrainSpritePosition.z + 183.7856f, bReverseQuad);
		XMFLOAT3 Pos = GetPosition();
		Pos.y = fHeight - 46.8876 - 46.8876 * 0.38819 + 6.5f;
		SetPosition(Pos);
	}
	switch (m_eTerrainSpriteType)
	{
	case TERRAINSPRITE_CROSS_FADE:
		if (m_fTime > m_fLifeTime * 0.7f)
		{
			if (m_fDeltaSize <= 1.f)
			{
				m_fDeltaSize += fTimeElapsed * 5.f;
			}
			Rotate(0.f, 0.f, 7.f);
		}
		break;
	case TERRAINSPRITE_FADE_IN:
		break;
	case TERRAINSPRITE_FADE_OUT:
		break;
	case TERRAINSPRITETYPE_END:
		break;
	default:
		break;
	}

}

void CTerrainSpriteObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);

	m_pcbMappedFrameworkInfo->m_fCurrentTime = fCurrentTime;
	m_pcbMappedFrameworkInfo->m_fElapsedTime = fElapsedTime;
	m_pcbMappedFrameworkInfo->m_fSpeed = 10.f;
	m_pcbMappedFrameworkInfo->m_nFlareParticlesToEmit = 100;
	m_pcbMappedFrameworkInfo->m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);
	m_pcbMappedFrameworkInfo->m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
	m_pcbMappedFrameworkInfo->m_nParticleType = 5;
	m_pcbMappedFrameworkInfo->m_fLifeTime = m_fLifeTime;
	m_pcbMappedFrameworkInfo->m_xmf3Color = m_xmf3Color;
	m_pcbMappedFrameworkInfo->m_fSize = Vector2::ScalarProduct(m_fSize, m_fDeltaSize, false);
	m_pcbMappedFrameworkInfo->m_iTextureCoord = XMUINT2(m_iTotalRow, m_iTotalCol);
	// 중간 정도 일때
	if (m_bStart)
		m_bStart = !m_bStart;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbFrameworkInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dGpuVirtualAddress);

	if (m_ppMaterials.data())
	{
		m_ppMaterials[0]->m_nType = (int)((m_fTime / m_fLifeTime) * 100); // 0.0f ~1.f -> 0 ~ 100 (uint로 전달하기 위해 변경, 이후 쉐이더에서 float로 변환)
	}
}

void CTerrainSpriteObject::SetEnable(bool bEnable)
{
	if ((!m_bEnable) && bEnable)
	{
		m_fTime = m_fLifeTime;
		m_fDeltaSize = 0.f;
	}
	m_bEnable = bEnable;

}

void CTerrainSpriteObject::SetType(TerrainSpriteType eType)
{
	m_eTerrainSpriteType = eType;
}

CDetailObject::CDetailObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr<CShader> pShader, void* pContext)
{
	/*std::shared_ptr<CTexture> pDetailMap = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);

	pDetailMap->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/DetailMap1.dds", 1, 1);*/
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(-1, 1);

	std::shared_ptr<CTexture> GrassTexture = std::make_shared<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	GrassTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/BillBoardImages/Grass_02.dds", RESOURCE_TEXTURE2D, 0);

	char* buf = new char[1024 * 1024];
	int* data = new int[1024 * 1024];
	std::ifstream in{ "Terrain/DetailMap1.bin", std::ios_base::binary };

	CSplatTerrain* pTerrain = (CSplatTerrain*)pContext;
	int terrainWidth = pTerrain->GetWidth();
	int terrainLength = pTerrain->GetLength();

	nDetails = 0;
	m_xmf2Size = XMFLOAT2(2, 2);
	m_xmf2Offset = XMFLOAT2(m_xmf2Size.y / 2.0f, 0.0f);

	in.read((char*)data, sizeof(int) * 1024 * 1024);
	for (int i = 0; i < 1024 * 1024; ++i) {
		//data[i] = atoi(&buf[i]);
		nDetails += data[i];
	}

	m_xmf3DetailPositions.resize(nDetails);
	m_xmf3DetailSizesAndWindOffset.resize(nDetails);
	m_xmf3DetailColors.resize(nDetails);
	m_xmf3DetailNormals.resize(nDetails);
	XMFLOAT3 color = XMFLOAT3(70.f / 255, 82.f / 255, 69.f / 255);
	int index = 0;
	for (int i = 0; i < 1024 * 1024; ++i) {
		if (data[i] > 0) {
			for (int j = 0; j < data[i]; ++j) {
				XMFLOAT3 pos;
				pos.z = i / 1024;
				pos.x = i % 1024;
				pos.z = pos.z * float(terrainWidth) / 1024;
				pos.x = pos.x * float(terrainLength) / 1024;

				pos.y = pTerrain->GetHeight(pos.x, pos.z);

				float randomValue = urd(dre) * 0.1f;
				m_xmf3DetailColors[index] = Vector3::Add(color, XMFLOAT3(randomValue, randomValue, randomValue));
				m_xmf3DetailPositions[index] = pos;
				m_xmf3DetailNormals[index] = Vector3::Normalize(XMFLOAT3(urd(dre), 0.0f, urd(dre)));
				m_xmf3DetailSizesAndWindOffset[index++] = XMFLOAT3( m_xmf2Size.x, m_xmf2Size.y, urd(dre) * 1.5f);
			}
		}
	}

	m_pd3dVertexBufferViews.resize(4);
	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_xmf3DetailPositions.data(), sizeof(XMFLOAT3) * nDetails, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_pd3dSizeBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_xmf3DetailSizesAndWindOffset.data(), sizeof(XMFLOAT3) * nDetails, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dSizeUploadBuffer);
	m_pd3dColorBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_xmf3DetailColors.data(), sizeof(XMFLOAT3) * nDetails, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dColorUploadBuffer);
	m_pd3dNormalBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_xmf3DetailNormals.data(), sizeof(XMFLOAT3) * nDetails, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * nDetails;

	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dSizeBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * nDetails;

	m_pd3dVertexBufferViews[2].BufferLocation = m_pd3dColorBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT3) * nDetails;

	m_pd3dVertexBufferViews[3].BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[3].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[3].SizeInBytes = sizeof(XMFLOAT3) * nDetails;

	pShader->CreateShaderResourceViews(pd3dDevice, GrassTexture.get(), 0, 2);
	SetShader(pShader, GrassTexture);
}

void CDetailObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_DETAIL_INFO) + 255) & ~255); //256의 배수
	int nDrawInstances = m_xmf3DetailPositions.size();

	m_ppd3dcbDetailInfo.resize(ceil(nDrawInstances / MAX_DETAILS_INSTANCES_ONE_DRAW_CALL) + 1);
	m_pcbMappedDetailInfo.resize(ceil(nDrawInstances / MAX_DETAILS_INSTANCES_ONE_DRAW_CALL) + 1);
	for (int i = 0; i < m_ppd3dcbDetailInfo.size(); ++i) {
		m_ppd3dcbDetailInfo[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

		m_ppd3dcbDetailInfo[i]->Map(0, NULL, (void**)&m_pcbMappedDetailInfo[i]);
	}


}

void CDetailObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime)
{
	memcpy(m_pcbMappedDetailInfo[nDrawSetIndex]->m_xmf3WorldPositions, m_xmf3DetailPositions.data() + nDrawInstanceOffset, nDrawInstanceRange * sizeof(XMFLOAT3));
	XMFLOAT4 xmf4SizeOffset = XMFLOAT4(m_xmf2Size.x, m_xmf2Size.y, m_xmf2Offset.x, m_xmf2Offset.y);
	m_pcbMappedDetailInfo[nDrawSetIndex]->m_xmf4SizeOffset = xmf4SizeOffset;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_ppd3dcbDetailInfo[nDrawSetIndex]->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dGpuVirtualAddress);
}

void CDetailObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, (int)b_UseTexture);
	m_ppMaterials[0]->UpdateShaderVariables(pd3dCommandList);
	m_ppMaterials[0]->m_pShader->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->IASetVertexBuffers(0, m_pd3dVertexBufferViews.size(), m_pd3dVertexBufferViews.data());

	int nDrawInstances = m_xmf3DetailPositions.size();

	pd3dCommandList->DrawInstanced(nDetails, 1, 0, 0);


	/*nDrawInstanceOffset = 0;
	nDrawInstanceRange = 0;
	nDrawSetIndex = 0;
	while (nDrawInstances > 0)
	{
		nDrawInstanceRange = (MAX_DETAILS_INSTANCES_ONE_DRAW_CALL <= nDrawInstances) ? MAX_DETAILS_INSTANCES_ONE_DRAW_CALL : nDrawInstances;
		UpdateShaderVariables(pd3dCommandList, 0, 0);

		pd3dCommandList->DrawInstanced(1, nDrawInstanceRange, 0, 0);
		nDrawInstances -= nDrawInstanceRange;
		nDrawInstanceOffset += nDrawInstanceRange;
		nDrawSetIndex++;
	}*/
}
