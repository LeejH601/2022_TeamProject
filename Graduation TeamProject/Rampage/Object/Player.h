#pragma once
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "..\Global\Camera.h"	// 왜 필요하지
#include "SwordTrailObject.h"

class CCamera;
class CPlayer : public CPhysicsObject
{
public:
	CCamera* m_pCamera = nullptr;
	CGameObject* m_pChildObject = nullptr;
	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

	int m_nAnimationNum = 0;
	const float m_fAnimationPlayerWeightMin = 0.1f;
	float m_fAnimationPlayWeight = 1.0f;

	unsigned int m_iAttackId = 0;
	bool m_bAttack = false;
	bool m_bAttacked = false;

	LPVOID m_pPlayerUpdatedContext = NULL;

	XMFLOAT3 m_xmf3TargetPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_fCMDConstant = 1.0f;

	XMFLOAT3 m_xmfDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float m_fTime = 0.f;
	float m_fMaxTime = 0.f;
	float m_fCurLagTime = 0.f;

	std::unique_ptr<CGameObject>* m_pSwordTrailReference = nullptr;

public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CPlayer();

	int GetAtkId() { return m_iAttackId; }
	const float& GetAnimationPlayWeightMin() { return m_fAnimationPlayerWeightMin; }
	float GetAnimationPlayWeight() { return m_fAnimationPlayWeight; }
	XMFLOAT3 GetATKDirection();
	XMFLOAT3 GetTargetPosition();

	void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera);
	virtual void Update(float fTimeElapsed);
	virtual bool CheckCollision(CGameObject* pTargetObject);

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	virtual void SetScale(float x, float y, float z);
	void Tmp();

	virtual void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera);
	virtual XMFLOAT4& GetTrailControllPoint(int n) { return XMFLOAT4{}; }
};

class CKnightPlayer : public CPlayer
{
private:
	CGameObject* pWeapon;
	BoundingBox m_BodyBoundingBox;
	BoundingBox m_WeaponBoundingBox;
	BoundingBox m_TransformedBodyBoundingBox;
	BoundingBox m_TransformedWeaponBoundingBox;
	CGameObject* pBodyBoundingBoxMesh;
	CGameObject* pWeaponBoundingBoxMesh;
	XMFLOAT4 m_xmf4TrailControllPoints[2];

public:
	CKnightPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CKnightPlayer();

	virtual void SetRigidDynamic();


	virtual BoundingBox GetBoundingBox() { return m_TransformedBodyBoundingBox; }
	virtual BoundingBox GetWeaponMeshBoundingBox() { return m_TransformedWeaponBoundingBox; }

	virtual bool CheckCollision(CGameObject* pTargetObject);

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual XMFLOAT4& GetTrailControllPoint(int n) { return m_xmf4TrailControllPoints[n]; };
};