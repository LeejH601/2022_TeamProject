#pragma once
#pragma once
#include "Object.h"
#include "StateMachine.h"

class CMonster : public CGameObject, public IEntity
{
public:
	float m_fShakeDistance;
	std::unique_ptr<CStateMachine<CMonster>> m_pStateMachine;

	CGameObject* pWeapon;
	BoundingBox m_BodyBoundingBox;
	BoundingBox m_WeaponBoundingBox;
	BoundingBox m_TransformedBodyBoudningBox;
	BoundingBox m_TransformedWeaponBoudningBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
public:
	CMonster();
	virtual ~CMonster();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class COrcObject : public CMonster
{
public:
	COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~COrcObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
	virtual void CheckCollision(CGameObject* pTargetObject) {
		if (pTargetObject)
		{
			BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
			if (m_TransformedWeaponBoudningBox.Intersects(TargetBoundingBox))
				pTargetObject->SetHit();
		}
	}

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGoblinObject : public CMonster
{
public:
	CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CGoblinObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
	virtual void CheckCollision(CGameObject* pTargetObject) {
		if (pTargetObject)
		{
			BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
			if (m_TransformedWeaponBoudningBox.Intersects(TargetBoundingBox))
				pTargetObject->SetHit();
		}
	}

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkeletonObject : public CMonster
{
public:
	CSkeletonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CSkeletonObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
	virtual void CheckCollision(CGameObject* pTargetObject) {
		if (pTargetObject)
		{
			BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
			if (m_TransformedWeaponBoudningBox.Intersects(TargetBoundingBox))
				pTargetObject->SetHit();
		}
	}

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};
