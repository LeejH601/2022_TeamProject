#pragma once
#include "Object.h"

class CPhysicsObject : public CGameObject
{
private:
protected:
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float fDistortionDegree = 0.0f;

	float m_fPitch;
	float m_fRoll;
	float m_fYaw;
	float m_fSpeedKperH;
	float m_fSpeedUperS;
	float m_fCurrentSpeedUperS;
	float m_fAccelerationUperS;
	LPVOID m_pUpdatedContext = NULL;
	bool m_bOnGround = false; // ���� ������ ����



public:
	float m_fTotalHP;
	float m_fHP;

public:
	virtual void UpdateMatrix();
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void OnUpdateCallback(float fTimeElapsed);
	void SetUpdatedContext(LPVOID pContext) { m_pUpdatedContext = pContext; }

	virtual void Update(float fTimeElapsed);
	virtual void Apply_Gravity(float fTimeElapsed);
	virtual void Apply_Friction(float fTimeElapsed);

	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	virtual void SetScale(float x, float y, float z);
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 xmf3Position);
	void SetLookAt(XMFLOAT3& xmf3LookAt);
	void SetSpeedUperS(float speed) { m_fSpeedUperS = speed; };
	void SetCurrSpeed(float speed) { m_fCurrentSpeedUperS = speed; };
	void Acceleration(float fTimeElapsed);
	void Deceleration(float fTimeElapsed);

	virtual XMFLOAT3 GetPosition();
	virtual XMFLOAT3 GetLook();
	virtual XMFLOAT3 GetUp();
	virtual XMFLOAT3 GetRight();
	virtual XMFLOAT3 GetScale() { return m_xmf3Scale; };
	virtual XMFLOAT3 GetVelocity();
	float GetCurrSpeed() { return m_fCurrentSpeedUperS; };

	bool GetOnGround();

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	float& GetSpeedKperH() { return m_fSpeedKperH; }
	float& GetSpeedUperS() { return m_fSpeedUperS; }

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

	virtual void DistortLookVec(CGameObject* pObject);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity);

	virtual void updateArticulationMatrix();
	physx::PxTransform MakeTransform(XMFLOAT4X4& xmf44);
	void SetJoint(physx::PxArticulationJointReducedCoordinate* joint, JointAxisDesc& JointDesc);
	physx::PxArticulationLink* SetLink(physx::PxArticulationReducedCoordinate* articulation, physx::PxArticulationLink* p_link, physx::PxTransform& parent, physx::PxTransform& child, float meshScale = 1.0f);
	virtual void CreateArticulation(float meshScale = 1.0f);
};
