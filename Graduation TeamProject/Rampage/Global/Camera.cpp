#include "Camera.h"

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
	m_ShakePath = std::make_unique<CPath>();
	m_ShakePath->m_xmf3OriginOffset = m_xmf3Position;
}
CCamera::~CCamera()
{
}

void CCamera::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	GenerateProjectionMatrix(1.0f, 5000.0f, float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT), 90.0f);
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
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}
void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}
void CCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}
void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void**)&m_pcbMappedCamera);
}
void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4InverseProjection, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4Projection))));

	m_pcbMappedCamera->m_xmf3CameraPosition = GetPosition();

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATUREINDEX_CAMERA, d3dGpuVirtualAddress);
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

CFirstPersonCamera::CFirstPersonCamera() : CCamera()
{
}

void CFirstPersonCamera::Rotate(float fPitch, float fYaw, float fRoll)
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

CPath::CPath(const CPath& path)
{
	m_vPaths.resize(path.m_vPaths.size());
	std::copy(path.m_vPaths.begin(), path.m_vPaths.end(), m_vPaths.begin());
}

CPath& CPath::operator=(const CPath& path)
{
	if (this != &path) {
		if (m_vPaths.data())
			m_vPaths.clear();

		m_vPaths.resize(path.m_vPaths.size());
		std::copy(path.m_vPaths.begin(), path.m_vPaths.end(), m_vPaths.begin());
	}

	return *this;
}

void CPath::Reset()
{
	m_ft = 0.0f;
	m_iIndex = 0;
	m_bPathEnd = false;
}

void CCameraMovementManager::ShaketoNextPostion(CCamera* camera, float fElapsedTime)
{
	/*CPath* path = camera->GetPath();

	if (path->m_bPathEnd || path->m_vPaths.size() == 0)
		return;

	XMFLOAT3& SeekPos = path->m_vPaths[path->m_iIndex];
	XMFLOAT3 Dir = Vector3::Normalize(Vector3::Subtract(SeekPos, path->m_xmf3Offset));

	float speed = .5f;

	XMFLOAT3 NextDistance = Vector3::ScalarProduct(Dir, fElapsedTime * speed, false);

	XMFLOAT3 NextPos = Vector3::Add(path->m_xmf3Offset, NextDistance);

	float length = Vector3::Length(Vector3::Subtract(SeekPos, NextPos));

	if (length <= 0.01f) {
		path->m_iIndex++;
		if (path->m_iIndex >= path->m_vPaths.size()) {
			path->m_iIndex = 0;
			path->m_bPathEnd = true;
		}
		NextPos = SeekPos;
		NextDistance = XMFLOAT3(0.f, 0.f, 0.f);
	}

	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("%f, %f, %f \n"), NextPos.x, NextPos.y, NextPos.z);
	OutputDebugString(pstrDebug);

	path->m_xmf3Offset = NextPos;
	camera->SetOffset(Vector3::Add(camera->GetOffset(), NextDistance));

	_stprintf_s(pstrDebug, 256, _T("%f, %f, %f \n"), camera->GetOffset().x, camera->GetOffset().y, camera->GetOffset().z);
	OutputDebugString(pstrDebug);*/


	CPath* path = camera->GetPath();
	if (path->m_fDuration > path->m_ft) {
		path->m_ft += fElapsedTime;

		XMFLOAT3 CameraDir = camera->GetRightVector();

		float ShakeConstant = urd(dre);
		float RotateConstant = urd(dre);
		RotateConstant *= XM_PI;

		CameraDir = Vector3::ScalarProduct(CameraDir, ShakeConstant * m_fMagnitude, false);
		XMMATRIX RotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&camera->GetLookVector()), RotateConstant);

		XMFLOAT3 ShakeOffset; XMStoreFloat3(&ShakeOffset, XMVector3TransformCoord(XMLoadFloat3(&CameraDir), RotateMatrix));
		/*ShakeOffset.x = urd(dre);
		ShakeOffset.y = urd(dre);
		ShakeOffset.z = 0.0f;
		ShakeOffset = Vector3::ScalarProduct(ShakeOffset, m_fMagnitude, false);
		ShakeOffset = Vector3::Add(path->m_xmf3OriginOffset, ShakeOffset);*/

		camera->SetPosition(Vector3::Add(path->m_xmf3OriginOffset, ShakeOffset));
	}
	else {
		camera->SetPosition(path->m_xmf3OriginOffset);
	}
}
