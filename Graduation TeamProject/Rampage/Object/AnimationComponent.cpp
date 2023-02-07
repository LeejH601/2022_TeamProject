#include "AnimationComponent.h"

CDamageAnimationComponent::CDamageAnimationComponent()
{
}

bool CDamageAnimationComponent::HandleMessage(const Telegram& msg)
{
	return false;
}

CShakeAnimationComponent::CShakeAnimationComponent()
{
}

bool CShakeAnimationComponent::HandleMessage(const Telegram& msg)
{
	return false;
}

CStaggerAnimationComponent::CStaggerAnimationComponent()
{
}

bool CStaggerAnimationComponent::HandleMessage(const Telegram& msg)
{
	return false;
}
