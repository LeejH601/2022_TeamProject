#include "../Global/Global.h"
#include "../Global/Locator.h"
#include "BillBoardComponent.h"
#include "..\Scene\SimulatorScene.h"
#include "..\Object\TextureManager.h"

std::vector<std::pair<std::shared_ptr<CMonster>, std::shared_ptr<CMultiSpriteObject>>> CAttackSpriteComponent::m_vSprite = std::vector<std::pair<std::shared_ptr<CMonster>, std::shared_ptr<CMultiSpriteObject>>>();

void CAttackSpriteComponent::Add_AttackComponent(std::pair<std::shared_ptr<CMonster>, std::shared_ptr<CMultiSpriteObject>> pAttackComponent)
{
	m_vSprite.emplace_back(pAttackComponent);
	SetSpeed(m_fSpeed);
	SetAlpha(m_fAlpha);
}

void CAttackSpriteComponent::Collision_Check()
{
	for (auto& pAttackComponent : m_vSprite) {
		if (pAttackComponent.first->bHit && pAttackComponent.second->m_bAnimation) {
			pAttackComponent.second->Set_Enable(true);
			pAttackComponent.second->SetPosition(pAttackComponent.first->GetPosition());
			UpdateData();
		}
	}
}


void CAttackSpriteComponent::SetSpriteEnable(int iIndex)
{
		if (m_vSprite.size() <= iIndex)
		return;
	m_vSprite[iIndex].second->Set_Enable(true);
	SetSpeed(m_fSpeed);
	SetAlpha(m_fAlpha);
}

void CAttackSpriteComponent::SetTexture(int iIndex, LPCTSTR pszFileName)
{
	if (m_vSprite.size() <= iIndex)
		return;
	std::shared_ptr pTexture = CSimulatorScene::GetInst()->GetTextureManager()->LoadBillBoardTexture(pszFileName);
	if(pTexture)
		m_vSprite[iIndex].second->ChangeTexture(pTexture);
}

void CAttackSpriteComponent::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;

	if (m_vSprite[0].second)
		m_vSprite[0].second->SetSpeed(fSpeed);
}

void CAttackSpriteComponent::SetAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;

	if (m_vSprite[0].second)
		m_vSprite[0].second->SetAlpah(m_fAlpha);
}

void CAttackSpriteComponent::UpdateData()
{
	if (m_vSprite[0].second) {
		m_vSprite[0].second->SetSpeed(m_fSpeed);
	}
	if (m_bEnable)
		m_bAnimation = true;
}

unsigned int& CAttackSpriteComponent::GetAttackNumber()
{
	return m_nMonsterAttackNumber;
}

float& CAttackSpriteComponent::GetSpeed()
{
	return m_fSpeed;
}

float& CAttackSpriteComponent::GetAlpha()
{
	return m_fAlpha;
}

//bool CAttackSpriteComponent::HandleMessage(const Telegram& msg)
//{
//	return false;
//}