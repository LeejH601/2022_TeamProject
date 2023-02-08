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
}

CMonster::~CMonster()
{
}

void CMonster::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	XMFLOAT3 m_xmf3OriginPos = GetPosition();

	if (m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst())
	{
		XMFLOAT3 xmf3Pos = Vector3::Add(m_xmf3OriginPos, Vector3::ScalarProduct(GetRight(), m_fShakeDistance, false));
		SetPosition(xmf3Pos);
	}

	CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);

	if (m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst())
		SetPosition(m_xmf3OriginPos);
}

void CMonster::Update(float fTimeElapsed)
{
	m_pStateMachine->Update(fTimeElapsed);
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
	CGameObject::Animate(fTimeElapsed);
}
void COrcObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);

	pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
	m_BodyBoundingBox.Transform(m_TransformedBodyBoudningBox, XMLoadFloat4x4(&m_xmf4x4Transform));

	if (pWeapon)
	{
		pWeaponBoundingBoxMesh->SetWorld(pWeapon->GetWorld());
		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoudningBox, XMLoadFloat4x4(&pWeapon->GetWorld()));
	}
}
void COrcObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Weapon");

	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 1.05f, 0.0f), XMFLOAT3(1.2f, 2.0f, 1.2f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.0f, 0.85f), XMFLOAT3(0.3f, 0.6f, 0.35f) };
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.05f, 0.0f), XMFLOAT3(1.2f, 2.0f, 1.2f));
	//pWeaponBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.32f), XMFLOAT3(0.18f, 0.28f, 0.71f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.85f), XMFLOAT3(0.3f, 0.6f, 0.35f));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGoblinObject::CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
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
		m_fShakeDistance = CShakeAnimationComponent::GetInst()->GetShakeDistance(m_pSkinnedAnimationController->m_fTime);
	}
	CGameObject::Animate(fTimeElapsed);
}
void CGoblinObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);

	pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
	m_BodyBoundingBox.Transform(m_TransformedBodyBoudningBox, XMLoadFloat4x4(&m_xmf4x4Transform));

	if (pWeapon)
	{
		pWeaponBoundingBoxMesh->SetWorld(pWeapon->GetWorld());
		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoudningBox, XMLoadFloat4x4(&pWeapon->GetWorld()));
	}
}
void CGoblinObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Weapon");
	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.6f, 1.1f, 0.6f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.0f, 0.22f), XMFLOAT3(0.14f, 0.14f, 0.83f) };
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.6f, 1.1f, 0.6f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.22f), XMFLOAT3(0.14f, 0.14f, 0.83f));
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
	CGameObject::Animate(fTimeElapsed);
}
void CSkeletonObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	XMMATRIX mtxScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, mtxScale);

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);

	pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
	m_BodyBoundingBox.Transform(m_TransformedBodyBoudningBox, XMLoadFloat4x4(&m_xmf4x4Transform));

	if (pWeapon)
	{
		XMMATRIX mtxScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
		XMFLOAT4X4 m_xmf4x4WeaponWorld = Matrix4x4::Multiply(pWeapon->GetWorld(), mtxScale);
		pWeaponBoundingBoxMesh->SetWorld(m_xmf4x4WeaponWorld);
		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoudningBox, XMLoadFloat4x4(&m_xmf4x4WeaponWorld));
	}
}
void CSkeletonObject::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("SM_Sword");
	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.0f, 0.48f), XMFLOAT3(0.04f, 0.14f, 1.22f) };
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, pWeapon, XMFLOAT3(0.0f, 0.0f, 0.48f), XMFLOAT3(0.04f, 0.14f, 1.22f));
}