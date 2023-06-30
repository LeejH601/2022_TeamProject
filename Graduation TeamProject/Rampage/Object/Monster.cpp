#include "Monster.h"
#include "Player.h"
#include "ModelManager.h"
#include "..\Global\Locator.h"
#include "..\Shader\BoundingBoxShader.h"
#include "Texture.h"
#include "..\Sound\SoundManager.h"

CMonster::CMonster()
{
	m_pStateMachine = std::make_unique<CStateMachine<CMonster>>(this);
	m_pStateMachine->SetCurrentState(Idle_Monster::GetInst());
	m_pStateMachine->SetPreviousState(Idle_Monster::GetInst());
	m_pStateMachine->SetGlobalState(Global_Monster::GetInst());
	m_fShakeDistance = 0.0f;

	m_fStunTime = 0.0f;
	m_fStunStartTime = 0.2f;

	m_fMaxDissolveTime = 3.0f;
	m_fDissolveThrethHold = 0.0f;
	m_fDissolveTime = 0.0f;
	TestDissolvetime = 0.0f;

	m_fHP = 300.0f;

	m_fSpeedKperH = 10.0f;
	m_fSpeedUperS = MeterToUnit(m_fSpeedKperH * 1000.0f) / 3600.0f;

	m_fAttackRange = MeterToUnit(1.0f);
	m_fAtkStartTime = 0.0f;
	m_fAtkEndTime = 0.0f;
	m_fSensingRange = MeterToUnit(20.0f);

	std::unique_ptr<PlayerAttackListener> pCollisionComponent = std::make_unique<PlayerAttackListener>();
	pCollisionComponent->SetObject(this);
	m_pListeners.push_back(std::move(pCollisionComponent));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAYER_ATTACK, m_pListeners.back().get());

	std::unique_ptr<PlayerLocationListener> pPlayerLocationListener = std::make_unique<PlayerLocationListener>();
	pPlayerLocationListener->SetObject(this);
	m_pListeners.push_back(std::move(pPlayerLocationListener));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::CHECK_IS_PLAYER_IN_FRONT_OF_MONSTER, m_pListeners.back().get());

	std::unique_ptr<DamageListener> pDamageListener = std::make_unique<DamageListener>();
	pDamageListener->SetObject(this);
	m_pListeners.push_back(std::move(pDamageListener));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::APPLY_DAMAGE, m_pListeners.back().get(), this);
}



CMonster::~CMonster()
{
}

void CMonster::SetWanderVec()
{
	m_xmf3WanderVec.x = RandomFloatInRange(-10.0f, 10.0f);
	m_xmf3WanderVec.z = RandomFloatInRange(-10.0f, 10.0f);

	m_xmf3WanderVec = Vector3::Normalize(m_xmf3WanderVec);
}

void CMonster::CheckIsPlayerInFrontOfThis(XMFLOAT3 xmf3PlayerPosition)
{
	XMFLOAT3 xmf3MonsterLook = GetLook();
	XMFLOAT3 xmf3ToPlayerVec = Vector3::Subtract(xmf3PlayerPosition, GetPosition());

	float fDotProduct = Vector3::DotProduct(xmf3MonsterLook, xmf3ToPlayerVec);
	m_fToPlayerLength = Vector3::Length(xmf3ToPlayerVec);

	if (!m_bCanChase)
		return;

	if (/*fDotProduct > 0.0f && */m_fToPlayerLength < 22.5f)
	{
		m_xmf3ChasingVec = Vector3::Normalize(xmf3ToPlayerVec);

		// 플레이어가 몬스터의 앞에 있음
		if (m_pStateMachine->GetCurrentState() != Chasing_Monster::GetInst() && m_pStateMachine->GetCurrentState() != Spawn_Monster::GetInst())
			m_pStateMachine->ChangeState(Chasing_Monster::GetInst());
	}
}

void CMonster::CalculateResultPosition()
{
	XMFLOAT3 xmf3ShakeVec = Vector3::ScalarProduct(Vector3::Normalize(GetRight()), m_fShakeDistance, false);

	m_xmf3CalPos = Vector3::Add(m_xmf3Position, xmf3ShakeVec);
}

void CMonster::ApplyDamage(float Damage, void* pData)
{
	m_fHP -= Damage;
	m_fHP = max(0.0f, m_fHP);

	m_fCurrShield -= 10.0f;
	m_fCurrShield = max(0.0f, m_fCurrShield);
}

