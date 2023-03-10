#pragma once
#include "PhysicsObject.h"
#include "StateMachine.h"

class CCamera;
class CPlayer : public CPhysicsObject, public IEntity
{
public:
	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

	int m_nAnimationNum = 0;
	int m_iAttack_Limit;
	bool m_bAttacked = false;
	bool m_bAttack = false;

	LPVOID m_pPlayerUpdatedContext = NULL;

	XMFLOAT3 m_xmf3CameraMoveDirection = XMFLOAT3(1.0f, 0.0f, 0.0f);
	float m_fCMDConstant = 1.0f;
public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CPlayer();

	void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera);
	virtual void Update(float fTimeElapsed);
	virtual bool CheckCollision(CGameObject* pTargetObject);

	virtual void SetScale(float x, float y, float z);
	void Tmp();

	virtual void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera);
};
