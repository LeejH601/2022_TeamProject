#include "Player.h"
#include "..\Global\Camera.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Global\Timer.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>(this);
	m_pStateMachine->SetCurrentState(Locator.GetPlayerState(typeid(Idle_Player)));
	m_pStateMachine->SetPreviousState(Locator.GetPlayerState(typeid(Idle_Player)));

	SetPosition(XMFLOAT3(-15.0f, 0.0f, 0.0f));
	SetScale(15.0f, 15.0f, 15.0f);
	Rotate(0.0f, 90.0f, 0.0f);

	std::shared_ptr<CGameObject> knightObject = std::make_shared<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	knightObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	knightObject->m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(14.0f, 14.0f, 14.0f);
	
	SetChild(knightObject);
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
		XMFLOAT3 xmf3Shift = XMFLOAT3{};

		if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fDistance);
		if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), -fDistance);
		if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, GetRight(), fDistance);
		if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, GetRight(), -fDistance);
		if (dwDirection & DIR_UP)xmf3Shift = Vector3::Add(xmf3Shift, GetUp(), fDistance);
		if (dwDirection & DIR_DOWN)xmf3Shift = Vector3::Add(xmf3Shift, GetUp(), -fDistance);

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
			Locator.GetMessageDispather()->RegisterMessage(msg);

			msg.Msg = (int)MESSAGE_TYPE::Msg_CameraMoveStart;
			msg.DispatchTime = Locator.GetTimer()->GetNowTime();
			msg.Receiver = (IEntity*)Locator.GetSimulaterCamera();
			msg.Sender = this;
			Locator.GetMessageDispather()->RegisterMessage(msg);

			msg.Msg = (int)MESSAGE_TYPE::Msg_CameraZoomStart;
			msg.DispatchTime = Locator.GetTimer()->GetNowTime();
			msg.Receiver = (IEntity*)Locator.GetSimulaterCamera();
			msg.Sender = (IEntity*)pTargetObject;
			Locator.GetMessageDispather()->RegisterMessage(msg);

			msg.Msg = (int)MESSAGE_TYPE::Msg_SoundEffectReady;
			msg.DispatchTime = Locator.GetTimer()->GetNowTime();
			msg.Receiver = (IEntity*)Locator.GetSoundPlayer();
			Locator.GetMessageDispather()->RegisterMessage(msg);

			if (m_iAttack_Limit < 1)
				m_bAttacked = true;
		}
	}
	return flag;
}

void CPlayer::Update(float fTimeElapsed)
{
	m_pStateMachine->Update(fTimeElapsed);
}

void CPlayer::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		
		if (dwDirection)
		{
			Rotate(0.0f, pCamera->GetYaw() - GetYaw(), 0.0f);
			Move(dwDirection, 50.0f * fTimeElapsed, false, pCamera);
		}
	}
}

void CPlayer::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	XMFLOAT3 UpVec = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, UpVec);
	
	m_xmf4x4Transform._11 = mtxLookAt._11, m_xmf4x4Transform._12 = mtxLookAt._21, m_xmf4x4Transform._13 = mtxLookAt._31;
	m_xmf4x4Transform._21 = mtxLookAt._12, m_xmf4x4Transform._22 = mtxLookAt._22, m_xmf4x4Transform._23 = mtxLookAt._32;
	m_xmf4x4Transform._31 = mtxLookAt._13, m_xmf4x4Transform._32 = mtxLookAt._23, m_xmf4x4Transform._33 = mtxLookAt._33;

	SetScale(8.0f, 8.0f, 8.0f);
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
	
	SetScale(8.0f, 8.0f, 8.0f);

	UpdateTransform(NULL);
}