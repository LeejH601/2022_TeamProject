#include "State.h"
#include "Player.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Sound\Sound.h"
#include "..\Sound\SoundManager.h"
#include "..\ImGui\ImGuiManager.h"

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
	if(player->m_bAttack)
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Atk1_Player)));
}

void Idle_Player::Exit(CPlayer* player)
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

	player->m_xmf3CameraMoveDirection = Vector3::Normalize(XMFLOAT3(-1.0f, -1.0f, 0.0f));
	player->m_fCMDConstant = 1.0f;

	PlayerAttackParams PlayerAttackParam;
	PlayerAttackParam.pPlayer = player;
	CMessageDispatcher::GetInst()->Dispatch_Message<PlayerAttackParams>(MessageType::PLAYER_ATTACK, &PlayerAttackParam, player);

	SoundPlayParams SoundPlayParam{ SOUND_PLAY_TYPE::SOUND_PT_ATK1 };
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, player);

	CComponentSet* componentset = CImGuiManager::GetInst()->GetComponentSet(0);
	if (componentset) {
		/*Locator.GetMainSceneCamera()->LoadComponentFromSet(componentset);
		Locator.GetSimulaterCamera()->LoadComponentFromSet(componentset);
		Locator.GetSoundPlayer()->LoadComponentFromSet(componentset);*/
	}
}

void Atk1_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	
	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	if (player->m_bAttack && 0.7 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition )
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Atk2_Player)));

	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));
	}
}

void Atk1_Player::Exit(CPlayer* player)
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

	player->m_xmf3CameraMoveDirection = Vector3::Normalize(XMFLOAT3(1.0f, -1.0f, 0.0f));
	player->m_fCMDConstant = 1.0f;

	PlayerAttackParams PlayerAttackParam;
	PlayerAttackParam.pPlayer = player;
	CMessageDispatcher::GetInst()->Dispatch_Message<PlayerAttackParams>(MessageType::PLAYER_ATTACK, &PlayerAttackParam, player);

	SoundPlayParams SoundPlayParam{ SOUND_PLAY_TYPE::SOUND_PT_ATK2 };
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, player);

	CComponentSet* componentset = CImGuiManager::GetInst()->GetComponentSet(1);
	if (componentset) {
		/*Locator.GetMainSceneCamera()->LoadComponentFromSet(componentset);
		Locator.GetSimulaterCamera()->LoadComponentFromSet(componentset);
		Locator.GetSoundPlayer()->LoadComponentFromSet(componentset);*/
	}
}

void Atk2_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	
	// 사용자가 좌클릭을 했으면 애니메이션을 0.7초 진행 후 Atk2_Player로 상태 변경
	if (player->m_bAttack && 0.7 < player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition)
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Atk3_Player)));

	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));
	}
}

void Atk2_Player::Exit(CPlayer* player)
{
}

void Atk3_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	player->m_bAttacked = false;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
	player->m_iAttack_Limit = 1;

	PlayerAttackParams PlayerAttackParam;
	PlayerAttackParam.pPlayer = player;
	CMessageDispatcher::GetInst()->Dispatch_Message<PlayerAttackParams>(MessageType::PLAYER_ATTACK, &PlayerAttackParam, player);

	SoundPlayParams SoundPlayParam{ SOUND_PLAY_TYPE::SOUND_PT_ATK3 };
	CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, player);

	CComponentSet* componentset = CImGuiManager::GetInst()->GetComponentSet(2);
	if (componentset) {
		/*Locator.GetMainSceneCamera()->LoadComponentFromSet(componentset);
		Locator.GetSimulaterCamera()->LoadComponentFromSet(componentset);
		Locator.GetSoundPlayer()->LoadComponentFromSet(componentset);*/
	}
}

void Atk3_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));
	}
}

void Atk3_Player::Exit(CPlayer* player)
{
}

void Run_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 16);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
	player->m_bAttack = false; // 사용자가 좌클릭시 true가 되는 변수
}

void Run_Player::Execute(CPlayer* player, float fElapsedTime)
{
	if (player->m_bAttack)
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Atk1_Player)));
}

void Run_Player::Exit(CPlayer* player)
{
}
