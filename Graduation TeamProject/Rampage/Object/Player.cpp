#include "Player.h"
#include "Monster.h"
#include "Object.h"
#include "Map.h"
#include "..\Global\Camera.h"
#include "..\Global\Locator.h"
#include "..\Global\Global.h"
#include "..\Global\Timer.h"
#include "..\Global\MessageDispatcher.h"
#include "..\Object\ParticleObject.h"
#include "..\Object\PlayerParticleObject.h"
#include "..\Object\ModelManager.h"
#include "..\Shader\BoundingBoxShader.h"
#include "..\Global\Logger.h"
#include <stdio.h>

CPlayer::CPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks)
{
	m_fHP = 100.f;
	m_fTotalHP = 100.f;
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	m_xmf3Velocity = XMFLOAT3{};

	m_pStateMachine = std::make_unique<CStateMachine<CPlayer>>(this);
	m_pStateMachine->SetCurrentState(Idle_Player::GetInst());
	m_pStateMachine->SetPreviousState(Idle_Player::GetInst());

	m_fSpeedKperH = 44.0f / 2.0f;
	m_fSpeedUperS = MeterToUnit(m_fSpeedKperH * 1000.0f) / 3600.0f;
	m_fAccelerationUperS = m_fSpeedUperS * 3.0f;

	m_fTotalStamina = 100.f;
	m_fStamina = 100.f;
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
		xmf3Direction = GetRight();
	}

	return XMFLOAT3(xmf3Direction);
}

XMFLOAT3 CPlayer::GetTargetPosition()
{
	return m_xmf3TargetPosition;
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	CPhysicsObject::Move(xmf3Shift, bUpdateVelocity);
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

	// 플레이어가 속도를 가진다면 해당 방향을 바라보게 하는 코드
	if (m_xmf3Velocity.x + m_xmf3Velocity.z)
		SetLookAt(Vector3::Add(GetPosition(), Vector3::Normalize(XMFLOAT3{ m_xmf3Velocity.x, 0.0f, m_xmf3Velocity.z })));

	m_pStateMachine->Update(fTimeElapsed);
	m_pStateMachine->Animate(fTimeElapsed);

	CPhysicsObject::Apply_Gravity(fTimeElapsed);

	XMFLOAT3 xmf3NewVelocity = Vector3::TransformCoord(m_xmf3Velocity, XMMatrixRotationAxis(XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f }, XMConvertToRadians(fDistortionDegree)));
	
	CPhysicsObject::Move(xmf3NewVelocity, false);

	// 플레이어가 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext) OnUpdateCallback(fTimeElapsed);

	CPhysicsObject::Apply_Friction(fTimeElapsed);

	m_xmf3PreviousPos = GetPosition();

	UpdateStamina(fTimeElapsed);
	//UpdateCombo(fTimeElapsed);
}

void CPlayer::UpdateStamina(float fTimeElapsed)
{
	if ((m_pStateMachine->GetCurrentState() == Idle_Player::GetInst()) || (m_pStateMachine->GetCurrentState() == Run_Player::GetInst()))
		m_fStamina += 2.f * m_fSpeedUperS * fTimeElapsed * ((m_fTotalStamina - m_fStamina) / m_fTotalStamina);
}

