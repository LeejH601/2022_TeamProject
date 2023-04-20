#include "Monster.h"
#include "ModelManager.h"
#include "..\Global\Locator.h"
#include "..\Shader\BoundingBoxShader.h"

CMonster::CMonster()
{
	m_pStateMachine = std::make_unique<CStateMachine<CMonster>>(this);
	m_pStateMachine->SetCurrentState(Idle_Monster::GetInst());
	m_pStateMachine->SetPreviousState(Idle_Monster::GetInst());
	m_pStateMachine->SetGlobalState(Global_Monster::GetInst());
	m_fStunStartTime = 0.0f;
	m_fShakeDistance = 0.0f;
	m_fStunTime = 0.0f;
	m_fMaxDissolveTime = 3.0f;
	m_fDissolveThrethHold = 0.0f;
	m_fDissolveTime = 0.0f;
	TestDissolvetime = 0.0f;

	m_fHP = 100.0f;

	m_fSpeedKperH = 2.0f;
	m_fSpeedMperS = m_fSpeedKperH * 1000.0f / 3600.0f;
	m_fSpeedUperS = m_fSpeedMperS * 8.0f / 1.0f;

	std::unique_ptr<PlayerAttackListener> pCollisionComponent = std::make_unique<PlayerAttackListener>();
	pCollisionComponent->SetObject(this);
	m_pListeners.push_back(std::move(pCollisionComponent));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::PLAYER_ATTACK, m_pListeners.back().get());

	std::unique_ptr<PlayerLocationListener> pPlayerLocationListener = std::make_unique<PlayerLocationListener>();
	pPlayerLocationListener->SetObject(this);
	m_pListeners.push_back(std::move(pPlayerLocationListener));

	CMessageDispatcher::GetInst()->RegisterListener(MessageType::CHECK_IS_PLAYER_IN_FRONT_OF_MONSTER, m_pListeners.back().get());
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

	if (fDotProduct > 0.0f && m_fToPlayerLength < 40.0f)
	{
		m_xmf3ChasingVec = Vector3::Normalize(xmf3ToPlayerVec);

		// 플레이어가 몬스터의 앞에 있음
		if (m_pStateMachine->GetCurrentState() != Chasing_Monster::GetInst())
			m_pStateMachine->ChangeState(Chasing_Monster::GetInst());
	}
}

void CMonster::CalculateResultPosition()
{
	XMFLOAT3 xmf3ShakeVec = Vector3::ScalarProduct(GetRight(), m_fShakeDistance, false);

	m_xmf3CalPos = Vector3::Add(m_xmf3Position, xmf3ShakeVec);
}

void CMonster::OnPrepareRender()
{
	if (m_bSimulateArticulate) {
		UpdateTransformFromArticulation(NULL, m_pArtiLinkNames, m_AritculatCacheMatrixs, m_xmf3Scale.x);
	}
	else {
		m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
		m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
		m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
		m_xmf4x4Transform._41 = m_xmf3CalPos.x; m_xmf4x4Transform._42 = m_xmf3CalPos.y; m_xmf4x4Transform._43 = m_xmf3CalPos.z;

		XMMATRIX mtxScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
		m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

		UpdateTransform(NULL);
	}
}

void CMonster::SetScale(float x, float y, float z)
{
	CPhysicsObject::SetScale(x, y, z);
	m_pSkinnedAnimationController->m_xmf3RootObjectScale = m_xmf3Scale;
}

