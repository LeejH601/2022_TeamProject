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
#include "..\Scene\SimulatorScene.h"

#define ANGLE_MARGIN 22.5f

Idle_Player::Idle_Player()
{
}

Idle_Player::~Idle_Player()
{
}

void Idle_Player::Enter(CPlayer* player)
{
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4); // 36 potion
	player->m_pSkinnedAnimationController->SetTrackWeight(0, 1.0);
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 2);
	player->m_pSkinnedAnimationController->SetTrackWeight(1, 0.0);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fSequenceWeight = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;

	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_fSequenceWeight = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_nType = ANIMATION_TYPE_LOOP;

	//player->m_pSkinnedAnimationController->m_pSubAnimationTracks[0].m_bEnable = TRUE;



	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
}

void Idle_Player::Execute(CPlayer* player, float fElapsedTime)
{
	if (player->m_bEvasioned)
		player->m_pStateMachine->ChangeState(Evasion_Player::GetInst());
	// 사용자가 좌클릭을 했으면 Atk1_Player로 상태 변경
	else if (player->m_bAttack && player->m_fStamina >= 5.0f)
		player->m_pStateMachine->ChangeState(Atk1_Player::GetInst());
	else if (player->m_bCharged)
		player->m_pStateMachine->ChangeState(ChargeStart_Player::GetInst());
}

