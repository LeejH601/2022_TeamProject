#include "Player.h"
#include "..\Global\Camera.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Global\Timer.h"
#include "Terrain.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	m_xmf3Velocity = XMFLOAT3{};

	std::shared_ptr<CGameObject> knightObject = std::make_shared<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	
	SetChild(knightObject);

	m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>(this);
	m_pStateMachine->SetCurrentState(Locator.GetPlayerState(typeid(Idle_Player)));
	m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));

	SetPosition(XMFLOAT3(-15.0f, 150.0f, 0.0f));
	SetScale(4.0f, 4.0f, 4.0f);
	Rotate(0.0f, 90.0f, 0.0f);
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	OnPrepareRender();
	CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera)
{
	if (dwDirection)
	{
		if (m_pStateMachine->GetCurrentState() == Locator.GetPlayerState(typeid(Idle_Player)))
			m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Run_Player)));

		XMFLOAT3 xmf3Shift = XMFLOAT3{};

		if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), fDistance);
		if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), -fDistance);
		if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), fDistance);
		if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
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

bool CPlayer::CheckCollision(CGameObject* pTargetObject)
{
	bool flag = false;
	if (m_pChild.get()) {
		if (!m_bAttacked && m_pChild->CheckCollision(pTargetObject)) {
			flag = true;
			m_iAttack_Limit--;

			Telegram msg;
			msg.Msg = (int)MESSAGE_TYPE::Msg_CameraShakeStart;
			msg.DispatchTime = Locator.GetTimer()->GetNowTime();
			msg.Receiver = (IEntity*)Locator.GetSimulaterCamera();
			//Locator.GetMessageDispather()->RegisterMessage(msg);
			Locator.GetMessageDispather()->Discharge(msg.Receiver, msg);

			msg.Msg = (int)MESSAGE_TYPE::Msg_CameraMoveStart;
			msg.DispatchTime = Locator.GetTimer()->GetNowTime();
			msg.Receiver = (IEntity*)Locator.GetSimulaterCamera();
			msg.Sender = this;
			//Locator.GetMessageDispather()->RegisterMessage(msg);
			Locator.GetMessageDispather()->Discharge(msg.Receiver, msg);

			msg.Msg = (int)MESSAGE_TYPE::Msg_CameraZoomStart;
			msg.DispatchTime = Locator.GetTimer()->GetNowTime();
			msg.Receiver = (IEntity*)Locator.GetSimulaterCamera();
			msg.Sender = (IEntity*)pTargetObject;
			//Locator.GetMessageDispather()->RegisterMessage(msg);
			Locator.GetMessageDispather()->Discharge(msg.Receiver, msg);

			msg.Msg = (int)MESSAGE_TYPE::Msg_SoundEffectReady;
			msg.DispatchTime = Locator.GetTimer()->GetNowTime();
			msg.Receiver = (IEntity*)Locator.GetSoundPlayer();
			//Locator.GetMessageDispather()->RegisterMessage(msg);
			Locator.GetMessageDispather()->Discharge(msg.Receiver, msg);

			if (m_iAttack_Limit < 1)
				m_bAttacked = true;
		}
	}
	return flag;
}

void CPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	if (m_pPlayerUpdatedContext)
	{
		CSplatTerrain* pTerrain = (CSplatTerrain*)m_pPlayerUpdatedContext;
		XMFLOAT3 xmf3TerrainPos = pTerrain->GetPosition();

		float fTerrainY = pTerrain->GetHeight(GetPosition().x - (xmf3TerrainPos.x), GetPosition().z - (xmf3TerrainPos.z));
		
		if (GetPosition().y < fTerrainY + xmf3TerrainPos.y)
			SetPosition(XMFLOAT3(GetPosition().x, fTerrainY + xmf3TerrainPos.y, GetPosition().z));
	}
}

void CPlayer::Update(float fTimeElapsed)
{
	m_pStateMachine->Update(fTimeElapsed);

	// Idle 상태로 복귀하는 코드
	if (!Vector3::Length(m_xmf3Velocity) && m_pStateMachine->GetCurrentState() == Locator.GetPlayerState(typeid(Run_Player)))
		m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));

	// Run 상태일때 플레이어를 이동시키고 방향전환 시켜주는 코드
	if (m_pStateMachine->GetCurrentState() == Locator.GetPlayerState(typeid(Run_Player)))
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(XMFLOAT3(0.0f, -100.0f, 0.0f), fTimeElapsed, false));

		if (m_xmf3Velocity.x + m_xmf3Velocity.z)
			SetLookAt(Vector3::Add(GetPosition(), Vector3::Normalize(XMFLOAT3{ m_xmf3Velocity.x, 0.0f, m_xmf3Velocity.z })));
		
		Move(m_xmf3Velocity, false);
	}
	// Run 상태가 아닐때 플레이어에게 중력만 작용하는 코드
	else
	{
		m_xmf3Velocity = XMFLOAT3{};
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(XMFLOAT3(0.0f, -100.0f, 0.0f), fTimeElapsed, false));
		Move(m_xmf3Velocity, false);
	}
	// 플레이어가 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pPlayerUpdatedContext)OnPlayerUpdateCallback(fTimeElapsed);

	Animate(fTimeElapsed);

	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (300.0f * fTimeElapsed);

	// 마찰력을 적용하는 코드, 마찰력이 속도의 크기보다 크면 속도를 표현하는 m_xmf3Velocity를 {0.0f, 0.0f, 0.0f}로 초기화
	// 이외에는 마찰로 인한 감속
	if (fDeceleration > fLength)
		m_xmf3Velocity = XMFLOAT3{};
	else
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPlayer::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (dwDirection)
		{
			Move(dwDirection, 40.0f * fTimeElapsed, true, pCamera);
		}
	}
}

void CPlayer::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	XMFLOAT3 UpVec = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, UpVec);
	
	m_xmf3Right.x = mtxLookAt._11, m_xmf3Right.y = mtxLookAt._21, m_xmf3Right.z = mtxLookAt._31;
	m_xmf3Up.x = mtxLookAt._12, m_xmf3Up.y = mtxLookAt._22, m_xmf3Up.z = mtxLookAt._32;
	m_xmf3Look.x = mtxLookAt._13, m_xmf3Look.y = mtxLookAt._23, m_xmf3Look.z = mtxLookAt._33;
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
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

void CPlayer::SetPosition(XMFLOAT3 xmf3Position)
{
	m_xmf3Position = xmf3Position;
}

void CPlayer::SetScale(float x, float y, float z)
{
	m_xmf3Scale.x = x;
	m_xmf3Scale.y = y;
	m_xmf3Scale.z = z;

	m_pChild->m_pSkinnedAnimationController->m_xmf3RootObjectScale = m_xmf3Scale;
}

XMFLOAT3 CPlayer::GetPosition()
{
	return m_xmf3Position;
}

XMFLOAT3 CPlayer::GetLook()
{
	return m_xmf3Look;
}

XMFLOAT3 CPlayer::GetUp()
{
	return m_xmf3Up;
}

XMFLOAT3 CPlayer::GetRight()
{
	return m_xmf3Right;
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
	m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
	m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
	m_xmf4x4Transform._41 = m_xmf3Position.x; m_xmf4x4Transform._42 = m_xmf3Position.y; m_xmf4x4Transform._43 = m_xmf3Position.z;
	
	XMMATRIX mtxScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CPlayer::Tmp()
{
	m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, m_nAnimationNum++);
}