void CMonster::Update(float fTimeElapsed)
{
	// 현재 행동을 선택함
	m_pStateMachine->Update(fTimeElapsed);

	CPhysicsObject::Apply_Gravity(fTimeElapsed);

	if (!m_bDissolved) {
		if (m_bSimulateArticulate) {
			TestDissolvetime += fTimeElapsed;
			if (TestDissolvetime > 20.0f)
				m_bDissolved = true;
		}
	}
	else{
		m_fDissolveTime += fTimeElapsed;
		m_fDissolveThrethHold = m_fDissolveTime / m_fMaxDissolveTime;
	}
	
	CPhysicsObject::Move(m_xmf3Velocity, false);

	// 플레이어가 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext) CPhysicsObject::OnUpdateCallback(fTimeElapsed);

	CalculateResultPosition();

	m_pStateMachine->Animate(fTimeElapsed);

	CPhysicsObject::Apply_Friction(fTimeElapsed);
}
void CMonster::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);

	m_BodyBoundingBox.Transform(m_TransformedBodyBoudningBox, XMLoadFloat4x4(&m_xmf4x4Transform));
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
#endif // RENDER_BOUNDING_BOX

	if (pWeapon)
	{
#ifdef RENDER_BOUNDING_BOX
		pWeaponBoundingBoxMesh->SetWorld(pWeapon->GetWorld());
#endif 
		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoundingBox, XMLoadFloat4x4(&pWeapon->GetWorld()));
	}
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
	
	m_BodyBoundingBox.Transform(m_TransformedBodyBoudningBox, XMLoadFloat4x4(&Matrix4x4::Multiply(XMMatrixRotationAxis(XMLoadFloat3(&xAxis), XMConvertToRadians(90.0f)), AritculatCacheMatrixs[0])));
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh->SetWorld(Matrix4x4::Multiply(XMMatrixRotationAxis(XMLoadFloat3(&xAxis), XMConvertToRadians(90.0f)), AritculatCacheMatrixs[0]));
#endif // RENDER_BOUNDING_BOX
	if (m_pSibling) m_pSibling->UpdateTransformFromArticulation(pxmf4x4Parent, pArtiLinkNames, AritculatCacheMatrixs, scale);
	if (m_pChild) m_pChild->UpdateTransformFromArticulation(&m_xmf4x4World, pArtiLinkNames, AritculatCacheMatrixs, scale);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
COrcObject::COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_MonsterType = MONSTER_TYPE::ORC;

	CLoadedModelInfo* pOrcModel = CModelManager::GetInst()->GetModelInfo("Object/Orc.bin");;
	if (!pOrcModel) pOrcModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Orc.bin");

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetChild(pOrcModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pOrcModel);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
COrcObject::~COrcObject()
{
}
void COrcObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Weapon");

	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 1.05f, 0.0f), XMFLOAT3(1.2f, 2.0f, 1.2f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.0f, 0.85f), XMFLOAT3(0.3f, 0.6f, 0.35f) };
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.05f, 0.0f), XMFLOAT3(1.2f, 2.0f, 1.2f));
	//pWeaponBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.32f), XMFLOAT3(0.18f, 0.28f, 0.71f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.85f), XMFLOAT3(0.3f, 0.6f, 0.35f));
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGoblinObject::CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_MonsterType = MONSTER_TYPE::GOBLIN;

	m_fStunTime = 0.0f;
	m_fStunStartTime = 0.2f;

	CLoadedModelInfo* pGoblinModel = CModelManager::GetInst()->GetModelInfo("Object/Goblin.bin");;
	if (!pGoblinModel) pGoblinModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Goblin.bin");

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetChild(pGoblinModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pGoblinModel);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
CGoblinObject::~CGoblinObject()
{
}
void CGoblinObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Weapon");
	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.6f, 1.1f, 0.6f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.0f, 0.22f), XMFLOAT3(0.14f, 0.14f, 0.83f) };
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.6f, 1.1f, 0.6f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.22f), XMFLOAT3(0.14f, 0.14f, 0.83f));
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkeletonObject::CSkeletonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_MonsterType = MONSTER_TYPE::SKELETON;

	CLoadedModelInfo* pSkeletonModel = CModelManager::GetInst()->GetModelInfo("Object/Skeleton.bin");;
	if (!pSkeletonModel) pSkeletonModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Skeleton.bin");

	SetChild(pSkeletonModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pSkeletonModel);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
CSkeletonObject::~CSkeletonObject()
{
}
void CSkeletonObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Sword");
	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.0f, 0.48f), XMFLOAT3(0.04f, 0.14f, 1.22f) };
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.48f), XMFLOAT3(0.04f, 0.14f, 1.22f));
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

void CMonsterRootAnimationController::OnRootMotion(CGameObject* pRootGameObject)
{
}
