#include "State.h"
#include "Player.h"
#include "..\Global\Locator.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Global\Global.h"
#include "..\Sound\Sound.h"

void Idle_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
}

void Idle_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);
}

void Idle_Player::Exit(CPlayer* player)
{

}

bool Idle_Player::OnMessage(CPlayer* player, const Telegram& msg)
{
	return false;
}

void Atk1_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	Telegram msg;
	msg.Msg = (int)MESSAGE_TYPE::Msg_PlaySoundShoot;
	Locator.GetSoundPlayer()->HandleMessage(msg);
}

void Atk1_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));
	}
}

void Atk1_Player::Exit(CPlayer* player)
{
}

bool Atk1_Player::OnMessage(CPlayer* player, const Telegram& msg)
{
	return false;
}

void Atk2_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
}

void Atk2_Player::Execute(CPlayer* player, float fElapsedTime)
{
	//player->Animate(fElapsedTime);
	CAnimationSet* pAnimationSet = player->m_pChild->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		player->m_pStateMachine->ChangeState(Locator.GetPlayerState(typeid(Idle_Player)));
	}
}

void Atk2_Player::Exit(CPlayer* player)
{
}

bool Atk2_Player::OnMessage(CPlayer* player, const Telegram& msg)
{
	return false;
}

void Atk3_Player::Enter(CPlayer* player)
{
	player->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	player->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	player->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
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

bool Atk3_Player::OnMessage(CPlayer* player, const Telegram& msg)
{
	return false;
}
