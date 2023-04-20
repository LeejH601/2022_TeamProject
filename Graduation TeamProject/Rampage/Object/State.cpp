#include "State.h"
#include "Player.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Sound\Sound.h"
#include "..\Sound\SoundManager.h"
#include "..\ImGui\ImGuiManager.h"
#include "..\Object\PlayerParticleObject.h"
#include "SwordTrailObject.h"

Idle_Player::Idle_Player()
{
}

Idle_Player::~Idle_Player()
{
}

void Idle_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
}

void Idle_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);

	// 사용자가 좌클릭을 했으면 Atk1_Player로 상태 변경
	if (player->m_bAttack)
		player->m_pStateMachine->ChangeState(Atk1_Player::GetInst());
}

void Idle_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void Idle_Player::Exit(CPlayer* player)
{

}

Atk1_Player::Atk1_Player()
{
	// ATK1 SOUND
	std::unique_ptr<SoundPlayComponent> pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(2);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(1.25f);
	pSoundComponent->SetMT(MONSTER_TYPE::NONE);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_SHOOT);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// DAMAGE SOUND
	pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(2);
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
	pShakeComponent->SetMagnitude(1.5f);
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

	// PARTICLE
	std::unique_ptr<ParticleComponent> pParticlenComponent = std::make_unique<ParticleComponent>();
	m_pListeners.push_back(std::move(pParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);

	// IMPACT
	std::unique_ptr<ImpactEffectComponent> pImpactComponent = std::make_unique<ImpactEffectComponent>();
	m_pListeners.push_back(std::move(pImpactComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_BILLBOARD, m_pListeners.back().get(), this);

	// SMOKEPARTICLE ANIMATION
	std::unique_ptr<SmokeParticleComponent> pSmokeParticlenComponent = std::make_unique<SmokeParticleComponent>();
	m_pListeners.push_back(std::move(pSmokeParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SMOKEPARTICLE, m_pListeners.back().get(), this);

	// UPDOWNPARTICLE ANIMATION
	std::unique_ptr<UpDownParticleComponent> pUpDownParticlenComponent = std::make_unique<UpDownParticleComponent>();
	m_pListeners.push_back(std::move(pUpDownParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);

	// TERRAIN SPRITE  ANIMATION
	std::unique_ptr<TerrainSpriteComponent> pTerrainSpriteComponent = std::make_unique<TerrainSpriteComponent>();
	m_pListeners.push_back(std::move(pTerrainSpriteComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SPRITE, m_pListeners.back().get(), this);

	// TRAIL ANIMATION
	std::unique_ptr<TrailParticleComponent> pTrailParticlenComponent = std::make_unique<TrailParticleComponent>();
	m_pListeners.push_back(std::move(pTrailParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_TRAILPARTICLE, m_pListeners.back().get(), this);
}

Atk1_Player::~Atk1_Player()
{
}

void Atk1_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_iAttack_Limit = 1;
	player->m_fCMDConstant = 1.0f;

	if (m_HitlagComponent.GetEnable())
		m_HitlagComponent.SetCurLagTime(0.0f);

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_bIsUpdateTrailVariables = true;
}

void Atk1_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	// 충돌 판정 메세지 전달
	if (0.365f < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition && 
		0.5f > player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
		
	}

	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	if (0.7 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_bIsUpdateTrailVariables = false;
		if (player->m_bAttack) {
			player->m_pStateMachine->ChangeState(Atk2_Player::GetInst());

		}
	}


	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}
}

void Atk1_Player::Animate(CPlayer* player, float fElapsedTime)
{
	if (m_HitlagComponent.GetEnable() && player->m_bAttacked)
	{
		if (m_HitlagComponent.GetCurLagTime() < m_HitlagComponent.GetMaxLagTime())
			player->Animate(fElapsedTime * 0.01f);
		else
			player->Animate(fElapsedTime);

		m_HitlagComponent.SetCurLagTime(m_HitlagComponent.GetCurLagTime() + fElapsedTime);
	}
	else
		player->Animate(fElapsedTime);
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
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[0].get())->m_bIsUpdateTrailVariables = false;
}

Atk2_Player::Atk2_Player()
{
	// ATK2 SOUND
	std::unique_ptr<SoundPlayComponent> pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(2);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(1.25f);
	pSoundComponent->SetMT(MONSTER_TYPE::NONE);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_SHOOT);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// DAMAGE SOUND
	pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(2);
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
	pShakeComponent->SetMagnitude(1.5f);
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

	// PARTICLE ANIMATION
	std::unique_ptr<ParticleComponent> pParticlenComponent = std::make_unique<ParticleComponent>();
	m_pListeners.push_back(std::move(pParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);

	// IMPACT
	std::unique_ptr<ImpactEffectComponent> pImpactComponent = std::make_unique<ImpactEffectComponent>();
	m_pListeners.push_back(std::move(pImpactComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_BILLBOARD, m_pListeners.back().get(), this);

	// SMOKEPARTICLE ANIMATION
	std::unique_ptr<SmokeParticleComponent> pSmokeParticlenComponent = std::make_unique<SmokeParticleComponent>();
	m_pListeners.push_back(std::move(pSmokeParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SMOKEPARTICLE, m_pListeners.back().get(), this);

	// UPDOWNPARTICLE ANIMATION
	std::unique_ptr<UpDownParticleComponent> pUpDownParticlenComponent = std::make_unique<UpDownParticleComponent>();
	m_pListeners.push_back(std::move(pUpDownParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);

	// TERRAIN SPRITE  ANIMATION
	std::unique_ptr<TerrainSpriteComponent> pTerrainSpriteComponent = std::make_unique<TerrainSpriteComponent>();
	m_pListeners.push_back(std::move(pTerrainSpriteComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SPRITE, m_pListeners.back().get(), this);

	// TRAIL ANIMATION
	std::unique_ptr<TrailParticleComponent> pTrailParticlenComponent = std::make_unique<TrailParticleComponent>();
	m_pListeners.push_back(std::move(pTrailParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_TRAILPARTICLE, m_pListeners.back().get(), this);
}

Atk2_Player::~Atk2_Player()
{
}

void Atk2_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.2f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_iAttack_Limit = 1;
	player->m_fCMDConstant = 1.0f;

	if (m_HitlagComponent.GetEnable())
		m_HitlagComponent.SetCurLagTime(0.0f);

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_bIsUpdateTrailVariables = true;
}

void Atk2_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	// 충돌 판정 메세지 전달
	if (0.35f < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.4f > player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}

	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	if (0.7 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_bIsUpdateTrailVariables = false;
		if (player->m_bAttack)
			player->m_pStateMachine->ChangeState(Atk3_Player::GetInst());
	}


	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

}

void Atk2_Player::Animate(CPlayer* player, float fElapsedTime)
{
	if (m_HitlagComponent.GetEnable() && player->m_bAttacked)
	{
		if (m_HitlagComponent.GetCurLagTime() < m_HitlagComponent.GetMaxLagTime())
			player->Animate(fElapsedTime * 0.01f);
		else
			player->Animate(fElapsedTime);

		m_HitlagComponent.SetCurLagTime(m_HitlagComponent.GetCurLagTime() + fElapsedTime);
	}
	else
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
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[1].get())->m_bIsUpdateTrailVariables = false;
}

Atk3_Player::Atk3_Player()
{
	// ATK3 SOUND
	std::unique_ptr<SoundPlayComponent> pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(2);
	pSoundComponent->SetDelay(0.0f);
	pSoundComponent->SetVolume(1.25f);
	pSoundComponent->SetMT(MONSTER_TYPE::NONE);
	pSoundComponent->SetSC(SOUND_CATEGORY::SOUND_SHOOT);
	m_pListeners.push_back(std::move(pSoundComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAY_SOUND, m_pListeners.back().get(), this);

	// DAMAGE SOUND
	pSoundComponent = std::make_unique<SoundPlayComponent>();
	pSoundComponent->SetSoundNumber(2);
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
	pShakeComponent->SetMagnitude(1.5f);
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
}

Atk3_Player::~Atk3_Player()
{
}

void Atk3_Player::Enter(CPlayer* player)
{
	player->m_fCMDConstant = 1.0f;

	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_iAttack_Limit = 1;

	if (m_HitlagComponent.GetEnable())
		m_HitlagComponent.SetCurLagTime(0.0f);

	SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);

	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_bIsUpdateTrailVariables = true;
}

void Atk3_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// 충돌 판정 메세지 전달
	if (0.375f < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.475f > player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}

	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	/*if (0.7 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_bIsUpdateTrailVariables = false;
		if (player->m_bAttack)
			player->m_pStateMachine->ChangeState(Atk4_Player::GetInst());
	}*/

	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	else if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{

	}


	

}

void Atk3_Player::Animate(CPlayer* player, float fElapsedTime)
{
	if (m_HitlagComponent.GetEnable() && player->m_bAttacked)
	{
		if (m_HitlagComponent.GetCurLagTime() < m_HitlagComponent.GetMaxLagTime())
			player->Animate(fElapsedTime * 0.01f);
		else
			player->Animate(fElapsedTime);

		m_HitlagComponent.SetCurLagTime(m_HitlagComponent.GetCurLagTime() + fElapsedTime);
	}
	else
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
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[2].get())->m_bIsUpdateTrailVariables = false;
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
	//player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 19);
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 16);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
}

