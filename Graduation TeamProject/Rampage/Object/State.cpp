#include "State.h"
#include "Animation.h"
#include "Player.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Sound\Sound.h"
#include "..\Sound\SoundManager.h"
#include "..\ImGui\ImGuiManager.h"
#include "..\Object\PlayerParticleObject.h"
#include "SwordTrailObject.h"
#include "..\Global\Logger.h"

Idle_Player::Idle_Player()
{
}

Idle_Player::~Idle_Player()
{
}

void Idle_Player::Enter(CPlayer* player)
{
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;


	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };
	player->m_bAttack = false; // ����ڰ� ��Ŭ���� true�� �Ǵ� ����

	player->m_fAnimationPlayWeight = 1.0f;
}

void Idle_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// ����ڰ� ��Ŭ���� ������ Atk1_Player�� ���� ����
	if (player->m_bAttack)
		player->m_pStateMachine->ChangeState(Atk1_Player::GetInst());

	if (player->m_bEvasioned)
		player->m_pStateMachine->ChangeState(Evasion_Player::GetInst());
}

void Idle_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime * player->GetAnimationPlayWeight());
}

void Idle_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._42 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}

void Idle_Player::Exit(CPlayer* player)
{

}

void Atk_Player::InitAtkPlayer()
{
	// ATK SOUND
	std::unique_ptr<SoundPlayComponent> pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(0);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(1.25f);
	pSoundComponent->SetMT(MONSTER_TYPE::NONE);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_SHOOT);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// DAMAGE SOUND
	pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(0);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(0.75f);
	pSoundComponent->SetMT(MONSTER_TYPE::NONE);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_SHOCK);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// GOBLIN MOAN SOUND
	pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(0);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(0.75f);
	pSoundComponent->SetMT(MONSTER_TYPE::GOBLIN);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_VOICE);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// ORC MOAN SOUND
	pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(0);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(0.75f);
	pSoundComponent->SetMT(MONSTER_TYPE::ORC);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_VOICE);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// SKELETON MOAN SOUND
	pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(0);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(0.75f);
	pSoundComponent->SetMT(MONSTER_TYPE::SKELETON);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_VOICE);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// CAMERA SHAKE
	std::unique_ptr<CameraShakeComponent> pShakeComponent = std::make_unique<CameraShakeComponent>();
	pShakeComponent->SetDuration(1.0f);
	pShakeComponent->SetMagnitude(0.5f);
	m_pListeners.push_back(std::move(pShakeComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_CAMERA, m_pListeners.back().get(), this);

	// CAMERA ZOOM IN/OUT
	std::unique_ptr<CameraZoomerComponent> pZoomerComponent = std::make_unique<CameraZoomerComponent>();
	pZoomerComponent->SetIsIn(false);
	pZoomerComponent->SetMaxDistance(10.0f);
	pZoomerComponent->SetMovingTime(0.25f);
	pZoomerComponent->SetRollBackTime(0.25f);
	m_pListeners.push_back(std::move(pZoomerComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_CAMERA, m_pListeners.back().get(), this);

	// CAMERA MOVE
	std::unique_ptr<CameraMoveComponent> pMoveComponent = std::make_unique<CameraMoveComponent>();
	pMoveComponent->SetMaxDistance(5.0f);
	pMoveComponent->SetMovingTime(0.5f);
	pMoveComponent->SetRollBackTime(0.5f);
	m_pListeners.push_back(std::move(pMoveComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_CAMERA, m_pListeners.back().get(), this);

	// DAMAGE ANIMATION
	std::unique_ptr<DamageAnimationComponent> pDamageAnimationComponent = std::make_unique<DamageAnimationComponent>();
	pDamageAnimationComponent->SetMaxDistance(15.0f);
	pDamageAnimationComponent->SetSpeed(100.0f);
	m_pListeners.push_back(std::move(pDamageAnimationComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_OBJECT, m_pListeners.back().get(), this);

	// SHAKE ANIMATION
	std::unique_ptr<ShakeAnimationComponent> pShakeAnimationComponent = std::make_unique<ShakeAnimationComponent>();
	pShakeAnimationComponent->SetDistance(0.25f);
	pShakeAnimationComponent->SetFrequency(0.01f);
	m_pListeners.push_back(std::move(pShakeAnimationComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_OBJECT, m_pListeners.back().get(), this);

	// STUN ANIMATION
	std::unique_ptr<StunAnimationComponent> pStunAnimationComponent = std::make_unique<StunAnimationComponent>();
	pStunAnimationComponent->SetStunTime(0.5f);
	m_pListeners.push_back(std::move(pStunAnimationComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_OBJECT, m_pListeners.back().get(), this);

	// HitLag ANIMATION
	std::unique_ptr<HitLagComponent> pHitLagComponent = std::make_unique<HitLagComponent>();
	pHitLagComponent->SetMaxLagTime(0.5f);
	pHitLagComponent->SetLagScale(0.5f);
	m_pListeners.push_back(std::move(pHitLagComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_HITLAG, m_pListeners.back().get(), this);

	// PARTICLE ANIMATION
	std::unique_ptr<ParticleComponent> pParticlenComponent = std::make_unique<ParticleComponent>();
	m_pListeners.push_back(std::move(pParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);

	// IMPACT
	std::unique_ptr<ImpactEffectComponent> pImpactComponent = std::make_unique<ImpactEffectComponent>();
	m_pListeners.push_back(std::move(pImpactComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_BILLBOARD, m_pListeners.back().get(), this);

	// UPDOWNPARTICLE ANIMATION
	std::unique_ptr<UpDownParticleComponent> pUpDownParticlenComponent = std::make_unique<UpDownParticleComponent>();
	m_pListeners.push_back(std::move(pUpDownParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);

	// SMOKEPARTICLE ANIMATION
	std::unique_ptr<SmokeParticleComponent> pSmokeParticlenComponent = std::make_unique<SmokeParticleComponent>();
	m_pListeners.push_back(std::move(pSmokeParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SMOKEPARTICLE, m_pListeners.back().get(), this);

	// TERRAIN SPRITE  ANIMATION
	std::unique_ptr<TerrainSpriteComponent> pTerrainSpriteComponent = std::make_unique<TerrainSpriteComponent>();
	m_pListeners.push_back(std::move(pTerrainSpriteComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SPRITE, m_pListeners.back().get(), this);

	// TRAIL ANIMATION
	std::unique_ptr<TrailParticleComponent> pTrailParticlenComponent = std::make_unique<TrailParticleComponent>();
	m_pListeners.push_back(std::move(pTrailParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_TRAILPARTICLE, m_pListeners.back().get(), this);

	// TRAIL Swipe
	std::unique_ptr<TrailComponent> pTrailComponent = std::make_unique<TrailComponent>();
	m_pListeners.push_back(std::move(pTrailComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SWORDTRAIL, m_pListeners.back().get(), this);
}

void Atk_Player::SetPlayerRootPos(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->SetPosition(xmf3Position);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}

void Atk_Player::CheckHitLag(CPlayer* player)
{
	HitLagComponent* pHitLagComponent = dynamic_cast<HitLagComponent*>(GetHitLagComponent());
	if (player->m_fCurLagTime > pHitLagComponent->GetMaxLagTime())
	{
		PlayerParams PlayerParam{ player };
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::UPDATE_HITLAG, &PlayerParam, this);
	}
}

void Atk_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	CAnimationSet* pAnimationSet = pPlayerController->m_pAnimationSets->m_pAnimationSets[pPlayerController->m_pAnimationTracks[0].m_nAnimationSet];

	XMFLOAT3 xmf3CurrentPos = XMFLOAT3{ player->m_pChild->m_xmf4x4Transform._41,
	0.0f,
	player->m_pChild->m_xmf4x4Transform._43 };

	player->m_pChild->m_xmf4x4Transform._41 -= player->m_xmf3RootTransfromPreviousPos.x;
	player->m_pChild->m_xmf4x4Transform._42 -= player->m_xmf3RootTransfromPreviousPos.y;
	player->m_pChild->m_xmf4x4Transform._43 -= player->m_xmf3RootTransfromPreviousPos.z;

	SetPlayerRootPos(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

void Atk_Player::Exit(CPlayer* player)
{
}



Atk1_Player::Atk1_Player()
{
	InitAtkPlayer();
}

Atk1_Player::~Atk1_Player()
{
}

void Atk1_Player::CheckComboAttack(CPlayer* player)
{
	// ����ڰ� ��Ŭ���� ������ �ִϸ��̼��� 0.7�� ���� �� Atk2_Player�� ���� ����
	if (0.55 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;
	}
	if (0.7 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;
		if (player->m_bAttack) {
			CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
			/*if (pPlayerController->m_bRootMotion)
				SetPlayerRootPos(player);*/
			player->m_pStateMachine->ChangeState(Atk2_Player::GetInst());
		}
	}
}

void Atk1_Player::SendCollisionMessage(CPlayer* player)
{
	// �浹 ���� �޼��� ����
	if (0.365f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.5f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);

	}
}

void Atk1_Player::SpawnTrailParticle(CPlayer* player)
{
	if (player->m_fTime > player->m_fMaxTime)
	{
		player->m_fMaxTime = 0.01f;
		// 0.35 ����
		if (0.35f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
			0.5f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
		{
			ParticleTrailParams ParticleTrail_comp_params;
			CGameObject* pWeapon = player->FindFrame("Weapon_r");
			pWeapon->FindFrame("Weapon_r");

			XMFLOAT3 xmf3Position;
			XMFLOAT3 xmf3Direction;
			memcpy(&xmf3Position, &(pWeapon->m_xmf4x4World._41), sizeof(XMFLOAT3));
			memcpy(&xmf3Direction, &(pWeapon->m_xmf4x4World._31), sizeof(XMFLOAT3));

			XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();

			xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };
			xmf3Direction = player->GetATKDirection();

			xmf3Position = ((CKnightPlayer*)player)->GetWeaponMeshBoundingBox().Center;
			xmf3Position = Vector3::Add(xmf3Position, Vector3::Normalize(xmf3Direction), -4.f);

			ParticleTrail_comp_params.pObject = CPlayerParticleObject::GetInst()->GetTrailParticleObjects();
			ParticleTrail_comp_params.xmf3Position = xmf3Position;
			ParticleTrail_comp_params.iPlayerAttack = 0;
			ParticleTrail_comp_params.m_fTime = player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
			ParticleTrail_comp_params.xmf3Velocity = Vector3::Normalize(Vector3::Subtract(xmf3Position, player->GetPosition()));
			CMessageDispatcher::GetInst()->Dispatch_Message<ParticleTrailParams>(MessageType::UPDATE_TRAILPARTICLE, &ParticleTrail_comp_params, player->m_pStateMachine->GetCurrentState());
		}

		player->m_fTime = 0.f;
	}
}

void Atk1_Player::Enter(CPlayer* player)
{
	CLogger::GetInst()->Log(std::string("Player Enter Atk1"));
	player->m_iAttackId += 1;

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // ����ڰ� ��Ŭ���� true�� �Ǵ� ����
	player->m_fCMDConstant = 1.0f;

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	player->m_fCurLagTime = 0.f;
	player->m_fAnimationPlayWeight = 1.0f;

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);


	TrailUpdateParams trailParam;
	trailParam.pObject = player->m_pSwordTrailReference[0].get();
	CMessageDispatcher::GetInst()->Dispatch_Message<TrailUpdateParams>(MessageType::UPDATE_SWORDTRAIL, &trailParam, this);

	if (player->m_pSwordTrailReference) {
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_faccumulateTime = 0.0f;
	}
}

void Atk1_Player::Execute(CPlayer* player, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	SendCollisionMessage(player);
	CheckComboAttack(player);
	CheckHitLag(player);

	CheckEvasion(player, 0.7f);

	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	SpawnTrailParticle(player);
}

void Atk1_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime * player->GetAnimationPlayWeight());
}

void Atk1_Player::Exit(CPlayer* player)
{
	if (player->m_pCamera)
	{
		player->m_pCamera->m_bCameraShaking = false;
		GetShakeAnimationComponent()->Reset();
		player->m_pCamera->m_bCameraZooming = false;
		GetCameraZoomerComponent()->Reset();
		player->m_pCamera->m_bCameraMoving = false;
		GetCameraMoveComponent()->Reset();
	}
	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;

	Atk_Player::Exit(player);
}

Atk2_Player::Atk2_Player()
{
	InitAtkPlayer();
}

Atk2_Player::~Atk2_Player()
{
}

void Atk2_Player::CheckComboAttack(CPlayer* player)
{
	if (0.45 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;
	}
	// ����ڰ� ��Ŭ���� ������ �ִϸ��̼��� 0.7�� ���� �� Atk2_Player�� ���� ����
	if (0.7 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;
		if (player->m_bAttack)
		{
			CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
			/*if (pPlayerController->m_bRootMotion)
				SetPlayerRootPos(player);*/
			player->m_pStateMachine->ChangeState(Atk3_Player::GetInst());
		}
	}
}

void Atk2_Player::SendCollisionMessage(CPlayer* player)
{
	// �浹 ���� �޼��� ����
	if (0.35f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.4f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}
}

void Atk2_Player::Enter(CPlayer* player)
{
	CLogger::GetInst()->Log(std::string("Player Enter Atk2"));
	player->m_iAttackId += 1;
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.2f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // ����ڰ� ��Ŭ���� true�� �Ǵ� ����
	player->m_fCMDConstant = 1.0f;

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	player->m_fCurLagTime = 0.f;
	player->m_fAnimationPlayWeight = 1.0f;

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	TrailUpdateParams trailParam;
	trailParam.pObject = player->m_pSwordTrailReference[1].get();
	CMessageDispatcher::GetInst()->Dispatch_Message<TrailUpdateParams>(MessageType::UPDATE_SWORDTRAIL, &trailParam, this);

	if (player->m_pSwordTrailReference) {
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_faccumulateTime = 0.0f;
	}
}

void Atk2_Player::Execute(CPlayer* player, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	SendCollisionMessage(player);
	CheckComboAttack(player);
	CheckHitLag(player);

	CheckEvasion(player, 0.7f);

	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	SpawnTrailParticle(player);
}

void Atk2_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime * player->GetAnimationPlayWeight());
}

void Atk2_Player::Exit(CPlayer* player)
{
	if (player->m_pCamera)
	{
		player->m_pCamera->m_bCameraShaking = false;
		GetShakeAnimationComponent()->Reset();
		player->m_pCamera->m_bCameraZooming = false;
		GetCameraZoomerComponent()->Reset();
		player->m_pCamera->m_bCameraMoving = false;
		GetCameraMoveComponent()->Reset();
	}

	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;

	Atk_Player::Exit(player);
}

void Atk2_Player::SpawnTrailParticle(CPlayer* player)
{
	if (player->m_fTime > player->m_fMaxTime)
	{
		player->m_fMaxTime = 0.05f; // 0.05
		if (0.3f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
			0.45f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
		{
			//0.3~0.6, 065
			ParticleTrailParams ParticleTrail_comp_params;
			CGameObject* pWeapon = player->FindFrame("Weapon_r");
			pWeapon->FindFrame("Weapon_r");
			XMFLOAT3 xmf3Position;
			XMFLOAT3 xmf3Direction;
			XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();
			xmf3Position = ((CKnightPlayer*)player)->GetWeaponMeshBoundingBox().Center;

			xmf3Direction = player->GetATKDirection();
			xmf3Position = Vector3::Add(xmf3Position, Vector3::Normalize(xmf3Direction), 4.2f);

			ParticleTrail_comp_params.pObject = CPlayerParticleObject::GetInst()->GetTrailParticleObjects();
			ParticleTrail_comp_params.xmf3Position = xmf3Position;
			ParticleTrail_comp_params.iPlayerAttack = 1;
			ParticleTrail_comp_params.m_fTime = player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
			CMessageDispatcher::GetInst()->Dispatch_Message<ParticleTrailParams>(MessageType::UPDATE_TRAILPARTICLE, &ParticleTrail_comp_params, player->m_pStateMachine->GetCurrentState());
		}

		player->m_fTime = 0.f;
	}
}

Atk3_Player::Atk3_Player()
{
	InitAtkPlayer();
}

Atk3_Player::~Atk3_Player()
{
}

void Atk3_Player::CheckComboAttack(CPlayer* player)
{
	// ����ڰ� ��Ŭ���� ������ �ִϸ��̼��� 0.7�� ���� �� Atk2_Player�� ���� ����
	/*if (0.7 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_bIsUpdateTrailVariables = false;
		if (player->m_bAttack)
			player->m_pStateMachine->ChangeState(Atk4_Player::GetInst());
	}*/
}

void Atk3_Player::SendCollisionMessage(CPlayer* player)
{
	// �浹 ���� �޼��� ����
	if (0.375f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.475f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}
}

void Atk3_Player::Enter(CPlayer* player)
{
	CLogger::GetInst()->Log(std::string("Player Enter Atk3"));
	player->m_iAttackId += 1;
	player->m_fCMDConstant = 1.0f;

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // ����ڰ� ��Ŭ���� true�� �Ǵ� ����

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	player->m_fCurLagTime = 0.f;
	player->m_fAnimationPlayWeight = 1.0f;

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	TrailUpdateParams trailParam;
	trailParam.pObject = player->m_pSwordTrailReference[2].get();
	CMessageDispatcher::GetInst()->Dispatch_Message<TrailUpdateParams>(MessageType::UPDATE_SWORDTRAIL, &trailParam, this);

	if (player->m_pSwordTrailReference) {
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_faccumulateTime = 0.0f;
	}
}

void Atk3_Player::Execute(CPlayer* player, float fElapsedTime)
{
	SendCollisionMessage(player);
	CheckComboAttack(player);
	CheckHitLag(player);

	CheckEvasion(player, 0.7f);

	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	if (0.45 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;
	}
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition > 0.65f)
		if (player->m_pSwordTrailReference) {
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;
		}

	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	else if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
	}
}

void Atk3_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime * player->GetAnimationPlayWeight());
}

void Atk3_Player::Exit(CPlayer* player)
{
	if (player->m_pCamera)
	{
		player->m_pCamera->m_bCameraShaking = false;
		GetShakeAnimationComponent()->Reset();
		player->m_pCamera->m_bCameraZooming = false;
		GetCameraZoomerComponent()->Reset();
		player->m_pCamera->m_bCameraMoving = false;
		GetCameraMoveComponent()->Reset();
	}

	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;

	Atk_Player::Exit(player);
}

void Atk3_Player::SpawnTrailParticle(CPlayer* player)
{
}

Run_Player::Run_Player()
{
	// SMOKEPARTICLE ANIMATION
	std::unique_ptr<SmokeParticleComponent> pSmokeParticlenComponent = std::make_unique<SmokeParticleComponent>();
	m_pListeners.push_back(std::move(pSmokeParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SMOKEPARTICLE, m_pListeners.back().get(), this);
}

Run_Player::~Run_Player()
{
}

void Run_Player::Enter(CPlayer* player)
{
	//player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 19);
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 16);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
	player->m_bAttack = false; // ����ڰ� ��Ŭ���� true�� �Ǵ� ����
	player->m_fAnimationPlayWeight = 1.0f;

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

}

void Run_Player::Execute(CPlayer* player, float fElapsedTime)
{
	XMFLOAT3 xmf3PlayerVel = player->GetVelocity();

	if (player->m_bAttack)
		player->m_pStateMachine->ChangeState(Atk1_Player::GetInst());

	if (player->m_bEvasioned)
		player->m_pStateMachine->ChangeState(Evasion_Player::GetInst());

	// Idle ���·� �����ϴ� �ڵ�
	if (Vector3::Length(xmf3PlayerVel) == 0)
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());

	// �߹ٴ� ����

	float fTime = player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
	XMFLOAT3 xmf3Position;
	ParticleSmokeParams Particlesmoke_comp_params;
	Particlesmoke_comp_params.pObject = CPlayerParticleObject::GetInst()->GetSmokeObjects();
	if (Particlesmoke_comp_params.pObject && (((int)(fTime * 10.f)) == 9)) // 0.5f����
	{
		CGameObject* pFoot_Left = player->FindFrame("foot_l");
		memcpy(&xmf3Position, &(pFoot_Left->m_xmf4x4World._41), sizeof(XMFLOAT3));
		Particlesmoke_comp_params.xmf3Position = xmf3Position;
		Particlesmoke_comp_params.xmfDirection = player->m_xmfDirection;

		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleSmokeParams>(MessageType::UPDATE_SMOKEPARTICLE, &Particlesmoke_comp_params, player->m_pStateMachine->GetCurrentState());
	}

	if (Particlesmoke_comp_params.pObject && (((int)(fTime * 10.f)) == 2)) // 0.6f����
	{
		CGameObject* pFoot_Right = player->FindFrame("foot_r");
		memcpy(&xmf3Position, &(pFoot_Right->m_xmf4x4World._41), sizeof(XMFLOAT3));
		Particlesmoke_comp_params.xmf3Position = xmf3Position;
		Particlesmoke_comp_params.xmfDirection = player->m_xmfDirection;

		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleSmokeParams>(MessageType::UPDATE_SMOKEPARTICLE, &Particlesmoke_comp_params, player->m_pStateMachine->GetCurrentState());
	}
}

void Run_Player::Animate(CPlayer* player, float fTimeElapsed)
{
	player->Animate(fTimeElapsed * player->GetAnimationPlayWeight());
}

void Run_Player::Exit(CPlayer* player)
{
}

void Run_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._42 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}

Atk4_Player::Atk4_Player()
{

}

Atk4_Player::~Atk4_Player()
{
}

void Atk4_Player::Enter(CPlayer* player)
{
	player->m_fCMDConstant = 1.0f;

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 3);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // ����ڰ� ��Ŭ���� true�� �Ǵ� ����

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	player->m_fCurLagTime = 0.f;
	player->m_fAnimationPlayWeight = 1.0f;

	if (player->m_pSwordTrailReference) {
		//dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_bIsUpdateTrailVariables = true;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_faccumulateTime = 0.0f;
	}

	/*SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);*/
}

void Atk4_Player::CheckComboAttack(CPlayer* player)
{
}

void Atk4_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// �浹 ���� �޼��� ����
	if (0.3 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.5 > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}

	if (0.7 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		/*if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_bIsUpdateTrailVariables = false;*/
		if (player->m_bAttack)
			player->m_pStateMachine->ChangeState(Atk5_Player::GetInst());
	}

	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	else if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{

	}
}