void Idle_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
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
	/*CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
	pPlayerController->m_pSubAnimationTracks[0].SetEnable(false);*/
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
	pHitLagComponent->SetLagScale(0.5f);
	pHitLagComponent->SetDuration(0.5f);
	pHitLagComponent->SetMinTimeScale(0.5f);
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
	pTrailParticlenComponent->SetEnable(true);
	pTrailParticlenComponent->SetTextureOffset(5);
	pTrailParticlenComponent->SetSpeed(15.0f);
	pTrailParticlenComponent->SetSize(XMFLOAT2(0.2, 0.2));
	pTrailParticlenComponent->SetEmitParticleNumber(50);
	pTrailParticlenComponent->SetEmissive(20.0f);
	pTrailParticlenComponent->SetLifeTime(0.4f);
	pTrailParticlenComponent->SetFieldSpeed(0.5);
	m_pListeners.push_back(std::move(pTrailParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_TRAILPARTICLE, m_pListeners.back().get(), this);

	// TRAIL Swipe
	std::unique_ptr<TrailComponent> pTrailComponent = std::make_unique<TrailComponent>();
	m_pTrailComponent = pTrailComponent.get();
	m_pListeners.push_back(std::move(pTrailComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SWORDTRAIL, m_pListeners.back().get(), this);

	std::unique_ptr<SlashHitComponent> pSlashHitComponent = std::make_unique<SlashHitComponent>();
	m_pListeners.push_back(std::move(pSlashHitComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SLASHHITPARTICLE, m_pListeners.back().get(), this);
}

void Atk_Player::SetPlayerRootVel(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->Move(Vector3::Subtract(xmf3Position, player->GetPosition()), true);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
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

	SetPlayerRootVel(player);

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
	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	if (0.55 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference) {
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;
		}
	}
	if (0.7 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;
		if (player->m_bAttack && player->m_fStamina >= 5.0f) {
			CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
			player->m_pStateMachine->ChangeState(Atk2_Player::GetInst());
		}
	}
}

void Atk1_Player::SendCollisionMessage(CPlayer* player)
{
	// 충돌 판정 메세지 전달
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
	if (m_bEmittedParticle)
		return;
	if (player->m_fTime > player->m_fMaxTime)
	{
		player->m_fMaxTime = 0.01f;
		// 0.35 시작
		if (0.45f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
			0.7f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
		{
			ParticleTrailParams ParticleTrail_comp_params;
			CGameObject* pWeapon = player->FindFrame("Weapon_r");
			pWeapon->FindFrame("Weapon_r");

			XMFLOAT3 xmf3Position;
			XMFLOAT3 xmf3Direction;
			memcpy(&xmf3Position, &(pWeapon->m_xmf4x4World._41), sizeof(XMFLOAT3));
			memcpy(&xmf3Direction, &(pWeapon->m_xmf4x4World._21), sizeof(XMFLOAT3));

			XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();

			xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };
			XMFLOAT4 dir = dynamic_cast<CKnightPlayer*>(player)->GetTrailControllPoint(1);


			xmf3Position = ((CKnightPlayer*)player)->GetWeaponMeshBoundingBox().Center;
			xmf3Position = XMFLOAT3(dir.x, dir.y, dir.z);

			ParticleTrail_comp_params.pObject = CPlayerParticleObject::GetInst()->GetTrailParticleObjects();
			dynamic_cast<CParticleObject*>(ParticleTrail_comp_params.pObject)->SetEmitAxis(xmf3Direction);
			ParticleTrail_comp_params.xmf3Position = xmf3Position;
			ParticleTrail_comp_params.iPlayerAttack = 0;
			ParticleTrail_comp_params.m_fTime = player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
			//ParticleTrail_comp_params.xmf3Velocity = Vector3::Normalize(Vector3::Subtract(xmf3Position, player->GetPosition()));
			CMessageDispatcher::GetInst()->Dispatch_Message<ParticleTrailParams>(MessageType::UPDATE_TRAILPARTICLE, &ParticleTrail_comp_params, this);
			m_bEmittedParticle = true;
		}

		player->m_fTime = 0.f;
	}
}

void Atk_Player::ResetComponents()
{
	SoundPlayComponent* pShootSoundSoundComponent = dynamic_cast<SoundPlayComponent*>(GetShootSoundComponent());
	pShootSoundSoundComponent->SetEnable(false);
	pShootSoundSoundComponent->SetSoundNumber(0);
	pShootSoundSoundComponent->SetDelay(0.0f);
	pShootSoundSoundComponent->SetVolume(1.25f);
	pShootSoundSoundComponent->SetMT(MONSTER_TYPE::NONE);

	SoundPlayComponent* pShockSoundSoundComponent = dynamic_cast<SoundPlayComponent*>(GetShockSoundComponent());
	pShockSoundSoundComponent->SetEnable(false);
	pShockSoundSoundComponent->SetSoundNumber(0);
	pShockSoundSoundComponent->SetDelay(0.0f);
	pShockSoundSoundComponent->SetVolume(0.75f);

	SoundPlayComponent* pGoblinMoanSoundComponent = dynamic_cast<SoundPlayComponent*>(GetGoblinMoanComponent());
	pGoblinMoanSoundComponent->SetEnable(false);
	pGoblinMoanSoundComponent->SetSoundNumber(0);
	pGoblinMoanSoundComponent->SetDelay(0.0f);
	pGoblinMoanSoundComponent->SetVolume(0.75f);

	SoundPlayComponent* pOrcMoanSoundComponent = dynamic_cast<SoundPlayComponent*>(GetOrcMoanComponent());
	pOrcMoanSoundComponent->SetEnable(false);
	pOrcMoanSoundComponent->SetSoundNumber(0);
	pOrcMoanSoundComponent->SetDelay(0.0f);
	pOrcMoanSoundComponent->SetVolume(0.75f);

	SoundPlayComponent* pSkeletonMoanSoundComponent = dynamic_cast<SoundPlayComponent*>(GetSkeletonMoanComponent());
	pSkeletonMoanSoundComponent->SetEnable(false);
	pSkeletonMoanSoundComponent->SetSoundNumber(0);
	pSkeletonMoanSoundComponent->SetDelay(0.0f);
	pSkeletonMoanSoundComponent->SetVolume(0.75f);

	CameraShakeComponent* pCameraShakeComponent = dynamic_cast<CameraShakeComponent*>(GetCameraShakeComponent());
	pCameraShakeComponent->SetEnable(false);
	pCameraShakeComponent->SetDuration(1.0f);
	pCameraShakeComponent->SetMagnitude(0.5f);

	CameraZoomerComponent* pCameraZoomComponent = dynamic_cast<CameraZoomerComponent*>(GetCameraZoomerComponent());
	pCameraZoomComponent->SetEnable(false);
	pCameraZoomComponent->SetIsIn(false);
	pCameraZoomComponent->SetMaxDistance(10.0f);
	pCameraZoomComponent->SetMovingTime(0.25f);
	pCameraZoomComponent->SetRollBackTime(0.25f);

	CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(GetCameraMoveComponent());
	pCameraMoveComponent->SetEnable(false);
	pCameraMoveComponent->SetMaxDistance(5.0f);
	pCameraMoveComponent->SetMovingTime(0.5f);
	pCameraMoveComponent->SetRollBackTime(0.5f);

	DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(GetDamageAnimationComponent());
	pDamageAnimationComponent->SetEnable(false);
	pDamageAnimationComponent->SetMaxDistance(15.0f);
	pDamageAnimationComponent->SetSpeed(100.0f);
	
	ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(GetShakeAnimationComponent());
	pShakeAnimationComponent->SetEnable(false);
	pShakeAnimationComponent->SetDistance(0.25f);
	pShakeAnimationComponent->SetFrequency(0.01f);
	
	StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(GetStunAnimationComponent());
	pStunAnimationComponent->SetEnable(false);
	pStunAnimationComponent->SetStunTime(0.5f);
	
	HitLagComponent* pHitLagComponent = dynamic_cast<HitLagComponent*>(GetHitLagComponent());
	pHitLagComponent->SetEnable(false);
	pHitLagComponent->SetLagScale(0.5f);
	pHitLagComponent->SetDuration(0.5f);
	pHitLagComponent->SetMinTimeScale(0.5f);

	ParticleComponent* pParticleComponent = dynamic_cast<ParticleComponent*>(GetParticleComponent());
	pParticleComponent->SetEnable(false);
	pParticleComponent->Reset();

	ImpactEffectComponent* pImpactComponent = dynamic_cast<ImpactEffectComponent*>(GetImpactComponent());
	pImpactComponent->SetEnable(false);
	pImpactComponent->Reset();

	// TRAIL ANIMATION
	TrailComponent* pTrailComponent = dynamic_cast<TrailComponent*>(GetTrailComponent());
	pTrailComponent->SetEnable(false);
	pTrailComponent->Reset();

	SlashHitComponent* pSlashHitComponent = dynamic_cast<SlashHitComponent*>(GetSlashHitComponent());
	pSlashHitComponent->SetEnable(false);
	pSlashHitComponent->Reset();
}

void Atk1_Player::Enter(CPlayer* player)
{
	CLogger::GetInst()->Log(std::string("Player Enter Atk1"));

	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
	pPlayerController->m_pSubAnimationTracks[0].SetEnable(false);

	player->m_iAttackId += 1;

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	player->m_pSkinnedAnimationController->SetTrackWeight(0, 1);
	player->m_pSkinnedAnimationController->SetTrackWeight(1, 0);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fSequenceWeight = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_fCMDConstant = 1.0f;


	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	m_bEmittedParticle = false;

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);
	OutputDebugString(L"Shoot Sound\n");

	TrailUpdateParams trailParam;
	trailParam.pObject = player->m_pSwordTrailReference[0].get();
	CMessageDispatcher::GetInst()->Dispatch_Message<TrailUpdateParams>(MessageType::UPDATE_SWORDTRAIL, &trailParam, this);

	if (player->m_pSwordTrailReference) {
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_faccumulateTime = 0.0f;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->SetLengthWeight(1.0f);
	}

	player->m_fStamina -= 5.0f;
	player->m_fStamina = max(0.0f, player->m_fStamina);
}

void Atk1_Player::Execute(CPlayer* player, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	SendCollisionMessage(player);
	CheckComboAttack(player);

	if (0.55 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference) {
			float scale = dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->GetLengthWeight();
			scale += fElapsedTime * 1.0f;
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->SetLengthWeight(scale);
		}
	}

	CheckEvasion(player, 0.7f);

	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		if (player->m_bAutoAttack)
			player->m_pStateMachine->ChangeState(Atk1_Player::GetInst());
		else
			player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	if (m_pTrailComponent->GetEnable())
		SpawnTrailParticle(player);

}

void Atk1_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void Atk1_Player::Exit(CPlayer* player)
{
	CLogger::GetInst()->Log(std::string("Player Exit Atk1"));

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
	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	if (0.7 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;
		if (player->m_bAttack && player->m_fStamina >= 10.0f)
		{
			CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
			player->m_pStateMachine->ChangeState(Atk3_Player::GetInst());
		}
	}
}

