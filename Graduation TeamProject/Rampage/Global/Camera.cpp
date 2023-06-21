#include "Camera.h"
#include "..\Object\Object.h"
#include "Global.h"
#include "Locator.h"
#include "..\Object\Player.h"
#include "..\Shader\Shader.h"
#include "..\Object\Map.h"
#include "..\Object\Terrain.h"

CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag = 0.0f;
	m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
CCamera::~CCamera()
{
}

void CCamera::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	GenerateProjectionMatrix(1.0f, 500.0f, float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT), 90.f);
	GenerateViewMatrix(XMFLOAT3(0.0f, 22.5f, -37.5f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CCamera::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	RegenerateViewMatrix();
	SetViewportsAndScissorRects(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);
}
void CCamera::OnPostRender()
{
	m_xmf3CalculatedPosition = m_xmf3Position;
}
void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}
void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}
void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	m_xmf4x4OrthoProjection = Matrix4x4::OrthographicLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	//m_bFrustumBoundingBox = BoundingFrustum(XMLoadFloat4x4(&m_xmf4x4Projection));
}
void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3CalculatedPosition = m_xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}
void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3CalculatedPosition, m_xmf3LookAtWorld, m_xmf3Up);
}
void CCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3CalculatedPosition, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3CalculatedPosition, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3CalculatedPosition, m_xmf3Look);
}
void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pcbMappedCamera, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void**)&m_pcbMappedCamera);
}
void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4OrthoProjection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4OrthoProjection)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4InverseProjection, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4Projection))));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4InverseView, XMMatrixTranspose(XMLoadFloat4x4(&Matrix4x4::Identity()))); // 임시
	//XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4InverseView, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4View))));
	m_pcbMappedCamera->m_xmf3CameraPosition = GetPosition();
	m_pcbMappedCamera->m_xmf3CameraDir = XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33);

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATUREINDEX_CAMERA, d3dGpuVirtualAddress);
}
void CCamera::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed)
{
}
void CCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
}
void CCamera::OnUpdateCallback(float fTimeElapsed)
{
}
void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	XMFLOAT3 UpVec = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, UpVec);
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}
void CCamera::ReleaseShaderVariables()
{
	if (m_pd3dcbCamera)
		m_pd3dcbCamera->Unmap(0, NULL);
}
void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

CThirdPersonCamera::CThirdPersonCamera() : CCamera()
{
	m_pPlayer = nullptr;
	SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));

	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, MeterToUnit(3.0f));
}

CThirdPersonCamera::~CThirdPersonCamera()
{
}

void CThirdPersonCamera::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if (cxDelta || cyDelta)
		Rotate(cyDelta, cxDelta, 0.0f);
}

void CThirdPersonCamera::SetPlayer(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	Rotate(20.0f, 0.0f, 0.0f);
}

void CThirdPersonCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		m_fPitch -= fPitch;
		if (m_fPitch > +75.0f) { fPitch -= (m_fPitch - 75.0f); m_fPitch = +75.0f; }
		if (m_fPitch < -75.0f) { fPitch -= (m_fPitch + 75.0f); m_fPitch = -75.0f; }
	}
	if (fYaw != 0.0f)
	{
		m_fYaw += fYaw;
		if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
		if (m_fYaw < 0.0f) m_fYaw += 360.0f;
	}
	if (fRoll != 0.0f)
	{
		m_fRoll += fRoll;
		if (m_fRoll > +20.0f) { fRoll -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
		if (m_fRoll < -20.0f) { fRoll -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
	}
}

void CThirdPersonCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	if (m_pPlayer)
	{
		XMMATRIX xmmtxRotate = XMMatrixIdentity();
		XMMATRIX xmmtxRotateX = XMMatrixIdentity();
		XMMATRIX xmmtxRotateY = XMMatrixIdentity();
		XMMATRIX xmmtxRotateZ = XMMatrixIdentity();

		XMFLOAT3 xmf3XAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
		XMFLOAT3 xmf3YAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
		XMFLOAT3 xmf3ZAxis = XMFLOAT3(0.0f, 0.0f, 1.0f);
		
		if (m_fPitch != 0.0f)
		{
			xmmtxRotateX = XMMatrixRotationAxis(XMLoadFloat3(&xmf3XAxis), XMConvertToRadians(m_fPitch));
		}
		if (m_fYaw != 0.0f)
		{
			xmmtxRotateY = XMMatrixRotationAxis(XMLoadFloat3(&xmf3YAxis), XMConvertToRadians(m_fYaw));
		}
		if (m_fRoll != 0.0f)
		{
			xmmtxRotateY = XMMatrixRotationAxis(XMLoadFloat3(&xmf3ZAxis), XMConvertToRadians(m_fRoll));
		}

		xmmtxRotate = XMMatrixMultiply(xmmtxRotateZ, XMMatrixMultiply(xmmtxRotateX, xmmtxRotateY));

		XMFLOAT3 xmf3Offset = Vector3::TransformNormal(m_xmf3Offset, xmmtxRotate);
		XMFLOAT3 xmf3Position = Vector3::Add(xmf3LookAt, xmf3Offset);
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);
		float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength)fDistance = fLength;
		if (fLength < 0.01f)fDistance = fLength;
		if (fDistance > 0)
		{
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
			SetLookAt(xmf3LookAt);
		}
	}

	CCamera::Update(XMFLOAT3{ 0.0f, 0.0f, 0.0f }, fTimeElapsed);
	OnUpdateCallback(fTimeElapsed);
}

void CThirdPersonCamera::OnUpdateCallback(float fTimeElapsed)
{
	if (m_pUpdatedContext)
	{
		CMap* pMap = (CMap*)m_pUpdatedContext;
		CSplatTerrain* pTerrain = (CSplatTerrain*)(pMap->GetTerrain().get());
		XMFLOAT3 xmf3TerrainPos = pTerrain->GetPosition();

		XMFLOAT3 xmf3CameraPosition = GetPosition();
		float fTerrainY = pTerrain->GetHeight(xmf3CameraPosition.x - (xmf3TerrainPos.x), xmf3CameraPosition.z - (xmf3TerrainPos.z));

		if (xmf3CameraPosition.y < fTerrainY + xmf3TerrainPos.y)
			xmf3CameraPosition.y = fTerrainY + xmf3TerrainPos.y;

		SetPosition(xmf3CameraPosition);
	}
}

CFloatingCamera::CFloatingCamera() : CCamera()
{
}

void CFloatingCamera::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if (pKeysBuffer[VK_RBUTTON] & 0xF0)
	{
		if (cxDelta || cyDelta)
			Rotate(cyDelta, cxDelta, 0.0f);
		if (dwDirection) {
			XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
			float fDistance = 40.0f * fTimeElapsed;
			if (pKeysBuffer[VK_SHIFT] & 0xF0)
				fDistance *= 10.0f;
			if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, GetLookVector(), fDistance);
			if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, GetLookVector(), -fDistance);
			if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, GetRightVector(), fDistance);
			if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, GetRightVector(), -fDistance);
			if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, GetUpVector(), fDistance);
			if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, GetUpVector(), -fDistance);

			Move(xmf3Shift);
		}
	}
	
}

void CFloatingCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if ((fYaw != 0.0f))
	{
		XMFLOAT3 xmf3Up = XMFLOAT3(0.f, 1.0f, 0.f);
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if ((fRoll != 0.0f))
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

}

void CFloatingCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	RegenerateViewMatrix();

	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Camera Position: %f, %f, %f\n"), m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
	OutputDebugString(pstrDebug);

	_stprintf_s(pstrDebug, 256, _T("Camera Look: %f, %f, %f\n"), m_xmf3Look.x, m_xmf3Look.y, m_xmf3Look.z);
	OutputDebugString(pstrDebug);

	_stprintf_s(pstrDebug, 256, _T("Camera Up: %f, %f, %f\n"), m_xmf3Up.x, m_xmf3Up.y, m_xmf3Up.z);
	OutputDebugString(pstrDebug);

	_stprintf_s(pstrDebug, 256, _T("Camera Right: %f, %f, %f\n"), m_xmf3Right.x, m_xmf3Right.y, m_xmf3Right.z);
	OutputDebugString(pstrDebug);
}

CSimulatorCamera::CSimulatorCamera() : CCamera()
{
}

CPath::CPath(const CPath& path)
{
	m_vMovingPaths.resize(path.m_vMovingPaths.size());
	std::copy(path.m_vMovingPaths.begin(), path.m_vMovingPaths.end(), m_vMovingPaths.begin());

	m_vZoomPaths.resize(path.m_vZoomPaths.size());
	std::copy(path.m_vZoomPaths.begin(), path.m_vZoomPaths.end(), m_vZoomPaths.begin());
}

CPath& CPath::operator=(const CPath& path)
{
	if (this != &path) {
		if (m_vMovingPaths.data())
			m_vMovingPaths.clear();

		m_vMovingPaths.resize(path.m_vMovingPaths.size());
		std::copy(path.m_vMovingPaths.begin(), path.m_vMovingPaths.end(), m_vMovingPaths.begin());

		if (m_vZoomPaths.data())
			m_vZoomPaths.clear();

		m_vZoomPaths.resize(path.m_vZoomPaths.size());
		std::copy(path.m_vZoomPaths.begin(), path.m_vZoomPaths.end(), m_vZoomPaths.begin());
	}

	return *this;
}

void CPath::Reset()
{
	m_ft = 0.0f;
	m_iIndex = 0;
	m_bPathEnd = false;
}

CCinematicCamera::CCinematicCamera()
{
}

void CCinematicCamera::SetStartCamera(CCamera* pCamera)
{
	m_StartCameraInfo.xmf3Look = pCamera->GetLookVector();
	m_StartCameraInfo.xmf3Up = pCamera->GetUpVector();
	m_StartCameraInfo.xmf3Right = pCamera->GetRightVector();
	m_StartCameraInfo.xmf3Position = pCamera->GetPosition();
}

void CCinematicCamera::SetEndCamera(CCamera* pCamera)
{
	m_EndCameraInfo.xmf3Look = pCamera->GetLookVector();
	m_EndCameraInfo.xmf3Up = pCamera->GetUpVector();
	m_EndCameraInfo.xmf3Right = pCamera->GetRightVector();
	m_EndCameraInfo.xmf3Position = pCamera->GetPosition();
}

void CCinematicCamera::PlayCinematicCamera()
{
	m_xmf3Look = m_StartCameraInfo.xmf3Look;
	m_xmf3Up = m_StartCameraInfo.xmf3Up;
	m_xmf3Right = m_StartCameraInfo.xmf3Right;
	m_xmf3Position = m_StartCameraInfo.xmf3Position;
	m_xmf3CalculatedPosition = m_StartCameraInfo.xmf3Position;
}