void Run_Player::Execute(CPlayer* player, float fElapsedTime)
{
	XMFLOAT3 xmf3PlayerVel = player->GetVelocity();

	if (player->m_bAttack)
		player->m_pStateMachine->ChangeState(Atk1_Player::GetInst());

	// Idle 상태로 복귀하는 코드
	if (Vector3::Length(xmf3PlayerVel) == 0)
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());

	// 발바닥 뼈에

	float fTime = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;
	XMFLOAT3 xmf3Position;
	ParticleSmokeParams Particlesmoke_comp_params;
	Particlesmoke_comp_params.pObject = CPlayerParticleObject::GetInst()->GetSmokeObjects();
	if (Particlesmoke_comp_params.pObject && (((int)(fTime * 10.f)) == 9)) // 0.5f마다
	{
		CGameObject* pFoot_Left = player->FindFrame("foot_l");
		memcpy(&xmf3Position, &(pFoot_Left->m_xmf4x4World._41), sizeof(XMFLOAT3));
		Particlesmoke_comp_params.xmf3Position = xmf3Position;
		Particlesmoke_comp_params.xmfDirection = player->m_xmfDirection;
		
		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleSmokeParams>(MessageType::UPDATE_SMOKEPARTICLE, &Particlesmoke_comp_params, player->m_pStateMachine->GetCurrentState());
	}

	if (Particlesmoke_comp_params.pObject && (((int)(fTime * 10.f)) == 2)) // 0.6f마다
	{
		CGameObject* pFoot_Right = player->FindFrame("foot_r");
		memcpy(&xmf3Position, &(pFoot_Right->m_xmf4x4World._41), sizeof(XMFLOAT3));
		Particlesmoke_comp_params.xmf3Position = xmf3Position;
		Particlesmoke_comp_params.xmfDirection = player->m_xmfDirection;

		CMessageDispatcher::GetInst()->Dispatch_Message<ParticleSmokeParams>(MessageType::UPDATE_SMOKEPARTICLE, &Particlesmoke_comp_params, player->m_pStateMachine->GetCurrentState());
	}
}


}

