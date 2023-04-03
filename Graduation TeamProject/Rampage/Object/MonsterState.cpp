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
	monster->m_bCanChase = true;
	monster->m_fIdleTime = 0.0f;
}

void Idle_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	monster->m_fIdleTime += fElapsedTime;
	if (m_fMaxIdleTime < monster->m_fIdleTime && !(monster->m_bIsDummy))
	{
		monster->m_pStateMachine->ChangeState(Wander_Monster::GetInst());
	}
}

void Idle_Monster::Exit(CMonster* monster)
{

}

void Damaged_Monster::Enter(CMonster* monster)
{
	if (monster->m_pStateMachine->GetPreviousState() != Stun_Monster::GetInst())
	{
		monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
		monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
		monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.2f;
		monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
		monster->m_bStunned = false;
		monster->m_bCanChase = false;

		monster->m_fTotalDamageDistance = 0.0f;
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
}

void Damaged_Monster::Exit(CMonster* monster)
{
}

void Stun_Monster::Enter(CMonster* monster)
{
	monster->m_bStunned = true;
	monster->m_bCanChase = false;
	monster->m_fStunTime = 0.0f;
}

void Stun_Monster::Execute(CMonster* monster, float fElapsedTime)
{
}

void Stun_Monster::Exit(CMonster* monster)
{
	monster->m_fStunTime = 0.0f;
}

void Wander_Monster::Enter(CMonster* monster)
{
	monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
	monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
	monster->m_fWanderTime = 0.0f;
	monster->m_bCanChase = true;

	monster->SetWanderVec();

	XMFLOAT3 xmf3LookVec = Vector3::Add(monster->GetPosition(), monster->GetWanderVec());
	monster->SetLookAt(xmf3LookVec);
}

void Wander_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	XMFLOAT3 xmf3Shift = Vector3::ScalarProduct(monster->GetWanderVec(), monster->GetSpeedUperS() * fElapsedTime, false);
	monster->Move(xmf3Shift, true);

	monster->m_fWanderTime += fElapsedTime;

	if (m_fMaxWanderTime < monster->m_fWanderTime)
	{
		monster->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	}
}

void Wander_Monster::Exit(CMonster* monster)
{
}

void Chasing_Monster::Enter(CMonster* monster)
{
	monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
	monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
	monster->m_bCanChase = true;

	XMFLOAT3 xmf3LookVec = Vector3::Add(monster->GetPosition(), monster->GetChasingVec());
	monster->SetLookAt(xmf3LookVec);
}

void Chasing_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	XMFLOAT3 xmf3Shift = Vector3::ScalarProduct(monster->GetChasingVec(), monster->GetSpeedUperS() * fElapsedTime, false);

	XMFLOAT3 xmf3LookVec = Vector3::Add(monster->GetPosition(), monster->GetChasingVec());
	monster->SetLookAt(xmf3LookVec);

	if (monster->m_fToPlayerLength < 5.0f)
	{
		// 공격
		monster->m_pStateMachine->ChangeState(Attack_Monster::GetInst());
	}

	if (monster->m_fToPlayerLength > 35.0f)
	{
		// 놓침
		monster->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	}

	monster->Move(xmf3Shift, true);
}

void Chasing_Monster::Exit(CMonster* monster)
{
}

void Attack_Monster::Enter(CMonster* monster)
{
	monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;

	monster->m_bCanChase = false;
}

void Attack_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	CAnimationSet* pAnimationSet = monster->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];

	if (monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		if (monster->m_fToPlayerLength < 5.0f)
		{
			monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
			monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
		}

		else
			monster->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	}
}

void Attack_Monster::Exit(CMonster* monster)
{
}
