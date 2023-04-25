#include "PhysicsObject.h"
#include "Terrain.h"
#include "..\Global\Locator.h"

void CPhysicsObject::OnPrepareRender()
{
	if (m_bSimulateArticulate) {
		UpdateTransformFromArticulation(NULL, m_pArtiLinkNames, m_AritculatCacheMatrixs, m_xmf3Scale.x);
	}
	else {
		m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
		m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
		m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
		m_xmf4x4Transform._41 = m_xmf3Position.x; m_xmf4x4Transform._42 = m_xmf3Position.y; m_xmf4x4Transform._43 = m_xmf3Position.z;

		XMMATRIX mtxScale = XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
		m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

		UpdateTransform(NULL);
	}
}

void CPhysicsObject::OnUpdateCallback(float fTimeElapsed)
{
	m_bOnGround = false;
	if (m_pUpdatedContext)
	{
		CSplatTerrain* pTerrain = (CSplatTerrain*)m_pUpdatedContext;
		XMFLOAT3 xmf3TerrainPos = pTerrain->GetPosition();

		float fTerrainY = pTerrain->GetHeight(GetPosition().x - (xmf3TerrainPos.x), GetPosition().z - (xmf3TerrainPos.z));

		if (GetPosition().y < fTerrainY + xmf3TerrainPos.y)
		{
			SetPosition(XMFLOAT3(GetPosition().x, fTerrainY + xmf3TerrainPos.y, GetPosition().z));
			m_bOnGround = true;
		}
	}
}

void CPhysicsObject::Update(float fTimeElapsed)
{
	Apply_Gravity(fTimeElapsed);

	Move(m_xmf3Velocity, false);

	// 터레인보다 아래에 있지 않도록 하는 코드
	if (m_pUpdatedContext)OnUpdateCallback(fTimeElapsed);

	Animate(fTimeElapsed);

	Apply_Friction(fTimeElapsed);
}

void CPhysicsObject::Apply_Gravity(float fTimeElapsed)
{
	// 중력을 적용하는 코드
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(XMFLOAT3(0.0f, -100.0f, 0.0f), fTimeElapsed, false));
}

void CPhysicsObject::Apply_Friction(float fTimeElapsed)
{
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (300.0f * fTimeElapsed);

	// 마찰력을 적용하는 코드, 마찰력이 속도의 크기보다 크면 속도를 표현하는 m_xmf3Velocity를 {0.0f, 0.0f, 0.0f}로 초기화
	// 이외에는 마찰로 인한 감속
	if (fDeceleration > fLength)
		m_xmf3Velocity = XMFLOAT3{};
	else
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPhysicsObject::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);
}

void CPhysicsObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	{
		if (fPitch != 0.0f)
		{
			m_fPitch += fPitch;
			if (m_fPitch > +89.0f) { fPitch -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { fPitch -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (fYaw != 0.0f)
		{
			m_fYaw += fYaw;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (fRoll != 0.0f)
		{
			m_fRoll += fRoll;
			if (m_fRoll > +20.0f) { fRoll -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { fRoll -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		if (fYaw != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPhysicsObject::SetPosition(XMFLOAT3 xmf3Position)
{
	m_xmf3Position = xmf3Position;
}

void CPhysicsObject::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	XMFLOAT3 UpVec = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, UpVec);

	m_xmf3Right.x = mtxLookAt._11, m_xmf3Right.y = mtxLookAt._21, m_xmf3Right.z = mtxLookAt._31;
	m_xmf3Up.x = mtxLookAt._12, m_xmf3Up.y = mtxLookAt._22, m_xmf3Up.z = mtxLookAt._32;
	m_xmf3Look.x = mtxLookAt._13, m_xmf3Look.y = mtxLookAt._23, m_xmf3Look.z = mtxLookAt._33;
}

void CPhysicsObject::SetScale(float x, float y, float z)
{
	m_xmf3Scale.x = x;
	m_xmf3Scale.y = y;
	m_xmf3Scale.z = z;
}

XMFLOAT3 CPhysicsObject::GetPosition()
{
	return m_xmf3Position;
}

XMFLOAT3 CPhysicsObject::GetLook()
{
	return m_xmf3Look;
}

XMFLOAT3 CPhysicsObject::GetUp()
{
	return m_xmf3Up;
}

XMFLOAT3 CPhysicsObject::GetRight()
{
	return m_xmf3Right;
}

XMFLOAT3 CPhysicsObject::GetVelocity()
{
	return m_xmf3Velocity;
}

bool CPhysicsObject::GetOnGround()
{
	return m_bOnGround;
}

void CPhysicsObject::Animate(float fTimeElapsed)
{
	if (m_bSimulateArticulate) {
		UpdateTransformFromArticulation(NULL, m_pArtiLinkNames, m_AritculatCacheMatrixs, m_xmf3Scale.x);
	}
	else {
		CGameObject::Animate(fTimeElapsed);
	}
}

void CPhysicsObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	OnPrepareRender();
	CGameObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CPhysicsObject::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
	}
}
void CPhysicsObject::updateArticulationMatrix()
{
	int index = 0;
	for (XMFLOAT4X4& world : m_AritculatCacheMatrixs) {
		physx::PxMat44 mat = m_pArticulationLinks[index++]->getGlobalPose();

		memcpy(&world, &mat, sizeof(XMFLOAT4X4));
		world = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), world);
	}
}
physx::PxTransform CPhysicsObject::MakeTransform(XMFLOAT4X4& xmf44)
{
	physx::PxMat44 mymatrix;
	XMFLOAT4X4 myTransform = xmf44;
	myTransform._41 = myTransform._41 * m_xmf3Scale.x;
	myTransform._42 = myTransform._42 * m_xmf3Scale.y;
	myTransform._43 = myTransform._43 * m_xmf3Scale.z;

	memcpy(&mymatrix, &myTransform, sizeof(physx::PxMat44));
	physx::PxTransform transfrom = physx::PxTransform(mymatrix);
	transfrom = transfrom.getNormalized();
	return transfrom;
}
void CPhysicsObject::SetJoint(physx::PxArticulationJointReducedCoordinate* joint, JointAxisDesc& JointDesc)
{
	int AxisCnt = 0;
	switch (JointDesc.type)
	{
	case physx::PxArticulationJointType::eSPHERICAL:
		joint->setJointType(physx::PxArticulationJointType::eSPHERICAL);
		if (JointDesc.eSwing1) {
			joint->setMotion(physx::PxArticulationAxis::eSWING1, physx::PxArticulationMotion::eLIMITED);
			joint->setLimitParams(physx::PxArticulationAxis::eSWING1, JointDesc.eS1LImit);
			joint->setDriveParams(physx::PxArticulationAxis::eSWING1, JointDesc.eS1Drive);
			AxisCnt++;
		}
		if (JointDesc.eSwing2) {
			joint->setMotion(physx::PxArticulationAxis::eSWING2, physx::PxArticulationMotion::eLIMITED);
			joint->setLimitParams(physx::PxArticulationAxis::eSWING2, JointDesc.eS2LImit);
			joint->setDriveParams(physx::PxArticulationAxis::eSWING2, JointDesc.eS2Drive);
			AxisCnt++;
		}
		if (JointDesc.eTWIST) {
			joint->setMotion(physx::PxArticulationAxis::eTWIST, physx::PxArticulationMotion::eLIMITED);
			joint->setLimitParams(physx::PxArticulationAxis::eTWIST, JointDesc.eTLImit);
			joint->setDriveParams(physx::PxArticulationAxis::eTWIST, JointDesc.eTDrive);
			AxisCnt++;
		}
		if (AxisCnt < 2) {
			TCHAR pstrDebug[256] = { 0 };
			_stprintf_s(pstrDebug, 256, _T("error!!! Insufficient number of rotational axes set.\n"));
			OutputDebugString(pstrDebug);
		}
		break;
	case physx::PxArticulationJointType::eREVOLUTE:
		joint->setJointType(physx::PxArticulationJointType::eREVOLUTE);
		if (JointDesc.eSwing1) {
			joint->setMotion(physx::PxArticulationAxis::eSWING1, physx::PxArticulationMotion::eLIMITED);
			joint->setLimitParams(physx::PxArticulationAxis::eSWING1, JointDesc.eS1LImit);
			joint->setDriveParams(physx::PxArticulationAxis::eSWING1, JointDesc.eS1Drive);
			AxisCnt++;
		}
		if (JointDesc.eSwing2) {
			joint->setMotion(physx::PxArticulationAxis::eSWING2, physx::PxArticulationMotion::eLIMITED);
			joint->setLimitParams(physx::PxArticulationAxis::eSWING2, JointDesc.eS2LImit);
			joint->setDriveParams(physx::PxArticulationAxis::eSWING2, JointDesc.eS2Drive);
			AxisCnt++;
		}
		if (JointDesc.eTWIST) {
			joint->setMotion(physx::PxArticulationAxis::eTWIST, physx::PxArticulationMotion::eLIMITED);
			joint->setLimitParams(physx::PxArticulationAxis::eTWIST, JointDesc.eTLImit);
			joint->setDriveParams(physx::PxArticulationAxis::eTWIST, JointDesc.eTDrive);
			AxisCnt++;
		}
		if (AxisCnt > 1) {
			TCHAR pstrDebug[256] = { 0 };
			_stprintf_s(pstrDebug, 256, _T("error!!! Too many rotational axes set.\n"));
			OutputDebugString(pstrDebug);
		}
		break;
	case physx::PxArticulationJointType::eFIX:
		joint->setJointType(physx::PxArticulationJointType::eFIX);
		if (AxisCnt > 0) {
			TCHAR pstrDebug[256] = { 0 };
			_stprintf_s(pstrDebug, 256, _T("error!!! Too many rotational axes set.\n"));
			OutputDebugString(pstrDebug);
		}
		break;
	default:
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("error!!! Invalid descriptor.\n"));
		OutputDebugString(pstrDebug);
		return;
	}
}
physx::PxArticulationLink* CPhysicsObject::SetLink(physx::PxArticulationReducedCoordinate* articulation, physx::PxArticulationLink* p_link, physx::PxTransform& parent, physx::PxTransform& child, float meshScale)
{
	physx::PxArticulationLink* link = articulation->createLink(p_link, child);
	if (meshScale > 0.00001f) {
		physx::PxVec3 distance = (child.p) * 0.5f;
		physx::PxReal len = distance.magnitude();
		physx::PxTransform center = physx::PxTransform(physx::PxVec3(len / 2.0f, 0.0f, 0.0f));
		physx::PxBoxGeometry linkGeometry = physx::PxBoxGeometry(0.05f * m_xmf3Scale.x * meshScale, 0.05f * m_xmf3Scale.y * meshScale, 0.05f * m_xmf3Scale.z * meshScale);
		physx::PxCapsuleGeometry linkCapsuleGeometry = physx::PxCapsuleGeometry(0.1f * m_xmf3Scale.x * meshScale, len / 2.0f * meshScale);
		physx::PxMaterial* material = Locator.GetPxPhysics()->createMaterial(0.9, 0.9f, 0.1);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*link, linkCapsuleGeometry, *material);
		physx::PxVec3 localpos = (child.p + parent.p) * 0.5f;
		physx::PxRigidBodyExt::updateMassAndInertia(*link, 1.0f);
	}

	return link;
}
void CPhysicsObject::CreateArticulation(float meshScale)
{
	auto DebugJointDot = [](physx::PxArticulationLink* link) {
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("dof: %d\n"), link->getInboundJointDof());
		OutputDebugString(pstrDebug);
	};

	m_pArticulation = Locator.GetPxPhysics()->createArticulationReducedCoordinate();
	m_pArticulation->setArticulationFlag(physx::PxArticulationFlag::eDISABLE_SELF_COLLISION, true);
	m_pArticulation->setSolverIterationCounts(30, 10);
	m_pArticulation->setMaxCOMLinearVelocity(FLT_MAX);

	JointAxisDesc FixDesc;
	FixDesc.type = physx::PxArticulationJointType::eFIX;
	FixDesc.eS1LImit.low = -FLT_MAX;
	FixDesc.eS1LImit.high = FLT_MAX;
	FixDesc.eS2LImit.low = -FLT_MAX;
	FixDesc.eS2LImit.high = FLT_MAX;
	FixDesc.eTLImit.low = -FLT_MAX;
	FixDesc.eTLImit.high = FLT_MAX;
	FixDesc.eS1Drive.maxForce = FLT_MAX;
	FixDesc.eS2Drive.maxForce = FLT_MAX;
	FixDesc.eTDrive.maxForce = FLT_MAX;
	JointAxisDesc SPHERICALDesc;
	SPHERICALDesc.type = physx::PxArticulationJointType::eSPHERICAL;
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eS1LImit.low = -FLT_MAX;
	SPHERICALDesc.eS1LImit.high = FLT_MAX;
	SPHERICALDesc.eS2LImit.low = -FLT_MAX;
	SPHERICALDesc.eS2LImit.high = FLT_MAX;
	SPHERICALDesc.eTLImit.low = -FLT_MAX;
	SPHERICALDesc.eTLImit.high = FLT_MAX;
	SPHERICALDesc.eS1Drive.maxForce = FLT_MAX;
	SPHERICALDesc.eS2Drive.maxForce = FLT_MAX;
	SPHERICALDesc.eTDrive.maxForce = FLT_MAX;
	JointAxisDesc REVOLUTEDesc;
	REVOLUTEDesc.type = physx::PxArticulationJointType::eREVOLUTE;
	REVOLUTEDesc.eS1LImit.low = -FLT_MAX;
	REVOLUTEDesc.eS1LImit.high = FLT_MAX;
	REVOLUTEDesc.eS2LImit.low = -FLT_MAX;
	REVOLUTEDesc.eS2LImit.high = FLT_MAX;
	REVOLUTEDesc.eTLImit.low = -FLT_MAX;
	REVOLUTEDesc.eTLImit.high = FLT_MAX;
	REVOLUTEDesc.eS1Drive.maxForce = FLT_MAX;
	REVOLUTEDesc.eS2Drive.maxForce = FLT_MAX;
	REVOLUTEDesc.eTDrive.maxForce = FLT_MAX;

	CGameObject::Animate(0.0f);
	CPhysicsObject::OnPrepareRender();

	CGameObject* obj = FindFrame(m_pChild->m_pstrFrameName);
	CGameObject* root = FindFrame("root");

	std::string target{ "root" };
	XMFLOAT4X4 ParentMt = obj->m_xmf4x4Transform;
	XMFLOAT4X4 ChildMt = root->m_xmf4x4Transform;
	physx::PxArticulationLink* root_link = SetLink(m_pArticulation, nullptr, MakeTransform(ParentMt), MakeTransform(ChildMt), 0.0f);
	physx::PxArticulationJointReducedCoordinate* joint = root_link->getInboundJoint();
	m_pArtiLinkNames.emplace_back(target);


	target = "pelvis";
	CGameObject* pelvis = FindFrame("pelvis");
	ParentMt = root->m_xmf4x4Transform;
	ChildMt = pelvis->m_xmf4x4Transform;
	physx::PxArticulationLink* pelvis_link = SetLink(m_pArticulation, root_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = pelvis_link->getInboundJoint();
	SetJoint(joint, FixDesc);

	m_pArtiLinkNames.emplace_back(target);
#define _test_ragdoll

	target = "spine_01";
	CGameObject* spine_01 = FindFrame("spine_01");
	ParentMt = pelvis->m_xmf4x4Transform;
	ChildMt = spine_01->m_xmf4x4Transform;
	physx::PxArticulationLink* Spine01_link = SetLink(m_pArticulation, pelvis_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = Spine01_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eTLImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -10.0f * physx::PxPi / 180.0f;  // x?
	SPHERICALDesc.eS2LImit.high = 10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTDrive.driveType = physx::PxArticulationDriveType::eNONE;
	SPHERICALDesc.eTDrive.damping = 1.0f;
	SPHERICALDesc.eTDrive.stiffness = 5.0f;
	SPHERICALDesc.eS1Drive.driveType = physx::PxArticulationDriveType::eNONE;
	SPHERICALDesc.eS1Drive.damping = 1.0f;
	SPHERICALDesc.eS1Drive.stiffness = 5.0f;
	SPHERICALDesc.eS2Drive.driveType = physx::PxArticulationDriveType::eNONE;
	SPHERICALDesc.eS2Drive.damping = 1.0f;
	SPHERICALDesc.eS2Drive.stiffness = 5.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "spine_02";
	CGameObject* spine_02 = FindFrame("spine_02");
	ParentMt = spine_01->m_xmf4x4Transform;
	ChildMt = spine_02->m_xmf4x4Transform;
	physx::PxArticulationLink* Spine02_link = SetLink(m_pArticulation, Spine01_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = Spine02_link->getInboundJoint();

#ifdef _test_ragdoll
	SetJoint(joint, FixDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "spine_03";
	CGameObject* spine_03 = FindFrame("spine_03");
	ParentMt = spine_02->m_xmf4x4Transform;
	ChildMt = spine_03->m_xmf4x4Transform;
	physx::PxArticulationLink* Spine03_link = SetLink(m_pArticulation, Spine02_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = Spine03_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eTLImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -10.0f * physx::PxPi / 180.0f;  // x?
	SPHERICALDesc.eS2LImit.high = 10.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif // _test_ragdoll
	//DebugJointDot(Spine03_link);
	m_pArtiLinkNames.emplace_back(target);

	target = "neck_01";
	CGameObject* neck_01 = FindFrame("neck_01");
	ParentMt = spine_03->m_xmf4x4Transform;
	ChildMt = neck_01->m_xmf4x4Transform;
	physx::PxArticulationLink* neck_link = SetLink(m_pArticulation, Spine03_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = neck_link->getInboundJoint();
	SPHERICALDesc.eTLImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -10.0f * physx::PxPi / 180.0f;  // x?
	SPHERICALDesc.eS2LImit.high = 40.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif 
	m_pArtiLinkNames.emplace_back(target);

	target = "head";
	CGameObject* head = FindFrame("head");
	ParentMt = neck_01->m_xmf4x4Transform;
	ChildMt = head->m_xmf4x4Transform;
	physx::PxArticulationLink* head_link = SetLink(m_pArticulation, neck_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = head_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = false;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eTLImit.low = -40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -15.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 30.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif // _test_ragdoll
	m_pArtiLinkNames.emplace_back(target);

	target = "clavicle_l";
	CGameObject* clavicle_l = FindFrame("clavicle_l");
	ParentMt = spine_03->m_xmf4x4Transform;
	ChildMt = clavicle_l->m_xmf4x4Transform;
	physx::PxArticulationLink* clavicle_l_link = SetLink(m_pArticulation, Spine03_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = clavicle_l_link->getInboundJoint();
	SetJoint(joint, FixDesc);
	m_pArtiLinkNames.emplace_back(target);

	target = "upperarm_l";
	CGameObject* upperarm_l = FindFrame("upperarm_l");
	ParentMt = clavicle_l->m_xmf4x4Transform;
	ChildMt = upperarm_l->m_xmf4x4Transform;
	physx::PxArticulationLink* upperarm_l_link = SetLink(m_pArticulation, clavicle_l_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = upperarm_l_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eS1LImit.low = -90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.low = -20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 20.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif // _test_ragdoll
	//DebugJointDot(upperarm_l_link);
	m_pArtiLinkNames.emplace_back(target);

	target = "lowerarm_l";
	CGameObject* lowerarm_l = FindFrame("lowerarm_l");
	ParentMt = upperarm_l->m_xmf4x4Transform;
	ChildMt = lowerarm_l->m_xmf4x4Transform;
	physx::PxArticulationLink* lowerarm_l_link = SetLink(m_pArticulation, upperarm_l_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = lowerarm_l_link->getInboundJoint();
	REVOLUTEDesc.eSwing1 = false;
	REVOLUTEDesc.eSwing2 = true;
	REVOLUTEDesc.eTWIST = false;
	REVOLUTEDesc.eS2LImit.low = 0.0f * physx::PxPi / 180.0f;
	REVOLUTEDesc.eS2LImit.high = 140.0f * physx::PxPi / 180.0f;
	REVOLUTEDesc.eTDrive.driveType = physx::PxArticulationDriveType::eNONE;
	REVOLUTEDesc.eTDrive.damping = 1.0f;
	REVOLUTEDesc.eTDrive.stiffness = 5.0f;
	REVOLUTEDesc.eS1Drive.driveType = physx::PxArticulationDriveType::eNONE;
	REVOLUTEDesc.eS1Drive.damping = 1.0f;
	REVOLUTEDesc.eS1Drive.stiffness = 5.0f;
	REVOLUTEDesc.eS2Drive.driveType = physx::PxArticulationDriveType::eNONE;
	REVOLUTEDesc.eS2Drive.damping = 1.0f;
	REVOLUTEDesc.eS2Drive.stiffness = 5.0f;
#ifdef _test_ragdoll
	SetJoint(joint, REVOLUTEDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "hand_l";
	CGameObject* hand_l = FindFrame("hand_l");
	ParentMt = lowerarm_l->m_xmf4x4Transform;
	ChildMt = hand_l->m_xmf4x4Transform;
	physx::PxArticulationLink* hand_l_link = SetLink(m_pArticulation, lowerarm_l_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = hand_l_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = false;
	SPHERICALDesc.eS1LImit.low = -20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 10.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);



	target = "clavicle_r";
	CGameObject* clavicle_r = FindFrame("clavicle_r");
	ParentMt = spine_03->m_xmf4x4Transform;
	ChildMt = clavicle_r->m_xmf4x4Transform;
	physx::PxArticulationLink* clavicle_r_link = SetLink(m_pArticulation, Spine03_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = clavicle_r_link->getInboundJoint();
	SetJoint(joint, FixDesc);
	m_pArtiLinkNames.emplace_back(target);

	target = "upperarm_r";
	CGameObject* upperarm_r = FindFrame("upperarm_r");
	ParentMt = clavicle_r->m_xmf4x4Transform;
	ChildMt = upperarm_r->m_xmf4x4Transform;
	physx::PxArticulationLink* upperarm_r_link = SetLink(m_pArticulation, clavicle_r_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = upperarm_r_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eS1LImit.low = -90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 90.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.low = -20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 20.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "lowerarm_r";
	CGameObject* lowerarm_r = FindFrame("lowerarm_r");
	ParentMt = upperarm_r->m_xmf4x4Transform;
	ChildMt = lowerarm_r->m_xmf4x4Transform;
	physx::PxArticulationLink* lowerarm_r_link = SetLink(m_pArticulation, upperarm_r_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = lowerarm_r_link->getInboundJoint();
	REVOLUTEDesc.eSwing1 = false;
	REVOLUTEDesc.eSwing2 = true;
	REVOLUTEDesc.eTWIST = false;
	REVOLUTEDesc.eS2LImit.low = 0.0f * physx::PxPi / 180.0f;
	REVOLUTEDesc.eS2LImit.high = 140.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, REVOLUTEDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "hand_r";
	CGameObject* hand_r = FindFrame("hand_r");
	ParentMt = lowerarm_r->m_xmf4x4Transform;
	ChildMt = hand_r->m_xmf4x4Transform;
	physx::PxArticulationLink* hand_r_ink = SetLink(m_pArticulation, lowerarm_r_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = hand_r_ink->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = false;
	SPHERICALDesc.eS1LImit.low = -20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 60.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	//DebugJointDot(hand_r_ink);
	m_pArtiLinkNames.emplace_back(target);


	target = "thigh_l";
	CGameObject* thigh_l = FindFrame("thigh_l");
	ParentMt = pelvis->m_xmf4x4Transform;
	ChildMt = thigh_l->m_xmf4x4Transform;
	physx::PxArticulationLink* thigh_l_link = SetLink(m_pArticulation, pelvis_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = thigh_l_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eS1LImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 45.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -110.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.low = -40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 40.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "calf_l";
	CGameObject* calf_l = FindFrame("calf_l");
	ParentMt = thigh_l->m_xmf4x4Transform;
	ChildMt = calf_l->m_xmf4x4Transform;
	physx::PxArticulationLink* calf_l_link = SetLink(m_pArticulation, thigh_l_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = calf_l_link->getInboundJoint();
	REVOLUTEDesc.eSwing1 = false;
	REVOLUTEDesc.eSwing2 = true;
	REVOLUTEDesc.eTWIST = false;
	REVOLUTEDesc.eS2LImit.low = 0.0f * physx::PxPi / 180.0f;
	REVOLUTEDesc.eS2LImit.high = 100.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, REVOLUTEDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "foot_l";
	CGameObject* foot_l = FindFrame("foot_l");
	ParentMt = calf_l->m_xmf4x4Transform;
	ChildMt = foot_l->m_xmf4x4Transform;
	physx::PxArticulationLink* foot_l_link = SetLink(m_pArticulation, calf_l_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = foot_l_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = false;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eS2LImit.low = -50.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.low = -30.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 30.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "ball_l";
	CGameObject* ball_l = FindFrame("ball_l");
	ParentMt = foot_l->m_xmf4x4Transform;
	ChildMt = ball_l->m_xmf4x4Transform;
	physx::PxArticulationLink* ball_l_link = SetLink(m_pArticulation, foot_l_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = ball_l_link->getInboundJoint();
	SetJoint(joint, FixDesc);
	m_pArtiLinkNames.emplace_back(target);


	target = "thigh_r";
	CGameObject* thigh_r = FindFrame("thigh_r");
	ParentMt = pelvis->m_xmf4x4Transform;
	ChildMt = thigh_r->m_xmf4x4Transform;
	physx::PxArticulationLink* thigh_r_link = SetLink(m_pArticulation, pelvis_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = thigh_r_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = true;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eS1LImit.low = -10.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS1LImit.high = 45.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.low = -110.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.low = -40.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 40.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "calf_r";
	CGameObject* calf_r = FindFrame("calf_r");
	ParentMt = thigh_r->m_xmf4x4Transform;
	ChildMt = calf_r->m_xmf4x4Transform;
	physx::PxArticulationLink* calf_r_link = SetLink(m_pArticulation, thigh_r_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = calf_r_link->getInboundJoint();
	REVOLUTEDesc.eSwing1 = false;
	REVOLUTEDesc.eSwing2 = true;
	REVOLUTEDesc.eTWIST = false;
	REVOLUTEDesc.eS2LImit.low = 0.0f * physx::PxPi / 180.0f;
	REVOLUTEDesc.eS2LImit.high = 100.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, REVOLUTEDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "foot_r";
	CGameObject* foot_r = FindFrame("foot_r");
	ParentMt = calf_r->m_xmf4x4Transform;
	ChildMt = foot_r->m_xmf4x4Transform;
	physx::PxArticulationLink* foot_r_link = SetLink(m_pArticulation, calf_r_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = foot_r_link->getInboundJoint();
	SPHERICALDesc.eSwing1 = false;
	SPHERICALDesc.eSwing2 = true;
	SPHERICALDesc.eTWIST = true;
	SPHERICALDesc.eS2LImit.low = -50.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eS2LImit.high = 20.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.low = -30.0f * physx::PxPi / 180.0f;
	SPHERICALDesc.eTLImit.high = 30.0f * physx::PxPi / 180.0f;
#ifdef _test_ragdoll
	SetJoint(joint, SPHERICALDesc);
#else
	SetJoint(joint, FixDesc);
#endif
	m_pArtiLinkNames.emplace_back(target);

	target = "ball_r";
	CGameObject* ball_r = FindFrame("ball_r");
	ParentMt = foot_r->m_xmf4x4Transform;
	ChildMt = ball_r->m_xmf4x4Transform;
	physx::PxArticulationLink* ball_r_link = SetLink(m_pArticulation, foot_r_link, MakeTransform(ParentMt), MakeTransform(ChildMt), meshScale);
	joint = ball_r_link->getInboundJoint();
	SetJoint(joint, FixDesc);
	m_pArtiLinkNames.emplace_back(target);


	m_pArticulation->setSleepThreshold(0.6f);

	//Locator.GetPxScene()->addArticulation(*m_pArticulation);

	physx::PxU32 nbLinks = m_pArticulation->getNbLinks();
	m_pArticulationLinks.resize(nbLinks);
	m_pArticulation->getLinks(m_pArticulationLinks.data(), nbLinks, 0);
	m_AritculatCacheMatrixs.resize(nbLinks);

	/*int index = 0;
	for (XMFLOAT4X4& world : m_AritculatCacheMatrixs) {
		physx::PxMat44 mat = m_pArticulationLinks[index++]->getGlobalPose();

		memcpy(&world, &mat, sizeof(XMFLOAT4X4));
		world = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), world);
	}


	m_pArticulationCache = m_pArticulation->createCache();
	m_nArtiCache = m_pArticulation->getCacheDataSize();*/
	//m_bSimulateArticulate = true;
}