void CMonster::SetElite(bool flag)
{
	if (m_bElite == flag)
		return;
	m_bElite = flag;
	if (m_bElite) {
		XMFLOAT3 scale = GetScale();
		SetScale(scale.x * 1.5f, scale.y * 1.5f, scale.z * 1.5f);
		m_fCurrShield = m_fMaxShield;
		/*m_fRimLightFactor = 0.8f;
		m_xmf3RimLightColor = XMFLOAT3(1.0f, 0.05f, 0.0f);*/
	}
	else {
		XMFLOAT3 scale = GetScale();
		SetScale(scale.x / 1.5f, scale.y / 1.5f, scale.z / 1.5f);
	}
}

void CMonster::UpdateMatrix()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
	m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
	m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
	m_xmf4x4Transform._41 = m_xmf3CalPos.x; m_xmf4x4Transform._42 = m_xmf3CalPos.y; m_xmf4x4Transform._43 = m_xmf3CalPos.z;

	XMMATRIX mtxScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);
}

void CMonster::SetScale(float x, float y, float z)
{
	CPhysicsObject::SetScale(x, y, z);
	m_pSkinnedAnimationController->m_xmf3RootObjectScale = m_xmf3Scale;
}

void CMonster::Update(float fTimeElapsed)
{
	if (!m_bDissolved) {
		if (m_bSimulateArticulate) {
			TestDissolvetime += fTimeElapsed;
			if (TestDissolvetime > 5.0f)
				m_bDissolved = true;
		}
	}
	else {
		m_fDissolveTime += fTimeElapsed;
		m_fDissolveThrethHold = m_fDissolveTime / m_fMaxDissolveTime;
		if (m_fDissolveThrethHold > 1.0f && m_bArticulationSleep == false) {
			m_bArticulationSleep = true;
			RegisterArticulationSleepParams Request_params;
			Request_params.pObject = this;
			CMessageDispatcher::GetInst()->Dispatch_Message<RegisterArticulationSleepParams>(MessageType::REQUEST_SLEEPARTI, &Request_params, nullptr);
		}
	}

	if (m_xmf3Velocity.x + m_xmf3Velocity.z)
		SetLookAt(Vector3::Add(GetPosition(), Vector3::Normalize(XMFLOAT3{ m_xmf3Velocity.x, 0.0f, m_xmf3Velocity.z })));

	// 현재 행동을 선택함
	m_pStateMachine->Update(fTimeElapsed);
	m_pStateMachine->Animate(fTimeElapsed);

	CPhysicsObject::Apply_Gravity(fTimeElapsed);

	XMFLOAT3 xmf3NewVelocity = Vector3::TransformCoord(m_xmf3Velocity, XMMatrixRotationAxis(XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f }, XMConvertToRadians(fDistortionDegree)));
	CPhysicsObject::Move(xmf3NewVelocity, false);

	// 플레이어가 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext) CPhysicsObject::OnUpdateCallback(fTimeElapsed);

	XMFLOAT3 xmf3ShakeVec = Vector3::ScalarProduct(Vector3::Normalize(GetRight()), MeterToUnit(m_fShakeDistance), false);
	m_xmf3CalPos = Vector3::Add(m_xmf3Position, xmf3ShakeVec);

	CPhysicsObject::Apply_Friction(fTimeElapsed);
}
void CMonster::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	CPhysicsObject::UpdateTransform(NULL);
	m_pSkinnedAnimationController->UpdateSocketsTransform();

	m_BodyBoundingBox.Transform(m_TransformedBodyBoundingBox, XMLoadFloat4x4(&m_xmf4x4Transform));
#ifdef RENDER_BOUNDING_BOX
	if (pBodyBoundingBoxMesh)
		pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
#endif // RENDER_BOUNDING_BOX

	if (pWeapon)
	{
#ifdef RENDER_BOUNDING_BOX
		if (pWeaponBoundingBoxMesh)
			pWeaponBoundingBoxMesh->SetWorld(pWeapon->GetWorld());
#endif 
		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoundingBox, XMLoadFloat4x4(&pWeapon->GetWorld()));
	}

	if (m_bSimulateArticulate) {
		m_pSkinnedAnimationController->UpdateBoneTransform();
	}

}

