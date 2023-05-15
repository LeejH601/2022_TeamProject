#include "../Object/Mesh.h"
#include "../Object/Texture.h"
#include "ParticleObject.h"
#include "../Shader/ParticleShader.h"

CParticleObject::CParticleObject()
{
}

CParticleObject::CParticleObject(std::shared_ptr<CTexture> pSpriteTexture, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, int iParticleType)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	std::shared_ptr<CParticleMesh> pParticleMesh = std::make_shared<CParticleMesh>(pd3dDevice, pd3dCommandList, xmf3Position, xmf3Velocity, fLifetime, xmf3Acceleration, xmf3Color, xmf2Size, nMaxParticles);
	SetMesh(pParticleMesh);

	m_iParticleType = iParticleType;

	m_fFieldSpeed = 10.0f;
	m_fNoiseStrength = 1.0f;;
	m_xmf3FieldMainDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fProgressionRate = 10.0f;
	m_fLengthScale = 1.0f;
	
	SetTexture(pSpriteTexture);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CParticleObject::~CParticleObject()
{
}

void CParticleObject::ReleaseUploadBuffers()
{
}

void CParticleObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_FRAMEWORK_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbFrameworkInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	m_pd3dcbFrameworkInfo->Map(0, NULL, (void**)&m_pcbMappedFrameworkInfo);


	ncbElementBytes = ((sizeof(CB_CURLNOISE_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbCurlNoiseInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	m_pd3dcbCurlNoiseInfo->Map(0, NULL, (void**)&m_pcbMappedCurlNoiseInfo);
}

void CParticleObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, float fCurrentTime, float fElapsedTime)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
	m_pcbMappedFrameworkInfo->m_fCurrentTime = fCurrentTime;
	m_pcbMappedFrameworkInfo->m_fElapsedTime = fElapsedTime;

	m_pcbMappedFrameworkInfo->m_fSpeed = m_fSpeed;
	m_pcbMappedFrameworkInfo->m_nFlareParticlesToEmit = m_iEmitParticleN;
	m_pcbMappedFrameworkInfo->m_xmf3Gravity = m_xmf3Direction; // 임시로 방향
	m_pcbMappedFrameworkInfo->m_xmf3Color = m_f3Color;
	m_pcbMappedFrameworkInfo->m_nParticleType = m_iParticleType;
	m_pcbMappedFrameworkInfo->m_fLifeTime = m_fLifeTime;
	m_pcbMappedFrameworkInfo->m_fSize = m_fSize;
	m_pcbMappedFrameworkInfo->m_bEmitter = dynamic_cast<CParticleMesh*>(m_pMesh.get())->m_bEmit;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbFrameworkInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dGpuVirtualAddress);


	m_pcbMappedCurlNoiseInfo->Dt = fElapsedTime;
	m_pcbMappedCurlNoiseInfo->Time = fCurrentTime;
	m_pcbMappedCurlNoiseInfo->FieldSpeed = m_fFieldSpeed;
	m_pcbMappedCurlNoiseInfo->NoiseStrength = m_fNoiseStrength;
	m_pcbMappedCurlNoiseInfo->ProgressionRate = m_fProgressionRate;
	m_pcbMappedCurlNoiseInfo->LengthScale = m_fLengthScale;
	m_pcbMappedCurlNoiseInfo->FieldMainDirection = m_xmf3FieldMainDirection; // 해당 축 방향으로 전진성을 가지는 편. noiseStrength가 낮을 때 이 방향으로 파티클이 나아가는 것처럼 만들어짐.
	m_pcbMappedCurlNoiseInfo->SpherePosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pcbMappedCurlNoiseInfo->SphereRadius = 5.0f;

	d3dGpuVirtualAddress = m_pd3dcbCurlNoiseInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dGpuVirtualAddress);
}

void CParticleObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CParticleObject::PreRender(ID3D12GraphicsCommandList* pd3dCommandList, CShader* pShader, int nPipelineState)
{
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

void CParticleObject::Update(float fTimeElapsed)
{

}

void CParticleObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShader* pShader)
{
	StreamOutRender(pd3dCommandList, pCamera, pShader);
	DrawRender(pd3dCommandList, pCamera, pShader);
}

