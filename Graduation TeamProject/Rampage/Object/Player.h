#pragma once
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "..\Global\Camera.h"	// 왜 필요하지

class CCamera;
class CPlayer : public CPhysicsObject
{
public:
	CCamera* m_pCamera = nullptr;

	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

	int m_nAnimationNum = 0;
	int m_iAttack_Limit;
	bool m_bAttacked = false;
	bool m_bAttack = false;
	bool m_bAttack2 = false;

	LPVOID m_pPlayerUpdatedContext = NULL;

	XMFLOAT3 m_xmf3TargetPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_fCMDConstant = 1.0f;

	XMFLOAT3 m_xmfDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CPlayer();

	XMFLOAT3 GetATKDirection();
	XMFLOAT3 GetTargetPosition();

	void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera);
	virtual void Update(float fTimeElapsed);
	virtual bool CheckCollision(CGameObject* pTargetObject);

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	virtual void SetScale(float x, float y, float z);
	void Tmp();

	virtual void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera);
};
