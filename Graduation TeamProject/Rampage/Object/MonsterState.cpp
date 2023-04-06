#include "MonsterState.h"
#include "Monster.h"
#include "AnimationComponent.h"
#include "..\Global\Locator.h"

Idle_Monster::Idle_Monster()
{
	// UPDOWNPARTICLE ANIMATION
	std::unique_ptr<UpDownParticleComponent> pUpDownParticlenComponent = std::make_unique<UpDownParticleComponent>();
	m_pListeners.push_back(std::move(pUpDownParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);
}

Idle_Monster::~Idle_Monster()
{
}

void Idle_Monster::Enter(CMonster* monster)
{
	monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
}

void Idle_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	static int iIndex = 0;
	if (monster->GetHit())
	{
		monster->m_pStateMachine->ChangeState(Damaged_Monster::GetInst());

		for (int i = 0; i < 2; i++)
		{
			XMFLOAT3 xmf3Position;
			ParticleUpDownParams ParticlesUpDown_comp_params;
			if (m_pUpDownParticle) // 0.6f¸¶´Ù
			{
				ParticlesUpDown_comp_params.pObjects = m_pUpDownParticle;

				ParticlesUpDown_comp_params.xmf3Position = monster->GetPosition();
				ParticlesUpDown_comp_params.iIndex = (++iIndex) % m_pUpDownParticle->size();
				CMessageDispatcher::GetInst()->Dispatch_Message<ParticleUpDownParams>(MessageType::UPDATE_PARTICLE, &ParticlesUpDown_comp_params, monster->m_pStateMachine->GetCurrentState());
			}
		}
	}

	//float fTime = monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition;


	//m_fTime -= fElapsedTime;

}

void Idle_Monster::Exit(CMonster* monster)
{

}

void Idle_Monster::SetUpDownParticleObjects(std::vector<std::unique_ptr<CGameObject>>* pParticleObjects)
{
	m_pUpDownParticle = pParticleObjects;
}

Damaged_Monster::Damaged_Monster()
{
	// UPDOWNPARTICLE ANIMATION
	std::unique_ptr<UpDownParticleComponent> pUpDownParticlenComponent = std::make_unique<UpDownParticleComponent>();
	m_pListeners.push_back(std::move(pUpDownParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_PARTICLE, m_pListeners.back().get(), this);
}

Damaged_Monster::~Damaged_Monster()
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
	monster->m_fStunTime = 0.0f;
}

void Stun_Monster::Execute(CMonster* monster, float fElapsedTime)
{
}

void Stun_Monster::Exit(CMonster* monster)
{
	monster->m_fStunTime = 0.0f;
}