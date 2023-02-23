#include "stdafx.h"
#include "Locator.h"
#include "Global.h"
#include "Component.h"
#include "Camera.h"
#include "..\Object\State.h"
#include "..\Object\AnimationComponent.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundPlayer.h"
#include "..\Sound\SoundManager.h"
#include "EntityManager.h"
#include "MessageDispatcher.h"

CLocator::~CLocator()
{
	m_pEntityManager->Clear();
	m_pEntityManager.reset();
}

bool CLocator::Init()
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	m_pPxPvd = physx::PxCreatePvd(*m_pFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 100);
	m_pPxPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	bool recordMemoryAllocations = true;

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, m_pPxPvd);

	physx::PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2); // 이 세가지 파라미터가 반드시 필요함.
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader; // 각각 어떤 역할을 하는 지는 추가적으로 조사해볼 필요가 있음.
	SceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	SceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);

	m_pPxScene = m_pPhysics->createScene(SceneDesc);

	//physx::PxTransform transform(physx::PxVec3(0.0f, 10.0f, 0.0f));

	//physx::PxMaterial* material = m_pPhysics->createMaterial(0.5, 0.5, 0.5);
	//physx::PxShape* shape = m_pPhysics->createShape(physx::PxBoxGeometry(1.0f, 1.0f, 1.0f), *material);

	//physx::PxRigidDynamic* actor = physx::PxCreateDynamic(*m_pPhysics, transform, physx::PxBoxGeometry(1.0f, 1.0f, 1.0f), *material, 1.0f);


	//physx::PxRigidStatic* plane = physx::PxCreateStatic(*m_pPhysics, physx::PxTransform(physx::PxVec3(0.0f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f))), physx::PxPlaneGeometry(), *material);

	////physx::PxActor* actor = m_pPhysics->createRigidDynamic(transform);

	////actor->attachShape(*actor);
	//m_pPxScene->addActor(*plane);
	//m_pPxScene->addActor(*actor);

	pvdClient = m_pPxScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	////////////////////////////////////////////////

	physx::PxArticulationReducedCoordinate* articulation = m_pPhysics->createArticulationReducedCoordinate();
	//physx::PxArticulationJointReducedCoordinate* joint;
	physx::PxArticulationLink* pelvislink;
	physx::PxReal scale = 10.0f;

	auto testlink = [scale](physx::PxArticulationLink* _link, physx::PxTransform& ParentPos, physx::PxTransform& ChildPos) {
		physx::PxBoxGeometry linkGeometry = physx::PxBoxGeometry(0.01f * scale, 0.05f * scale, 0.01f * scale);
		physx::PxMaterial* material = Locator.GetPxPhysics()->createMaterial(0.5, 0.5, 0.5);
		physx::PxRigidActorExt::createExclusiveShape(*_link, linkGeometry, *material);
		physx::PxRigidBodyExt::updateMassAndInertia(*_link, 1.0f);
		physx::PxArticulationJointReducedCoordinate* joint = _link->getInboundJoint();
		if (joint) {
			joint->setJointType(physx::PxArticulationJointType::eSPHERICAL);
			physx::PxArticulationDrive drive;
			drive.damping = 2.0f;
			drive.stiffness = 0.2f;
			drive.driveType = physx::PxArticulationDriveType::eFORCE;
			joint->setDriveParams(physx::PxArticulationAxis::eSWING2, drive);
			joint->setDriveTarget(physx::PxArticulationAxis::eSWING2, 10.0f);

			//joint->setMotion(physx::PxArticulationAxis::eTWIST, physx::PxArticulationMotion::eFREE);
			
			joint->setParentPose(ParentPos);
			joint->setChildPose(ChildPos);
		}
	};

	// Create the pelvis link
	physx::PxTransform pelvisPose = physx::PxTransform(physx::PxIdentity);
	pelvislink = articulation->createLink(nullptr, pelvisPose);
	testlink(pelvislink, physx::PxTransform(physx::PxIdentity), pelvisPose);

	// Create the spine links
	physx::PxTransform spine1Pose = physx::PxTransform(physx::PxVec3(0.0f * scale, 0.1f * scale, 0.0f * scale));
	pelvislink = articulation->createLink(pelvislink, spine1Pose);
	testlink(pelvislink, pelvisPose, spine1Pose);

	physx::PxTransform spine2Pose = physx::PxTransform(physx::PxVec3(0.0f * scale, 0.1f * scale, 0.0f * scale));
	pelvislink = articulation->createLink(pelvislink, spine2Pose);
	testlink(pelvislink, spine1Pose, spine2Pose);

	physx::PxTransform spine3Pose = physx::PxTransform(physx::PxVec3(0.0f * scale, 0.1f * scale, 0.0f * scale));
	pelvislink = articulation->createLink(pelvislink, spine3Pose);
	testlink(pelvislink, spine2Pose, spine3Pose);

	// Create the left leg links
	physx::PxTransform leftLeg1Pose = physx::PxTransform(physx::PxVec3(-0.1f * scale, -0.2f * scale, 0.0f * scale));
	physx::PxArticulationLink* LFlink = articulation->createLink(pelvislink, leftLeg1Pose);
	testlink(LFlink, spine3Pose, leftLeg1Pose);

	physx::PxTransform leftLeg2Pose = physx::PxTransform(physx::PxVec3(0.0f * scale, -0.2f * scale, 0.0f * scale));
	LFlink = articulation->createLink(LFlink, leftLeg2Pose);
	testlink(LFlink, leftLeg1Pose, leftLeg2Pose);

	physx::PxTransform leftFootPose = physx::PxTransform(physx::PxVec3(0.0f * scale, -0.2f * scale, -0.1f * scale));
	LFlink = articulation->createLink(LFlink, leftFootPose);
	testlink(LFlink, leftLeg2Pose, leftFootPose);

	// Create the right leg links
	physx::PxTransform rightLeg1Pose = physx::PxTransform(physx::PxVec3(0.1f * scale, -0.2f * scale, 0.0f * scale));
	physx::PxArticulationLink*  RPlink = articulation->createLink(pelvislink, rightLeg1Pose);
	testlink(RPlink, spine3Pose, rightLeg1Pose);

	physx::PxTransform rightLeg2Pose = physx::PxTransform(physx::PxVec3(0.0f * scale, -0.2f * scale, 0.0f * scale));
	RPlink = articulation->createLink(RPlink, rightLeg2Pose);
	testlink(RPlink, rightLeg1Pose, rightLeg2Pose);


	physx::PxTransform rightFootPose = physx::PxTransform(physx::PxVec3(0.0f * scale, -0.2f * scale, -0.1f * scale));
	RPlink = articulation->createLink(RPlink, rightFootPose);
	testlink(RPlink, rightLeg2Pose, rightFootPose);

	//// Create the left arm links
	//physx::PxTransform leftArm1Pose = physx::PxTransform(physx::PxVec3(-0.2f, 0.1f, 0.0f));
	//link = articulation->createLink(link, leftArm1Pose);

	//physx::PxTransform leftArm2Pose = physx::PxTransform(physx::PxVec3(0.0f, 0.1f, 0.0f));
	//link = articulation->createLink(link, leftArm2Pose);

	//physx::PxTransform leftHandPose = physx::PxTransform(physx::PxVec3(0.0f, 0.1f, -0.1f));
	//link = articulation->createLink(link, leftHandPose);

	//// Create the right arm links
	//physx::PxTransform rightArm1Pose = physx::PxTransform(physx::PxVec3(0.2f, 0.1f, 0.0f));
	//link = articulation->createLink(link, rightArm1Pose);

	//physx::PxTransform rightArm2Pose = physx::PxTransform(physx::PxVec3(0.0f, 0.1f, 0.0f));
	//link = articulation->createLink(link, rightArm2Pose);

	//physx::PxTransform rightHandPose = PxTransform
	//m_pPxScene->addArticulation(*articulation);

	//physx::PxArticulationReducedCoordinate* articulation = m_pPhysics->createArticulationReducedCoordinate();

	//articulation->setArticulationFlag(physx::PxArticulationFlag::eFIX_BASE, true);
	//articulation->setSolverIterationCounts(1);
	////articulation->setMaxCOMLinearVelocity(maxCOMLinearVelocity);

	//physx::PxArticulationLink* link = articulation->createLink(parent, PxTransform());
	//physx::PxRigidActorExt::createExclusiveShape(*link, linkGeometry, material);
	//physx::PxRigidBodyExt::updateMassAndInertia(*link, 1.0f);

	/////////////////////////////////////////////////////


	m_pMessageDispatcher = std::make_shared<CMessageDispatcher>();
	m_pEntityManager = std::make_shared<CEntityManager>();

	std::shared_ptr<CComponentSet> componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);

	CSoundManager::GetInst()->RegisterSound("Sound/David Bowie - Starman.mp3", false, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Air Cut by Langerium Id-84616.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Bloody Blade 2 by Kreastricon62 Id-323526.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/Swing by XxChr0nosxX Id-268227.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Sword by hello_flowers Id-37596.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Sword4 by Streety Id-30246.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/Sword7 by Streety Id-30248.wav", false, SOUND_CATEGORY::SOUND_SHOOT);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh1.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh2.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh3.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh4.wav", false, SOUND_CATEGORY::SOUND_SHOCK);
	CSoundManager::GetInst()->RegisterSound("Sound/effect/HammerFlesh5.wav", false, SOUND_CATEGORY::SOUND_SHOCK);

	for (auto& [num, componentSet] : m_sComponentSets) {
		std::shared_ptr<CComponent> component;

		component = std::make_shared<CCameraMover>();
		componentSet->AddComponent(component);
		component = std::make_shared<CCameraShaker>();
		componentSet->AddComponent(component);
		component = std::make_shared<CCameraZoomer>();
		componentSet->AddComponent(component);

		component = std::make_shared<CDamageSoundComponent>(CSoundManager::GetInst()->GetSoundSystem());
		componentSet->AddComponent(component);
		component = std::make_shared<CEffectSoundComponent>(CSoundManager::GetInst()->GetSoundSystem());
		componentSet->AddComponent(component);
		component = std::make_shared<CShootSoundComponent>(CSoundManager::GetInst()->GetSoundSystem());
		componentSet->AddComponent(component);

		component = std::make_shared<CDamageAnimationComponent>();
		componentSet->AddComponent(component);
		component = std::make_shared<CShakeAnimationComponent>();
		componentSet->AddComponent(component);
		component = std::make_shared<CStunAnimationComponent>();
		componentSet->AddComponent(component);
	}

	std::shared_ptr<CState<CPlayer>> state = std::make_shared<Idle_Player>();
	SetPlayerState(state);
	state = std::make_shared<Atk1_Player>();
	SetPlayerState(state);
	state = std::make_shared<Atk2_Player>();
	SetPlayerState(state);
	state = std::make_shared<Atk3_Player>();
	SetPlayerState(state);

	m_pSoundPlayer = std::make_shared<CSoundPlayer>();
	return true;
}



