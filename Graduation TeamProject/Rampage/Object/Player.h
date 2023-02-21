#pragma once
#include "Object.h"
#include "StateMachine.h"

class CPlayer : public CGameObject, public IEntity
{
public:
	std::unique_ptr<CStateMachine<CPlayer>> m_pStateMachine;

	int m_iAttack_Limit;
	bool m_bAttacked = false;

	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float m_fPitch;
	float m_fRoll;
	float m_fYaw;

	XMFLOAT3 m_xmf3CameraMoveDirection = XMFLOAT3(1.0f, 0.0f, 0.0f);
	float m_fCMDConstant = 1.0f;
public:
	CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks);
	virtual ~CPlayer();

	void OnPrepareRender();
	void Update(float fTimeElapsed);
	void ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

	virtual bool CheckCollision(CGameObject* pTargetObject);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 xmf3Position);

	virtual XMFLOAT3 GetPosition();
	virtual XMFLOAT3 GetLook();
	virtual XMFLOAT3 GetUp();
	virtual XMFLOAT3 GetRight();

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetLookAt(XMFLOAT3& xmf3LookAt);

	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera);
};
