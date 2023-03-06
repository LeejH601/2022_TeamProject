#include "Camera.h"
#include "..\Object\Object.h"
#include "MessageDispatcher.h"
#include "Global.h"
#include "Locator.h"
#include "..\Object\Player.h"

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
	GenerateProjectionMatrix(1.0f, 5000.0f, float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT), 110.f);
	GenerateViewMatrix(XMFLOAT3(0.0f, 22.5f, -37.5f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CCamera::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	SetViewportsAndScissorRects(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);
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
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pcbMappedCamera, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//m_pd3dcbCamera->Map(0, NULL, (void**)&m_pcbMappedCamera);
}
void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4InverseProjection, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4Projection))));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4InverseView, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4View))));
	m_pcbMappedCamera->m_xmf3CameraPosition = GetPosition();

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATUREINDEX_CAMERA, d3dGpuVirtualAddress);
}
void CCamera::Animate(float fTimeElapsed)
{
	m_xmf3CalculatedPosition = m_xmf3Position;

	for (CComponent* component : m_vComponentSet) {
		component->Update(fTimeElapsed);
	}
}
void CCamera::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed)
{
}
void CCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
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
bool CCamera::HandleMessage(const Telegram& msg)
{
	MESSAGE_TYPE type = static_cast<MESSAGE_TYPE>(msg.Msg);
	if (type == MESSAGE_TYPE::Msg_CameraMoveStart || type == MESSAGE_TYPE::Msg_CameraShakeStart || type == MESSAGE_TYPE::Msg_CameraZoomStart)
	{
		CComponent* com = nullptr;
		switch (type)
		{
		case MESSAGE_TYPE::Msg_CameraMoveStart:
			com = FindComponent(typeid(CCameraMover));
			break;
		case MESSAGE_TYPE::Msg_CameraShakeStart:
			com = FindComponent(typeid(CCameraShaker));
			break;
		case MESSAGE_TYPE::Msg_CameraZoomStart:
			com = FindComponent(typeid(CCameraZoomer));
			break;
		default:
			break;
		}
		return (com) ? com->HandleMessage(msg) : false;
	}

	return false;
}
void CCamera::ReleaseShaderVariables()
{
	if (m_pd3dcbCamera)
		m_pd3dcbCamera->Unmap(0, NULL);
}
void CCamera::LoadComponentFromSet(CComponentSet* componentset)
{
	m_vComponentSet.clear();
	m_vComponentSet.emplace_back(componentset->FindComponent(typeid(CCameraMover)));
	m_vComponentSet.emplace_back(componentset->FindComponent(typeid(CCameraShaker)));
	m_vComponentSet.emplace_back(componentset->FindComponent(typeid(CCameraZoomer)));
}
CComponent* CCamera::FindComponent(const std::type_info& typeinfo)
{
	for (CComponent* component : m_vComponentSet) {
		if (strcmp(typeinfo.name(), typeid(*component).name()) == 0)
			return component;
	}
	return nullptr;
}
void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

CThirdPersonCamera::CThirdPersonCamera() : CCamera()
{
	m_pPlayer = nullptr;
	SetOffset(XMFLOAT3(0.0f, 20.0f, -30.0f));
}

void CThirdPersonCamera::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if (pKeysBuffer[VK_RBUTTON] & 0xF0)
	{
		if (cxDelta || cyDelta)
			Rotate(cyDelta, cxDelta, 0.0f);
	}
}

void CThirdPersonCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		m_fPitch += fPitch;
		if (m_fPitch > +45.0f) { fPitch -= (m_fPitch - 45.0f); m_fPitch = +45.0f; }
		if (m_fPitch < -45.0f) { fPitch -= (m_fPitch + 45.0f); m_fPitch = -45.0f; }
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

		m_xmf3Position = xmf3Position;
		SetLookAt(xmf3LookAt);
	}

	Animate(fTimeElapsed);
	RegenerateViewMatrix();
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

CCameraShaker::CCameraShaker()
{
	m_fDuration = 0.2f;
	m_ft = 0.0f;
	m_fMagnitude = 0.5f;
}

void CCameraShaker::Update(float fElapsedTime)
{
	if (!m_pCamera)
		m_pCamera = Locator.GetSimulaterCamera();
	if (!m_bEnable)
		return;
	if (m_bShakeEnd || !m_pCamera->m_bCameraShaking)
		return;

	if (m_fDuration > m_ft) {
		m_ft += fElapsedTime;

		XMFLOAT3 CameraDir = m_pCamera->GetRightVector();

		float ShakeConstant = urd(dre);
		float RotateConstant = urd(dre);
		RotateConstant *= XM_PI;

		CameraDir = Vector3::ScalarProduct(CameraDir, ShakeConstant * m_fMagnitude, false);
		XMMATRIX RotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&m_pCamera->GetLookVector()), RotateConstant);

		XMFLOAT3 ShakeOffset; XMStoreFloat3(&ShakeOffset, XMVector3TransformCoord(XMLoadFloat3(&CameraDir), RotateMatrix));
		/*ShakeOffset.x = urd(dre);
		ShakeOffset.y = urd(dre);
		ShakeOffset.z = 0.0f;
		ShakeOffset = Vector3::ScalarProduct(ShakeOffset, m_fMagnitude, false);
		ShakeOffset = Vector3::Add(path->m_xmf3OriginOffset, ShakeOffset);*/

		m_pCamera->m_xmf3CalculatedPosition = Vector3::Add(m_pCamera->m_xmf3CalculatedPosition, ShakeOffset);
	}
	else {
		m_bShakeEnd = true;
		m_pCamera->m_bCameraShaking = false;
		//path->m_ft =
	}
}

