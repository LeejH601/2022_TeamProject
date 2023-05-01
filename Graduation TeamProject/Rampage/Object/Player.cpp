#include "Player.h"
#include "Monster.h"
#include "Object.h"
#include "..\Global\Camera.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\Timer.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Object\ParticleObject.h"
#include "..\Object\PlayerParticleObject.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\BoundingBoxShader.h"
#include <stdio.h>

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	m_xmf3Velocity = XMFLOAT3{};

	m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>(this);
	m_pStateMachine->SetCurrentState(Idle_Player::GetInst());
	m_pStateMachine->SetPreviousState(Idle_Player::GetInst());

	m_fSpeedKperH = 6.0f;
	m_fSpeedMperS = m_fSpeedKperH * 1000.0f / 3600.0f;
	m_fSpeedUperS = m_fSpeedMperS * 100.0f / 4.0f;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
}

XMFLOAT3 CPlayer::GetATKDirection()
{
	CState<CPlayer>* pPlayerState = m_pStateMachine->GetCurrentState();
	XMFLOAT3 xmf3Direction = { 0.0f, 0.0f, 0.0f };
	if (pPlayerState == Atk1_Player::GetInst())
	{
		xmf3Direction = Vector3::Add(GetUp(), GetRight());
		xmf3Direction = Vector3::ScalarProduct(xmf3Direction, -1.0f);

	}

	else if (pPlayerState == Atk2_Player::GetInst())
	{
		xmf3Direction = GetRight();
	}

	else if (pPlayerState == Atk3_Player::GetInst())
	{
		xmf3Direction = GetLook();
	}

	return XMFLOAT3(xmf3Direction);
}

XMFLOAT3 CPlayer::GetTargetPosition()
{
	return m_xmf3TargetPosition;
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity, CCamera* pCamera)
{
	if (dwDirection)
	{
		if (m_pStateMachine->GetCurrentState() == Idle_Player::GetInst())
			m_pStateMachine->ChangeState(Run_Player::GetInst());

		if (m_pStateMachine->GetCurrentState() != Run_Player::GetInst())
			return;

		XMFLOAT3 xmf3Shift = XMFLOAT3{};

		if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)));
		if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), -1.0f);
		if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)));
		if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), -1.0f);
		xmf3Shift = Vector3::Normalize(xmf3Shift);
		xmf3Shift = Vector3::ScalarProduct(xmf3Shift, fDistance, false);

		CPhysicsObject::Move(xmf3Shift, bUpdateVelocity);
	}
}

bool CPlayer::CheckCollision(CGameObject* pTargetObject)
{
	return false;
}

void CPlayer::Update(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;
	m_fCurLagTime += fTimeElapsed;

	m_pStateMachine->Update(fTimeElapsed);

	CPhysicsObject::Apply_Gravity(fTimeElapsed);
	CPhysicsObject::Move(m_xmf3Velocity, false);

	// 플레이어가 속도를 가진다면 해당 방향을 바라보게 하는 코드
	if (m_xmf3Velocity.x + m_xmf3Velocity.z)
		SetLookAt(Vector3::Add(GetPosition(), Vector3::Normalize(XMFLOAT3{ m_xmf3Velocity.x, 0.0f, m_xmf3Velocity.z })));

	m_pStateMachine->Animate(fTimeElapsed);

	// 플레이어가 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext) OnUpdateCallback(fTimeElapsed);

	CPhysicsObject::Apply_Friction(fTimeElapsed);
}

void CPlayer::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (dwDirection)
		{
			Move(dwDirection, m_fSpeedUperS * fTimeElapsed, true, pCamera);

			XMFLOAT3 xmf3Shift = XMFLOAT3{};

			if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)));
			if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), -1.0f);
			if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)));
			if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), -1.0f);
			
			m_xmfDirection = xmf3Shift;
		}
	}
}

void CPlayer::SetScale(float x, float y, float z)
{
	CPhysicsObject::SetScale(x, y, z);
	
	m_pSkinnedAnimationController->m_xmf3RootObjectScale = m_xmf3Scale;
}

void CPlayer::Tmp()
{
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, m_nAnimationNum++);
}

