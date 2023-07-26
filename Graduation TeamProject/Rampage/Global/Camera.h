#pragma once
#include "stdafx.h"
#include "MessageDispatcher.h"

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CCamera;
class CGameObject;

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4X4 m_xmf4x4OrthoProjection;
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

class CCamera
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
	//원근 투영 변환 행렬
	XMFLOAT4X4 m_xmf4x4Projection;
	//직교 투영 변환 행렬
	XMFLOAT4X4 m_xmf4x4OrthoProjection;
	//뷰포트와 씨저 사각형
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	ComPtr<ID3D12Resource> m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO* m_pcbMappedCamera = NULL;

	std::vector<std::unique_ptr<IMessageListener>> m_pListeners;

	LPVOID m_pUpdatedContext = NULL;
public:
	CCamera();
	virtual ~CCamera();

	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Reset();

	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPostRender();

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	const XMFLOAT3& GetCalculatedPosition() { return(m_xmf3CalculatedPosition); }

	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }
	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; m_xmf3Position.x += xmf3Offset.x; m_xmf3Position.y += xmf3Offset.y; m_xmf3Position.z += xmf3Offset.z; }
	void SetOffsetOnly(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; };

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	void SetUpdatedContext(LPVOID pContext) { m_pUpdatedContext = pContext; }

	void SetRightVector(XMFLOAT3 xmf3Right) { m_xmf3Right = xmf3Right; }
	void SetUpVector(XMFLOAT3 xmf3Up) { m_xmf3Up = xmf3Up; }
	void SetLookVector(XMFLOAT3 xmf3Look) { m_xmf3Look = xmf3Look; }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetPitch(float pitch) { m_fPitch = pitch; }
	void SetRoll(float roll) { m_fRoll = roll; }
	void SetYaw(float yaw) { m_fYaw = yaw; }

	XMFLOAT4X4& GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4& GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT& GetViewport() { return(m_d3dViewport); }
	D3D12_RECT& GetScissorRect() { return(m_d3dScissorRect); }

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void OnUpdateCallback(float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt);
};

class CPlayer;
class CThirdPersonCamera : public CCamera
{
protected:
	CPlayer* m_pPlayer;
public:
	XMFLOAT4 m_xmf4RotationQuaternion;
public:
	CThirdPersonCamera();
	virtual ~CThirdPersonCamera();

	virtual void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);

	void SetPlayer(CPlayer* pPlayer);
	CPlayer* GetPlayer() { return(m_pPlayer); }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void OnUpdateCallback(float fTimeElapsed);
};

enum class CINEMATIC_FOCUSMODE {
	FOCUS_SEQUENCE,
	FOCUS_POINT,
	FOUCS_PLAYER,
	CINEMATIC_FOCUSMODE_END,
};

enum class CINEMATIC_SIMULATION_DIMENSION {
	DIMENSION_WORLD,
	DIMENSION_LOCAL,
	CINEMATIC_SIMULATION_DIMENSION_END,
};

class CCinematicCamera : public CCamera
{
protected:
	struct CameraInfo {
		XMFLOAT3 xmf3Look;
		XMFLOAT3 xmf3Up;
		XMFLOAT3 xmf3Right;

		XMFLOAT3 xmf3Position;
		float fSegmentTime;
	};

	int m_iCurrentCameraInfoIndex;
	std::vector<CameraInfo> m_vCameraInfos;

	float m_fAcceleration;
	float m_fCurrentSpeed;
	float m_fTotalDistance;
	float m_fTotalParamT;

	XMFLOAT3 m_xmf3FocusPoint;
	CINEMATIC_FOCUSMODE m_Cinematic_simulation_mode = CINEMATIC_FOCUSMODE::FOCUS_SEQUENCE;

	CINEMATIC_SIMULATION_DIMENSION m_Cinematic_Simulation_Dimension = CINEMATIC_SIMULATION_DIMENSION::DIMENSION_WORLD;
	CGameObject* m_LocalBaseObject = nullptr;

public:
	CCinematicCamera();
	virtual ~CCinematicCamera() { }

	void AddPlayerCameraInfo(CPlayer* pPlayer, CCamera* pCamera);
	void AddCameraInfo(CCamera* pCamera, float SegmentTime = 1.0f);
	void ClearCameraInfo();
	virtual void PlayCinematicCamera();
	void SetSimulationDimension(CINEMATIC_SIMULATION_DIMENSION dimension) { m_Cinematic_Simulation_Dimension = dimension; };
	void SetFocusMode(CINEMATIC_FOCUSMODE focusMode) { m_Cinematic_simulation_mode = focusMode; };
	void SetLocalObject(CGameObject* object) { m_LocalBaseObject = object; };
	void SetFocusPoint(XMFLOAT3 point) { m_xmf3FocusPoint = point; };

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
};