void Atk4_Player::SendCollisionMessage(CPlayer* player)
{
}

void Atk4_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime * player->GetAnimationPlayWeight());
}

void Atk4_Player::Exit(CPlayer* player)
{
	/*if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_bIsUpdateTrailVariables = false;*/
}

void Atk4_Player::SpawnTrailParticle(CPlayer* player)
{
}

Atk5_Player::Atk5_Player()
{
}

Atk5_Player::~Atk5_Player()
{
}

void Atk5_Player::Enter(CPlayer* player)
{
	player->m_fCMDConstant = 1.0f;

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 7);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // ����ڰ� ��Ŭ���� true�� �Ǵ� ����

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	player->m_fCurLagTime = 0.f;
	player->m_fAnimationPlayWeight = 1.0f;

	if (player->m_pSwordTrailReference) {
		//dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[4].get())->m_bIsUpdateTrailVariables = true;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[4].get())->m_faccumulateTime = 0.0f;
	}

	/*SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);*/
}

void Atk5_Player::SpawnTrailParticle(CPlayer* player)
{
}

void Atk5_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// �浹 ���� �޼��� ����
	if (0.3 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.5 > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}

	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	else if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{

	}
}

void Atk5_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime * player->GetAnimationPlayWeight());
}

void Atk5_Player::Exit(CPlayer* player)
{
	/*if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[4].get())->m_bIsUpdateTrailVariables = false;*/
}

