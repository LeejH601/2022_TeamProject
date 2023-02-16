#include "Player.h"
#include "..\Global\Camera.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Global\Timer.h"

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>(this);
	m_pStateMachine->SetCurrentState(Locator.GetPlayerState(typeid(Idle_Player)));
	m_pStateMachine->SetPreviousState(Locator.GetPlayerState(typeid(Idle_Player)));

	std::shared_ptr<CGameObject> knightObject = std::make_shared<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	knightObject->SetPosition(XMFLOAT3(-8.0f, 0.0f, 0.0f));
	knightObject->SetScale(14.0f, 14.0f, 14.0f);
	knightObject->Rotate(0.0f, 90.0f, 0.0f);
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
	CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);
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
