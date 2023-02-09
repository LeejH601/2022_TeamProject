#include "stdafx.h"
#include "Locator.h"
#include "Global.h"
#include "Component.h"
#include "Camera.h"
#include "..\Object\State.h"
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
	m_pMessageDispatcher = std::make_shared<CMessageDispatcher>();
	m_pEntityManager = std::make_shared<CEntityManager>();

	std::shared_ptr<CComponentSet> componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);

	CSoundManager::GetInst()->RegisterSound("Sound/David Bowie - Starman.mp3", false);

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
		((CShootSoundComponent*)component.get())->SetSound(&*CSoundManager::GetInst()->FindSound("Sound/David Bowie - Starman.mp3"));
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
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, effectsound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, effectsound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CEffectSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CShootSoundComponent>:"));
	CShootSoundComponent* shootsound = (CShootSoundComponent*)componentset->FindComponent(typeid(CShootSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, shootsound->GetEnable());
	WriteStringFromFile(pInFile, std::string("<Delay>:"));
	WriteFloatFromFile(pInFile, shootsound->m_fDelay);
	WriteStringFromFile(pInFile, std::string("<Volume>:"));
	WriteFloatFromFile(pInFile, shootsound->m_fVolume);
	WriteStringFromFile(pInFile, std::string("</CShootSoundComponent>:"));

	WriteStringFromFile(pInFile, std::string("<CDamageSoundComponent>:"));
	CDamageSoundComponent* Damagesound = (CDamageSoundComponent*)componentset->FindComponent(typeid(CDamageSoundComponent));
	WriteStringFromFile(pInFile, std::string("<Enable>:"));
	WriteIntegerFromFile(pInFile, Damagesound->GetEnable());
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
					component->SetEnable(ReadFloatFromFile(pInFile));
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
				component->SetEnable(ReadFloatFromFile(pInFile));
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
				component->SetEnable(ReadFloatFromFile(pInFile));
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