void Atk5_Player::CheckComboAttack(CPlayer* player)
{
}

void Atk5_Player::SendCollisionMessage(CPlayer* player)
{
}

Evasion_Player::Evasion_Player()
{
}

Evasion_Player::~Evasion_Player()
{
}

void Evasion_Player::Enter(CPlayer* player)
{
	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	player->m_fCMDConstant = 1.0f;

	player->m_bEvasioned = false;

	DWORD dwDirection = player->m_dwDirectionCache;
	XMFLOAT3 xmf3Direction = player->m_xmf3DirectionCache; xmf3Direction = Vector3::Normalize(xmf3Direction);
	XMFLOAT3 xmf3CameraDirection = player->m_pCamera->GetLookVector();
	xmf3CameraDirection.y = 0.0f; xmf3CameraDirection = Vector3::Normalize(xmf3CameraDirection);

	float ceta_Player_and_Camera = Vector3::DotProduct(xmf3Direction, xmf3CameraDirection);
	float ceta_Player_and_CameraCross = Vector3::DotProduct(xmf3Direction, Vector3::CrossProduct(player->GetUp(), xmf3CameraDirection));

	float drgree_45 = 45.0f * XM_PI / 180.0f;
	float drgree_135 = 135.0f * XM_PI / 180.0f;

	CState<CPlayer>& previousState = *player->m_pStateMachine->GetPreviousState();
	if (dynamic_cast<Idle_Player*>(&previousState)) {
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
	}
	else if (dynamic_cast<Run_Player*>(&previousState)) {
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
	}
	else {
		if (ceta_Player_and_Camera > 0.000001f) {
			if (ceta_Player_and_CameraCross > 0.000001f) {
				// ���� �� ��и�
				float ceta = XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3Direction), XMLoadFloat3(&xmf3CameraDirection)).m128_f32[0];
				if (ceta > drgree_45) { // ���� �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 14);
				}
				else { // ���� �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
				}
			}
			else {
				// ������ �� ��и�
				float ceta = XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3Direction), XMLoadFloat3(&xmf3CameraDirection)).m128_f32[0];
				if (ceta > drgree_45) { // ������ �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 13);
				}
				else { // ���� �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
				}
			}
		}
		else if (ceta_Player_and_Camera < 0.000001f) {
			if (ceta_Player_and_CameraCross > 0.000001f) {
				// ���� �� ��и�
				float ceta = XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3Direction), XMLoadFloat3(&xmf3CameraDirection)).m128_f32[0];
				if (ceta > drgree_135) { // �Ĺ� �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 11);
				}
				else { // ���� �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 14);
				}
			}
			else {
				// ������ �� ��и�
				float ceta = XMVector3AngleBetweenVectors(XMLoadFloat3(&xmf3Direction), XMLoadFloat3(&xmf3CameraDirection)).m128_f32[0];
				if (ceta > drgree_135) { // �Ĺ� �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 11);
				}
				else { // ������ �ִϸ��̼�
					player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 13);
				}
			}
		}
		else {

		}
	}

	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;

	player->m_fCurLagTime = 0.f;
	player->m_fAnimationPlayWeight = 1.0f;
}