bool CMonster::SetHit(CGameObject* pHitter)
{
	return false;
}
void CMonster::PlayMonsterEffectSound()
{
}
bool CMonster::CheckCollision(CGameObject* pTargetObject)
{
	if (pTargetObject)
	{
		BoundingOrientedBox* TargetBoundingBox = pTargetObject->GetBoundingBox();
		if (m_TransformedWeaponBoundingBox.Intersects(*TargetBoundingBox)) {
			if(pTargetObject->SetHit(this))
				PlayMonsterEffectSound();
			return true;
		}
	}
	return false;
}
void CMonster::UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> pArtiLinkNames, std::vector<XMFLOAT4X4>& AritculatCacheMatrixs, float scale)
{
	std::string target = m_pstrFrameName;
	auto it = std::find(pArtiLinkNames.begin(), pArtiLinkNames.end(), target);
	int distance = std::distance(pArtiLinkNames.begin(), it);
	if (distance < pArtiLinkNames.size()) {
		m_xmf4x4World = AritculatCacheMatrixs[distance];
	}
	else {
		m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;
	}

	XMFLOAT3 xAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);

	m_BodyBoundingBox.Transform(m_TransformedBodyBoundingBox, XMLoadFloat4x4(&Matrix4x4::Multiply(XMMatrixRotationAxis(XMLoadFloat3(&xAxis), XMConvertToRadians(90.0f)), AritculatCacheMatrixs[0])));
#ifdef RENDER_BOUNDING_BOX
	if (pBodyBoundingBoxMesh)
		pBodyBoundingBoxMesh->SetWorld(Matrix4x4::Multiply(XMMatrixRotationAxis(XMLoadFloat3(&xAxis), XMConvertToRadians(90.0f)), AritculatCacheMatrixs[0]));

#endif // RENDER_BOUNDING_BOX
	if (m_pSibling) m_pSibling->UpdateTransformFromArticulation(pxmf4x4Parent, pArtiLinkNames, AritculatCacheMatrixs, scale);
	if (m_pChild) m_pChild->UpdateTransformFromArticulation(&m_xmf4x4World, pArtiLinkNames, AritculatCacheMatrixs, scale);
}

void CMonster::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	m_bRimLightEnable = (m_bElite && GetHasShield()) ? 1 : 0;
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_bRimLightEnable, 34);
	UpdateTransform(NULL);
	CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
COrcObject::COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_MonsterType = MONSTER_TYPE::ORC;
	m_fAtkStartTime = 0.55f;
	m_fAtkEndTime = 0.92;

	m_iAttackAnimationNum = 0;
	m_fAttackSoundDelay = 0.55f;
	m_fAttackSoundVolume = 1.5f;
	m_strAttackSoundPath = "Sound/shoot/Air Cut by Langerium Id-84616.wav";

	CLoadedModelInfo* pOrcModel = CModelManager::GetInst()->GetModelInfo("Object/Orc.bin");;
	if (!pOrcModel) pOrcModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Orc.bin");

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetChild(pOrcModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pOrcModel);
	CGameObject* weaponFrame = FindFrame("hand_r");
	m_pSkinnedAnimationController->SetSocket(0, std::string("hand_r"), weaponFrame);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
COrcObject::~COrcObject()
{
}
void COrcObject::PlayMonsterEffectSound()
{
	CSoundManager::GetInst()->PlaySound("Sound/effect/MP_Left Hook.mp3", 2.0f, 0.0f);
}
void COrcObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Weapon");

	m_BodyBoundingBox = BoundingOrientedBox{ XMFLOAT3(0.0f, 1.05f, 0.0f),  XMFLOAT3(0.6f, 1.0f, 0.6f), XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
	m_WeaponBoundingBox = BoundingOrientedBox{ XMFLOAT3(0.0f, 0.0f, 0.35f), XMFLOAT3(0.15f, 0.3f, 0.725f), XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.05f, 0.0f), XMFLOAT3(0.6f, 1.0f, 0.6f));
	//pWeaponBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.32f), XMFLOAT3(0.18f, 0.28f, 0.71f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.35f), XMFLOAT3(0.15f, 0.3f, 0.725f));
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGoblinObject::CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_MonsterType = MONSTER_TYPE::GOBLIN;
	m_fAtkStartTime = 0.53f;
	m_fAtkEndTime = 0.69f;

	m_iAttackAnimationNum = 2;
	m_fAttackSoundDelay = 0.53f;
	m_fAttackSoundVolume = 1.0f;
	m_strAttackSoundPath = "Sound/shoot/Air Cut by Langerium Id-84616.wav";

	CLoadedModelInfo* pGoblinModel = CModelManager::GetInst()->GetModelInfo("Object/Goblin.bin");;
	if (!pGoblinModel) pGoblinModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Goblin.bin");

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetChild(pGoblinModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pGoblinModel);
	CGameObject* weaponFrame = FindFrame("hand_r");
	m_pSkinnedAnimationController->SetSocket(0, std::string("hand_r"), weaponFrame);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
