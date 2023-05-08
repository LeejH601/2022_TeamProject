#include "SwordTrailObject.h"
#include "Texture.h"
#include "..\Shader\Shader.h"
#include "..\Shader\SwordTrailShader.h"

CSwordTrailObject::CSwordTrailObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pTrailShader)
{
	ZeroMemory(&m_xmf4x4SwordTrailControllPointers, sizeof(XMFLOAT4X4));
	m_xmf4TrailControllPoints1.resize(MAX_TRAILCONTROLLPOINTS);
	m_xmf4TrailControllPoints2.resize(MAX_TRAILCONTROLLPOINTS);

	m_xmf4TrailBasePoints1.resize(14);
	m_xmf4TrailBasePoints2.resize(14);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	std::shared_ptr<CSwordTrailShader> pSwordTrailShader = std::make_shared<CSwordTrailShader>();
	pSwordTrailShader->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
	pSwordTrailShader->BuildObjects(pd3dDevice, pd3dCommandList);

	pSwordTrailShader->CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, 6, 0);

	std::shared_ptr<CTexture> pTexture = std::make_shared<CTexture>(2, RESOURCE_TEXTURE2D, 0, 1);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/T_Sword_Slash_11.dds", RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/VAP1_Noise_4.dds", RESOURCE_TEXTURE2D, 1);

	pSwordTrailShader->CreateShaderResourceViews(pd3dDevice, pTexture.get(), 0, 2);

	SetShader(pSwordTrailShader, pTexture);

	m_fR_CurvePoints[0] = 0.0f; m_fR_CurvePoints[1] = 0.14; m_fR_CurvePoints[2] = 0.459;  m_fR_CurvePoints[3] = 1.892;
	m_fG_CurvePoints[0] = 0.0f; m_fG_CurvePoints[1] = 0.005; m_fG_CurvePoints[2] = 0.067;  m_fG_CurvePoints[3] = 0.595;
	m_fB_CurvePoints[0] = 0.0f; m_fB_CurvePoints[1] = 0.257; m_fB_CurvePoints[2] = 0.26;  m_fB_CurvePoints[3] = 0.0f;
	m_fColorCurveTimes[0] = 0.0f; m_fColorCurveTimes[1] = 0.3; m_fColorCurveTimes[2] = 0.6;  m_fColorCurveTimes[3] = 1.0;
	m_nCurves = 4;
	m_fNoiseConstants = 1.4f;

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

	memcpy(m_pcbMappedTrail->m_fColorCurveTimes, m_fColorCurveTimes, sizeof(m_fColorCurveTimes));
	memcpy(m_pcbMappedTrail->m_fR_CurvePoints, m_fR_CurvePoints, sizeof(m_fR_CurvePoints));
	memcpy(m_pcbMappedTrail->m_fG_CurvePoints, m_fG_CurvePoints, sizeof(m_fG_CurvePoints));
	memcpy(m_pcbMappedTrail->m_fB_CurvePoints, m_fB_CurvePoints, sizeof(m_fB_CurvePoints));
	m_pcbMappedTrail->m_nCurves = m_nCurves;
	m_pcbMappedTrail->m_fNoiseConstants = m_fNoiseConstants;

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
	m_faccumulateTime += fTimeElapsed * 1.0f;
}

void CSwordTrailObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (m_nDrawedControllPoints < 2)
		return;

	if (m_ppMaterials[0])
	{
		if (m_ppMaterials[0]->m_pShader)
			m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, nullptr, 0);
		m_ppMaterials[0]->UpdateShaderVariables(pd3dCommandList);
	}
	UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	pd3dCommandList->DrawInstanced(m_nDrawedControllPoints - 1, 1, 0, 0);
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

	{
		float nTrailTotalLength1 = 0;
		std::vector<float> fSectorLengths1; fSectorLengths1.resize(MAX_TRAILCONTROLLPOINTS / 10);
		float nTrailTotalLength2 = 0;
		std::vector<float> fSectorLengths2; fSectorLengths2.resize(MAX_TRAILCONTROLLPOINTS / 10);
		for (int i = 1; i < m_nTrailBasePoints + 1; ++i) {
			fSectorLengths1[i - 1] = XMVector4Length(XMVectorSubtract(XMLoadFloat4(&m_xmf4TrailBasePoints1[i + 1]), XMLoadFloat4(&m_xmf4TrailBasePoints1[i]))).m128_f32[0];
			nTrailTotalLength1 += fSectorLengths1[i - 1];
			fSectorLengths2[i - 1] = XMVector4Length(XMVectorSubtract(XMLoadFloat4(&m_xmf4TrailBasePoints2[i + 1]), XMLoadFloat4(&m_xmf4TrailBasePoints2[i]))).m128_f32[0];
			nTrailTotalLength2 += fSectorLengths2[i - 1];
		}
		float gapOne = nTrailTotalLength1 / (MAX_TRAILCONTROLLPOINTS / 10 * m_nTrailBasePoints);
		float gapTwo = nTrailTotalLength2 / (MAX_TRAILCONTROLLPOINTS / 10 * m_nTrailBasePoints);

		int index = 0;
		float gapT1 = 0, gapT2 = 0;
		for (int i = 0; i < m_nTrailBasePoints * MAX_TRAILCONTROLLPOINTS / 10; ++i) {
			int SampleIndex1 = 0;
			float gapAcculater = gapT1;
			int SampleIndex2 = 0;
			for (int j = 0; j < m_nTrailBasePoints; ++j) {
				if (gapAcculater - fSectorLengths1[j] < 0) {
					SampleIndex1 = j;
					break;
				}
				gapAcculater -= fSectorLengths1[j];
			}

			float t = gapAcculater / fSectorLengths1[SampleIndex1];
			XMVECTOR points[4];
			points[0] = XMLoadFloat4(&m_xmf4TrailBasePoints1[SampleIndex1]);
			points[1] = XMLoadFloat4(&m_xmf4TrailBasePoints1[SampleIndex1 + 1]);
			points[2] = XMLoadFloat4(&m_xmf4TrailBasePoints1[SampleIndex1 + 2]);
			points[3] = XMLoadFloat4(&m_xmf4TrailBasePoints1[SampleIndex1 + 3]);
			XMStoreFloat4(&m_xmf4TrailControllPoints1[index], XMVectorCatmullRom(points[0], points[1], points[2], points[3], t));

			gapAcculater = gapT2;
			for (int j = 0; j < m_nTrailBasePoints; ++j) {
				if (gapAcculater - fSectorLengths2[j] < 0) {
					SampleIndex2 = j;
					break;
				}
				gapAcculater -= fSectorLengths2[j];
			}

			t = gapAcculater / fSectorLengths2[SampleIndex2];
			points[0] = XMLoadFloat4(&m_xmf4TrailBasePoints2[SampleIndex2]);
			points[1] = XMLoadFloat4(&m_xmf4TrailBasePoints2[SampleIndex2 + 1]);
			points[2] = XMLoadFloat4(&m_xmf4TrailBasePoints2[SampleIndex2 + 2]);
			points[3] = XMLoadFloat4(&m_xmf4TrailBasePoints2[SampleIndex2 + 3]);

			XMStoreFloat4(&m_xmf4TrailControllPoints2[index], XMVectorCatmullRom(points[0], points[1], points[2], points[3], t));

			gapT1 += gapOne;
			gapT2 += gapTwo;
			index++;
		}
		m_nDrawedControllPoints = index;
	}


	/*int index = 0;
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
	}*/

	//m_nDrawedControllPoints = index;
}