void CPlayer::ProcessInput(DWORD dwDirection, float cxDelta, float cyDelta, float fTimeElapsed, CCamera* pCamera)
{
	static UCHAR pKeysBuffer[256];

	GetKeyboardState(pKeysBuffer);

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (dwDirection)
		{
			Move(dwDirection, m_fCurrentSpeedUperS * fTimeElapsed + 0.00001f, true, pCamera);

			XMFLOAT3 xmf3Shift = XMFLOAT3{};

			if (dwDirection & DIR_FORWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)));
			if (dwDirection & DIR_BACKWARD)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetLookVector().x, 0.0f, pCamera->GetLookVector().z)), -1.0f);
			if (dwDirection & DIR_RIGHT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)));
			if (dwDirection & DIR_LEFT)xmf3Shift = Vector3::Add(xmf3Shift, Vector3::Normalize(XMFLOAT3(pCamera->GetRightVector().x, 0.0f, pCamera->GetRightVector().z)), -1.0f);
			
			m_xmfDirection = xmf3Shift;
			m_dwDirectionCache = dwDirection;
			m_xmf3DirectionCache = xmf3Shift;
		}
	}
	if(dwDirection == 0) {
		//m_dwDirectionCache = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("speed : %f\n"), m_fCurrentSpeedUperS);
		OutputDebugString(pstrDebug);

		Deceleration(fTimeElapsed);

		m_xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, m_fCurrentSpeedUperS * fTimeElapsed);

		if (m_fCurrentSpeedUperS <= 0.0f) {
			m_dwDirectionCache = 0;
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
// ui 구하기, 
void CPlayer::UpdateCombo(float fTimeElapsed)
{
	m_iCombo++;
	//m_fComboTime -= fTimeElapsed;
	//if (m_fComboTime < 0.f)
	//{
	//	
	//	m_fComboTime = m_fComboFullTime;
	//	
	//	m_iCombo = 0;
	//}
	//else if (m_bCombo)
	//{
	//	m_fComboTime = m_fComboFullTime;
	//	m_iCombo++;
	//	m_bCombo = false;
	//}
}

#define KNIGHT_ROOT_MOTION
CKnightPlayer::CKnightPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks) : CPlayer(pd3dDevice, pd3dCommandList, nAnimationTracks)
{
	CLoadedModelInfo* pKnightModel = CModelManager::GetInst()->GetModelInfo("Object/SK_FKnightB_Dragonblade.bin");;
	if (!pKnightModel) pKnightModel = CModelManager::GetInst()->LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, "Object/SK_FKnightB_Dragonblade.bin");

	CGameObject* obj = pKnightModel->m_pModelRootObject->FindFrame("SK_FKnightB_05");
	m_pPelvisObject = pKnightModel->m_pModelRootObject->FindFrame("pelvis");
	SetChild(pKnightModel->m_pModelRootObject, true);
	//m_pSkinnedAnimationController = std::make_unique<CKightNoMoveRootAnimationController>(pd3dDevice, pd3dCommandList, nAnimationTracks, pKnightModel);
	m_pSkinnedAnimationController = std::make_unique<CKightRootMoveAnimationController>(pd3dDevice, pd3dCommandList, 2, pKnightModel); // 애니메이션 트랙 갯수 고정
	m_pSkinnedAnimationController->SetTrackWeight(0,1.f);
	m_pSkinnedAnimationController->SetTrackWeight(1, 0.f);
	CGameObject* weaponFrame = FindFrame("Weapon_r");
	m_pSkinnedAnimationController->SetSocket(0, std::string("Weapon_r"), weaponFrame);
	weaponFrame = FindFrame("Weapon_l");
	m_pSkinnedAnimationController->SetSocket(0, std::string("Weapon_l"), weaponFrame);
	weaponFrame = FindFrame("head");
	m_pSkinnedAnimationController->SetSocket(0, std::string("head"), weaponFrame);

	int nSubAnimationTrack = 1;
	m_pSkinnedAnimationController->m_nSubAnimationTracks = nSubAnimationTrack;
	m_pSkinnedAnimationController->m_pSubAnimationTracks.resize(nSubAnimationTrack);
	m_pSkinnedAnimationController->m_nLayerBlendBaseBoneIndex.resize(nSubAnimationTrack);
	m_pSkinnedAnimationController->m_nLayerBlendRange.resize(nSubAnimationTrack);
	m_pSkinnedAnimationController->m_fLayerBlendWeights.resize(nSubAnimationTrack);

	m_pSkinnedAnimationController->m_pSubAnimationTracks[0].m_bEnable = FALSE;
	m_pSkinnedAnimationController->m_pSubAnimationTracks[0].SetAnimationSet(36);
	m_pSkinnedAnimationController->m_pSubAnimationTracks[0].m_fPosition = 0.0f;
	m_pSkinnedAnimationController->m_pSubAnimationTracks[0].m_fWeight = 0.0f;
	m_pSkinnedAnimationController->m_pSubAnimationTracks[0].m_nType = ANIMATION_TYPE_ONCE;
	m_pSkinnedAnimationController->m_nLayerBlendBaseBoneIndex[0] = 10;
	m_pSkinnedAnimationController->m_nLayerBlendRange[0] = 79;
	m_pSkinnedAnimationController->m_fLayerBlendWeights[0] = 1.0f;

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
void CKnightPlayer::SetTargetPosition(const BoundingOrientedBox& targetBoundingBox)
{
	// 두 바운딩 박스의 중심점을 구함
	XMFLOAT3 playerWeaponBoxCenter = m_TransformedWeaponBoundingBox.Center;
	XMFLOAT3 monsterBodyBoxCenter = targetBoundingBox.Center;

	// 플레이어 무기의 바운딩 박스의 중심에서 몬스터 몸체의 바운딩 박스의 중심을 향한 선분
	XMVECTOR direction = XMLoadFloat3(&monsterBodyBoxCenter) - XMLoadFloat3(&playerWeaponBoxCenter);
	direction = XMVector3Normalize(direction);

	// 몬스터 몸체의 바운딩 박스의 면과 교차하는 점을 구함
	float distance;

	if (targetBoundingBox.Intersects(XMLoadFloat3(&playerWeaponBoxCenter), direction, distance))
	{
		XMVECTOR intersectionPoint = XMLoadFloat3(&playerWeaponBoxCenter) + distance * direction;
		XMStoreFloat3(&m_xmf3TargetPosition, intersectionPoint);
	}
}
bool CKnightPlayer::CheckCollision(CGameObject* pTargetObject)
{
	if (m_iAttackId == ((CMonster*)pTargetObject)->GetPlayerAtkId() || ((CMonster*)pTargetObject)->m_fHP <= 0.0f)
		return false;

	BoundingOrientedBox* TargetBoundingBox = pTargetObject->GetBoundingBox();
	if (pTargetObject->m_bEnable && ((CMonster*)pTargetObject)->m_fHP > 0 && m_TransformedWeaponBoundingBox.Intersects(*TargetBoundingBox)) {
		
		SetTargetPosition(*TargetBoundingBox);
		XMFLOAT3 xmf3NowTrailVertexPoint = XMFLOAT3(m_xmf4TrailControllPoints[0].x, m_xmf4TrailControllPoints[0].y, m_xmf4TrailControllPoints[0].z);
		m_xmf3AtkDirection = Vector3::Normalize(Vector3::Subtract(xmf3NowTrailVertexPoint, m_xmf4PrevTrailVertexPoint));

		CollideParams collide_params;
		collide_params.xmf3CollidePosition = m_xmf3TargetPosition;
		CMessageDispatcher::GetInst()->Dispatch_Message<CollideParams>(MessageType::COLLISION, &collide_params, nullptr);

		SoundPlayParams SoundPlayParam;
		SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOCK;
		CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, m_pStateMachine->GetCurrentState());
		
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("CheckCollision\n"));
		OutputDebugString(pstrDebug);

		m_bCombo = true;
		m_fHP -= 30.f;
		if (m_pCamera)
		{
			if (m_pStateMachine->GetCurrentState()->GetCameraShakeComponent()->GetEnable())
				m_pCamera->m_bCameraShaking = true;
			if (m_pStateMachine->GetCurrentState()->GetCameraZoomerComponent()->GetEnable())
				m_pCamera->m_bCameraZooming = true;
			if (m_pStateMachine->GetCurrentState()->GetCameraMoveComponent()->GetEnable())
				m_pCamera->m_bCameraMoving = true;
		}

		// Damage 메세지를 보냄

		DamageParams damageParam;
		damageParam.fDamage = 30.0f;
		damageParam.fMaxStunTime = 0.0f;
		damageParam.pPlayer = this;

		CMessageDispatcher::GetInst()->Dispatch_Message<DamageParams>(MessageType::APPLY_DAMAGE, &damageParam, pTargetObject);

		std::string logMessage = "Atk player->monster"; logMessage = logMessage + " ID == { " + std::to_string(m_iAttackId) + " }";
		CLogger::GetInst()->LogCollision(this, pTargetObject, logMessage);

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
}

