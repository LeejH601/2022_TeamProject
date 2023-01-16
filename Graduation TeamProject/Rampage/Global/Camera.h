#pragma once
#include "stdafx.h"

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4X4 m_xmf4x4InverseProjection;
};
class CCamera
{
protected:
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	float m_fPitch;
	float m_fRoll;
	float m_fYaw;

	XMFLOAT3 m_xmf3LookAtWorld;
	XMFLOAT3 m_xmf3Offset;
	float m_fTimeLag;

	//카메라 변환 행렬
	XMFLOAT4X4 m_xmf4x4View;
	//투영 변환 행렬
	XMFLOAT4X4 m_xmf4x4Projection;
	//뷰포트와 씨저 사각형
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	ComPtr<ID3D12Resource> m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO* m_pcbMappedCamera = NULL;
public:
	CCamera();
	virtual ~CCamera();

	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }

	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }
	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; m_xmf3Position.x += xmf3Offset.x; m_xmf3Position.y += xmf3Offset.y; m_xmf3Position.z += xmf3Offset.z; }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) { }
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt);
};

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera();
	virtual ~CFirstPersonCamera() { }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};