void Atk2_Player::SendCollisionMessage(CPlayer* player)
{
	// 충돌 판정 메세지 전달
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

	float prevAnimPosition = player->m_pSkinnedAnimationController->GetTrackPosition(0);

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	player->m_pSkinnedAnimationController->SetTrackWeight(0, 0);

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 0);
	player->m_pSkinnedAnimationController->SetTrackPosition(1, prevAnimPosition);
	player->m_pSkinnedAnimationController->SetTrackWeight(1, 1);
	player->m_pSkinnedAnimationController->SetTrackUpdateEnable(1, false);

	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.2f;
	//player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fSequenceWeight = 0.0f / ;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_fCMDConstant = 1.0f;

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	m_bEmittedParticle = false;

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	TrailUpdateParams trailParam;
	trailParam.pObject = player->m_pSwordTrailReference[1].get();
	CMessageDispatcher::GetInst()->Dispatch_Message<TrailUpdateParams>(MessageType::UPDATE_SWORDTRAIL, &trailParam, this);

	if (player->m_pSwordTrailReference) {
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_faccumulateTime = 0.0f;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->SetLengthWeight(1.0f);
	}

	player->m_fStamina -= 10.0f;
	player->m_fStamina = max(0.0f, player->m_fStamina);
}

void Atk2_Player::Execute(CPlayer* player, float fElapsedTime)
{
	float weight1 = player->m_pSkinnedAnimationController->GetTrackWeight(0);
	if (weight1 < 1.0f) {
		weight1 += fElapsedTime * 10.0f;
		weight1 = min(1.0f, weight1);
		player->m_pSkinnedAnimationController->SetTrackWeight(0, weight1);
		player->m_pSkinnedAnimationController->SetTrackWeight(1, 1.0f - weight1);
	}
	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	SendCollisionMessage(player);
	CheckComboAttack(player);

	if (0.45 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference) {
			float scale = dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->GetLengthWeight();
			scale += fElapsedTime * 1.0f;
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->SetLengthWeight(scale);
		}
	}

	CheckEvasion(player, 0.7f);

	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		if (player->m_bAutoAttack)
			player->m_pStateMachine->ChangeState(Atk2_Player::GetInst());
		else
			player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	if (m_pTrailComponent->GetEnable())
		SpawnTrailParticle(player);
}

void Atk2_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
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
	if (m_bEmittedParticle)
		return;
	if (player->m_fTime > player->m_fMaxTime)
	{
		player->m_fMaxTime = 0.01f;
		// 0.35 시작
		if (0.3f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
			0.7f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
		{
			ParticleTrailParams ParticleTrail_comp_params;
			CGameObject* pWeapon = player->FindFrame("Weapon_r");
			pWeapon->FindFrame("Weapon_r");

			XMFLOAT3 xmf3Position;
			XMFLOAT3 xmf3Direction;
			memcpy(&xmf3Position, &(pWeapon->m_xmf4x4World._41), sizeof(XMFLOAT3));
			memcpy(&xmf3Direction, &(pWeapon->m_xmf4x4World._21), sizeof(XMFLOAT3));
			xmf3Direction = player->GetRight();

			XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();

			xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };
			XMFLOAT4 dir = dynamic_cast<CKnightPlayer*>(player)->GetTrailControllPoint(1);

			XMFLOAT3 center = player->GetBoundingBox()->Center;
			center = Vector3::Add(center, Vector3::Add(Vector3::ScalarProduct(player->GetLook(), 4.0f), player->GetRight(), 1.f), 1.0f);

			ParticleTrail_comp_params.pObject = CPlayerParticleObject::GetInst()->GetTrailParticleObjects();
			dynamic_cast<CParticleObject*>(ParticleTrail_comp_params.pObject)->SetEmitAxis(Vector3::ScalarProduct(xmf3Direction, -1.0f, false));
			ParticleTrail_comp_params.xmf3Position = center;
			ParticleTrail_comp_params.iPlayerAttack = 0;
			ParticleTrail_comp_params.m_fTime = player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
			//ParticleTrail_comp_params.xmf3Velocity = Vector3::Normalize(Vector3::Subtract(xmf3Position, player->GetPosition()));
			CMessageDispatcher::GetInst()->Dispatch_Message<ParticleTrailParams>(MessageType::UPDATE_TRAILPARTICLE, &ParticleTrail_comp_params, this);
			m_bEmittedParticle = true;
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
	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	/*if (0.7 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_bIsUpdateTrailVariables = false;
		if (player->m_bAttack)
			player->m_pStateMachine->ChangeState(Atk4_Player::GetInst());
	}*/
}

void Atk3_Player::SendCollisionMessage(CPlayer* player)
{
	// 충돌 판정 메세지 전달
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
	player->m_pSkinnedAnimationController->SetTrackWeight(0, 1);
	player->m_pSkinnedAnimationController->SetTrackWeight(1, 0);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fSequenceWeight = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	m_bEmittedParticle = false;

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	TrailUpdateParams trailParam;
	trailParam.pObject = player->m_pSwordTrailReference[2].get();
	CMessageDispatcher::GetInst()->Dispatch_Message<TrailUpdateParams>(MessageType::UPDATE_SWORDTRAIL, &trailParam, this);

	if (player->m_pSwordTrailReference) {
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_faccumulateTime = 0.0f;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->SetLengthWeight(1.0f);
	}

	player->m_fStamina -= 20.0f;
	player->m_fStamina = max(0.0f, player->m_fStamina);
}

void Atk3_Player::Execute(CPlayer* player, float fElapsedTime)
{
	SendCollisionMessage(player);
	CheckComboAttack(player);

	CheckEvasion(player, 0.7f);

	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	if (0.45 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference) {
			float scale = dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->GetLengthWeight();
			scale += fElapsedTime * 1.0f;
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->SetLengthWeight(scale);
		}
	}

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
		if (player->m_bAutoAttack)
			player->m_pStateMachine->ChangeState(Atk3_Player::GetInst());
		else
			player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	else if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
	}

	if (m_pTrailComponent->GetEnable())
		SpawnTrailParticle(player);
}

