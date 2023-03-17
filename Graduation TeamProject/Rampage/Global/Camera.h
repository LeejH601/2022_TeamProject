#pragma once
#include "stdafx.h"
#include "Component.h"
#include "Entity.h"

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CCamera;
class CGameObject;

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4X4 m_xmf4x4InverseProjection;
	XMFLOAT4X4 m_xmf4x4InverseView;
	XMFLOAT3 m_xmf3CameraPosition;
	XMFLOAT3 m_xmf3CameraDir;
};

class CPath
{
public:
	CPath() {};
	~CPath() {};

	CPath(const CPath& path);
	CPath& operator=(const CPath& path);

	void Reset();

	bool m_bPathEnd = false;
	float m_ft = 0.0f;


	std::vector<XMFLOAT3> m_vMovingPaths;
	std::vector<XMFLOAT3> m_vZoomPaths;
	int m_iIndex = 0;

	float m_fDuration = 1.0f;
	float m_fMagnitude = 1.0f;

	float m_fMovingCurrDistance = 0.0f;
	float m_fMovingMaxDistance = 10.0f;

	XMFLOAT3 m_xmf3OriginOffset = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 m_xmf3Offset = XMFLOAT3(0.f, 0.f, 0.f);
};

class CCameraMover : public CComponent
{
	CCamera* m_pCamera = NULL;
	XMFLOAT3 m_xmf3Direction;
public:
	bool m_bMoveEnd = true;

	float m_fMaxDistance;
	float m_fCurrDistance;
	float m_fSpeed;
	float m_fMovingTime;
	
	float m_fBackSpeed;
	float m_fRollBackTime;

	XMFLOAT3 offset;

public:
	CCameraMover();
	virtual ~CCameraMover() {};

	void SetDirection(XMFLOAT3 Dir) { m_xmf3Direction = Dir; }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	virtual void Update(float fElapsedTime);
	virtual void Reset();
	virtual bool HandleMessage(const Telegram& msg);
};

class CCameraShaker : public CComponent
{
private:
	CCamera* m_pCamera = NULL;
	std::uniform_real_distribution<float> urd{ -1.0f, 1.0f };

public:
	float m_fDuration;
	float m_ft;
	bool m_bShakeEnd = true;
	float m_fMagnitude;

public:
	CCameraShaker();
	virtual ~CCameraShaker() {};

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	virtual void Update(float fElapsedTime);
	virtual void Reset();
	virtual bool HandleMessage(const Telegram& msg);
};

class CCameraZoomer : public CComponent
{
	CCamera* m_pCamera = NULL;

	XMFLOAT3 m_xmf3Direction;
public:
	bool m_bZoomEnd = true;
	bool m_bIsIN;

	float m_fMaxDistance;
	float m_fCurrDistance;
	float m_fSpeed;
	float m_fMovingTime;

	float m_fBackSpeed;
	float m_fRollBackTime;

	XMFLOAT3 offset;


public:
	CCameraZoomer();
	virtual ~CCameraZoomer();

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	virtual void Update(float fElapsedTime);
	virtual void Reset();
	virtual bool HandleMessage(const Telegram& msg);
};

class CCamera : public IEntity
{
public:
	bool m_bCameraShaking = false;
	bool m_bCameraMoving = false;
	bool m_bCameraZooming = false;

	XMFLOAT3 m_xmf3CalculatedPosition;

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
	std::vector<CComponent*> m_vComponentSet;
	BoundingFrustum m_bFrustumBoundingBox{};

public:
	CCamera();
	virtual ~CCamera();

	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	void UpdateCameraFrustumBox();

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }

	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }
	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; m_xmf3Position.x += xmf3Offset.x; m_xmf3Position.y += xmf3Offset.y; m_xmf3Position.z += xmf3Offset.z; }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	XMFLOAT4X4& GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4& GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT& GetViewport() { return(m_d3dViewport); }
	D3D12_RECT& GetScissorRect() { return(m_d3dScissorRect); }

	void LoadComponentFromSet(CComponentSet* componentset);
	CComponent* FindComponent(const std::type_info& typeinfo);

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Animate(float fTimeElapsed);
	virtual void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt);

	virtual bool HandleMessage(const Telegram& msg);
};

class CPlayer;
class CThirdPersonCamera : public CCamera
{
protected:
	CPlayer* m_pPlayer;
public:
	CThirdPersonCamera();
	virtual ~CThirdPersonCamera() { }

	virtual void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	CPlayer* GetPlayer() { return(m_pPlayer); }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
};
class CFloatingCamera : public CCamera
{
public:
	CFloatingCamera();
	virtual ~CFloatingCamera() { }

	virtual void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
};
class CSimulatorCamera : public CCamera
{
public:
	CSimulatorCamera();
	virtual ~CSimulatorCamera() { }
};