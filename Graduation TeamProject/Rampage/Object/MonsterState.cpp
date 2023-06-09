#include "MonsterState.h"
#include "Monster.h"
#include "..\Global\Locator.h"


Spawn_Monster::Spawn_Monster()
{
	// TERRAIN SPRITE  ANIMATION
	std::unique_ptr<TerrainSpriteComponent> pTerrainSpriteComponent = std::make_unique<TerrainSpriteComponent>();
	m_pListeners.push_back(std::move(pTerrainSpriteComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_SPRITE, m_pListeners.back().get(), this);
}

Spawn_Monster::~Spawn_Monster()
{
}

void Spawn_Monster::Enter(CMonster* monster)
{
	monster->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	monster->m_pSkinnedAnimationController->m_fTime = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition = 0.0f;
	monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nType = ANIMATION_TYPE_LOOP;
	monster->m_bCanChase = true;
	monster->m_fSpawnTime = 0.0f;
	monster->m_bDissolved = false;
	monster->m_fDissolveTime = 0.f;
	monster->m_fHP = 100.0f;
	monster->m_fDissolveThrethHold = 0.f;
	monster->CPhysicsObject::OnUpdateCallback(0.f);

	XMFLOAT3 xmf3Position = monster->GetPosition();
	//xmf3Position.y -= 10.f;
	OnGroundParams OnGround_params;
	OnGround_params.xmf3OnGroundPosition = xmf3Position;
	CMessageDispatcher::GetInst()->Dispatch_Message<OnGroundParams>(MessageType::ONGROUND, &OnGround_params, this);
}

void Spawn_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	monster->m_fSpawnTime += fElapsedTime;

	if (m_fMaxSpawnTime < monster->m_fSpawnTime)
	{
		monster->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	}
}

void Spawn_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(fElapsedTime);
}

void Spawn_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
}

void Spawn_Monster::Exit(CMonster* monster)
{
}

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

void Idle_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(fElapsedTime);
}

void Idle_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
}

void Idle_Monster::Exit(CMonster* monster)
{

}

Damaged_Monster::Damaged_Monster()
{
	// UPDOWNPARTICLE ANIMATION
	std::unique_ptr<UpDownParticleComponent> pUpDownParticlenComponent = std::make_unique<UpDownParticleComponent>();
	m_pListeners.push_back(std::move(pUpDownParticlenComponent));
	CMessageDispatcher::GetInst()->RegisterListener(MessageType::UPDATE_UPDOWNPARTICLE, m_pListeners.back().get(), this);
}

Damaged_Monster::~Damaged_Monster()
{
}

void Damaged_Monster::Enter(CMonster* monster)
{
	if (monster->m_pStateMachine->GetPreviousState() != Stun_Monster::GetInst())
	{
		monster->m_fHP -= 30.0f; // 몬스터 체력 고정 감소

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
	XMFLOAT3 xmf3LookVec = Vector3::Add(monster->GetPosition(), monster->GetHitterVec());
	monster->SetLookAt(Vector3::Add(monster->GetPosition(), XMFLOAT3(-monster->GetHitterVec().x, 0.0f, -monster->GetHitterVec().z)));

	CAnimationSet* pAnimationSet = monster->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_nAnimationSet];
	
	float fDamageDistance = monster->m_fDamageAnimationSpeed * fElapsedTime;
	float fElapsed = monster->m_pSkinnedAnimationController->m_fTime + monster->m_fStunTime;

	monster->m_fShakeDistance = 0.0f;

	if (monster->m_fShakeDuration > fElapsed)
	{
		float fShakeDistance = (monster->m_fMaxShakeDistance - (monster->m_fMaxShakeDistance / monster->m_fShakeDuration)
			* fElapsed) * cos((2 * PI * fElapsed) / monster->m_fShakeFrequency);
		monster->m_fShakeDistance = fShakeDistance;
	}

	XMFLOAT3 xmf3DamageVec = XMFLOAT3(0.0f, 0.0f, 0.0f);

	monster->m_fDamageDistance = 0.0f;

	if (monster->m_fTotalDamageDistance < monster->m_fMaxDamageDistance)
	{
		monster->m_fDamageDistance = fDamageDistance;
		monster->m_fTotalDamageDistance += fDamageDistance;

		if (monster->m_fMaxDamageDistance < monster->m_fTotalDamageDistance)
			monster->m_fDamageDistance -= (monster->m_fTotalDamageDistance - monster->m_fMaxDamageDistance);
	}

	xmf3DamageVec = Vector3::ScalarProduct(XMFLOAT3{ monster->GetHitterVec().x, 0.0f, monster->GetHitterVec().z }, MeterToUnit(monster->m_fDamageDistance), false);
	monster->Move(xmf3DamageVec, true);

	if (monster->m_fStunStartTime < monster->m_pSkinnedAnimationController->m_fTime && !monster->m_bStunned)
		monster->m_pStateMachine->ChangeState(Stun_Monster::GetInst());

	if (monster->m_pSkinnedAnimationController->m_pAnimationTracks[0].m_fPosition == pAnimationSet->m_fLength)
	{
		monster->SetNotHit();
		monster->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	}

}

void Damaged_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(fElapsedTime);
}

