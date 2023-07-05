#pragma once
#pragma once
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "MonsterState.h"

class CMonster : public CPhysicsObject
{
public:
	CGameObject* m_pChasingTargetObject;

	XMFLOAT3 m_xmf3CalPos;

	XMFLOAT3 m_xmf3HitterVec;
	XMFLOAT3 m_xmf3WanderVec;

	unsigned int m_iPlayerAtkId = 999;

	bool m_bIsDummy = false;

	bool m_bStunned;
	bool m_bCanChase;
	
	float m_fMaxIdleTime;
	float m_fIdleTime;

	float m_fSpawnTime;

	float m_fMaxWanderTime;
	float m_fWanderTime;

	float m_fDeadTime;
	float m_fToPlayerLength;

	float m_fAttackRange;
	float m_fAtkStartTime;
	float m_fAtkEndTime;
	float m_fSensingRange;
	
	float m_fStunTime;
	float m_fMaxStunTime;
	float m_fStunStartTime;
	float m_fShakeDuration;
	float m_fShakeDistance;
	float m_fShakeFrequency;
	float m_fMaxShakeDistance;
	float m_fDamageDistance;
	float m_fMaxDamageDistance;
	float m_fDamageAnimationSpeed;
	bool m_bElite = false;
	float m_fCurrShield;
	float m_fMaxShield = 100.f;
	float m_fCurrChargeShieldTime = 0.0f;
	float m_fMaxChargeShieldTime = 3.0f;

	float m_fTotalDamageDistance;
	float TestDissolvetime = 0.0f;

	bool m_bArticulationSleep = false;

	MONSTER_TYPE m_MonsterType;
	std::unique_ptr<CStateMachine<CMonster>> m_pStateMachine;

	int m_iAttackAnimationNum;
	std::string m_strAttackSoundPath;
	float m_fAttackSoundVolume;
	float m_fAttackSoundDelay;

	CGameObject* pWeapon;
	BoundingOrientedBox m_BodyBoundingBox;
	BoundingOrientedBox m_WeaponBoundingBox;
	BoundingOrientedBox m_TransformedBodyBoundingBox;
	BoundingOrientedBox m_TransformedWeaponBoundingBox;

	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;

	/*float m_fRimLightFactor;
	XMFLOAT3 m_xmf3RimLightColor;

	ComPtr<ID3D12Resource> m_pd3dcbRimLightInfo = nullptr;
	VS_CB_RIMLIGHT_INFO* m_pcbMappedRimLightlInfo = nullptr;*/
public:
	CMonster();
	virtual ~CMonster();

	MONSTER_TYPE GetMonsterType() { return m_MonsterType; }

	unsigned int GetPlayerAtkId() { return m_iPlayerAtkId; }
	XMFLOAT3 GetHitterVec() { return m_xmf3HitterVec; }
	XMFLOAT3 GetWanderVec() { return m_xmf3WanderVec; }

	void SetWanderVec();
	void CheckIsPlayerInFrontOfThis(CGameObject* pPlayer);
	void CalculateResultPosition();
	void ApplyDamage(float Damage, void* pData = nullptr);
	void SetElite(bool flag);
	void HandleDamage(CPlayer* pPlayer, float fDamage);
	void HandleAllyDamagedMessage(CGameObject* pPlayer);
	bool GetHasShield() { return m_fCurrShield > 0.0f; };
	virtual void PlayMonsterEffectSound();

	virtual void UpdateMatrix();
	virtual void SetScale(float x, float y, float z);
	virtual void Update(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual bool CheckCollision(CGameObject* pTargetObject);
	virtual void UpdateTransformFromArticulation(XMFLOAT4X4* pxmf4x4Parent, std::vector<std::string> pArtiLinkNames, std::vector<XMFLOAT4X4>& AritculatCacheMatrixs, float scale = 1.0f);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class COrcObject : public CMonster
{
public:
	COrcObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~COrcObject();

	virtual void PlayMonsterEffectSound();
	virtual BoundingOrientedBox* GetBoundingBox() { return &m_TransformedBodyBoundingBox; }
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGoblinObject : public CMonster
{
public:
	CGoblinObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CGoblinObject();

	virtual void PlayMonsterEffectSound();
	virtual BoundingOrientedBox* GetBoundingBox() { return &m_TransformedBodyBoundingBox; }
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkeletonObject : public CMonster
{
public:
	CSkeletonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CSkeletonObject();

	virtual void PlayMonsterEffectSound();
	virtual BoundingOrientedBox* GetBoundingBox() { return &m_TransformedBodyBoundingBox; }
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

class CMonsterRootAnimationController : public CAnimationController
{
public:
	CMonsterRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CMonsterRootAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed);
};

struct MonsterSpawnInfo {
	XMFLOAT3 xmf3Position;
	bool bIsElite;
};

class CMonsterPool
{
	DECLARE_SINGLE(CMonsterPool);

public:
	void SpawnMonster(MONSTER_TYPE monsterType, int MonsterN, MonsterSpawnInfo* monsterSpawnInfo);

	bool SetNonActiveMonster(MONSTER_TYPE monsterType, CMonster* pMonster);
	bool SetActiveMonster(MONSTER_TYPE monsterType, MonsterSpawnInfo monsterSpawnInfo);

private:
	std::vector<std::vector<CGameObject*>> m_pNonActiveMonsters; // 사용 가능한 몬스터
};