void Atk3_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
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
	if (m_bEmittedParticle)
		return;
	if (player->m_fTime > player->m_fMaxTime)
	{
		player->m_fMaxTime = 0.01f;
		// 0.45 시작
		if (0.35f < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
			0.7f > player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
		{
			ParticleTrailParams ParticleTrail_comp_params;
			CGameObject* pWeapon = player->FindFrame("Weapon_r");
			pWeapon->FindFrame("Weapon_r");

			XMFLOAT3 xmf3Position;
			XMFLOAT3 xmf3Direction;
			memcpy(&xmf3Position, &(pWeapon->m_xmf4x4World._41), sizeof(XMFLOAT3));
			memcpy(&xmf3Direction, &(pWeapon->m_xmf4x4World._21), sizeof(XMFLOAT3));
			xmf3Direction = player->GetRight();

			XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();

			xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };
			XMFLOAT4 dir = dynamic_cast<CKnightPlayer*>(player)->GetTrailControllPoint(1);


			XMFLOAT3 center = player->GetBoundingBox()->Center;
			center = Vector3::Add(center, Vector3::Add(Vector3::ScalarProduct(player->GetLook(), 4.0f), player->GetRight(), 1.f), 1.0f);

			ParticleTrail_comp_params.pObject = CPlayerParticleObject::GetInst()->GetTrailParticleObjects();
			dynamic_cast<CParticleObject*>(ParticleTrail_comp_params.pObject)->SetEmitAxis(Vector3::ScalarProduct(xmf3Direction, -1.0f, false));
			ParticleTrail_comp_params.xmf3Position = center;
			ParticleTrail_comp_params.iPlayerAttack = 0;
			ParticleTrail_comp_params.m_fTime = player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
			//ParticleTrail_comp_params.xmf3Velocity = Vector3::Normalize(Vector3::Subtract(xmf3Position, player->GetPosition()));
			CMessageDispatcher::GetInst()->Dispatch_Message<ParticleTrailParams>(MessageType::UPDATE_TRAILPARTICLE, &ParticleTrail_comp_params, this);
			m_bEmittedParticle = true;
		}

		player->m_fTime = 0.f;
	}
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
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 19);
	//player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 16);
	player->m_pSkinnedAnimationController->SetTrackWeight(0, 1.0f);
	player->m_pSkinnedAnimationController->SetTrackEnable(1, true);
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 4);
	player->m_pSkinnedAnimationController->SetTrackWeight(1, 0.0f);


	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;

	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_nType = ANIMATION_TYPE_LOOP;

	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };
}

void Run_Player::Execute(CPlayer* player, float fElapsedTime)
{
	static int animationList[4] = { 4, 19, 16, 0 };
	static float valuePoint[3] = { 0.0f, 12.22222 ,24.44444444 };
	static int prevAnimSetLow = 0;
	XMFLOAT3 xmf3PlayerVel = player->GetVelocity();

	player->Acceleration(fElapsedTime);

	float value = player->GetCurrSpeed() / valuePoint[2];
	value = value * 2.0f;
	int animSetLow = floor(value);

	if (prevAnimSetLow != animSetLow) {
		float trackZeroPos = player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_fPosition;
		player->m_pSkinnedAnimationController->m_pAnimationTracks[0].SetPosition(trackZeroPos);
	}

	prevAnimSetLow = animSetLow;

	//animSetLow = min(1, animSetLow);

	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].SetAnimationSet(animationList[animSetLow]);
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].SetAnimationSet(animationList[animSetLow + 1]);

	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fWeight = 1.0f - (value - (int)value);
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_fWeight = 1.0f - player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fWeight;

	if (player->m_bEvasioned)
		player->m_pStateMachine->ChangeState(Evasion_Player::GetInst());

	else if (player->m_bAttack)
		player->m_pStateMachine->ChangeState(Atk1_Player::GetInst());

	else if (player->m_bCharged)
		player->m_pStateMachine->ChangeState(ChargeStart_Player::GetInst());

	// Idle 상태로 복귀하는 코드
	else if (player->m_dwDirectionCache == 0) {
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
		player->SetCurrSpeed(0.0f);
	}




	// 발바닥 뼈에

	//float fTime = player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
	//XMFLOAT3 xmf3Position;
	//ParticleSmokeParams Particlesmoke_comp_params;
	//Particlesmoke_comp_params.pObject = CPlayerParticleObject::GetInst()->GetSmokeObjects();
	//if (Particlesmoke_comp_params.pObject && (((int)(fTime * 10.f)) == 9)) // 0.5f마다
	//{
	//	CGameObject* pFoot_Left = player->FindFrame("foot_l");
	//	memcpy(&xmf3Position, &(pFoot_Left->m_xmf4x4World._41), sizeof(XMFLOAT3));
	//	Particlesmoke_comp_params.xmf3Position = xmf3Position;
	//	Particlesmoke_comp_params.xmfDirection = player->m_xmfDirection;

	//	CMessageDispatcher::GetInst()->Dispatch_Message<ParticleSmokeParams>(MessageType::UPDATE_SMOKEPARTICLE, &Particlesmoke_comp_params, player->m_pStateMachine->GetCurrentState());
	//}

	//if (Particlesmoke_comp_params.pObject && (((int)(fTime * 10.f)) == 2)) // 0.6f마다
	//{
	//	CGameObject* pFoot_Right = player->FindFrame("foot_r");
	//	memcpy(&xmf3Position, &(pFoot_Right->m_xmf4x4World._41), sizeof(XMFLOAT3));
	//	Particlesmoke_comp_params.xmf3Position = xmf3Position;
	//	Particlesmoke_comp_params.xmfDirection = player->m_xmfDirection;

	//	CMessageDispatcher::GetInst()->Dispatch_Message<ParticleSmokeParams>(MessageType::UPDATE_SMOKEPARTICLE, &Particlesmoke_comp_params, player->m_pStateMachine->GetCurrentState());
	//}
}

void Run_Player::Animate(CPlayer* player, float fTimeElapsed)
{
	player->Animate(fTimeElapsed);
}

void Run_Player::Exit(CPlayer* player)
{
	/*CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
	pPlayerController->m_pSubAnimationTracks[0].SetEnable(false);*/
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
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	if (player->m_pSwordTrailReference) {
		//dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_bIsUpdateTrailVariables = true;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_faccumulateTime = 0.0f;
	}

	/*SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);*/

	player->m_fStamina -= player->m_fTotalStamina * 0.2f;
	if (player->m_fStamina < 0.f)
		player->m_fStamina = 0.f;
}

void Atk4_Player::CheckComboAttack(CPlayer* player)
{
}