void Evasion_Player::Execute(CPlayer* player, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		CState<CPlayer>& previousState = *player->m_pStateMachine->GetPreviousState();
		if (dynamic_cast<Idle_Player*>(&previousState)) {
			player->m_pStateMachine->ChangeState(&previousState);
		}
		else if (dynamic_cast<Run_Player*>(&previousState)) {
			player->m_pStateMachine->ChangeState(&previousState);
		}
		else {
			player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
		}
	}
}

void Evasion_Player::Exit(CPlayer* player)
{

}

void Evasion_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime * player->GetAnimationPlayWeight());
}

void Evasion_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	CAnimationSet* pAnimationSet = pPlayerController->m_pAnimationSets->m_pAnimationSets[pPlayerController->m_pAnimationTracks[0].m_nAnimationSet];

	XMFLOAT3 xmf3CurrentPos = XMFLOAT3{ player->m_pChild->m_xmf4x4Transform._41,
	0.0f,
	player->m_pChild->m_xmf4x4Transform._43 };

	player->m_pChild->m_xmf4x4Transform._41 -= player->m_xmf3RootTransfromPreviousPos.x;
	player->m_pChild->m_xmf4x4Transform._42 -= player->m_xmf3RootTransfromPreviousPos.y;
	player->m_pChild->m_xmf4x4Transform._43 -= player->m_xmf3RootTransfromPreviousPos.z;

	SetPlayerRootPos(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

void Evasion_Player::SetPlayerRootPos(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->SetPosition(xmf3Position);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}

void Atk_Player::CheckEvasion(CPlayer* player, float holdTime)
{
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition > holdTime)
	{
		if (player->m_bEvasioned)
			player->m_pStateMachine->ChangeState(Evasion_Player::GetInst());
	}
}
