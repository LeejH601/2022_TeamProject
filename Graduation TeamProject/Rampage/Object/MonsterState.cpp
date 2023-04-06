#include "MonsterState.h"
#include "Monster.h"
#include "..\Global\Locator.h"

Idle_Monster::Idle_Monster()
{
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
		monster->m_bCanChase = false;

		monster->m_fTotalDamageDistance = 0.0f;

		static int iIndex = 0;

		for (int i = 0; i < 2; i++)
		{
			XMFLOAT3 xmf3Position;
			ParticleUpDownParams ParticlesUpDown_comp_params;
			if (m_pUpDownParticle)
			{
				ParticlesUpDown_comp_params.pObjects = m_pUpDownParticle;
				ParticlesUpDown_comp_params.xmf3Position = monster->GetPosition();
				ParticlesUpDown_comp_params.iIndex = (++iIndex) % m_pUpDownParticle->size();
				CMessageDispatcher::GetInst()->Dispatch_Message<ParticleUpDownParams>(MessageType::UPDATE_PARTICLE, &ParticlesUpDown_comp_params, monster->m_pStateMachine->GetCurrentState());
			}
		}		
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

void Damaged_Monster::SetUpDownParticleObjects(std::vector<std::unique_ptr<CGameObject>>* pParticleObjects)
{
	m_pUpDownParticle = pParticleObjects;
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

void Dead_Monster::Enter(CMonster* monster)
{
	monster->Animate(0.f);
	monster->m_bSimulateArticulate = true;
	int index = 0;
	for (XMFLOAT4X4& matrix : monster->m_AritculatCacheMatrixs) {
		std::string linkName = monster->m_pArtiLinkNames[index];
		CGameObject* obj = monster->FindFrame(linkName.c_str());
		matrix = obj->m_xmf4x4World;

		float data[16] = {
			matrix._11,matrix._12,matrix._13,matrix._14,
			matrix._21,matrix._22,matrix._23,matrix._24,
			matrix._31,matrix._32,matrix._33,matrix._34,
			matrix._41,matrix._42,matrix._43,matrix._44,
		};
		physx::PxTransform transform{ physx::PxMat44(data) };
		monster->m_pArticulationLinks[index]->setGlobalPose(transform);
		index++;
	}
	RegisterArticulationParams Request_params;
	Request_params.pObject = monster;
	XMFLOAT3 force = monster->GetHitterVec();
	force = Vector3::ScalarProduct(force, 300.0f, false);
	Request_params.m_force = force;
	CMessageDispatcher::GetInst()->Dispatch_Message<RegisterArticulationParams>(MessageType::REQUEST_REGISTERARTI, &Request_params, nullptr);
}

void Dead_Monster::Execute(CMonster* monster, float fElapsedTime)
{

}

void Dead_Monster::Exit(CMonster* monster)
{
	monster->m_bSimulateArticulate = false;
}