CState<CPlayer>* CLocator::GetPlayerState(const std::type_info& type)
{
	/*auto comp = [](size_t hashcode, CState<CPlayer>& state) {
		return hashcode < typeid(state).hash_code();
	};*/
	//auto it = m_sPlayerStateSet.find<size_t, Statecomp, Statecomp::is_transparent>(type.hash_code());
	statedummy.first = type.hash_code();
	std::set<PlayerStatePair, Comp_PlayerState>::iterator it = m_sPlayerStateSet.find(statedummy);
	if (it == m_sPlayerStateSet.end())
		return nullptr;

	return it->second.get();
}

void CLocator::SetPlayerState(std::shared_ptr<CState<CPlayer>>& state)
{
	PlayerStatePair pair = std::make_pair(typeid(*state.get()).hash_code(), state);
	m_sPlayerStateSet.insert(pair);
}

void DataLoader::SaveComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets)
{
	std::string path;
	FILE* pInFile;

	for (auto [num, componentset] : ComponentSets) {
		path = file_path + std::to_string(num) + file_ext;
		::fopen_s(&pInFile, path.c_str(), "wb");

		SaveComponentSet(pInFile, componentset.get());

		fclose(pInFile);
	}
}

void DataLoader::LoadComponentSets(std::set<CoptSetPair, Comp_ComponentSet>& ComponentSets)
{
	std::string path;
	FILE* pInFile;

	for (auto [num, componentset] : ComponentSets) {
		path = file_path + std::to_string(num) + file_ext;

		::fopen_s(&pInFile, path.c_str(), "rb");
		if (!pInFile)
			continue;

		LoadComponentSet(pInFile, componentset.get());

		fclose(pInFile);
	}
}

