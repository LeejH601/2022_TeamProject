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

	m_xmf3Velocity = XMFLOAT3{};

	std::shared_ptr<CGameObject> knightObject = std::make_shared<CKnightObject>(pd3dDevice, pd3dCommandList, 1);

	SetChild(knightObject);

	m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>(this);
	m_pStateMachine->SetCurrentState(Locator.GetPlayerState(typeid(Idle_Player)));
	m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));

	SetPosition(XMFLOAT3(100.0f, 150.0f, -100.0f));
	SetScale(4.0f, 4.0f, 4.0f);
	Rotate(0.0f, 90.0f, 0.0f);

	m_fSpeedKperH = 10.0f;
	m_fSpeedMperS = m_fSpeedKperH * 1000.0f / 3600.0f;
	m_fSpeedUperS = m_fSpeedMperS * 8.0f / 1.0f;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera)
{
	if (dwDirection)
	{
		if (m_pStateMachine->GetCurrentState() == Locator.GetPlayerState(typeid(Idle_Player)))
			m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Run_Player)));

		XMFLOAT3 xmf3Shift = XMFLOAT3{};

		if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)));
		if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), -1.0f);
		if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)));
		if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), -1.0f);

		CPhysicsObject::Move(xmf3Shift, bUpdateVelocity);
	}
}

bool CPlayer::CheckCollision(CGameObject* pTargetObject)
{
	bool flag = false;
	if (m_pChild.get()) {
		if (!m_bAttacked && m_pChild->CheckCollision(pTargetObject)) {

			pTargetObject->SetHit(this);

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

void CPlayer::Update(float fTimeElapsed)
{
	m_pStateMachine->Update(fTimeElapsed);

	// Idle 상태로 복귀하는 코드
	if (!Vector3::Length(m_xmf3Velocity) && m_pStateMachine->GetCurrentState() == Locator.GetPlayerState(typeid(Run_Player)))
		m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));

	// Run 상태일때 플레이어를 이동시키고 방향전환 시켜주는 코드
	if (m_pStateMachine->GetCurrentState() == Locator.GetPlayerState(typeid(Run_Player)))
	{
		CPhysicsObject::Apply_Gravity(fTimeElapsed);

		if (m_xmf3Velocity.x + m_xmf3Velocity.z)
			SetLookAt(Vector3::Add(GetPosition(), Vector3::Normalize(XMFLOAT3{ m_xmf3Velocity.x, 0.0f, m_xmf3Velocity.z })));
		
		CPhysicsObject::Move(m_xmf3Velocity, false);
	}
	// Run 상태가 아닐때 플레이어에게 중력만 작용하는 코드
	else
	{
		m_xmf3Velocity = XMFLOAT3{};
		CPhysicsObject::Apply_Gravity(fTimeElapsed);
		CPhysicsObject::Move(m_xmf3Velocity, false);
	}

	// 플레이어가 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext) CPhysicsObject::OnUpdateCallback(fTimeElapsed);

	Animate(fTimeElapsed);

	CPhysicsObject::Apply_Friction(fTimeElapsed);
}

void CPlayer::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (dwDirection)
		{
			Move(dwDirection, m_fSpeedUperS * fTimeElapsed, true, pCamera);
		}
	}
}

void CPlayer::SetScale(float x, float y, float z)
{
	CPhysicsObject::SetScale(x, y, z);
	
	m_pChild->m_pSkinnedAnimationController->m_xmf3RootObjectScale = m_xmf3Scale;
}

void CPlayer::Tmp()
{
	m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, m_nAnimationNum++);
}