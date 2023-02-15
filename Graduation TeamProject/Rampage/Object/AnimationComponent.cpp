#include "AnimationComponent.h"

CDamageAnimationComponent::CDamageAnimationComponent()
{
	m_bEnable = true;
}

float CDamageAnimationComponent::GetDamageDistance(float t)
{ 
	return m_fSpeed * t;
}

bool CDamageAnimationComponent::HandleMessage(const Telegram& msg)
{
	return false;
}

CShakeAnimationComponent::CShakeAnimationComponent()
{
	m_bEnable = true;
}

float CShakeAnimationComponent::GetShakeDistance(float t)
{
	float fResult = m_fDistance * sin( (2 * PI * t) / m_fFrequency);
	return fResult;
}

bool CShakeAnimationComponent::HandleMessage(const Telegram& msg)
{
	return false;
}

CStunAnimationComponent::CStunAnimationComponent()
{
	m_bEnable = true;
}

bool CStunAnimationComponent::HandleMessage(const Telegram& msg)
{
	return false;
}
