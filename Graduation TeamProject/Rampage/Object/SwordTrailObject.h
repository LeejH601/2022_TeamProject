#pragma once
#include "Object.h"

#define MAX_TRAILCONTROLLPOINTS 100

struct VS_CB_SWTRAIL_INFO
{
	XMFLOAT4 m_xmf4TrailControllPoints1[MAX_TRAILCONTROLLPOINTS];
	XMFLOAT4 m_xmf4TrailControllPoints2[MAX_TRAILCONTROLLPOINTS];
	UINT m_nDrawedControllPoints;
	float m_faccumulateTime;
};

class CSwordTrailObject : public CGameObject
{
	XMFLOAT4X4 m_xmf4x4SwordTrailControllPointers;
	std::vector<XMFLOAT4> m_xmf4TrailControllPoints1;
	std::vector<XMFLOAT4> m_xmf4TrailControllPoints2;
	UINT m_nDrawedControllPoints;

	std::vector<XMFLOAT4> m_xmf4TrailBasePoints1;
	std::vector<XMFLOAT4> m_xmf4TrailBasePoints2;
	UINT m_nTrailBasePoints = 0;

	ComPtr<ID3D12Resource> m_pd3dcbTrail = NULL;
	VS_CB_SWTRAIL_INFO* m_pcbMappedTrail = NULL;

	std::unique_ptr<CTexture> m_pTexture;

public:
	float m_faccumulateTime;
	bool m_bIsUpdateTrailVariables = false;

public:
	CSwordTrailObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pTrailShader);
	virtual ~CSwordTrailObject();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

	void SetNextControllPoint(XMFLOAT4* point1, XMFLOAT4* point2);
};

class CSwordTrailObjectEmitter : public CGameObject {
	std::unique_ptr<CShader> m_pShader;
	std::vector<std::unique_ptr<CSwordTrailObject>> m_pObjects;

public:
	CSwordTrailObjectEmitter();
	virtual ~CSwordTrailObjectEmitter();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
};