CGoblinObject::~CGoblinObject()
{
}
void CGoblinObject::PlayMonsterEffectSound()
{
	CSoundManager::GetInst()->PlaySound("Sound/effect/MP_Left Hook.mp3", 1.5f, 0.0f);
}
void CGoblinObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Weapon");
	m_BodyBoundingBox = BoundingOrientedBox{ XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.3f, 0.55f, 0.3f), XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
	m_WeaponBoundingBox = BoundingOrientedBox{ XMFLOAT3(0.0f, 0.0f, 0.22f), XMFLOAT3(0.07f, 0.07f, 0.415f), XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };

#ifdef RENDER_BOUNDING_BOX
	//pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.3f, 0.55f, 0.3f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.22f), XMFLOAT3(0.07f, 0.07f, 0.415f));
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkeletonObject::CSkeletonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_MonsterType = MONSTER_TYPE::SKELETON;
	m_fAtkStartTime = 1.15f;
	m_fAtkEndTime = 1.3f;

	m_iAttackAnimationNum = 2;
	m_fAttackSoundDelay = 1.15f;
	m_fAttackSoundVolume = 0.35f;
	m_strAttackSoundPath = "Sound/shoot/ethanchase7744__sword-slash.wav";

	CLoadedModelInfo* pSkeletonModel = CModelManager::GetInst()->GetModelInfo("Object/Skeleton.bin");;
	if (!pSkeletonModel) pSkeletonModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Skeleton.bin");

	SetChild(pSkeletonModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pSkeletonModel);
	CGameObject* weaponFrame = FindFrame("hand_r");
	m_pSkinnedAnimationController->SetSocket(1, std::string("hand_r"), weaponFrame);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
CSkeletonObject::~CSkeletonObject()
{
}
void CSkeletonObject::PlayMonsterEffectSound()
{
	//CSoundManager::GetInst()->PlaySound("Sound/effect/Buffer Spell.wav", 1.0f, 0.0f);
}
void CSkeletonObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Sword");
	m_BodyBoundingBox = BoundingOrientedBox{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.35f, 1.0f, 0.35f), XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
	m_WeaponBoundingBox = BoundingOrientedBox{ XMFLOAT3(0.0f, 0.0f, 0.48f), XMFLOAT3(0.02f, 0.07f, 0.61f), XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.35f, 1.0f, 0.35f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.48f), XMFLOAT3(0.02f, 0.07f, 0.61f));
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMonsterRootAnimationController::CMonsterRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel) : CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pModel)
{
}

CMonsterRootAnimationController::~CMonsterRootAnimationController()
{
}

void CMonsterRootAnimationController::OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed)
{
}

bool CMonsterPool::SetNonActiveMonster(MONSTER_TYPE monsterType, CMonster* pMonster) // Active 여부 반환
{
	if(m_pNonActiveMonsters.size() != (static_cast<int>(MONSTER_TYPE::NONE)))
		m_pNonActiveMonsters.resize(static_cast<int>(MONSTER_TYPE::NONE));

	// NonActiveMonster 있는 몬스터를 Enable true 변경 및 
	auto it = std::find(m_pNonActiveMonsters[static_cast<int>(monsterType)].begin(), 
		m_pNonActiveMonsters[static_cast<int>(monsterType)].end(), 
		pMonster);

	if ((it == m_pNonActiveMonsters[static_cast<int>(monsterType)].end())) {
		pMonster->SetEnable(false);
		m_pNonActiveMonsters[static_cast<int>(monsterType)].push_back(pMonster);
		return true;
	}
	return false;
}

bool CMonsterPool::SetActiveMonster(MONSTER_TYPE monsterType, MonsterSpawnInfo monsterSpawnInfo)
{
	// NonActiveMonster 있는 몬스터를 Enable true 변경 및 
	if (m_pNonActiveMonsters[static_cast<int>(monsterType)].size() > 0) {
		CMonster* pMonster = dynamic_cast<CMonster*>(m_pNonActiveMonsters[static_cast<int>(monsterType)].back());
		m_pNonActiveMonsters[static_cast<int>(monsterType)].pop_back();
		pMonster->SetEnable(true);
		pMonster->SetPosition(monsterSpawnInfo.xmf3Position);
		monsterSpawnInfo.bIsElite ? pMonster->SetElite(true) : pMonster->SetElite(false);
		pMonster->m_pStateMachine->ChangeState(Spawn_Monster::GetInst());
		return true;
	}
	return false;
}

void CMonsterPool::SpawnMonster(MONSTER_TYPE monsterType, int MonsterN, MonsterSpawnInfo* monsterSpawnInfo)
{
	// 개수, 위치배열, 방향
	// 플레이어 위치 근처
	for (int i = 0; i < MonsterN; i++)
		SetActiveMonster(monsterType, monsterSpawnInfo[i]);
}