void Run_Player::Exit(CPlayer* player)
{
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

	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 3);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_iAttack_Limit = 1;

	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_bIsUpdateTrailVariables = true;

	/*SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);*/
}

void Atk4_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// 충돌 판정 메세지 전달
	if (0.3 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.5 > player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}

	if (0.7 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition) {
		if (player->m_pSwordTrailReference)
			dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_bIsUpdateTrailVariables = false;
		if (player->m_bAttack)
			player->m_pStateMachine->ChangeState(Atk5_Player::GetInst());
	}


	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	else if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{

	}
}

void Atk4_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void Atk4_Player::Exit(CPlayer* player)
{
	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[3].get())->m_bIsUpdateTrailVariables = false;
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

	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 7);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_iAttack_Limit = 1;

	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[4].get())->m_bIsUpdateTrailVariables = true;

	/*SoundPlayParams SoundPlayParam;
	SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOOT;
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);*/
}

void Atk5_Player::Execute(CPlayer* player, float fElapsedTime)
{
	// 충돌 판정 메세지 전달
	if (0.3 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition &&
		0.5 > player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
	{
		PlayerParams PlayerParam;
		PlayerParam.pPlayer = player;
		CMessageDispatcher::GetInst()->Dispatch_Message<PlayerParams>(MessageType::PLAYER_ATTACK, &PlayerParam, player);
	}

	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Idle_Player::GetInst());
	}

	else if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{

	}
}

void Atk5_Player::Animate(CPlayer* player, float fElapsedTime)
{
	player->Animate(fElapsedTime);
}

void Atk5_Player::Exit(CPlayer* player)
{
	if (player->m_pSwordTrailReference)
		dynamic_cast<CSwordTrailObject*>(player->m_pSwordTrailReference[4].get())->m_bIsUpdateTrailVariables = false;
}