void CCameraShaker::Reset()
{
	m_bShakeEnd = false;
	m_ft = 0.0f;
}

bool CCameraShaker::HandleMessage(const Telegram& msg)
{
	if (!m_pCamera)
		m_pCamera = Locator.GetSimulaterCamera();

	if (!m_pCamera->m_bCameraShaking && m_bEnable) {
		m_pCamera->m_bCameraShaking = true;

		Reset();

		return true;
	}

	return false;
}

CCameraMover::CCameraMover()
{
	m_fMaxDistance = 2.0f;
	m_fCurrDistance = 0.0f;
	m_fMovingTime = 0.02f;
	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fRollBackTime = 0.10f;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;
	m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void CCameraMover::Update(float fElapsedTime)
{
	if (!m_pCamera)
		m_pCamera = Locator.GetSimulaterCamera();

	if (!m_bEnable)
		return;
	if (m_bMoveEnd || !m_pCamera->m_bCameraMoving)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * 1.0f * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, length, false));

		m_pCamera->m_xmf3CalculatedPosition = Vector3::Add(m_pCamera->m_xmf3CalculatedPosition, offset);
	}
	else if (m_fCurrDistance > m_fMaxDistance * 2.0f) {
		m_bMoveEnd = true;
		m_pCamera->m_bCameraMoving = false;
	}
	else if (m_fCurrDistance > m_fMaxDistance) {
		float length = m_fBackSpeed * 1.0f * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, -length, false));

		m_pCamera->m_xmf3CalculatedPosition = Vector3::Add(m_pCamera->m_xmf3CalculatedPosition, offset);
	}

}

void CCameraMover::Reset()
{
	m_fCurrDistance = 0.f;
	m_bMoveEnd = false;
	offset.x = 0.0f;
	offset.y = 0.0f;
	offset.z = 0.0f;
}

bool CCameraMover::HandleMessage(const Telegram& msg)
{
	if (!m_pCamera)
		m_pCamera = Locator.GetSimulaterCamera();

	if (!m_pCamera->m_bCameraMoving && m_bEnable) {
		m_pCamera->m_bCameraMoving = true;

		Reset();

		CPlayer* player = (CPlayer*)msg.Sender;
		m_xmf3Direction.x = player->m_xmf3CameraMoveDirection.x * player->m_fCMDConstant;
		m_xmf3Direction.y = player->m_xmf3CameraMoveDirection.y;
		m_xmf3Direction.z = player->m_xmf3CameraMoveDirection.z;

		return true;
	}

	return false;
}

CCameraZoomer::CCameraZoomer()
{
	m_fMaxDistance = 2.0f;
	m_fCurrDistance = 0.0f;
	m_fMovingTime = 0.01f;
	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fRollBackTime = 0.10f;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;
	m_bIsIN = true;
	m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CCameraZoomer::~CCameraZoomer()
{
}

void CCameraZoomer::Update(float fElapsedTime)
{
	if (!m_pCamera)
		m_pCamera = Locator.GetSimulaterCamera();
	if (!m_bEnable)
		return;
	if (m_bZoomEnd || !m_pCamera->m_bCameraZooming)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	float ZoomConstant = (m_bIsIN) ? 1.0f : -1.0f;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, length * ZoomConstant, false));

		m_pCamera->m_xmf3CalculatedPosition = Vector3::Add(m_pCamera->m_xmf3CalculatedPosition, offset);
	}
	else if (m_fCurrDistance > m_fMaxDistance * 2.0f) {
		m_bZoomEnd = true;
		m_pCamera->m_bCameraZooming = false;
	}
	else if (m_fCurrDistance > m_fMaxDistance) {
		float length = m_fBackSpeed * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, -length * ZoomConstant, false));

		m_pCamera->m_xmf3CalculatedPosition = Vector3::Add(m_pCamera->m_xmf3CalculatedPosition, offset);
	}
}

void CCameraZoomer::Reset()
{
	m_fCurrDistance = 0.f;
	m_bZoomEnd = false;
	offset.x = 0.0f;
	offset.y = 0.0f;
	offset.z = 0.0f;
}

bool CCameraZoomer::HandleMessage(const Telegram& msg)
{
	if (!m_pCamera)
		m_pCamera = Locator.GetSimulaterCamera();

	if (!m_pCamera->m_bCameraZooming && m_bEnable) {
		m_pCamera->m_bCameraZooming = true;

		Reset();
		CGameObject* obj = (CGameObject*)(msg.Sender);
		m_xmf3Direction = Vector3::Normalize(Vector3::Subtract(obj->GetPosition(), m_pCamera->GetPosition()));

		return true;
	}

	return false;
}