void CParticleObject::OnPostRender()
{
	if (m_pMesh)
	{
		m_nVertices = m_pMesh->OnPostRender(0); //Read Stream Output Buffer Filled Size
	}
}

void CParticleObject::SetSize(XMFLOAT2 fSize)
{
	m_fSize = fSize;
}

void CParticleObject::SetLifeTime(float fLifeTime)
{
	m_fLifeTime = fLifeTime;
}

void CParticleObject::SetStartAlpha(float fAlpha)
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
	static_cast<CParticleMesh*>(m_pMesh.get())->m_nMaxParticle = iMaxParticleN;
}

void CParticleObject::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

void CParticleObject::SetParticleType(int iParticleType)
{
	m_iParticleType = iParticleType;
}

int CParticleObject::GetParticleType()
{
	return m_iParticleType;
}

void CParticleObject::SetEmit(bool bEmit)
{
	dynamic_cast<CParticleMesh*>(m_pMesh.get())->m_bEmit = bEmit;
}

void CParticleObject::EmitParticle(int emitType)
{
	ParticleEmitDataParam param; 
	switch (emitType)
	{
	case 0:
		break;
	case 1:
		param.m_fLifeTime = m_fLifeTime;
		param.m_nEmitNum = m_iEmitParticleN;
		param.m_fEmitedSpeed = m_fSpeed;
		param.m_xmf3EmitedPosition.x = m_xmf4x4Transform._41;
		param.m_xmf3EmitedPosition.y = m_xmf4x4Transform._42 + 5.0f;
		param.m_xmf3EmitedPosition.z = m_xmf4x4Transform._43;
		break;
	default:
		break;
	}
	dynamic_cast<CParticleMesh*>(m_pMesh.get())->EmitParticle(emitType, param);
}

void CParticleObject::SetDirection(XMFLOAT3 xmf3Direction)
{
	m_xmf3Direction = xmf3Direction;
}

XMFLOAT3 CParticleObject::GetDirection()
{
	return m_xmf3Direction;
}

bool CParticleObject::CheckCapacity()
{
	int MaxParticle = static_cast<CParticleMesh*>(m_pMesh.get())->m_nMaxParticle;
	int Vertices = static_cast<CParticleMesh*>(m_pMesh.get())->GetVertices();
	if ((MaxParticle - Vertices) > 1000)
		return true;
	return false;
}

CSmokeParticleObject::CSmokeParticleObject(LPCTSTR pszFileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Velocity, float fLifetime, XMFLOAT3 xmf3Acceleration, XMFLOAT3 xmf3Color, XMFLOAT2 xmf2Size, UINT nMaxParticles, CParticleShader* pShader, int iParticleType) : CParticleObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	std::shared_ptr<CParticleMesh> pParticleMesh = std::make_shared<CParticleMesh>(pd3dDevice, pd3dCommandList, xmf3Position, xmf3Velocity, fLifetime, xmf3Acceleration, xmf3Color, xmf2Size, nMaxParticles);
	SetMesh(pParticleMesh);

	m_iParticleType = iParticleType;

	std::shared_ptr<CTexture> pParticleTexture = std::make_shared<CTexture>(3, RESOURCE_TEXTURE1D, 0, 1);
	pParticleTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, 0);

	srand((unsigned)time(NULL));

	XMFLOAT4* pxmf4RandomValues = new XMFLOAT4[1024];
	for (int i = 0; i < 1024; i++)
	{
		pxmf4RandomValues[i].x = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].y = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].z = float((rand() % 10000) - 5000) / 5000.0f;
		pxmf4RandomValues[i].w = float((rand() % 10000) - 5000) / 5000.0f;
	}

	pParticleTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 1024, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 1/*, RESOURCE_BUFFER*/);
	pParticleTexture->CreateBuffer(pd3dDevice, pd3dCommandList, pxmf4RandomValues, 256, sizeof(XMFLOAT4), DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, 2/*, RESOURCE_TEXTURE1D*/);

	pShader->CreateShaderResourceViews(pd3dDevice, pParticleTexture.get(), 0, 12);

	SetTexture(pParticleTexture);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CSmokeParticleObject::~CSmokeParticleObject()
{
}