void DataLoader::SaveComponentSet(FILE* pInFile, CComponentSet* componentset)
{
	std::string str = "<Components>:";
	WriteStringFromFile(pInFile, str);

	WriteStringFromFile(pInFile, std::string("<CCameraMover>:"));
	CCameraMover* mover = (CCameraMover*)componentset->FindComponent(typeid(CCameraMover));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, mover->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, mover->m_fMaxDistance);
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, mover->m_fMovingTime);
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, mover->m_fRollBackTime);
	WriteStringFromFile(pInFile, std::string("</CCameraMover>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraShaker>:"));
	CCameraShaker* shaker = (CCameraShaker*)componentset->FindComponent(typeid(CCameraShaker));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, shaker->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Duration>:"));
	WriteFloatFromFile(pInFile, shaker->m_fDuration);
	WriteStringFromFile(pInFile, std::string("<Magnitude>:"));
	WriteFloatFromFile(pInFile, shaker->m_fMagnitude);
	WriteStringFromFile(pInFile, std::string("</CCameraShaker>:"));

	WriteStringFromFile(pInFile, std::string("<CCameraZoomer>:"));
	CCameraZoomer* zoomer = (CCameraZoomer*)componentset->FindComponent(typeid(CCameraZoomer));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, zoomer->GetEnable());
	WriteStringFromFile(pInFile, std::string("<MaxDistance>:"));
	WriteFloatFromFile(pInFile, zoomer->m_fMaxDistance);
	WriteStringFromFile(pInFile, std::string("<MovingTime>:"));
	WriteFloatFromFile(pInFile, zoomer->m_fMovingTime);
	WriteStringFromFile(pInFile, std::string("<RollBackTime>:"));
	WriteFloatFromFile(pInFile, zoomer->m_fRollBackTime);
	WriteStringFromFile(pInFile, std::string("<IsZoomIN>:"));
	WriteIntegerFromFile(pInFile, zoomer->m_bIsIN);
	WriteStringFromFile(pInFile, std::string("</CCameraZoomer>:"));

	WriteStringFromFile(pInFile, std::string("<CEffectSoundComponent>:"));
	CEffectSoundComponent* effectsound = (CEffectSoundComponent*)componentset->FindComponent(typeid(CEffectSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, effectsound->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, effectsound->m_nSoundNumber);
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, effectsound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, effectsound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CEffectSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CShootSoundComponent>:"));
	CShootSoundComponent* shootsound = (CShootSoundComponent*)componentset->FindComponent(typeid(CShootSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, shootsound->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, shootsound->m_nSoundNumber);
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, shootsound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, shootsound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CShootSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CDamageSoundComponent>:"));
	CDamageSoundComponent* Damagesound = (CDamageSoundComponent*)componentset->FindComponent(typeid(CDamageSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, Damagesound->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Sound>:"));
	WriteIntegerFromFile(pInFile, Damagesound->m_nSoundNumber);
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, Damagesound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, Damagesound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CDamageSoundComponent>:"));

	str = "</Components>:";
	WriteStringFromFile(pInFile, str);
}

void DataLoader::LoadComponentSet(FILE* pInFile, CComponentSet* componentset)
{
	char buf[256];
	std::string str;
	str.resize(256);
	ReadStringFromFile(pInFile, buf);

	for (; ; )
	{
		ReadStringFromFile(pInFile, buf);

		if (!strcmp(buf, "<CCameraMover>:"))
		{
			CCameraMover* component = (CCameraMover*)componentset->FindComponent(typeid(CCameraMover));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					component->m_fMaxDistance = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					component->m_fMovingTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					component->m_fRollBackTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraMover>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraShaker>:"))
		{
			CCameraShaker* component = (CCameraShaker*)componentset->FindComponent(typeid(CCameraShaker));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Duration>:"))
				{
					component->m_fDuration = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Magnitude>:"))
				{
					component->m_fMagnitude = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraShaker>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CCameraZoomer>:"))
		{
			CCameraZoomer* component = (CCameraZoomer*)componentset->FindComponent(typeid(CCameraZoomer));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<MaxDistance>:"))
				{
					component->m_fMaxDistance = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<MovingTime>:"))
				{
					component->m_fMovingTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<RollBackTime>:"))
				{
					component->m_fRollBackTime = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<IsZoomIN>:"))
				{
					component->m_bIsIN = ReadIntegerFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "</CCameraZoomer>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CEffectSoundComponent>:"))
		{
			CEffectSoundComponent* component = (CEffectSoundComponent*)componentset->FindComponent(typeid(CEffectSoundComponent));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					component->m_nSoundNumber = ReadIntegerFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					component->m_fDelay = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					component->m_fVolume = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "</CEffectSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CShootSoundComponent>:"))
		{
			CShootSoundComponent* component = (CShootSoundComponent*)componentset->FindComponent(typeid(CShootSoundComponent));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					component->m_nSoundNumber = ReadIntegerFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					component->m_fDelay = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					component->m_fVolume = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "</CShootSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "<CDamageSoundComponent>:"))
		{
			CDamageSoundComponent* component = (CDamageSoundComponent*)componentset->FindComponent(typeid(CDamageSoundComponent));

			for (; ; )
			{
				ReadStringFromFile(pInFile, buf);

				if (!strcmp(buf, "<Enable>:"))
				{
					component->SetEnable(ReadIntegerFromFile(pInFile));
				}
				else if (!strcmp(buf, "<Sound>:"))
				{
					component->m_nSoundNumber = ReadIntegerFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Delay>:"))
				{
					component->m_fDelay = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "<Volume>:"))
				{
					component->m_fVolume = ReadFloatFromFile(pInFile);
				}
				else if (!strcmp(buf, "</CDamageSoundComponent>:"))
				{
					break;
				}
			}
		}
		else if (!strcmp(buf, "</Components>:"))
		{
			break;
		}
	}
}
