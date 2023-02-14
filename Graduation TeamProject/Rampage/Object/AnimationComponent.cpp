#include "AnimationComponent.h"

CDamageAnimationComponent::CDamageAnimationComponent()
{
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
}

bool CStunAnimationComponent::HandleMessage(const Telegram& msg)
{
	return false;
}