void CKnightPlayer::OnUpdateCallback(float fTimeElapsed)
{
	if (m_pUpdatedContext)
	{
		CMap* pMap = (CMap*)m_pUpdatedContext;
		CSplatTerrain* pTerrain = (CSplatTerrain*)(pMap->GetTerrain().get());
		XMFLOAT3 xmf3TerrainPos = pTerrain->GetPosition();
		XMFLOAT3 xmf3Pos = XMFLOAT3{
			m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._41,
			m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._42,
			m_pSkinnedAnimationController->m_pRootMotionObject->GetWorld()._43
		};

		XMFLOAT3 xmf3ResultPlayerPos = GetPosition();

		xmf3ResultPlayerPos.x = std::clamp(xmf3ResultPlayerPos.x, xmf3TerrainPos.x + TERRAIN_SPAN, xmf3TerrainPos.x + pTerrain->GetWidth() - TERRAIN_SPAN);
		xmf3ResultPlayerPos.z = std::clamp(xmf3ResultPlayerPos.z, xmf3TerrainPos.z + TERRAIN_SPAN, xmf3TerrainPos.z + pTerrain->GetLength() - TERRAIN_SPAN);

		float fTerrainY = pTerrain->GetHeight(xmf3Pos.x - (xmf3TerrainPos.x), xmf3Pos.z - (xmf3TerrainPos.z));

		if (xmf3ResultPlayerPos.y < fTerrainY + xmf3TerrainPos.y)
			xmf3ResultPlayerPos.y = fTerrainY + xmf3TerrainPos.y;

		fDistortionDegree = 0.0f;
		
		for (int i = 0; i < pMap->GetMapObjects().size(); ++i) {
			if (pMap->GetMapObjects()[i]->CheckCollision(this))
			{
				DistortLookVec(pMap->GetMapObjects()[i].get());
				break;
			}
		}
		
		SetPosition(xmf3ResultPlayerPos);

		UpdateTransform(NULL);
	}
}
void CKnightPlayer::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	CPhysicsObject::UpdateTransform(NULL);

	if (pBodyBoundingBoxMesh)
		pBodyBoundingBoxMesh->SetWorld(m_xmf4x4Transform);

	m_BodyBoundingBox.Transform(m_TransformedBodyBoundingBox, XMLoadFloat4x4(&m_xmf4x4Transform));

	if (pWeapon)
	{
		XMFLOAT4X4 xmf4x4World = pWeapon->GetWorld();

		/*XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(-15.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f));
		xmf4x4World = Matrix4x4::Multiply(mtxRotate, xmf4x4World);*/

		//XMFLOAT3 xmf3Position = XMFLOAT3{ xmf4x4World._41, xmf4x4World._42, xmf4x4World._43 };

		XMFLOAT3 controllBasePos = XMFLOAT3(0.0f, 0.2f, 0.0f);
		controllBasePos = Vector3::TransformCoord(controllBasePos, xmf4x4World);
		XMFLOAT3 offsetPosition = XMFLOAT3(0.0f, 2.0f, 0.0f);
		offsetPosition = Vector3::TransformCoord(offsetPosition, xmf4x4World);
		//offsetPosition = Vector3::Add(controllBasePos, m_TransformedWeaponBoundingBox.Extents);


	/*	XMFLOAT3 xmf3Direction = XMFLOAT3{ xmf4x4World._31, xmf4x4World._32, xmf4x4World._33 };
		xmf3Position = Vector3::Add(xmf3Position, xmf3Direction, -0.4f);
		xmf4x4World._41 = xmf3Position.x;
		xmf4x4World._42 = xmf3Position.y;
		xmf4x4World._43 = xmf3Position.z;*/

		if (pWeaponBoundingBoxMesh)
			pWeaponBoundingBoxMesh->SetWorld(xmf4x4World);

		m_WeaponBoundingBox.Transform(m_TransformedWeaponBoundingBox, XMLoadFloat4x4(&xmf4x4World));

		m_xmf4PrevTrailVertexPoint = XMFLOAT3( m_xmf4TrailControllPoints[1].x, m_xmf4TrailControllPoints[1].y, m_xmf4TrailControllPoints[1].z);

		m_xmf4TrailControllPoints[0] = XMFLOAT4(controllBasePos.x, controllBasePos.y, controllBasePos.z, 1.0f);
		m_xmf4TrailControllPoints[1] = XMFLOAT4(offsetPosition.x, offsetPosition.y, offsetPosition.z, 1.0f);
	}
}
void CKnightPlayer::PrepareBoundingBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	pWeapon = CGameObject::FindFrame("Dragonblade");
	UpdateTransform(NULL);

	XMFLOAT3 center = pWeapon->m_pMesh->GetBoundingCenter();
	XMFLOAT3 extent = pWeapon->m_pMesh->GetBoundingExtent();
	//XMStoreFloat3(&center, XMVector3TransformCoord(XMVECTOR({ center.x, center.y, center.z, 1.0f }), XMLoadFloat4x4(&pWeapon->m_xmf4x4World)));
	//XMStoreFloat3(&extent, XMVector3TransformCoord(XMVECTOR({ extent.x, extent.y, extent.z, 0.0f }), XMLoadFloat4x4(&pWeapon->m_xmf4x4World)));
