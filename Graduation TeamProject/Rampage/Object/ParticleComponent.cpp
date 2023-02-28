#include "ParticleComponent.h"

std::shared_ptr<CParticleObject> CParticleComponent::m_pParticleObject = NULL;

void CParticleComponent::Set_ParticleComponent(std::shared_ptr<CParticleObject> pParticleComponent)
{
	m_pParticleObject = pParticleComponent;
}

void CParticleComponent::Update()
{
	if (m_pParticleObject) {
		m_pParticleObject->SetEnable(m_bEnable);
		m_pParticleObject->SetSize(m_fSize);
	}
}
