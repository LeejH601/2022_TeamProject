#include "MonsterState.h"
#include "Monster.h"

void Idle_Monster::Enter(CMonster* monster)
{
	monster->m_pChild->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	monster->m_pChild->m_pSkinnedAnimationController->m_fTime = 0.0f;
	monster->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	monster->m_pChild->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
}

void Idle_Monster::Execute(CMonster* monster, float fElapsedTime)
{
}

void Idle_Monster::Exit(CMonster* monster)
{

}

bool Idle_Monster::OnMessage(CMonster* monster, const Telegram& msg)
{
	return false;
}