void CCinematicCamera::InitToPlayerCameraPos(CGameObject* pPlayer)
{
	CThirdPersonCamera* pMainSceneCamera = dynamic_cast<CThirdPersonCamera*>(((CKnightPlayer*)pPlayer)->GetCamera());

	XMFLOAT3 xmf3PlayerPos = XMFLOAT3{
		((CKnightPlayer*)pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._41,
		 pPlayer->GetPosition().y,
		((CKnightPlayer*)pPlayer)->m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._43 };
	xmf3PlayerPos.y += MeterToUnit(0.9f);

	XMMATRIX xmmtxRotate = XMMatrixIdentity();
	XMMATRIX xmmtxRotateX = XMMatrixIdentity();
	XMMATRIX xmmtxRotateY = XMMatrixIdentity();
	XMMATRIX xmmtxRotateZ = XMMatrixIdentity();

	XMFLOAT3 xmf3XAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 xmf3YAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3ZAxis = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float fPitch = pMainSceneCamera->GetPitch();
	float fYaw = pMainSceneCamera->GetYaw();
	float fRoll = pMainSceneCamera->GetRoll();

	if (fPitch != 0.0f)
	{
		xmmtxRotateX = XMMatrixRotationAxis(XMLoadFloat3(&xmf3XAxis), XMConvertToRadians(fPitch));
	}
	if (fYaw != 0.0f)
	{
		xmmtxRotateY = XMMatrixRotationAxis(XMLoadFloat3(&xmf3YAxis), XMConvertToRadians(fYaw));
	}
	if (fRoll != 0.0f)
	{
		xmmtxRotateY = XMMatrixRotationAxis(XMLoadFloat3(&xmf3ZAxis), XMConvertToRadians(fRoll));
	}

	xmmtxRotate = XMMatrixMultiply(xmmtxRotateZ, XMMatrixMultiply(xmmtxRotateX, xmmtxRotateY));

	XMFLOAT3 xmf3Offset = Vector3::TransformNormal(XMFLOAT3(0.0f, 0.0f, MeterToUnit(3.0f)), xmmtxRotate);
	XMFLOAT3 xmf3Position = Vector3::Add(xmf3PlayerPos, xmf3Offset);

	m_xmf3Position = xmf3Position;
	m_xmf3CalculatedPosition = xmf3Position;

	SetLookAt(xmf3PlayerPos);

	CCamera::Update(XMFLOAT3{ 0.0f, 0.0f, 0.0f }, 0.0f);
	OnUpdateCallback(0.0f);

	m_StartCameraInfo.xmf3Look = GetLookVector();
	m_StartCameraInfo.xmf3Up = GetUpVector();
	m_StartCameraInfo.xmf3Right = GetRightVector();
	m_StartCameraInfo.xmf3Position = GetPosition();

	m_EndCameraInfo.xmf3Look = XMFLOAT3{ -0.384055f, -0.417339f, -0.823608f };
	m_EndCameraInfo.xmf3Up = XMFLOAT3{ -0.176376f, 0.908751f, -0.378237f };
	m_EndCameraInfo.xmf3Right = XMFLOAT3{ -0.906308f, -0.000001f, 0.422619f };
	m_EndCameraInfo.xmf3Position = XMFLOAT3{ 134.94f, 27.656675f, 176.956421f };
}

void CCinematicCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if ((fYaw != 0.0f))
	{
		XMFLOAT3 xmf3Up = XMFLOAT3(0.f, 1.0f, 0.f);
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if ((fRoll != 0.0f))
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

}

XMFLOAT3 InterpolateXMFLOAT3(XMFLOAT3 xmf3Origin, XMFLOAT3 xmf3Target, float t)
{
	XMFLOAT3 xmf3Result;

	xmf3Result.x = xmf3Origin.x * (1.0f - t) + xmf3Target.x * t;
	xmf3Result.y = xmf3Origin.y * (1.0f - t) + xmf3Target.y * t;
	xmf3Result.z = xmf3Origin.z * (1.0f - t) + xmf3Target.z * t;

	return xmf3Result;
}

void CCinematicCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	m_xmf3Look = InterpolateXMFLOAT3(m_xmf3Look, m_EndCameraInfo.xmf3Look, fTimeElapsed);
	m_xmf3Up = InterpolateXMFLOAT3(m_xmf3Up, m_EndCameraInfo.xmf3Up, fTimeElapsed);
	m_xmf3Right = InterpolateXMFLOAT3(m_xmf3Right, m_EndCameraInfo.xmf3Right, fTimeElapsed);
	SetPosition(InterpolateXMFLOAT3(m_xmf3Position, m_EndCameraInfo.xmf3Position, fTimeElapsed));
}
