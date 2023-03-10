#include "ParticleComponent.h"
#include "..\Object\TextureManager.h"

std::shared_ptr<CParticleObject> CParticleComponent::m_pParticleObject = NULL;

void CParticleComponent::SetTexture(int iIndex, LPCTSTR pszFileName)
{
	std::shared_ptr pTexture = CTextureManager::GetInst()->LoadParticleTexture(pszFileName);
	if (pTexture)
		m_pParticleObject->ChangeTexture(pTexture);
}

void CParticleComponent::Set_ParticleComponent(std::shared_ptr<CParticleObject> pParticleComponent)
{
	m_pParticleObject = pParticleComponent;
}

void CParticleComponent::Update()
{
	if (m_pParticleObject) {
		m_pParticleObject->SetEnable(m_bEnable);
		m_pParticleObject->SetSize(m_fSize);
		m_pParticleObject->SetAlpha(m_fAlpha);
		m_pParticleObject->SetColor(m_f3Color);
		m_pParticleObject->SetSpeed(m_fSpeed);
		m_pParticleObject->SetLifeTime(m_fLifeTime);
		m_pParticleObject->SetMaxParticleN(m_nParticleNumber);
		m_pParticleObject->SetEmitParticleN(m_nParticleNumber);
	}
}

int& CParticleComponent::GetParticleNumber()
{
	return m_nParticleNumber;
}

int& CParticleComponent::GetParticleIndex()
{
	return m_nParticleIndex;
}
