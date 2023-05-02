#include <math.h>
#include "../Object/Texture.h"
#include "BillBoardObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"


CBillBoardObject::CBillBoardObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, float fSize, bool bBillBoard)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetTexture(pSpriteTexture);
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
}


CMultiSpriteObject::CMultiSpriteObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nRows, int nCols, float fSize, bool bBillBoard, float fSpeed) : CBillBoardObject(pSpriteTexture, pd3dDevice, pd3dCommandList, pShader, fSize, bBillBoard)
{
	m_fSpeed = fSpeed;
}

CMultiSpriteObject::~CMultiSpriteObject()
{
}

void CMultiSpriteObject::AnimateRowColumn(float fTimeElapsed)
{
	if (!m_ppMaterials.empty() && m_ppMaterials[0]->GetTexture())
	{
		int m_nRows = m_ppMaterials[0]->GetTexture()->GetRow();
		int m_nCols = m_ppMaterials[0]->GetTexture()->GetColumn();
		
		if (fTimeElapsed >= 0.0f)
		{
			m_fAccumulatedTime += fTimeElapsed;

			float fraction = std::fmodf(m_fAccumulatedTime/ m_fLifeTime, 1.0f);
			interval = 1.0f / (m_nRows * m_nCols);

			m_nCol = (int)(fraction / (interval * m_nRows));
			float remainvalue = (fraction / (interval * m_nRows)) - m_nCol;
			m_nRow = (int)(remainvalue * m_nRows);

			if(m_fAccumulatedTime > m_fLifeTime)
				m_bEnable = false;
		}
		m_xmf4x4World._11 = 1.0f / float(m_nRows);
		m_xmf4x4World._22 = 1.0f / float(m_nCols);
		m_xmf4x4World._31 = float(m_nRow) / float(m_nRows);
		m_xmf4x4World._32 = float(m_nCol) / float(m_nCols);
	}
	
}

void CMultiSpriteObject::SetEnable(bool bEnable)
{
	CBillBoardObject::SetEnable(bEnable);
	m_nRow = 0;
	m_nCol = 0;
	m_fAccumulatedTime = 0.0f;
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

		m_fTime -= fTimeElapsed * m_fSpeed;
		//AnimateRowColumn(m_fTime);
		AnimateRowColumn(fTimeElapsed);

		if (m_fTime <= 0.f)
			m_fTime = 0.5f;
	}
}

CTerrainSpriteObject::CTerrainSpriteObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nRows, int nCols, float fSize, float fSpeed) : CMultiSpriteObject(pSpriteTexture, pd3dDevice, pd3dCommandList, pShader, nRows, nCols, fSize, false, fSpeed)
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
	m_pcbMappedFrameworkInfo->m_nMaxFlareType2Particles = 15 * 1.5f;
	m_pcbMappedFrameworkInfo->m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
	m_pcbMappedFrameworkInfo->m_nParticleType = 0;
	m_pcbMappedFrameworkInfo->m_fLifeTime = m_fLifeTime;
	m_pcbMappedFrameworkInfo->m_fSize = Vector2::ScalarProduct(m_fSize, m_fDeltaSize, false);
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
