#include "../Object/Texture.h"
#include "BillBoardObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"

CBillBoardObject::CBillBoardObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, float fSize)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetTexture(pSpriteTexture);
	SetMesh(std::make_shared<CBillBoardMesh>(pd3dDevice, pd3dCommandList, fSize));
}

CBillBoardObject::~CBillBoardObject()
{
}

void CBillBoardObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CBillBoardObject::Animate(float fTimeElapsed)
{
	if (m_bEnable) {
		CGameObject::Animate(fTimeElapsed);
		if (m_pTarget) {
			XMFLOAT3 position = m_pTarget->GetPosition();
			position.y += 10.f;
			SetPosition(position);
		}
	}
}

void CBillBoardObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if(m_bEnable)
		CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CBillBoardObject::Set_Target(CGameObject* pTarget)
{
	if (pTarget)
		m_pTarget = pTarget;
}

void CBillBoardObject::Set_Enable(bool bEnable)
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
	m_pcbMappedFrameworkInfo->m_fCurrentTime = fCurrentTime;
	m_pcbMappedFrameworkInfo->m_fElapsedTime = fElapsedTime;
	m_pcbMappedFrameworkInfo->m_fSpeed = 10.f;
	m_pcbMappedFrameworkInfo->m_nFlareParticlesToEmit = 100;
	m_pcbMappedFrameworkInfo->m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);
	m_pcbMappedFrameworkInfo->m_nMaxFlareType2Particles = 15 * 1.5f;
	m_pcbMappedFrameworkInfo->m_xmf3Color = XMFLOAT3(1.f, 0.f, 0.f);
	m_pcbMappedFrameworkInfo->m_nParticleType = ParticleType::PARTICLE_TYPE_END;
	m_pcbMappedFrameworkInfo->m_fLifeTime = m_fLifeTime;
	m_pcbMappedFrameworkInfo->m_fSize = m_fSize;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbFrameworkInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dGpuVirtualAddress);
}


CMultiSpriteObject::CMultiSpriteObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nRows, int nCols, float fSize, float fSpeed) : CBillBoardObject(pSpriteTexture, pd3dDevice, pd3dCommandList, pShader, fSize)
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
		m_xmf4x4World._11 = 1.0f / float(m_nRows);
		m_xmf4x4World._22 = 1.0f / float(m_nCols);
		m_xmf4x4World._31 = float(m_nRow) / float(m_nRows);
		m_xmf4x4World._32 = float(m_nCol) / float(m_nCols);
		if (fTimeElapsed <= 0.0f)
		{
			if ((m_nRow + 1) == m_nRows && (m_nCol + 1) == m_nCols) {
				m_bEnable = false;
				m_nRow = 0;
				m_nCol = 0;
			}
			else
			{
				if (++m_nCol == m_nCols) {
					m_nRow++;
					m_nCol = 0;
				}
				if (m_nRow == m_nRows)
					m_nRow = 0;
			}
		}
	}
	
}

void CMultiSpriteObject::SetEnable(bool bEnable)
{
	CBillBoardObject::Set_Enable(bEnable);
	m_nRow = 0;
	m_nCol = 0;
}

void CMultiSpriteObject::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CMultiSpriteObject::SetAlpah(float fAlpha)
{
	m_fAlpha = fAlpha;
	if (m_ppMaterials.data())
	{
		m_ppMaterials[0]->m_nType = (int)(fAlpha * 100); // 0.0f ~1.f -> 0 ~ 100 (uint로 전달하기 위해 변경, 이후 쉐이더에서 float로 변환)
	}
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
		AnimateRowColumn(m_fTime);

		if (m_fTime <= 0.f)
			m_fTime = 0.5f;
	}
}

