#include "SwordTrailObject.h"
#include "Texture.h"
#include "..\Shader\Shader.h"

CSwordTrailObject::CSwordTrailObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pTrailShader)
{
	ZeroMemory(&m_xmf4x4SwordTrailControllPointers, sizeof(XMFLOAT4X4));
	m_xmf4TrailControllPoints1.resize(MAX_TRAILCONTROLLPOINTS);
	m_xmf4TrailControllPoints2.resize(MAX_TRAILCONTROLLPOINTS);

	m_xmf4TrailBasePoints1.resize(14);
	m_xmf4TrailBasePoints2.resize(14);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTrailShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 6, 0);

	std::shared_ptr<CTexture> pTexture = std::make_shared<CTexture>(2, RESOURCE_TEXTURE2D, 0, 1);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/T_Sword_Slash_21.dds", RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/VAP1_Noise_14.dds", RESOURCE_TEXTURE2D, 1);

	pTrailShader->CreateShaderResourceViews(pd3dDevice, pTexture.get(), 0, 2);

	SetTexture(pTexture);
	m_nTrailBasePoints = 0;
}

CSwordTrailObject::~CSwordTrailObject()
{
}

void CSwordTrailObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_SWTRAIL_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbTrail = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pcbMappedTrail, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbTrail->Map(0, NULL, (void**)&m_pcbMappedTrail);


	//CreateShaderResourceViews(pd3dDevice, m_pTexture.get(), 0, 2);
}

void CSwordTrailObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{

	memcpy(m_pcbMappedTrail->m_xmf4TrailControllPoints1, m_xmf4TrailControllPoints1.data(), sizeof(m_pcbMappedTrail->m_xmf4TrailControllPoints1));
	memcpy(m_pcbMappedTrail->m_xmf4TrailControllPoints2, m_xmf4TrailControllPoints2.data(), sizeof(m_pcbMappedTrail->m_xmf4TrailControllPoints2));
	m_pcbMappedTrail->m_nDrawedControllPoints = m_nDrawedControllPoints;
	m_pcbMappedTrail->m_faccumulateTime = m_faccumulateTime;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbTrail->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(7, d3dGpuVirtualAddress);
}

void CSwordTrailObject::ReleaseShaderVariables()
{
	if (m_pd3dcbTrail)
		m_pd3dcbTrail->Unmap(0, NULL);
}

void CSwordTrailObject::Update(float fTimeElapsed)
{
	m_faccumulateTime += fTimeElapsed * 3.0f;
}

void CSwordTrailObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (m_nDrawedControllPoints < 2)
		return;

	if (m_ppMaterials[0])
	{
		if (m_ppMaterials[0]->m_pShader)
			m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, 0);
		m_ppMaterials[0]->UpdateShaderVariables(pd3dCommandList);
	}
	UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(m_nDrawedControllPoints-1, 1, 0, 0);
}

void CSwordTrailObject::SetNextControllPoint(XMFLOAT4* point1, XMFLOAT4* point2)
{
	/*memcpy(&m_xmf4x4SwordTrailControllPointers._31, point2, sizeof(XMFLOAT4));
	memcpy(&m_xmf4x4SwordTrailControllPointers._41, point1, sizeof(XMFLOAT4));*/
	int dummy = 1;

	if (point1 != nullptr && point2 != nullptr) {
		if (m_nTrailBasePoints < 10) {
			m_xmf4TrailBasePoints1[dummy + m_nTrailBasePoints] = *point1;
			m_xmf4TrailBasePoints2[dummy + m_nTrailBasePoints++] = *point2;
		}
		else {
			memcpy(m_xmf4TrailBasePoints1.data() + dummy, m_xmf4TrailBasePoints1.data() + dummy + 1, (m_nTrailBasePoints - 1) * sizeof(XMFLOAT4));
			memcpy(m_xmf4TrailBasePoints2.data() + dummy, m_xmf4TrailBasePoints2.data() + dummy + 1, (m_nTrailBasePoints - 1) * sizeof(XMFLOAT4));

			m_xmf4TrailBasePoints1[dummy + m_nTrailBasePoints - 1] = *point1;
			m_xmf4TrailBasePoints2[dummy + m_nTrailBasePoints - 1] = *point2;
		}
	}
	else {
		if (m_nTrailBasePoints > 0) {
			memcpy(m_xmf4TrailBasePoints1.data() + dummy, m_xmf4TrailBasePoints1.data() + dummy + 1, (m_nTrailBasePoints - 1) * sizeof(XMFLOAT4));
			memcpy(m_xmf4TrailBasePoints2.data() + dummy, m_xmf4TrailBasePoints2.data() + dummy + 1, (m_nTrailBasePoints - 1) * sizeof(XMFLOAT4));
			m_nTrailBasePoints--;
		}


	}

	if (m_nTrailBasePoints > 0) {
		m_xmf4TrailBasePoints1[0] = m_xmf4TrailBasePoints1[1];
		m_xmf4TrailBasePoints2[0] = m_xmf4TrailBasePoints2[1];

		for (int i = 1; i <= 3; ++i) {
			m_xmf4TrailBasePoints1[m_nTrailBasePoints + i] = m_xmf4TrailBasePoints1[m_nTrailBasePoints];
			m_xmf4TrailBasePoints2[m_nTrailBasePoints + i] = m_xmf4TrailBasePoints2[m_nTrailBasePoints];
		}
	}


	int index = 0;
	for (int i = 0; i < m_nTrailBasePoints; ++i) {
		m_xmf4TrailControllPoints1[index] = m_xmf4TrailBasePoints1[i + 1];
		m_xmf4TrailControllPoints2[index++] = m_xmf4TrailBasePoints2[i + 1];
		for (int j = 1; j < 10; ++j) {
			float t = j * 0.10f;
			XMVECTOR points[4];
			points[0] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i]);
			points[1] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i + 1]);
			points[2] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i + 2]);
			points[3] = XMLoadFloat4(&m_xmf4TrailBasePoints1[i + 3]);

			XMStoreFloat4(&m_xmf4TrailControllPoints1[index], XMVectorCatmullRom(points[0], points[1], points[2], points[3], t));

			points[0] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i]);
			points[1] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i + 1]);
			points[2] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i + 2]);
			points[3] = XMLoadFloat4(&m_xmf4TrailBasePoints2[i + 3]);

			XMStoreFloat4(&m_xmf4TrailControllPoints2[index++], XMVectorCatmullRom(points[0], points[1], points[2], points[3], t));
		}
	}

	m_nDrawedControllPoints = index;
}
