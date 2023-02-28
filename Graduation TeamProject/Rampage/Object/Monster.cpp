#include "Monster.h"
#include "MonsterState.h"
#include "ModelManager.h"
#include "AnimationComponent.h"
#include "..\Global\Locator.h"
#include "..\Shader\BoundingBoxShader.h"

CMonster::CMonster()
{
	m_pStateMachine = std::make_unique<CStateMachine<CMonster>>(this);
	m_pStateMachine->SetCurrentState(Idle_Monster::GetInst());
	m_pStateMachine->SetPreviousState(Idle_Monster::GetInst());
	m_fStunStartTime = 0.0f;
	m_fShakeDistance = 0.0f;
	m_fStunTime = 0.0f;
}

CMonster::~CMonster()
{
}

void CMonster::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CMonster::Update(float fTimeElapsed)
{
	m_pStateMachine->Update(fTimeElapsed);
}
void CMonster::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst() ||
		m_pStateMachine->GetCurrentState() == Stun_Monster::GetInst())
	{
		XMFLOAT3 xmf3ShakeVec = Vector3::ScalarProduct(GetRight(), m_fShakeDistance, false);
		XMFLOAT3 xmf3DamageVec = Vector3::ScalarProduct(m_xmf3HitterVec, m_fDamageDistance, false);
		//XMFLOAT3 xmf3DamageVec = XMFLOAT3{};
		XMFLOAT3 xmf3Pos = Vector3::Add(Vector3::Add(GetPosition(), xmf3ShakeVec), xmf3DamageVec);

		m_xmf4x4Transform._41 = xmf3Pos.x;
		m_xmf4x4Transform._42 = xmf3Pos.y;
		m_xmf4x4Transform._43 = xmf3Pos.z;
	}

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
		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoudningBox, XMLoadFloat4x4(&pWeapon->GetWorld()));
	}
}
void CMonster::UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> m_pArtiLinkNames, std::vector<physx::PxArticulationLink*> m_pArticulationLinks, float scale)
{
	std::string target = CGameObject::GetFrameName();
	auto it = std::find(m_pArtiLinkNames.begin(), m_pArtiLinkNames.end(), target);
	int distance = std::distance(m_pArtiLinkNames.begin(), it);
	if (distance < m_pArtiLinkNames.size()) {
		CGameObject* obj = FindFrame(it->c_str());
		physx::PxTransform transform = m_pArticulationLinks[distance]->getGlobalPose();
		//physx::PxTransform transform = m_pArticulationLinks[index]->getInboundJoint()->getChildPose();
		physx::PxMat44 World = physx::PxMat44(transform);

		memcpy(&obj->m_xmf4x4World, &World, sizeof(XMFLOAT4X4));
	}
	else {
		if (m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst() ||
			m_pStateMachine->GetCurrentState() == Stun_Monster::GetInst())
		{
			XMFLOAT3 xmf3ShakeVec = Vector3::ScalarProduct(GetRight(), m_fShakeDistance, false);
			XMFLOAT3 xmf3DamageVec = Vector3::ScalarProduct(m_xmf3HitterVec, m_fDamageDistance, false);
			//XMFLOAT3 xmf3DamageVec = XMFLOAT3{};
			XMFLOAT3 xmf3Pos = Vector3::Add(Vector3::Add(GetPosition(), xmf3ShakeVec), xmf3DamageVec);

			m_xmf4x4Transform._41 = xmf3Pos.x;
			m_xmf4x4Transform._42 = xmf3Pos.y;
			m_xmf4x4Transform._43 = xmf3Pos.z;
		}

		m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;
	}

	if (m_pSibling) m_pSibling->UpdateTransformFromArticulation(pxmf4x4Parent, m_pArtiLinkNames, m_AritculatCacheMatrixs, m_xmf4x4Scale._11);
	if (m_pChild) m_pChild->UpdateTransformFromArticulation(&m_xmf4x4World, m_pArtiLinkNames, m_AritculatCacheMatrixs, m_xmf4x4Scale._11);

	m_BodyBoundingBox.Transform(m_TransformedBodyBoudningBox, XMLoadFloat4x4(&m_xmf4x4Transform));
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
#endif // RENDER_BOUNDING_BOX

	if (pWeapon)
	{
#ifdef RENDER_BOUNDING_BOX
		pWeaponBoundingBoxMesh->SetWorld(pWeapon->GetWorld());
#endif 
		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoudningBox, XMLoadFloat4x4(&pWeapon->GetWorld()));
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
COrcObject::COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	CLoadedModelInfo* pOrcModel = CModelManager::GetInst()->GetModelInfo("Object/Orc.bin");;
	if (!pOrcModel) pOrcModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Orc.bin");

	SetChild(pOrcModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pOrcModel);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
COrcObject::~COrcObject()
{
}
void COrcObject::Animate(float fTimeElapsed)
{
	if (m_bSimulateArticulate) {
		UpdateTransformFromArticulation(NULL, m_pArtiLinkNames, m_pArticulationLinks, m_xmf4x4Scale._11);
	}
	else {
		CGameObject::Animate(fTimeElapsed);
	}
}
void COrcObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	CMonster::UpdateTransform(pxmf4x4Parent);
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
	m_fStunTime = 0.0f;
	m_fStunStartTime = 0.4f;

	CLoadedModelInfo* pGoblinModel = CModelManager::GetInst()->GetModelInfo("Object/Goblin.bin");;
	if (!pGoblinModel) pGoblinModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/Goblin.bin");

	SetChild(pGoblinModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pGoblinModel);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
}
CGoblinObject::~CGoblinObject()
{
}
void CGoblinObject::Animate(float fTimeElapsed)
{
	if (m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst())
	{
		if (m_fStunStartTime < m_pSkinnedAnimationController->m_fTime && !m_bStunned)
			m_pStateMachine->ChangeState(Stun_Monster::GetInst());
		CGameObject::Animate(fTimeElapsed);
	}

	else if (m_pStateMachine->GetCurrentState() == Stun_Monster::GetInst())
	{
	}

	else {
		if (m_bSimulateArticulate) {
				UpdateTransformFromArticulation(NULL, m_pArtiLinkNames, m_pArticulationLinks, m_xmf4x4Scale._11);
		}
		else {
			CGameObject::Animate(fTimeElapsed);
		}
	}
}
void CGoblinObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	CMonster::UpdateTransform(pxmf4x4Parent);
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
	CLoadedModelInfo* pSkeletonModel = CModelManager::GetInst()->GetModelInfo("Object/SK_Skeleton.bin");;
	if (!pSkeletonModel) pSkeletonModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/SK_Skeleton.bin");

	SetChild(pSkeletonModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pSkeletonModel);

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
	/*CLoadedModelInfo* pArmorModel = CModelManager::GetInst()->GetModelInfo("Object/SK_Armor.bin");;
	if (!pArmorModel) pArmorModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/SK_Armor.bin");

	SetChild(pArmorModel->m_pModelRootObject, true);*/
	//m_pSkinnedAnimationController = std::make_unique<CAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pArmorModel);
}
CSkeletonObject::~CSkeletonObject()
{
}
void CSkeletonObject::Animate(float fTimeElapsed)
{
	if (m_bSimulateArticulate) {
		UpdateTransformFromArticulation(NULL, m_pArtiLinkNames, m_pArticulationLinks, m_xmf4x4Scale._11);
	}
	else {
		CGameObject::Animate(fTimeElapsed);
	}
}
void CSkeletonObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	CMonster::UpdateTransform(pxmf4x4Parent);
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
