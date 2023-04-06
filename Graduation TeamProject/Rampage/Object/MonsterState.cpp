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
	XMFLOAT3 force = monster->GetHitDirection();
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