#ifdef RENDER_BOUNDING_BOX
	pBodyBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.35f, 1.0f, 0.35f));
	pWeaponBoundingBoxMesh = CBoundingBoxShader::GetInst()->AddBoundingObject(pd3dDevice, pd3dCommandList, this, center, extent);
#endif // RENDER_BOUNDING_BOX
	m_BodyBoundingBox = BoundingOrientedBox{ XMFLOAT3(0.0f, 0.75f, 0.0f), XMFLOAT3(0.35f, 1.0f, 0.35f), XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
	m_WeaponBoundingBox = BoundingOrientedBox{ center, extent, XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f} };
}

void CKnightPlayer::DrinkPotion()
{
	m_pSkinnedAnimationController->m_pSubAnimationTracks[0].m_bEnable = TRUE;
	m_pSkinnedAnimationController->m_pSubAnimationTracks[0].m_fPosition = 0.0f;
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
			//			m_xmf3RootTransfromPreviousPosition = xmf3Position;
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
	CPlayer* player = (CPlayer*)pRootGameObject;

	if (m_bRootMotion)
	{
		CPlayer* player = (CPlayer*)pRootGameObject;
		player->m_pStateMachine->GetCurrentState()->OnRootMotion(player, fTimeElapsed);
	}
	else
	{
		m_pRootMotionObject->m_xmf4x4Transform._41 = 0.f;
		m_pRootMotionObject->m_xmf4x4Transform._43 = 0.f;
	}
}