void Atk4_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// 충돌 판정 메세지 전달
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
	player->Animate(fElapsedTime);
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
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	if (player->m_pSwordTrailReference) {
		//dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[4].get())->m_bIsUpdateTrailVariables = true;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[4].get())->m_faccumulateTime = 0.0f;
	}

	/*SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);*/

	player->m_fStamina -= player->m_fTotalStamina * 0.2f;
	if (player->m_fStamina < 0.f)
		player->m_fStamina = 0.f;
}

void Atk5_Player::SpawnTrailParticle(CPlayer* player)
{
}

void Atk5_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// 충돌 판정 메세지 전달
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
	player->Animate(fElapsedTime);
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
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();
	pPlayerController->m_pSubAnimationTracks[0].SetEnable(false);

	m_xmf3VelociyuCache = player->GetVelocity();
	m_fSpeedCache = player->GetCurrSpeed();
	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };

	player->m_fCMDConstant = 1.0f;

	float degrees = 0.0f;
	DWORD dwDirection = player->m_dwDirectionCache;
	XMFLOAT3 xmf3Direction = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	{

		if (dwDirection & DIR_FORWARD)xmf3Direction = Vector3::Add(xmf3Direction, Vector3::Normalize(XMFLOAT3(player->m_pCamera->GetLookVector().x, 0.0f, player->m_pCamera->GetLookVector().z)));
		if (dwDirection & DIR_BACKWARD)xmf3Direction = Vector3::Add(xmf3Direction, Vector3::Normalize(XMFLOAT3(player->m_pCamera->GetLookVector().x, 0.0f, player->m_pCamera->GetLookVector().z)), -1.0f);
		if (dwDirection & DIR_RIGHT)xmf3Direction = Vector3::Add(xmf3Direction, Vector3::Normalize(XMFLOAT3(player->m_pCamera->GetRightVector().x, 0.0f, player->m_pCamera->GetRightVector().z)));
		if (dwDirection & DIR_LEFT)xmf3Direction = Vector3::Add(xmf3Direction, Vector3::Normalize(XMFLOAT3(player->m_pCamera->GetRightVector().x, 0.0f, player->m_pCamera->GetRightVector().z)), -1.0f);

		XMVECTOR playerLookVec = XMLoadFloat3(&(player->GetLook()));
		XMVECTOR rollVec = XMLoadFloat3(&xmf3Direction);

		playerLookVec = XMVector3Normalize(playerLookVec);
		rollVec = XMVector3Normalize(rollVec);

		float dot = XMVectorGetX(XMVector3Dot(rollVec, playerLookVec));

		if (dot < -1.0f + FLT_EPSILON || dot > 1.0f - FLT_EPSILON)
		{
			dot < -1.0f + FLT_EPSILON ? degrees = 180.0f : degrees = 0.0f;
		}
		else
		{
			float angle = acosf(dot);
			degrees = XMConvertToDegrees(angle);

			XMVECTOR cross = XMVector3Cross(rollVec, playerLookVec);

			if (XMVectorGetY(cross) > 0.0f)
				degrees = 360.0f - degrees;
		}
	}

	if (0.0f <= degrees && degrees < 45.0f - ANGLE_MARGIN)
	{
		//정면
		//OutputDebugString(L"정면\n");
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
	}
	else if (degrees < 90.0f - ANGLE_MARGIN)
	{
		//정면 - 오른쪽
		//OutputDebugString(L"정면 - 오른쪽\n");
		XMFLOAT3 newLookAt = Vector3::Add(player->GetLook(), player->GetRight());
		player->SetLookAt(Vector3::Add(player->GetPosition(),
			Vector3::Normalize(newLookAt)));
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
	}
	else if (degrees < 135.0f - ANGLE_MARGIN)
	{
		//오른쪽
		//OutputDebugString(L"오른쪽\n");
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 14);
	}
	else if (degrees < 180.0f - ANGLE_MARGIN)
	{
		//오른쪽 - 뒤
		//OutputDebugString(L"오른쪽 - 뒤\n");
		XMFLOAT3 newLookAt = Vector3::Add(player->GetLook(), Vector3::ScalarProduct(player->GetRight(), -1.0f));
		player->SetLookAt(Vector3::Add(player->GetPosition(),
			Vector3::Normalize(newLookAt)));
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 11);
	}
	else if (degrees < 225.0f - ANGLE_MARGIN)
	{
		//뒤
		//OutputDebugString(L"뒤\n");
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 11);
	}
	else if (degrees < 270.0f - ANGLE_MARGIN)
	{
		//뒤 - 왼쪽
		//OutputDebugString(L"뒤 - 왼쪽\n");
		XMFLOAT3 newLookAt = Vector3::Add(player->GetLook(), player->GetRight());
		player->SetLookAt(Vector3::Add(player->GetPosition(),
			Vector3::Normalize(newLookAt)));
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 11);
	}
	else if (degrees < 315.0f - ANGLE_MARGIN)
	{
		//왼쪽
		//OutputDebugString(L"왼쪽\n");
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 13);
	}
	else if (degrees < 360.0f - ANGLE_MARGIN)
	{
		//왼쪽 - 정면
		//OutputDebugString(L"왼쪽 - 정면\n");
		XMFLOAT3 newLookAt = Vector3::Add(player->GetLook(), Vector3::ScalarProduct(player->GetRight(), -1.0f));
		player->SetLookAt(Vector3::Add(player->GetPosition(),
			Vector3::Normalize(newLookAt)));
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
	}
	else
	{
		//정면
		//OutputDebugString(L"정면\n");
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 12);
	}

	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fWeight = 1.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 4);
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_fWeight = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[1].m_nType = ANIMATION_TYPE_ONCE;

	player->m_fInvincibleTime = FLT_MAX;

	player->m_fStamina -= 20.0f;
	player->m_fStamina = max(0.0f, player->m_fStamina);
}

void Evasion_Player::Execute(CPlayer* player, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		CState<CPlayer>& previousState = *player->m_pStateMachine->GetPreviousState();


		if (dynamic_cast<Idle_Player*>(&previousState)) {
			player->m_pStateMachine->ChangeState(&previousState);
			player->Move(XMFLOAT3(0, 0, 0), true);
			player->SetCurrSpeed(0.0f);
		}
		else if (dynamic_cast<Run_Player*>(&previousState)) {
			if (player->m_dwDirectionCache != 0) {
				player->m_pStateMachine->ChangeState(&previousState);
				player->Move(m_xmf3VelociyuCache, true);
				player->SetCurrSpeed(m_fSpeedCache);
			}
			else
				player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
		}
		else {
			player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
			player->Move(XMFLOAT3(0, 0, 0), true);
			player->SetCurrSpeed(0.0f);
		}
	}
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition > pAnimationSet->m_fLength - 0.2f && player->m_dwDirectionCache != 0) {
		player->m_pStateMachine->ChangeState(Run_Player::GetInst());
		player->Move(m_xmf3VelociyuCache, true);
		player->SetCurrSpeed(m_fSpeedCache);
	}
}

