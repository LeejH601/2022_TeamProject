#include "PhysicsObject.h"
#include "Terrain.h"

void CPhysicsObject::OnPrepareRender()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
	m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
	m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
	m_xmf4x4Transform._41 = m_xmf3Position.x; m_xmf4x4Transform._42 = m_xmf3Position.y; m_xmf4x4Transform._43 = m_xmf3Position.z;

	XMMATRIX mtxScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CPhysicsObject::OnUpdateCallback(float fTimeElapsed)
{
	if (m_pUpdatedContext)
	{
		CSplatTerrain* pTerrain = (CSplatTerrain*)m_pUpdatedContext;
		XMFLOAT3 xmf3TerrainPos = pTerrain->GetPosition();

		float fTerrainY = pTerrain->GetHeight(GetPosition().x - (xmf3TerrainPos.x), GetPosition().z - (xmf3TerrainPos.z));

		if (GetPosition().y < fTerrainY + xmf3TerrainPos.y)
			SetPosition(XMFLOAT3(GetPosition().x, fTerrainY + xmf3TerrainPos.y, GetPosition().z));
	}
}

void CPhysicsObject::Update(float fTimeElapsed)
{
	Apply_Gravity(fTimeElapsed);

	Move(m_xmf3Velocity, false);

	// 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext)OnUpdateCallback(fTimeElapsed);

	Animate(fTimeElapsed);

	Apply_Friction(fTimeElapsed);
}

void CPhysicsObject::Apply_Gravity(float fTimeElapsed)
{
	// 중력을 적용하는 코드
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(XMFLOAT3(0.0f, -100.0f, 0.0f), fTimeElapsed, false));
}

void CPhysicsObject::Apply_Friction(float fTimeElapsed)
{
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (300.0f * fTimeElapsed);

	// 마찰력을 적용하는 코드, 마찰력이 속도의 크기보다 크면 속도를 표현하는 m_xmf3Velocity를 {0.0f, 0.0f, 0.0f}로 초기화
	// 이외에는 마찰로 인한 감속
	if (fDeceleration > fLength)
		m_xmf3Velocity = XMFLOAT3{};
	else
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPhysicsObject::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
}

void CPhysicsObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	{
		if (fPitch != 0.0f)
		{
			m_fPitch += fPitch;
			if (m_fPitch > +89.0f) { fPitch -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { fPitch -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
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
		if (fYaw != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPhysicsObject::SetPosition(XMFLOAT3 xmf3Position)
{
	m_xmf3Position = xmf3Position;
}

void CPhysicsObject::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	XMFLOAT3 UpVec = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, UpVec);

	m_xmf3Right.x = mtxLookAt._11, m_xmf3Right.y = mtxLookAt._21, m_xmf3Right.z = mtxLookAt._31;
	m_xmf3Up.x = mtxLookAt._12, m_xmf3Up.y = mtxLookAt._22, m_xmf3Up.z = mtxLookAt._32;
	m_xmf3Look.x = mtxLookAt._13, m_xmf3Look.y = mtxLookAt._23, m_xmf3Look.z = mtxLookAt._33;
}

void CPhysicsObject::SetScale(float x, float y, float z)
{
	m_xmf3Scale.x = x;
	m_xmf3Scale.y = y;
	m_xmf3Scale.z = z;
}

XMFLOAT3 CPhysicsObject::GetPosition()
{
	return m_xmf3Position;
}

XMFLOAT3 CPhysicsObject::GetLook()
{
	return m_xmf3Look;
}

XMFLOAT3 CPhysicsObject::GetUp()
{
	return m_xmf3Up;
}

XMFLOAT3 CPhysicsObject::GetRight()
{
	return m_xmf3Right;
}

void CPhysicsObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	OnPrepareRender();
	CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CPhysicsObject::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
	}
}