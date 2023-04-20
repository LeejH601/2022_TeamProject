#include "Player.h"
#include "Monster.h"
#include "..\Global\Camera.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\Timer.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Object\ParticleObject.h"
#include "..\Object\PlayerParticleObject.h"
#include "Object.h"
#include <stdio.h>

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	m_xmf3Velocity = XMFLOAT3{};

	std::shared_ptr<CGameObject> knightObject = std::make_shared<CKnightObject>(pd3dDevice, pd3dCommandList, 1);
	m_pChildObject = knightObject.get();

	SetChild(knightObject);
	
	m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>(this);
	m_pStateMachine->SetCurrentState(Idle_Player::GetInst());
	m_pStateMachine->ChangeState(Idle_Player::GetInst());

	SetPosition(XMFLOAT3(100.0f, 150.0f, -100.0f));
	SetScale(4.0f, 4.0f, 4.0f);
	Rotate(0.0f, 90.0f, 0.0f);

	m_fSpeedKperH = 6.0f;
	m_fSpeedMperS = m_fSpeedKperH * 1000.0f / 3600.0f;
	m_fSpeedUperS = m_fSpeedMperS * 100.0f / 4.0f;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
}

XMFLOAT3 CPlayer::GetATKDirection()
{
	CState<CPlayer>* pPlayerState = m_pStateMachine->GetCurrentState();
	XMFLOAT3 xmf3Direction = { 0.0f, 0.0f, 0.0f };
	if (pPlayerState == Atk1_Player::GetInst())
	{
		xmf3Direction = Vector3::Add(GetUp(), GetRight());
		xmf3Direction = Vector3::ScalarProduct(xmf3Direction, -1.0f);

	}

	else if (pPlayerState == Atk2_Player::GetInst())
	{
		xmf3Direction = GetRight();
	}

	else if (pPlayerState == Atk3_Player::GetInst())
	{
		xmf3Direction = GetLook();
	}

	return XMFLOAT3(xmf3Direction);
}

XMFLOAT3 CPlayer::GetTargetPosition()
{
	return m_xmf3TargetPosition;
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera)
{
	if (dwDirection)
	{
		if (m_pStateMachine->GetCurrentState() == Idle_Player::GetInst())
			m_pStateMachine->ChangeState(Run_Player::GetInst());

		if (m_pStateMachine->GetCurrentState() != Run_Player::GetInst())
			return;

		XMFLOAT3 xmf3Shift = XMFLOAT3{};

		if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)));
		if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), -1.0f);
		if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)));
		if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), -1.0f);
		xmf3Shift = Vector3::Normalize(xmf3Shift);
		xmf3Shift = Vector3::ScalarProduct(xmf3Shift, fDistance, false);

		CPhysicsObject::Move(xmf3Shift, bUpdateVelocity);
	}
}

XMFLOAT4& CPlayer::GetTrailControllPoint(int n)
{
	return ((CKnightObject*)(m_pChild.get()))->GetTrailControllPoint(n);
}

bool CPlayer::CheckCollision(CGameObject* pTargetObject)
{
	if (m_pChild.get()) {
		if ( (m_iAttackId != ((CMonster*)pTargetObject)->GetPlayerAtkId()) && m_pChild->CheckCollision(pTargetObject)) {

			SoundPlayParams SoundPlayParam;
			SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOCK;
			CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, m_pStateMachine->GetCurrentState());

			if (m_pCamera)
			{
				if (m_pStateMachine->GetCurrentState()->GetCameraShakeComponent()->GetEnable())
					m_pCamera->m_bCameraShaking = true;
				if (m_pStateMachine->GetCurrentState()->GetCameraZoomerComponent()->GetEnable())
					m_pCamera->m_bCameraZooming = true;
				if (m_pStateMachine->GetCurrentState()->GetCameraMoveComponent()->GetEnable())
					m_pCamera->m_bCameraMoving = true;
			}

			m_xmf3TargetPosition = pTargetObject->GetPosition();
			pTargetObject->SetHit(this);

			return true;
		}
	}
	return false;
}