#define KNIGHT_ROOT_MOTION
CKnightPlayer::CKnightPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks) : CPlayer(pd3dDevice, pd3dCommandList, nAnimationTracks)
{
	CLoadedModelInfo* pKnightModel = CModelManager::GetInst()->GetModelInfo("Object/SK_FKnightB_long_sword_2.bin");;
	if (!pKnightModel) pKnightModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/SK_FKnightB_long_sword_2.bin");

	CGameObject* obj = pKnightModel->m_pModelRootObject->FindFrame("SK_FKnightB_05");
	m_pPelvisObject = pKnightModel->m_pModelRootObject->FindFrame("pelvis");
	SetChild(pKnightModel->m_pModelRootObject, true);
	//m_pSkinnedAnimationController = std::make_unique<CKightNoMoveRootAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pKnightModel);
	m_pSkinnedAnimationController = std::make_unique<CKightRootMoveAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pKnightModel);

	auto Find_Frame_Index = [](std::string& target, std::vector<std::string>& source) {
		int cnt = 0;
		for (std::string& str : source) {
			if (strcmp(str.c_str(), target.c_str()) == 0)
				break;
			cnt++;
		}
		return cnt;
	};

	PrepareBoundingBox(pd3dDevice, pd3dCommandList);
#ifdef KNIGHT_ROOT_MOTION
	m_pSkinnedAnimationController->m_pRootMotionObject = pKnightModel->m_pModelRootObject->FindFrame("SK_FKnightB");
	if (m_pSkinnedAnimationController->m_pRootMotionObject) {
		m_pSkinnedAnimationController->m_bRootMotion = true;
		m_pSkinnedAnimationController->m_xmf3RootObjectScale = XMFLOAT3(4.0f, 4.0f, 4.0f);
	}