void Evasion_Player::Exit(CPlayer* player)
{
	player->m_bEvasioned = false;
	player->m_fInvincibleTime = 0.0f;
}

void Evasion_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
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

	SetPlayerRootVel(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

void Evasion_Player::SetPlayerRootVel(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->Move(Vector3::Subtract(xmf3Position, player->GetPosition()), true);

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


Damaged_Player::Damaged_Player()
{
}

Damaged_Player::~Damaged_Player()
{
}

#define SET_LOOKAT_WHEN_DAMAGED

void Damaged_Player::Enter(CPlayer* player)
{
	dynamic_cast<CKnightPlayer*>(player)->CanclePotion();
#ifdef SET_LOOKAT_WHEN_DAMAGED
	player->SetLookAt(Vector3::Add(player->GetPosition(), Vector3::Normalize(player->m_xmf3ToHitterVec)));
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 32);
#else
	float degrees = 0.0f;

	XMVECTOR playerLookVec = XMLoadFloat3(&(player->GetLook()));
	XMVECTOR toHitterVec = XMLoadFloat3(&(player->m_xmf3ToHitterVec));

	playerLookVec = XMVector3Normalize(playerLookVec);
	toHitterVec = XMVector3Normalize(toHitterVec);

	float dot = XMVectorGetX(XMVector3Dot(toHitterVec, playerLookVec));

	if (dot < -1.0f + FLT_EPSILON || dot > 1.0f - FLT_EPSILON)
	{
		dot < -1.0f + FLT_EPSILON ? degrees = 180.0f : degrees = 0.0f;
	}
	else
	{
		float angle = acosf(dot);
		degrees = XMConvertToDegrees(angle);

		XMVECTOR cross = XMVector3Cross(toHitterVec, playerLookVec);

		if (XMVectorGetY(cross) > 0.0f)
			degrees = 360.0f - degrees;
	}

	if (0.0f <= degrees && degrees < 45.0f)
	{
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 32);
	}
	else if (degrees <= 180.0f)
	{
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 34);
	}
	else if (degrees <= 325.0f)
	{
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 33);
	}
	else
	{
		player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 32);
	}
#endif // SET_LOOKAT_WHEN_DAMAGED
	player->m_pSkinnedAnimationController->SetTrackWeight(0, 1.0f);
	player->m_pSkinnedAnimationController->SetTrackWeight(1, 0.0f);

	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fSequenceWeight = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;

	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
}

void Damaged_Player::Execute(CPlayer* player, float fElapsedTime)
{
	player->SetLookAt(Vector3::Add(player->GetPosition(), Vector3::Normalize(player->m_xmf3ToHitterVec)));

	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
}

void Damaged_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void Damaged_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	CAnimationSet* pAnimationSet = pPlayerController->m_pAnimationSets->m_pAnimationSets[pPlayerController->m_pAnimationTracks[0].m_nAnimationSet];

	XMFLOAT3 xmf3CurrentPos = XMFLOAT3{ player->m_pChild->m_xmf4x4Transform._41,
	0.0f,
	player->m_pChild->m_xmf4x4Transform._43 };

	player->m_pChild->m_xmf4x4Transform._41 -= player->m_xmf3RootTransfromPreviousPos.x;
	player->m_pChild->m_xmf4x4Transform._42 -= player->m_xmf3RootTransfromPreviousPos.y;
	player->m_pChild->m_xmf4x4Transform._43 -= player->m_xmf3RootTransfromPreviousPos.z;

	SetPlayerRootVel(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

void Damaged_Player::Exit(CPlayer* player)
{
	player->m_bEvasioned = false;
	player->m_fInvincibleTime = 1.0f;
}

void Damaged_Player::SetPlayerRootVel(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->Move(Vector3::Subtract(xmf3Position, player->GetPosition()), true);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}


ChargeStart_Player::ChargeStart_Player()
{
}

ChargeStart_Player::~ChargeStart_Player()
{
}

void ChargeStart_Player::SetPlayerRootVel(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->Move(Vector3::Subtract(xmf3Position, player->GetPosition()), true);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}


void ChargeStart_Player::Enter(CPlayer* player)
{
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 7);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fWeight = 1.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;

	player->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void ChargeStart_Player::Execute(CPlayer* player, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength) {
		player->m_pStateMachine->ChangeState(Charge_Player::GetInst());
	}

	static float fAcculateTime = 0.0f;
	static float fSpawnVPParticleTime = 0.032f;
	CVertexPointParticleObject* vpObj = dynamic_cast<CVertexPointParticleObject*>(CPlayerParticleObject::GetInst()->GetVertexPointParticleObject());
	if (vpObj) {
		fAcculateTime += fElapsedTime;
		if (fAcculateTime >= fSpawnVPParticleTime) {
			fAcculateTime = 0.0f;
			vpObj->SetTextureIndex(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::SmokeTexture));
			vpObj->SetFieldSpeed(0.3f);
			vpObj->SetColor(XMFLOAT3(0.4745, 0.9254, 1.0));
			vpObj->SetSpeed(6.0f);
			vpObj->SetLifeTime(0.5f);
			vpObj->SetRotateFactor(true);
			vpObj->SetSize(XMFLOAT2(0.2, 0.2));
			vpObj->EmitParticle(5);
			vpObj->SetEmit(true);
		}
	}
}

