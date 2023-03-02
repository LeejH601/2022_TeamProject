#include "../Object/Mesh.h"
#include "../Object/Texture.h"
#include "ParticleObject.h"
#include "../Shader/ParticleShader.h"

CParticleObject::CParticleObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, bool bEnable) : m_bEnable(bEnable)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	std::shared_ptr<CParticleMesh> pParticleMesh = std::make_shared<CParticleMesh>(pd3dDevice, pd3dCommandList, xmf3Position, xmf3Velocity, fLifetime, xmf3Acceleration, xmf3Color, xmf2Size, nMaxParticles);
	SetMesh(pParticleMesh);

	SetTexture(pSpriteTexture);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	pShader->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
}

CParticleObject::~CParticleObject()
{
}

void CParticleObject::ReleaseUploadBuffers()
{
}

void CParticleObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_FRAMEWORK_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbFrameworkInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	m_pd3dcbFrameworkInfo->Map(0, NULL, (void**)&m_pcbMappedFrameworkInfo);
}

void CParticleObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime)
{
	m_pcbMappedFrameworkInfo->m_fCurrentTime = fCurrentTime;
	m_pcbMappedFrameworkInfo->m_fElapsedTime = fElapsedTime;
	m_pcbMappedFrameworkInfo->m_fSecondsPerFirework = 0.4f;
	m_pcbMappedFrameworkInfo->m_nFlareParticlesToEmit = m_iEmitParticleN;
	m_pcbMappedFrameworkInfo->m_xmf3Gravity = XMFLOAT3(0.0f, 0.f, 0.0f);
	m_pcbMappedFrameworkInfo->m_nMaxFlareType2Particles = 15 * 1.5f;
	m_pcbMappedFrameworkInfo->m_xmf3Color = m_f3Color;
	m_pcbMappedFrameworkInfo->m_nParticleType = PARTICLE_TYPE_EMITTER;
	m_pcbMappedFrameworkInfo->m_fLifeTime = m_fLifeTime;
	m_pcbMappedFrameworkInfo->m_fSize = m_fSize;


	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbFrameworkInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dGpuVirtualAddress);
}

void CParticleObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}


void CParticleObject::PreRender(ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nPipelineState)
{
	if (!m_bEnable)
		return;
	UpdateShaderVariables(pd3dCommandList);
	pShader->OnPrepareRender(pd3dCommandList, nPipelineState);
	for (int i = 0; i < m_nMaterials; ++i)
	{
		if (m_ppMaterials[i])
		{
			if (m_ppMaterials[i]->m_pShader) m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, 0);
			m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
		}
	}
}

void CParticleObject::StreamOutRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	PreRender(pd3dCommandList, pShader, 0);
	if (m_pMesh) m_pMesh->PreRender(pd3dCommandList, 0); //Stream Output
	if (m_pMesh) m_pMesh->Render(pd3dCommandList, 0); //Stream Output
	if (m_pMesh) m_pMesh->PostRender(pd3dCommandList, 0); //Stream Output
}

void CParticleObject::DrawRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	PreRender(pd3dCommandList, pShader, 1);
	if (m_pMesh) m_pMesh->PreRender(pd3dCommandList, 1); //Draw
	if (m_pMesh) m_pMesh->Render(pd3dCommandList, 1); //Draw
}

void CParticleObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	if (!m_bEnable)
		return;
	StreamOutRender(pd3dCommandList, pCamera, pShader);
	DrawRender(pd3dCommandList, pCamera, pShader);
}

void CParticleObject::OnPostRender()
{
	if (m_pMesh) m_pMesh->OnPostRender(0); //Read Stream Output Buffer Filled Size
}

bool& CParticleObject::GetEnable()
{
	return m_bEnable;
}

void CParticleObject::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

void CParticleObject::SetSize(float fSize)
{
	m_fSize = fSize;
}

void CParticleObject::SetLifeTime(float fLifeTime)
{
	m_fLifeTime = fLifeTime;
}

void CParticleObject::SetAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;
	if (m_ppMaterials.data())
	{
		m_ppMaterials[0]->m_nType = (int)(fAlpha * 100);
	}
}

void CParticleObject::SetColor(XMFLOAT3 fColor)
{
	m_f3Color = fColor;
}

void CParticleObject::SetEmitParticleN(int iParticleN)
{
	m_iEmitParticleN = iParticleN;
}

void CParticleObject::SetMaxParticleN(int iMaxParticleN)
{
	static_cast<CParticleMesh*>(m_pMesh.get())->m_nMaxParticles = iMaxParticleN;
}
