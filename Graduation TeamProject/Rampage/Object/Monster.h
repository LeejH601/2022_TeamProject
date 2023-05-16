#pragma once
#pragma once
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "MonsterState.h"

class CMonster : public CPhysicsObject
{
public:
	XMFLOAT3 m_xmf3CalPos;

	XMFLOAT3 m_xmf3HitterVec;
	XMFLOAT3 m_xmf3WanderVec;
	XMFLOAT3 m_xmf3ChasingVec;

	float m_fHP;
	unsigned int m_iPlayerAtkId = 999;

	bool m_bIsDummy = false;

	bool m_bStunned;
	bool m_bCanChase;
	
	float m_fIdleTime;
	float m_fSpawnTime;
	float m_fWanderTime;
	float m_fDeadTime;
	float m_fToPlayerLength;

	float m_fAttackRange;
	float m_fSensingRange;
	
	float m_fStunTime;
	float m_fStunStartTime;
	float m_fShakeDistance;
	float m_fDamageDistance;
	float m_fTotalDamageDistance;
	float TestDissolvetime = 0.0f;

	MONSTER_TYPE m_MonsterType;
	std::unique_ptr<CStateMachine<CMonster>> m_pStateMachine;

	CGameObject* pWeapon;
	BoundingOrientedBox m_BodyBoundingBox;
	BoundingOrientedBox m_WeaponBoundingBox;
	BoundingOrientedBox m_TransformedBodyBoundingBox;
	BoundingOrientedBox m_TransformedWeaponBoundingBox;

	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
public:
	CMonster();
	virtual ~CMonster();

	MONSTER_TYPE GetMonsterType() { return m_MonsterType; }

	unsigned int GetPlayerAtkId() { return m_iPlayerAtkId; }
	XMFLOAT3 GetHitterVec() { return m_xmf3HitterVec; }
	XMFLOAT3 GetWanderVec() { return m_xmf3WanderVec; }
	XMFLOAT3 GetChasingVec() { return m_xmf3ChasingVec; }

	void SetWanderVec();
	void CheckIsPlayerInFrontOfThis(XMFLOAT3 xmf3PlayerPosition);
	void CalculateResultPosition();

	virtual void UpdateMatrix();
	virtual void SetScale(float x, float y, float z);
	virtual void Update(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void SetHit(CGameObject* pHitter);
	virtual bool CheckCollision(CGameObject* pTargetObject) {
		if (pTargetObject)
		{
			BoundingOrientedBox TargetBoundingBox = pTargetObject->GetBoundingBox();
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

	virtual BoundingOrientedBox GetBoundingBox() { return m_TransformedBodyBoundingBox; }
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGoblinObject : public CMonster
{
public:
	CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CGoblinObject();

	virtual BoundingOrientedBox GetBoundingBox() { return m_TransformedBodyBoundingBox; }
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkeletonObject : public CMonster
{
public:
	CSkeletonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CSkeletonObject();

	virtual BoundingOrientedBox GetBoundingBox() { return m_TransformedBodyBoundingBox; }
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

class CMonsterRootAnimationController : public CAnimationController
{
public:
	CMonsterRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CMonsterRootAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed);
};

class CMonsterPool
{
	DECLARE_SINGLE(CMonsterPool);

public:
	void SpawnMonster(int MonsterN, XMFLOAT3* xmfPositions);

	bool SetNonActiveMonster(CMonster* pMonster);
	bool SetActiveMonster(XMFLOAT3 xmfPosition);

private:
	std::vector<CGameObject*> m_pNonActiveMonsters; // 사용 가능한 몬스터
};