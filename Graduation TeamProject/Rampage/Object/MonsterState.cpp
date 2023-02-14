#include "MonsterState.h"
#include "Monster.h"
#include "AnimationComponent.h"
#include "..\Global\Locator.h"

void Idle_Monster::Enter(CMonster* monster)
{
	monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
}

void Idle_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	if (monster->GetHit())
	{
		monster->m_pStateMachine->ChangeState(Damaged_Monster::GetInst());
	}
}

void Idle_Monster::Exit(CMonster* monster)
{

}

bool Idle_Monster::OnMessage(CMonster* monster, const Telegram& msg)
{
	return false;
}

void Damaged_Monster::Enter(CMonster* monster)
{
	if (monster->m_pStateMachine->GetPreviousState() != Stun_Monster::GetInst())
	{
		monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
		monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
		monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
		monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
		monster->m_bStunned = false;
	}
}

void Damaged_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = monster->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	if (monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		monster->SetNotHit();
		monster->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	}
	else
	{
		monster->m_fShakeDistance = CShakeAnimationComponent::GetInst()->GetShakeDistance(monster->m_pSkinnedAnimationController->m_fTime);
		
		if (monster->m_fTotalDamageDistance < CDamageAnimationComponent::GetInst()->GetMaxEistance())
		{
			monster->m_fDamageDistance = CDamageAnimationComponent::GetInst()->GetDamageDistance(monster->m_pSkinnedAnimationController->m_fTime);
			monster->m_fTotalDamageDistance += monster->m_fDamageDistance;
		}
		else
			monster->m_fDamageDistance = 0.0f;
	}
}

void Damaged_Monster::Exit(CMonster* monster)
{
	monster->m_fTotalDamageDistance = 0.0f;
}

bool Damaged_Monster::OnMessage(CMonster* monster, const Telegram& msg)
{
	return false;
}

void Stun_Monster::Enter(CMonster* monster)
{
	monster->m_bStunned = true;
	monster->m_fStunTime = 0.0f;
}

void Stun_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	if (monster->m_fStunTime < CStunAnimationComponent::GetInst()->GetStunTime())
	{
		monster->m_fStunTime += fElapsedTime;
		monster->m_fShakeDistance = CShakeAnimationComponent::GetInst()->GetShakeDistance(monster->m_pSkinnedAnimationController->m_fTime);
		
		if (monster->m_fTotalDamageDistance < CDamageAnimationComponent::GetInst()->GetMaxEistance())
		{
			monster->m_fDamageDistance = CDamageAnimationComponent::GetInst()->GetDamageDistance(monster->m_pSkinnedAnimationController->m_fTime);
			monster->m_fTotalDamageDistance += monster->m_fDamageDistance;
		}
		else
			monster->m_fDamageDistance = 0.0f;
	}
	else
		monster->m_pStateMachine->ChangeState(Damaged_Monster::GetInst());
}

void Stun_Monster::Exit(CMonster* monster)
{
}

bool Stun_Monster::OnMessage(CMonster* monster, const Telegram& msg)
{
	return false;
}