#endif // KNIGHT_ROOT_MOTION
}
CKnightPlayer::~CKnightPlayer()
{
}
void CKnightPlayer::SetRigidDynamic()
{
	physx::PxPhysics* pPhysics = Locator.GetPxPhysics();

	physx::PxTransform transform(physx::PxVec3(0.0f, 10.0f, 0.0f));

	physx::PxMaterial* material = pPhysics->createMaterial(0.5, 0.5, 0.5);
	physx::PxShape* shape = pPhysics->createShape(physx::PxBoxGeometry(3.0f, 5.0f, 3.0f), *material);

	physx::PxRigidDynamic* actor = physx::PxCreateDynamic(*pPhysics, transform, physx::PxBoxGeometry(3.0f, 5.0f, 3.0f), *material, 1.0f);

	Rigid = actor;
}
bool CKnightPlayer::CheckCollision(CGameObject* pTargetObject)
{
	if (m_iAttackId == ((CMonster*)pTargetObject)->GetPlayerAtkId())
		return false;

	BoundingBox TargetBoundingBox = pTargetObject->GetBoundingBox();
	if (m_TransformedWeaponBoundingBox.Intersects(TargetBoundingBox)) {
		SoundPlayParams SoundPlayParam;
		SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOCK;
		CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, m_pStateMachine->GetCurrentState());

		if (m_pCamera)
		{
			if (m_pStateMachine->GetCurrentState()->GetCameraShakeComponent()->GetEnable())
				m_pCamera->m_bCameraShaking = true;
			if (m_pStateMachine->GetCurrentState()->GetCameraZoomerComponent()->GetEnable())
				m_pCamera->m_bCameraZooming = true;
			if (m_pStateMachine->GetCurrentState()->GetCameraMoveComponent()->GetEnable())
				m_pCamera->m_bCameraMoving = true;
		}

		m_xmf3TargetPosition = pTargetObject->GetPosition();
		pTargetObject->SetHit(this);

		return true;
	}
	return false;
}
void CKnightPlayer::Animate(float fTimeElapsed)
{
	CGameObject::Animate(fTimeElapsed);
	if (Rigid) {
		physx::PxRigidDynamic* actor = (physx::PxRigidDynamic*)Rigid;
		physx::PxTransform transform = actor->getGlobalPose();

		physx::PxMat44 Matrix(transform);
		Matrix = Matrix.inverseRT();
		m_pChild->m_xmf4x4Transform._11 = Matrix.column0.x; m_pChild->m_xmf4x4Transform._12 = Matrix.column0.y; m_pChild->m_xmf4x4Transform._13 = Matrix.column0.z;
		m_pChild->m_xmf4x4Transform._21 = Matrix.column1.x; m_pChild->m_xmf4x4Transform._22 = Matrix.column1.y; m_pChild->m_xmf4x4Transform._23 = Matrix.column1.z;
		m_pChild->m_xmf4x4Transform._31 = Matrix.column2.x; m_pChild->m_xmf4x4Transform._32 = Matrix.column2.y; m_pChild->m_xmf4x4Transform._33 = Matrix.column2.z;
		
		SetPosition(XMFLOAT3(transform.p.x, transform.p.y, transform.p.z));
	}
	OnUpdateCallback(fTimeElapsed);
}
void CKnightPlayer::OnUpdateCallback(float fTimeElapsed)
{
	if (m_pUpdatedContext)
	{
		CSplatTerrain* pTerrain = (CSplatTerrain*)m_pUpdatedContext;
		XMFLOAT3 xmf3TerrainPos = pTerrain->GetPosition();
		XMFLOAT3 xmf3Pos = XMFLOAT3{
			m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._41,
			m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._42,
			m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._43
		};

		float fTerrainY = pTerrain->GetHeight(xmf3Pos.x - (xmf3TerrainPos.x), xmf3Pos.z - (xmf3TerrainPos.z));

		if (GetPosition().y < fTerrainY + xmf3TerrainPos.y)
		{
			SetPosition(XMFLOAT3(GetPosition().x, fTerrainY + xmf3TerrainPos.y, GetPosition().z));
			UpdateTransform(NULL);
		}
	}
}
void CKnightPlayer::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	CPhysicsObject::UpdateTransform(NULL);

	pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);
	m_BodyBoundingBox.Transform(m_TransformedBodyBoundingBox, XMLoadFloat4x4(&m_xmf4x4Transform));

	if (pWeapon)
	{
		XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();

		XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(-15.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f));
		xmf4x4World = Matrix4x4::Multiply(mtxRotate, xmf4x4World);

		XMFLOAT3 xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };

		XMFLOAT3 controllBasePos = XMFLOAT3(0.0f, 0.2f, 0.0f);
		controllBasePos = Vector3::TransformCoord(controllBasePos, xmf4x4World);
		XMFLOAT3 offsetPosition = XMFLOAT3(0.0f, 1.2f, 0.0f);
		offsetPosition = Vector3::TransformCoord(offsetPosition, xmf4x4World);
		//offsetPosition = Vector3::Add(controllBasePos, m_TransformedWeaponBoundingBox.Extents);


		XMFLOAT3 xmf3Direction = XMFLOAT3{ xmf4x4World._31, xmf4x4World._32, xmf4x4World._33 };
		xmf3Position = Vector3::Add(xmf3Position, xmf3Direction, -0.8f);
		xmf4x4World._41 = xmf3Position.x;
		xmf4x4World._42 = xmf3Position.y;
		xmf4x4World._43 = xmf3Position.z;

		pWeaponBoundingBoxMesh->SetWorld(xmf4x4World);

		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoundingBox, XMLoadFloat4x4(&xmf4x4World));

		m_xmf4TrailControllPoints[0] = XMFLOAT4(controllBasePos.x, controllBasePos.y, controllBasePos.z, 1.0f);
		m_xmf4TrailControllPoints[1] = XMFLOAT4(offsetPosition.x, offsetPosition.y, offsetPosition.z, 1.0f);
	}
}
void CKnightPlayer::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("Weapon_r");
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 0.6f, 0.8f), XMFLOAT3(0.025f, 1.05f, 0.125f));
	m_BodyBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.7f, 2.0f, 0.7f) };
	m_WeaponBoundingBox = BoundingBox{ XMFLOAT3(0.0f, 0.4f, 0.8f), XMFLOAT3(0.025f, 0.55f, 0.125f) };
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
CKightRootRollBackAnimationController::CKightRootRollBackAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel) : CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pModel)
{
}
CKightRootRollBackAnimationController::~CKightRootRollBackAnimationController()
{
}
#define _WITH_DEBUG_ROOT_MOTION
void CKightRootRollBackAnimationController::OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed)
{
	if (m_bRootMotion)
	{
		if (m_pAnimationTracks[0].m_fPosition == 0.0f)
		{
			m_xmf3FirstRootMotionPosition = m_pRootMotionObject->GetPosition();
		}
		else if (m_pAnimationTracks[0].m_fPosition < 0.0f)
		{
			XMFLOAT3 xmf3Position = m_pRootMotionObject->GetPosition();

			//		XMFLOAT3 xmf3RootPosition = m_pModelRootObject->GetPosition();
			//		pRootGameObject->m_xmf4x4ToParent = m_pModelRootObject->m_xmf4x4World;
			//		pRootGameObject->SetPosition(xmf3RootPosition);
			//XMFLOAT3 xmf3Offset = Vector3::Subtract(xmf3Position, m_xmf3FirstRootMotionPosition);
			XMFLOAT3 xmf3Offset = Vector3::Subtract(m_xmf3FirstRootMotionPosition, xmf3Position);
			xmf3Offset = {
				xmf3Offset.x * m_xmf3RootObjectScale.x,
				xmf3Offset.y * m_xmf3RootObjectScale.y,
				xmf3Offset.z * m_xmf3RootObjectScale.z
			};

			//			xmf3Position = pRootGameObject->GetPosition();
			//			XMFLOAT3 xmf3Look = pRootGameObject->GetLook();
			//			xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fabs(xmf3Offset.x));

			pRootGameObject->m_xmf4x4Transform._41 += xmf3Offset.x;
			pRootGameObject->m_xmf4x4Transform._42 += xmf3Offset.y;
			pRootGameObject->m_xmf4x4Transform._43 += xmf3Offset.z;

			//			pRootGameObject->MoveForward(fabs(xmf3Offset.x));
			//			pRootGameObject->SetPosition(xmf3Position);
			//			m_xmf3PreviousPosition = xmf3Position;
#ifdef _WITH_DEBUG_ROOT_MOTION
			TCHAR pstrDebug[256] = { 0 };
			_stprintf_s(pstrDebug, 256, _T("Offset: (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f)\n"), xmf3Offset.x, xmf3Offset.y, xmf3Offset.z, pRootGameObject->m_xmf4x4Transform._41, pRootGameObject->m_xmf4x4Transform._42, pRootGameObject->m_xmf4x4Transform._43);
			OutputDebugString(pstrDebug);
#endif
		}
	}
}
CKightNoMoveRootAnimationController::CKightNoMoveRootAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel) : CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pModel)
{
}
CKightNoMoveRootAnimationController::~CKightNoMoveRootAnimationController()
{
}
void CKightNoMoveRootAnimationController::OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed)
{
	if (m_bRootMotion)
	{
		CPlayer* player = (CPlayer*)pRootGameObject;
		m_xmf3FirstRootMotionPosition = pRootGameObject->GetPosition();
	
		m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
		m_pRootMotionObject->m_xmf4x4Transform._42 = 0.f;
		m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
	}
}
CKightRootMoveAnimationController::CKightRootMoveAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel) : CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pModel)
{
}
CKightRootMoveAnimationController::~CKightRootMoveAnimationController()
{
}
void CKightRootMoveAnimationController::OnRootMotion(CGameObject* pRootGameObject, float fTimeElapsed)
{
	CAnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[0].m_nAnimationSet];

	if (m_bRootMotion)
	{
		CPlayer* player = (CPlayer*)pRootGameObject;
		player->m_pStateMachine->GetCurrentState()->OnRootMotion(player, fTimeElapsed);
	}
	else
	{
		CPlayer* player = (CPlayer*)pRootGameObject;
		m_xmf3FirstRootMotionPosition = pRootGameObject->GetPosition();

		m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
		m_pRootMotionObject->m_xmf4x4Transform._42 = 0.f;
		m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
	}
}