void CPlayer::Update(float fTimeElapsed)
{
	m_pStateMachine->Update(fTimeElapsed);

	m_fTime += fTimeElapsed;

	if (m_fTime > m_fMaxTime)
	{
		if (m_pStateMachine->GetCurrentState() == Atk1_Player::GetInst())
		{
			m_fMaxTime = 0.005f;
			// 0.35 시작
			if (0.35f < m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
				0.45f > m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
			{
				ParticleTrailParams ParticleTrail_comp_params;
				CGameObject* pWeapon = m_pChild->FindFrame("Weapon_r");
				pWeapon->FindFrame("Weapon_r");

				XMFLOAT3 xmf3Position;
				XMFLOAT3 xmf3Direction;
				memcpy(&xmf3Position, &(pWeapon->m_xmf4x4World._41), sizeof(XMFLOAT3));
				memcpy(&xmf3Direction, &(pWeapon->m_xmf4x4World._31), sizeof(XMFLOAT3));

				XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();

				xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };
				xmf3Direction = GetATKDirection();

				
				xmf3Position = static_cast<CKnightObject*>(m_pChild.get())->GetWeaponMeshBoundingBox().Center;
				xmf3Position = Vector3::Add(xmf3Position, Vector3::Normalize(xmf3Direction), 4.5f);

				ParticleTrail_comp_params.pObject = CPlayerParticleObject::GetInst()->GetTrailParticleObjects();
				ParticleTrail_comp_params.xmf3Position = xmf3Position;
				ParticleTrail_comp_params.iPlayerAttack = 0;
				ParticleTrail_comp_params.m_fTime = m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
				CMessageDispatcher::GetInst()->Dispatch_Message<ParticleTrailParams>(MessageType::UPDATE_TRAILPARTICLE, &ParticleTrail_comp_params, m_pStateMachine->GetCurrentState());
			}
		}
		// 0.25 4
		if (m_pStateMachine->GetCurrentState() == Atk2_Player::GetInst())
		{
			m_fMaxTime = 0.05f; // 0.05
			if (0.3f < m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
				0.45f > m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
			{
				//0.3~0.6, 065
				ParticleTrailParams ParticleTrail_comp_params;
				CGameObject* pWeapon = m_pChild->FindFrame("Weapon_r");
				pWeapon->FindFrame("Weapon_r");
				XMFLOAT3 xmf3Position;
				XMFLOAT3 xmf3Direction;
				XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();
				xmf3Position = static_cast<CKnightObject*>(m_pChild.get())->GetWeaponMeshBoundingBox().Center;

				xmf3Direction = GetATKDirection();
				xmf3Position = Vector3::Add(xmf3Position, Vector3::Normalize(xmf3Direction), 4.2f);

				ParticleTrail_comp_params.pObject = CPlayerParticleObject::GetInst()->GetTrailParticleObjects();
				ParticleTrail_comp_params.xmf3Position = xmf3Position;
				ParticleTrail_comp_params.iPlayerAttack = 1;
				ParticleTrail_comp_params.m_fTime = m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
				CMessageDispatcher::GetInst()->Dispatch_Message<ParticleTrailParams>(MessageType::UPDATE_TRAILPARTICLE, &ParticleTrail_comp_params, m_pStateMachine->GetCurrentState());
			}
		}
		m_fTime = 0.f;
	}

	CPhysicsObject::Apply_Gravity(fTimeElapsed);
	CPhysicsObject::Move(m_xmf3Velocity, false);

	// 플레이어가 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext) CPhysicsObject::OnUpdateCallback(fTimeElapsed);

	// 플레이어가 속도를 가진다면 해당 방향을 바라보게 하는 코드
	if (m_xmf3Velocity.x + m_xmf3Velocity.z)
		SetLookAt(Vector3::Add(GetPosition(), Vector3::Normalize(XMFLOAT3{ m_xmf3Velocity.x, 0.0f, m_xmf3Velocity.z })));

	m_pStateMachine->Animate(fTimeElapsed);

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

			XMFLOAT3 xmf3Shift = XMFLOAT3{};

			if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)));
			if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), -1.0f);
			if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)));
			if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), -1.0f);
			
			m_xmfDirection = xmf3Shift;
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