void Damaged_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
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
	XMFLOAT3 xmf3LookVec = Vector3::Add(monster->GetPosition(), monster->GetHitterVec());
	monster->SetLookAt(Vector3::Add(monster->GetPosition(), XMFLOAT3(-monster->GetHitterVec().x, 0.0f, -monster->GetHitterVec().z)));

	monster->m_fShakeDistance = 0.0f;

	float fElapsed = monster->m_pSkinnedAnimationController->m_fTime + monster->m_fStunTime;
	float fDamageDistance = monster->m_fDamageAnimationSpeed * fElapsedTime;

	if (monster->m_fShakeDuration > fElapsed)
	{
		float fShakeDistance = (monster->m_fMaxShakeDistance - (monster->m_fMaxShakeDistance / monster->m_fShakeDuration)
			* fElapsed) * cos((2 * PI * fElapsed) / monster->m_fShakeFrequency);
		monster->m_fShakeDistance = fShakeDistance;
	}

	XMFLOAT3 xmf3DamageVec = XMFLOAT3(0.0f, 0.0f, 0.0f);

	monster->m_fDamageDistance = 0.0f;

	if (monster->m_fTotalDamageDistance < monster->m_fMaxDamageDistance)
	{
		monster->m_fDamageDistance = fDamageDistance;
		monster->m_fTotalDamageDistance += fDamageDistance;

		if (monster->m_fMaxDamageDistance < monster->m_fTotalDamageDistance)
			monster->m_fDamageDistance -= (monster->m_fTotalDamageDistance - monster->m_fMaxDamageDistance);
	}

	xmf3DamageVec = Vector3::ScalarProduct(XMFLOAT3{ monster->GetHitterVec().x, 0.0f, monster->GetHitterVec().z }, MeterToUnit(monster->m_fDamageDistance), false);
	monster->Move(xmf3DamageVec, true);

	if (monster->m_fStunTime < monster->m_fMaxStunTime)
		monster->m_fStunTime += fElapsedTime;
	else
		monster->m_pStateMachine->ChangeState(Damaged_Monster::GetInst());
}

void Stun_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(0.0f);
}

void Stun_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
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

void Wander_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(fElapsedTime);
}

void Wander_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
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

	if (monster->m_fToPlayerLength < monster->m_fAttackRange)
	{
		// 공격
		monster->m_pStateMachine->ChangeState(Attack_Monster::GetInst());
	}

	if (monster->m_fToPlayerLength > monster->m_fSensingRange)
	{
		// 놓침
		monster->m_pStateMachine->ChangeState(Idle_Monster::GetInst());
	}

	monster->Move(xmf3Shift, true);
}

void Chasing_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(fElapsedTime);
}

void Chasing_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
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

void Attack_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(fElapsedTime);
}

void Attack_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
}

void Attack_Monster::Exit(CMonster* monster)
{
}

void Dead_Monster::Enter(CMonster* monster)
{
	monster->m_fDeadTime = 0.0f;
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
	if (monster->m_bArticulationOnPxScene) {
		RegisterArticulationParams Request_params;
		Request_params.pObject = monster;
		XMFLOAT3 force = monster->GetHitterVec();
		XMStoreFloat3(&force, XMVector3Rotate(XMLoadFloat3(&force), XMVECTOR({ 0, 1.0, 0, 1 })));
		force = Vector3::ScalarProduct(force, 50.0f, false);
		Request_params.m_force = force;
		CMessageDispatcher::GetInst()->Dispatch_Message<RegisterArticulationParams>(MessageType::REQUEST_REGISTERARTI, &Request_params, nullptr);
		monster->m_bArticulationOnPxScene = true;
	}
	else {
		RegisterArticulationParams Request_params;
		Request_params.pObject = monster;
		XMFLOAT3 force = monster->GetHitterVec();
		force = Vector3::ScalarProduct(force, 50.0f, false);
		Request_params.m_force = force;
		CMessageDispatcher::GetInst()->Dispatch_Message<RegisterArticulationParams>(MessageType::REQUEST_REGISTERARTI, &Request_params, nullptr);
		monster->m_bArticulationOnPxScene = true;
	}
	monster->m_bArticulationSleep = false;
}

void Dead_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	monster->m_fDeadTime += fElapsedTime;

	if (m_fMaxDeadTime < monster->m_fDeadTime)
	{
		if (monster->m_pStateMachine->GetCurrentState() != Spawn_Monster::GetInst())
		{
			CMonsterPool::GetInst()->SetNonActiveMonster(monster);
			monster->TestDissolvetime = 0.0f;
		}

	}
}

void Dead_Monster::Animate(CMonster* monster, float fElapsedTime)
{
	monster->Animate(fElapsedTime);
}

void Dead_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
}

void Dead_Monster::Exit(CMonster* monster)
{
	monster->m_bSimulateArticulate = false;
}

Global_Monster::Global_Monster()
{
}

Global_Monster::~Global_Monster()
{
}

void Global_Monster::Enter(CMonster* monster)
{
}

void Global_Monster::Execute(CMonster* monster, float fElapsedTime)
{
	if (monster->m_fHP < 0.0f) {
		if(monster->m_pStateMachine->GetCurrentState() != Dead_Monster::GetInst())
			monster->m_pStateMachine->ChangeState(Dead_Monster::GetInst());
	}
}

void Global_Monster::Animate(CMonster* monster, float fElapsedTime)
{
}

void Global_Monster::OnRootMotion(CMonster* monster, float fTimeElapsed)
{
}

void Global_Monster::Exit(CMonster* monster)
{
}