class CDollyCamera : public CCinematicCamera
{
protected:
	struct CubicPoly
	{
		XMFLOAT3 c0, c1, c2, c3;

		XMFLOAT3 eval(float t)
		{
			float t2 = t * t;
			float t3 = t2 * t;

			XMFLOAT3 term0 = c0;
			XMFLOAT3 term1 = Vector3::ScalarProduct(c1, t, false);
			XMFLOAT3 term2 = Vector3::ScalarProduct(c2, t2, false);
			XMFLOAT3 term3 = Vector3::ScalarProduct(c3, t3, false);

			XMFLOAT3 result;
			result = Vector3::Add(term0, Vector3::Add(term1, Vector3::Add(term2, term3)));

			return result;
		}
	};

	void InitCubicPoly(XMFLOAT3 x0, XMFLOAT3 x1, XMFLOAT3 t0, XMFLOAT3 t1, CubicPoly& p)
	{
		p.c0 = x0;
		p.c1 = t0;

		XMFLOAT3 term0 = Vector3::ScalarProduct(t1, -1, false);
		XMFLOAT3 term1 = Vector3::ScalarProduct(t0, -2, false);
		XMFLOAT3 term2 = Vector3::ScalarProduct(x1, 3, false);
		XMFLOAT3 term3 = Vector3::ScalarProduct(x0, -3, false);
		p.c2 = Vector3::Add(term0, Vector3::Add(term1, Vector3::Add(term2, term3)));

		term0 = t1;
		term1 = t0;
		term2 = Vector3::ScalarProduct(x1, -2, false);
		term3 = Vector3::ScalarProduct(x0, 2, false);
		p.c3 = Vector3::Add(term0, Vector3::Add(term1, Vector3::Add(term2, term3)));
	}

	void InitNonuniformCatmullRom(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, float dt0, float dt1, float dt2, CubicPoly& p)
	{
		// compute tangents when parameterized in [t1,t2]
		XMFLOAT3 t1, t2;

		t1.x = (p1.x - p0.x) / dt0 - (p2.x - p0.x) / (dt0 + dt1) + (p2.x - p1.x) / dt1;
		t2.x = (p2.x - p1.x) / dt1 - (p3.x - p1.x) / (dt1 + dt2) + (p3.x - p2.x) / dt2;

		t1.y = (p1.y - p0.y) / dt0 - (p2.y - p0.y) / (dt0 + dt1) + (p2.y - p1.y) / dt1;
		t2.y = (p2.y - p1.y) / dt1 - (p3.y - p1.y) / (dt1 + dt2) + (p3.y - p2.y) / dt2;

		t1.z = (p1.z - p0.z) / dt0 - (p2.z - p0.z) / (dt0 + dt1) + (p2.z - p1.z) / dt1;
		t2.z = (p2.z - p1.z) / dt1 - (p3.z - p1.z) / (dt1 + dt2) + (p3.z - p2.z) / dt2;

		// rescale tangents for parametrization in [0,1]
		t1 = Vector3::ScalarProduct(t1, dt1, false);
		t2 = Vector3::ScalarProduct(t2, dt1, false);

		InitCubicPoly(p1, p2, t1, t2, p);
	}

	float VecDistSquared(const XMFLOAT3& p, const XMFLOAT3& q)
	{
		float dx = q.x - p.x;
		float dy = q.y - p.y;
		float dz = q.z - p.z;
		return dx * dx + dy * dy + dz * dz;
	}

	void InitCentripetalCR(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, const XMFLOAT3& p3,
		CubicPoly& p)
	{
		float dt0 = powf(VecDistSquared(p0, p1), 0.25f);
		float dt1 = powf(VecDistSquared(p1, p2), 0.25f);
		float dt2 = powf(VecDistSquared(p2, p3), 0.25f);

		// safety check for repeated points
		if (dt1 < 1e-4f)    dt1 = 1.0f;
		if (dt0 < 1e-4f)    dt0 = dt1;
		if (dt2 < 1e-4f)    dt2 = dt1;

		InitNonuniformCatmullRom(p0, p1, p2, p3, dt0, dt1, dt2, p);
	}

	struct DollyTrack {
		XMFLOAT3 xmf3Position;
		float fSegmentTime;
	};

	std::vector<DollyTrack> m_vDollyTracks;
	std::vector<CubicPoly> m_vCubicPolys;
	float m_fPathLength;
	int m_nTrackIndex = 0;
	float m_fDt = 0;

public:
	CDollyCamera();
	virtual ~CDollyCamera();

	virtual void PlayCinematicCamera();

	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);

	void CaculateCubicPolyData();
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

class CSimulatorThirdPersonCamera : public CThirdPersonCamera
{
public:
	virtual void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed);
};