#pragma once
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "..\Global\Camera.h"	// 왜 필요하지
#include "SwordTrailObject.h"
#include "VertexPointParticleObject.h"
#include "UIObject.h"

class CCamera;
class CMonster;
class CPlayer : public CPhysicsObject
{
public:
	CCamera* m_pCamera = nullptr;
	CGameObject* m_pChildObject = nullptr;
	CGameObject* m_pPelvisObject = nullptr;

	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

	int m_nAnimationNum = 0;

	unsigned int m_iAttackId = 0;
	bool m_bAttack = false;
	bool m_bEvasioned = false;
	bool m_bCharged = false;

	LPVOID m_pPlayerUpdatedContext = NULL;

	XMFLOAT3 m_xmf3RootTransfromPreviousPos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3PreviousPos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3TargetPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_fCMDConstant = 1.0f;

	XMFLOAT3 m_xmfDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float m_fTime = 0.f;
	float m_fMaxTime = 0.f;

	std::unique_ptr<CGameObject>* m_pSwordTrailReference = nullptr;

	DWORD m_dwDirectionCache;
	XMFLOAT3 m_xmf3DirectionCache;

	XMFLOAT3 m_xmf3ToHitterVec;

	float m_fStamina;
	float m_fTotalStamina;
	UINT m_iCombo = 0;
	float m_fComboFullTime = 10.f;
	float m_fComboTime = m_fComboFullTime;
	bool m_bCombo = false;

	UINT m_iPotionN = 5;

	bool m_bIsDash = false;
	bool m_bIsDrinkPotion = false;
	
	XMFLOAT3 m_xmf3AtkDirection;

	float m_fInvincibleTime = 0.0f;
	std::vector<std::shared_ptr<CAnimationCallbackHandler>> m_pAnimationcHandlers;


public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CPlayer();

	int GetAtkId() { return m_iAttackId; }
	XMFLOAT3 GetATKDirection();
	XMFLOAT3 GetTargetPosition();

	void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera);
	virtual void Update(float fTimeElapsed);
	virtual bool CheckCollision(CGameObject* pTargetObject);

	CCamera* GetCamera() { return m_pCamera; }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	virtual void SetScale(float x, float y, float z);
	void Tmp();

	void UpdateStamina(float fTimeElapsed);
	void UpdateCombo(float fTimeElapsed);
	void HandleDamage(CMonster* pMonster, float fDamage);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	virtual void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera);
	virtual XMFLOAT4& GetTrailControllPoint(int n) { return XMFLOAT4{}; }
};

class CKnightPlayer : public CPlayer
{
private:
	CGameObject* pWeapon;
	BoundingOrientedBox m_BodyBoundingBox;
	BoundingOrientedBox m_WeaponBoundingBox;
	BoundingOrientedBox m_TransformedBodyBoundingBox;
	BoundingOrientedBox m_TransformedWeaponBoundingBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
	XMFLOAT4 m_xmf4TrailControllPoints[2];
	XMFLOAT3 m_xmf4PrevTrailVertexPoint;
	bool	m_bMonsterAttack = false;
public:
	int m_nRemainPotions;

public:
	CKnightPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CKnightPlayer();

	virtual void SetRigidDynamic();


	virtual BoundingOrientedBox* GetBoundingBox() { return &m_TransformedBodyBoundingBox; }
	virtual BoundingOrientedBox GetWeaponMeshBoundingBox() { return m_TransformedWeaponBoundingBox; }

	void SetTargetPosition(const BoundingOrientedBox& targetBoundingBox);
	virtual bool CheckCollision(CGameObject* pTargetObject);

	virtual void Animate(float fTimeElapsed);
	virtual void OnUpdateCallback(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void DrinkPotion();
	void CanclePotion();

	virtual XMFLOAT4& GetTrailControllPoint(int n) { return m_xmf4TrailControllPoints[n]; };
	XMFLOAT3 GetTrailPrevControllPoint() { return m_xmf4PrevTrailVertexPoint; };
	bool& GetMonsterAttack() {return m_bMonsterAttack;}
	void SetMonsterAttack(bool bMonsterAttack) { m_bMonsterAttack = bMonsterAttack; };
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CKightRootRollBackAnimationController : public CAnimationController
{
public:
	CKightRootRollBackAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CKightRootRollBackAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CKightNoMoveRootAnimationController : public CAnimationController
{
public:
	CKightNoMoveRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CKightNoMoveRootAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CKightRootMoveAnimationController : public CAnimationController
{
public:
	CKightRootMoveAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~CKightRootMoveAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CDrinkPotionCallbackHandler : public CAnimationCallbackHandler 
{
public:
	CVertexPointParticleObject* m_pVertexPointParticleObject = nullptr;
	bool m_bEmitedParticle = false;
	CKnightPlayer* m_pPlayer;
	CUIObject* m_pNumUIObject = nullptr;
public:
	virtual void HandleCallback(void* pCallbackData, float fTrackPosition);
};