void ChargeStart_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void ChargeStart_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	CAnimationSet* pAnimationSet = pPlayerController->m_pAnimationSets->m_pAnimationSets[pPlayerController->m_pAnimationTracks[0].m_nAnimationSet];

	XMFLOAT3 xmf3CurrentPos = XMFLOAT3{ player->m_pChild->m_xmf4x4Transform._41,
	0.0f,
	player->m_pChild->m_xmf4x4Transform._43 };

	player->m_pChild->m_xmf4x4Transform._41 -= player->m_xmf3RootTransfromPreviousPos.x;
	player->m_pChild->m_xmf4x4Transform._42 -= player->m_xmf3RootTransfromPreviousPos.y;
	player->m_pChild->m_xmf4x4Transform._43 -= player->m_xmf3RootTransfromPreviousPos.z;

	SetPlayerRootVel(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

void ChargeStart_Player::Exit(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	CAnimationSet* pAnimationSet = pPlayerController->m_pAnimationSets->m_pAnimationSets[pPlayerController->m_pAnimationTracks[0].m_nAnimationSet];

	XMFLOAT3 xmf3CurrentPos = XMFLOAT3{ player->m_pChild->m_xmf4x4Transform._41,
	0.0f,
	player->m_pChild->m_xmf4x4Transform._43 };

	player->m_pChild->m_xmf4x4Transform._41 -= player->m_xmf3RootTransfromPreviousPos.x;
	player->m_pChild->m_xmf4x4Transform._42 -= player->m_xmf3RootTransfromPreviousPos.y;
	player->m_pChild->m_xmf4x4Transform._43 -= player->m_xmf3RootTransfromPreviousPos.z;

	SetPlayerRootVel(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

Charge_Player::Charge_Player()
{
}

Charge_Player::~Charge_Player()
{
}


void Charge_Player::Enter(CPlayer* player)
{
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 9);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fWeight = 1.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;

	player->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_fChargedTime = 0.0f;
}

void Charge_Player::SetPlayerRootVel(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->Move(Vector3::Subtract(xmf3Position, player->GetPosition()), true);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}

void Charge_Player::Execute(CPlayer* player, float fElapsedTime)
{
	m_fChargedTime += fElapsedTime;
	if (m_fChargedTime >= m_fChargedEndTime)
		player->m_pStateMachine->ChangeState(ChargeAttack_Player::GetInst());
}

void Charge_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void Charge_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	CAnimationSet* pAnimationSet = pPlayerController->m_pAnimationSets->m_pAnimationSets[pPlayerController->m_pAnimationTracks[0].m_nAnimationSet];

	XMFLOAT3 xmf3CurrentPos = XMFLOAT3{ player->m_pChild->m_xmf4x4Transform._41,
	0.0f,
	player->m_pChild->m_xmf4x4Transform._43 };

	player->m_pChild->m_xmf4x4Transform._41 -= player->m_xmf3RootTransfromPreviousPos.x;
	player->m_pChild->m_xmf4x4Transform._42 -= player->m_xmf3RootTransfromPreviousPos.y;
	player->m_pChild->m_xmf4x4Transform._43 -= player->m_xmf3RootTransfromPreviousPos.z;

	SetPlayerRootVel(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

void Charge_Player::Exit(CPlayer* player)
{
	player->m_bCharged = false;
}

ChargeAttack_Player::ChargeAttack_Player()
{
	InitAtkPlayer();

	TrailComponent* pTrailComponent = nullptr;
	for (auto& component : m_pListeners) {
		if (dynamic_cast<TrailComponent*>(component.get())) {
			pTrailComponent = dynamic_cast<TrailComponent*>(component.get());
			break;
		}
	}
	if (pTrailComponent) {
		pTrailComponent->SetEnable(true);
		pTrailComponent->m_fEmissiveFactor = 100.0f;
	}

	CameraShakeComponent* pCameraShake = dynamic_cast<CameraShakeComponent*>(GetCameraShakeComponent());
	pCameraShake->SetEnable(true);
	pCameraShake->SetMagnitude(0.2f);
	pCameraShake->SetDuration(2.0f);
	pCameraShake->SetFrequency(0.001f);

	HitLagComponent* pHitLag = dynamic_cast<HitLagComponent*>(GetHitLagComponent());
	pHitLag->SetEnable(true);
	pHitLag->SetDuration(0.8);
	pHitLag->SetLagScale(0.1f);
	pHitLag->SetMinTimeScale(0.001f); 

	SlashHitComponent* pSlashHit = dynamic_cast<SlashHitComponent*>(GetSlashHitComponent());
	pSlashHit->SetColor(XMFLOAT3(1, 1, 0.4));
	pSlashHit->SetEmissive(30.0f);
	pSlashHit->SetEnable(true);
	pSlashHit->SetLifeTime(0.8f);
	pSlashHit->SetSize(XMFLOAT2(1.0f, 50.0f));
	pSlashHit->SetTextureIndex(0); // 파티클 첫번째 텍스쳐

	ImpactEffectComponent* ImpactEffect = dynamic_cast<ImpactEffectComponent*>(GetImpactComponent());
	ImpactEffect->SetEnable(true);
	ImpactEffect->SetColorR(1);
	ImpactEffect->SetColorG(1);
	ImpactEffect->SetColorB(1);
	ImpactEffect->SetEmissive(3.0f);
	ImpactEffect->SetLifeTime(1.0);
	ImpactEffect->SetSize(XMFLOAT2(20, 20));
	ImpactEffect->SetTotalRowColumn(8, 8);
	ImpactEffect->SetTextureIndex(3);

	ParticleComponent* particleComp = dynamic_cast<ParticleComponent*>(GetParticleComponent());
	particleComp->SetEnable(true);
	particleComp->SetColor(XMFLOAT3(0.9882, 0.4313, 0.1333));
	particleComp->SetEmissive(20.0f);
	particleComp->SetEmitParticleNumber(100);
	//particleComp->SetParticleNumber(150);
	particleComp->SetLifeTime(1.0f);
	particleComp->SetSize(XMFLOAT2(0.8, 0.8));
	particleComp->SetTextureIndex(0);
	particleComp->SetSpeed(100.0f);


	// ATK SOUND
	SoundPlayComponent* pAtkSoundComponent = dynamic_cast<SoundPlayComponent*>(GetShootSoundComponent());
	pAtkSoundComponent->SetEnable(true);
	pAtkSoundComponent->SetSoundNumber(6);
	pAtkSoundComponent->SetDelay(0.63f);

	// DAMAGE SOUND
	SoundPlayComponent* pShockSoundComponent = dynamic_cast<SoundPlayComponent*>(GetShockSoundComponent());
	pShockSoundComponent->SetEnable(true);
	pShockSoundComponent->SetSoundNumber(9);

	// GOBLIN MOAN SOUND
	SoundPlayComponent* pGobSoundComponent = dynamic_cast<SoundPlayComponent*>(GetGoblinMoanComponent());
	pGobSoundComponent->SetEnable(true);
	pGobSoundComponent->SetSoundNumber(2);

	// ORC MOAN SOUND
	SoundPlayComponent* pOrcSoundComponent = dynamic_cast<SoundPlayComponent*>(GetOrcMoanComponent());
	pOrcSoundComponent->SetEnable(true);
	pOrcSoundComponent->SetSoundNumber(2);

	// SKELETON MOAN SOUND
	SoundPlayComponent* pSkelSoundComponent = dynamic_cast<SoundPlayComponent*>(GetSkeletonMoanComponent());
	pSkelSoundComponent->SetEnable(true);
	pSkelSoundComponent->SetSoundNumber(0);

	//m_fPlayerCameraOffset = MeterToUnit(2.0f);
}

void ChargeAttack_Player::SetPlayerRootVel(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->Move(Vector3::Subtract(xmf3Position, player->GetPosition()), true);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}

ChargeAttack_Player::~ChargeAttack_Player()
{
}

void ChargeAttack_Player::Enter(CPlayer* player)
{
	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fWeight = 1.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;

	player->m_pSkinnedAnimationController->SetTrackEnable(1, false);

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	TrailUpdateParams trailParam;
	trailParam.pObject = player->m_pSwordTrailReference[3].get();
	CMessageDispatcher::GetInst()->Dispatch_Message<TrailUpdateParams>(MessageType::UPDATE_SWORDTRAIL, &trailParam, this);

	if (player->m_pSwordTrailReference) {
		XMFLOAT3 dir = player->m_xmfDirection;
		dir = XMFLOAT3(-dir.z, dir.y, dir.x);
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_faccumulateTime = 0.0f;
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->SetLengthWeight(4.0f);
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->SetOffset(XMFLOAT4(dir.x * 4.0f, 0.0f, dir.z * 4.0f, 0));
	}
	m_bEnableSpecialMove = false;
}

void ChargeAttack_Player::Execute(CPlayer* player, float fElapsedTime)
{
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition < 0.76f) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;
	}
	else {
		if (m_bEnableSpecialMove == false) {
			SpecialMoveUpdateParams param;
			param.bEnable = true;
			CMessageDispatcher::GetInst()->Dispatch_Message<SpecialMoveUpdateParams>(MessageType::UPDATE_SPMOVE, &param, nullptr);
			m_bEnableSpecialMove = true;
			m_xmf3PlayerCameraOffsetCache = player->m_pCamera->GetOffset();
			player->m_pCamera->SetOffset(XMFLOAT3(m_xmf3PlayerCameraOffsetCache.x, m_xmf3PlayerCameraOffsetCache.y, m_xmf3PlayerCameraOffsetCache.z + m_fPlayerCameraOffset));
		}
		if (player->m_pSwordTrailReference) {
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::UPDATE_NEW_CONTROL_POINT;

		}

		if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition > 2.0f) {
			if (player->m_pSwordTrailReference) {
				dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;
			}
		}
		else {
			if (0.83 < player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
				if (player->m_pSwordTrailReference) {
					static float MaxScale = 10.0f;
					dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;
					float scale = dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->GetLengthWeight();
					scale += fElapsedTime * 5.0f;
					scale = min(MaxScale, scale);
					dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->SetLengthWeight(scale);
				}
				//m_bPlayingMoveRunning = true;
			}
		}
	}


	CAnimationSet* pAnimationSet = player->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength) {
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}
}

