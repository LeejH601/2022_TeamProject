#pragma once
#pragma once
#include "PhysicsObject.h"
#include "StateMachine.h"

class CMonster : public CPhysicsObject
{
public:
	XMFLOAT3 m_xmf3HitterVec;

	bool m_bStunned;
	float m_fStunTime;
	float m_fStunStartTime;
	float m_fShakeDistance;
	float m_fDamageDistance;
	float m_fTotalDamageDistance;
	float TestDissolvetime = 0.0f;

	std::unique_ptr<CStateMachine<CMonster>> m_pStateMachine;

	CGameObject* pWeapon;
	BoundingBox m_BodyBoundingBox;
	BoundingBox m_WeaponBoundingBox;
	BoundingBox m_TransformedBodyBoudningBox;
	BoundingBox m_TransformedWeaponBoundingBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
public:
	CMonster();
	virtual ~CMonster();

	virtual void SetScale(float x, float y, float z);
	virtual void Animate(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void SetHit(CGameObject* pHitter)
	{
		bHit = true;
		m_xmf3HitterVec = Vector3::Normalize(Vector3::Subtract(GetPosition(), pHitter->GetPosition()));
		m_xmf3HitterVec.y = 0.0f;

		SetLookAt(Vector3::Add(GetPosition(), XMFLOAT3(-m_xmf3HitterVec.x, 0.0f, -m_xmf3HitterVec.z)));

		SoundPlayParams SoundPlayParam{ SOUND_PLAY_TYPE::SOUND_PT_NONE };
		CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, this);
	}
	virtual bool CheckCollision(CGameObject* pTargetObject) {
		if (pTargetObject)
		{
			BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
			if (m_TransformedWeaponBoundingBox.Intersects(TargetBoundingBox)) {
				pTargetObject->SetHit(this);
				return true;
			}
		}
		return false;
	}

	virtual void UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> pArtiLinkNames, std::vector<XMFLOAT4X4>& AritculatCacheMatrixs, float scale = 1.0f);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class COrcObject : public CMonster
{
public:
	COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~COrcObject();

	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoudningBox; }
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
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

class CMonsterRootAnimationController : public CAnimationController
{
public:
	CMonsterRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CMonsterRootAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject);
};