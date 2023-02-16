#pragma once
#include "Object.h"
#include "StateMachine.h"

class CPlayer : public CGameObject, public IEntity
{
public:
	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

	int m_iAttack_Limit;
	bool m_bAttacked = false;

	XMFLOAT3 m_xmf3CameraMoveDirection = XMFLOAT3(1.0f, 0.0f, 0.0f);
	float m_fCMDConstant = 1.0f;

public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CPlayer();

	void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

	virtual bool CheckCollision(CGameObject* pTargetObject);
};
