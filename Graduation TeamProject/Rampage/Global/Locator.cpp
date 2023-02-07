#include "Locator.h"
#include "Global.h"
#include "Component.h"
#include "Camera.h"
#include "..\Object\State.h"
#include "..\Object\AnimationComponent.h"
#include "..\Sound\SoundComponent.h"
#include "..\Sound\SoundManager.h"
#include "..\Sound\SoundPlayer.h"

bool CLocator::Init()
{
	std::shared_ptr<CComponentSet> componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);
	componentset = std::make_shared<CComponentSet>();
	SetComponentSet(componentset);


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
		((CSoundComponent*)component.get())->SetSound("Sound/David Bowie - Starman.mp3");
		componentSet->AddComponent(component);

		component = std::make_shared<CDamageAnimationComponent>();
		componentSet->AddComponent(component);
		component = std::make_shared<CShakeAnimationComponent>();
		componentSet->AddComponent(component);
		component = std::make_shared<CStaggerAnimationComponent>();
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
	if(it == m_sPlayerStateSet.end())
		return nullptr;

	return it->second.get();
}

void CLocator::SetPlayerState(std::shared_ptr<CState<CPlayer>>& state)
{
	PlayerStatePair pair = std::make_pair(typeid(*state.get()).hash_code(), state);
	m_sPlayerStateSet.insert(pair);
}
