#include "..\Global\Global.h"
#include "SoundPlayer.h"
#include "SoundComponent.h"
#include "..\Global\Locator.h"

void CSoundPlayer::Update(float fElapsedTime)
{
	if (m_pEffectComponent)
		m_pEffectComponent->Update(fElapsedTime);
	if (m_pDamageComponent)
		m_pDamageComponent->Update(fElapsedTime);
	if (m_pShootComponent)
		m_pShootComponent->Update(fElapsedTime);
}

void CSoundPlayer::LoadComponentFromSet(CComponentSet* componentset)
{
	m_pDamageComponent = componentset->FindComponent(typeid(CDamageSoundComponent));
	m_pEffectComponent = componentset->FindComponent(typeid(CEffectSoundComponent));
	m_pShootComponent = componentset->FindComponent(typeid(CShootSoundComponent));
}