void ChargeAttack_Player::Animate(CPlayer* player, float fElapsedTime)
{
	if (!m_bPlayingMoveRunning)
		player->Animate(fElapsedTime);
}

void ChargeAttack_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	CAnimationSet* pAnimationSet = pPlayerController->m_pAnimationSets->m_pAnimationSets[pPlayerController->m_pAnimationTracks[0].m_nAnimationSet];

	XMFLOAT3 xmf3CurrentPos = XMFLOAT3{ player->m_pChild->m_xmf4x4Transform._41,
	0.0f,
	player->m_pChild->m_xmf4x4Transform._43 };

	player->m_pChild->m_xmf4x4Transform._41 -= player->m_xmf3RootTransfromPreviousPos.x;
	player->m_pChild->m_xmf4x4Transform._42 -= player->m_xmf3RootTransfromPreviousPos.y;
	player->m_pChild->m_xmf4x4Transform._43 -= player->m_xmf3RootTransfromPreviousPos.z;

	SetPlayerRootVel(player);

	player->m_xmf3RootTransfromPreviousPos = xmf3CurrentPos;
}

void ChargeAttack_Player::Exit(CPlayer* player)
{
	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_eTrailUpdateMethod = TRAIL_UPDATE_METHOD::DELETE_CONTROL_POINT;
	//player->m_pCamera->SetOffset(m_xmf3PlayerCameraOffsetCache);
}

Dead_Player::Dead_Player()
{
}

Dead_Player::~Dead_Player()
{
}

void Dead_Player::Enter(CPlayer* player)
{
	player->SetLookAt(Vector3::Add(player->GetPosition(), Vector3::Normalize(player->m_xmf3ToHitterVec)));

	player->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 31);
	player->m_pSkinnedAnimationController->SetTrackWeight(0, 1.0f);
	player->m_pSkinnedAnimationController->SetTrackWeight(1, 0.0f);
	player->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;


	player->m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.f, 0.f , 0.f };
	player->m_fHP = 0.0f;

	PlayerParams PlayerParam;
	PlayerParam.pPlayer = player;
	CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_DEAD, &PlayerParam, nullptr);
}

void Dead_Player::Execute(CPlayer* player, float fElapsedTime)
{
}

void Dead_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void Dead_Player::OnRootMotion(CPlayer* player, float fTimeElapsed)
{
}

void Dead_Player::Exit(CPlayer* player)
{
}

void Dead_Player::SetPlayerRootVel(CPlayer* player)
{
	CAnimationController* pPlayerController = player->m_pSkinnedAnimationController.get();

	player->UpdateTransform(NULL);

	XMFLOAT3 xmf3Position = XMFLOAT3{ pPlayerController->m_pRootMotionObject->GetWorld()._41,
		player->GetPosition().y,
		pPlayerController->m_pRootMotionObject->GetWorld()._43 };

	player->Move(Vector3::Subtract(xmf3Position, player->GetPosition()), true);

	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
	pPlayerController->m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
}
