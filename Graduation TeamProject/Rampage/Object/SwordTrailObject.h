#pragma once
#include "Object.h"

#define MAX_TRAILCONTROLLPOINTS 200
#define MAX_COLORCURVES 8

struct VS_CB_SWTRAIL_INFO
{
	XMFLOAT4 m_xmf4TrailControllPoints1[MAX_TRAILCONTROLLPOINTS];
	XMFLOAT4 m_xmf4TrailControllPoints2[MAX_TRAILCONTROLLPOINTS];
	float m_fR_CurvePoints[MAX_COLORCURVES];
	float m_fG_CurvePoints[MAX_COLORCURVES];
	float m_fB_CurvePoints[MAX_COLORCURVES];
	float m_fColorCurveTimes[MAX_COLORCURVES];
	UINT m_nCurves;
	float m_fNoiseConstants;
	UINT m_nDrawedControllPoints;
	float m_faccumulateTime;
	float m_fEmissiveFactor;
	UINT gnBaseTextureIndex;
	UINT gnNoiseTextureIndex;
	float gnScale;
};
enum class TRAIL_UPDATE_METHOD {
	UPDATE_NEW_CONTROL_POINT,
	NON_UPDATE_NEW_CONTROL_POINT,
	DELETE_CONTROL_POINT,
	COUNT
};
class TrailComponent;
class CSwordTrailObject : public CGameObject
{
	friend class TrailComponent;
	XMFLOAT4X4 m_xmf4x4SwordTrailControllPointers;
	std::vector<XMFLOAT4> m_xmf4TrailControllPoints1;
	std::vector<XMFLOAT4> m_xmf4TrailControllPoints2;
	UINT m_nDrawedControllPoints;

	std::vector<XMFLOAT4> m_xmf4TrailBasePoints1;
	std::vector<XMFLOAT4> m_xmf4TrailBasePoints2;
	UINT m_nTrailBasePoints = 0;

	ComPtr<ID3D12Resource> m_pd3dcbTrail = NULL;
	VS_CB_SWTRAIL_INFO* m_pcbMappedTrail = NULL;

	int m_iNoiseTextureIndex;

	float m_fR_CurvePoints[MAX_COLORCURVES];
	float m_fG_CurvePoints[MAX_COLORCURVES];
	float m_fB_CurvePoints[MAX_COLORCURVES];
	float m_fColorCurveTimes[MAX_COLORCURVES];
	UINT m_nCurves;
	float m_fNoiseConstants;
	float m_fEmissiveFactor;

	float m_fLengthWeight = 1.0f;
	XMFLOAT4 m_xmf4Offset = XMFLOAT4(0, 0, 0, 0);

public:
	float m_faccumulateTime;
	TRAIL_UPDATE_METHOD m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;

public:
	CSwordTrailObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CShader* pTrailShader);
	virtual ~CSwordTrailObject();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

	void SetNextControllPoint(XMFLOAT4* point1, XMFLOAT4* point2);
	void SetNoiseTextureIndex(int index) { m_iNoiseTextureIndex = index; };
	void SetLengthWeight(float value) { m_fLengthWeight = value; };
	float GetLengthWeight() { return m_fLengthWeight; };
	void SetOffset(XMFLOAT4 offset) { m_xmf4Offset = offset; };
};

class CSwordTrailObjectEmitter : public CGameObject {
	std::unique_ptr<CShader> m_pShader;
	std::vector<std::unique_ptr<CSwordTrailObject>> m_pObjects;

public:
	CSwordTrailObjectEmitter();
	virtual ~CSwordTrailObjectEmitter();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
};