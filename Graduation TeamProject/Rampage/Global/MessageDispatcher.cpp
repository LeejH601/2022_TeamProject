#include "MessageDispatcher.h"
#include "Camera.h"
#include "..\Scene\Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Monster.h"
#include "..\Object\MonsterState.h"
#include "..\Object\ParticleObject.h"
#include "..\Sound\SoundManager.h"

void CMessageDispatcher::RegisterListener(MessageType messageType, IMessageListener* listener, void* filterObject)
{
	ListenerInfo info = { listener, filterObject };
	m_listeners[static_cast<int>(messageType)].push_back(info);
}
void PlayerAttackComponent::HandleMessage(const Message& message, const PlayerAttackParams& params)
{
    if (message.getType() == MessageType::PLAYER_ATTACK) {
		if (params.pPlayer->CheckCollision(m_pObject))
		{
			CollideParams collide_params;
			collide_params.xmf3CollidePosition = m_pObject->GetPosition();
			CMessageDispatcher::GetInst()->Dispatch_Message<CollideParams>(MessageType::COLLISION, &collide_params, nullptr);
		}
    }
}
void SoundPlayComponent::HandleMessage(const Message& message, const SoundPlayParams& params)
{
	if (!m_bEnable)
		return;

    if (message.getType() == MessageType::PLAY_SOUND && m_sc == params.sound_category) {
        auto pSound = CSoundManager::GetInst()->FindSound(m_nSoundNumber, m_sc);
        CSoundManager::GetInst()->PlaySound(pSound->GetPath(), m_fVolume, m_fDelay);
    }
}
void CameraShakeComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (!m_bEnable)
		return;
	if (pCamera->m_bCameraShaking && m_fDuration > m_ft) {
		m_ft += fElapsedTime;

		XMFLOAT3 CameraDir = pCamera->GetRightVector();

		float ShakeConstant = urd(dre);
		float RotateConstant = urd(dre);
		RotateConstant *= XM_PI;

		CameraDir = Vector3::ScalarProduct(CameraDir, ShakeConstant * m_fMagnitude, false);
		XMMATRIX RotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&pCamera->GetLookVector()), RotateConstant);

		XMFLOAT3 ShakeOffset; XMStoreFloat3(&ShakeOffset, XMVector3TransformCoord(XMLoadFloat3(&CameraDir), RotateMatrix));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, ShakeOffset);
	}
	else {
		m_ft = 0.0f;
		pCamera->m_bCameraShaking = false;
	}
}
void CameraShakeComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (m_bEnable)
		Update(params.pCamera, params.fElapsedTime);
}
void CameraZoomerComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (!m_bEnable || !pCamera->m_bCameraZooming)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	float ZoomConstant = (m_bIsIN) ? 1.0f : -1.0f;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, length * ZoomConstant, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
	else if (m_fCurrDistance > m_fMaxDistance * 2.0f) {
		pCamera->m_bCameraZooming = false;
		m_fCurrDistance = 0.f;
		offset.x = 0.0f;
		offset.y = 0.0f;
		offset.z = 0.0f;
	}
	else if (m_fCurrDistance > m_fMaxDistance) {
		float length = m_fBackSpeed * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, -length * ZoomConstant, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}
void CameraZoomerComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (m_bEnable)
		Update(params.pCamera, params.fElapsedTime);
}
void CameraMoveComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (!m_bEnable || !pCamera->m_bCameraMoving)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * 1.0f * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, length, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
	else if (m_fCurrDistance > m_fMaxDistance * 2.0f) {
		pCamera->m_bCameraMoving = false;
		m_fCurrDistance = 0.f;
		offset.x = 0.0f;
		offset.y = 0.0f;
		offset.z = 0.0f;
	}
	else if (m_fCurrDistance > m_fMaxDistance) {
		float length = m_fBackSpeed * 1.0f * fElapsedTime;
		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(m_xmf3Direction, -length, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}
void CameraMoveComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (m_bEnable)
		Update(params.pCamera, params.fElapsedTime);
}
void DamageAnimationComponent::HandleMessage(const Message& message, const AnimationCompParams& params)
{
	if (!m_bEnable)
		return;

	float fDamageDistance = m_fSpeed * params.fElapsedTime;

	for (int i = 0; i < params.pObjects->size(); ++i)
	{
		CGameObject* pObject = ((*(params.pObjects))[i]).get();

		CMonster* pMonster = dynamic_cast<CMonster*>(pObject);

		if (pMonster)
		{
			pMonster->m_fDamageDistance = 0.0f;

			if ((pMonster->m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst()
				|| pMonster->m_pStateMachine->GetCurrentState() == Stun_Monster::GetInst()))
			{
				if (pMonster->m_fTotalDamageDistance < m_fMaxDistance)
				{
					pMonster->m_fDamageDistance = fDamageDistance;
					pMonster->m_fTotalDamageDistance += fDamageDistance;

					if (m_fMaxDistance < pMonster->m_fTotalDamageDistance)
						pMonster->m_fDamageDistance -= (pMonster->m_fTotalDamageDistance - m_fMaxDistance);
				}
			}
		}
	}
}
void ShakeAnimationComponent::HandleMessage(const Message& message, const AnimationCompParams& params)
{
	if (!m_bEnable)
		return;

	for (int i = 0; i < params.pObjects->size(); ++i)
	{
		CGameObject* pObject = ((*(params.pObjects))[i]).get();

		CMonster* pMonster = dynamic_cast<CMonster*>(pObject);

		if (pMonster)
		{
			pMonster->m_fShakeDistance = 0.0f;

			float fShakeDistance = m_fDistance * sin((2 * PI * pMonster->m_pSkinnedAnimationController->m_fTime + pMonster->m_fStunTime) / m_fFrequency);
			
			if (pMonster->m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst() || pMonster->m_pStateMachine->GetCurrentState() == Stun_Monster::GetInst())
				pMonster->m_fShakeDistance = fShakeDistance;
		}
	}
}
void StunAnimationComponent::HandleMessage(const Message& message, const AnimationCompParams& params)
{
	if (!m_bEnable)
		return;

	for (int i = 0; i < params.pObjects->size(); ++i)
	{
		CGameObject* pObject = ((*(params.pObjects))[i]).get();

		CMonster* pMonster = dynamic_cast<CMonster*>(pObject);

		if (pMonster)
		{
			if (pMonster->m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst())
			{
				if (pMonster->m_fStunStartTime < pMonster->m_pSkinnedAnimationController->m_fTime && !pMonster->m_bStunned)
					pMonster->m_pStateMachine->ChangeState(Stun_Monster::GetInst());
			}

			else if (pMonster->m_pStateMachine->GetCurrentState() == Stun_Monster::GetInst())
			{
				if (pMonster->m_fStunTime < m_fStunTime)
					pMonster->m_fStunTime += params.fElapsedTime;
				else
					pMonster->m_pStateMachine->ChangeState(Damaged_Monster::GetInst());
			}
		}
	}
}
void ParticleComponent::HandleMessage(const Message& message, const ParticleCompParams& params)
{
	//if (!m_bEnable)
	//	return;
	/*if (!m_bEnable)
		return;*/

	CParticleObject* pParticle = dynamic_cast<CParticleObject*>(params.pObject);

	if (pParticle)
	{

		pParticle->SetEnable(true);
		pParticle->SetSize(m_fSize);
		pParticle->SetAlpha(m_fAlpha);
		pParticle->SetColor(m_xmf3Color);
		pParticle->SetSpeed(m_fSpeed);
		pParticle->SetLifeTime(m_fLifeTime);
		pParticle->SetMaxParticleN(m_nParticleNumber);
		pParticle->SetEmitParticleN(m_nParticleNumber);
		pParticle->SetPosition(params.xmf3Position);
	}
}
void SceneCollideListener::HandleMessage(const Message& message, const CollideParams& params)
{
	m_pScene->HandleCollision(params);
}
