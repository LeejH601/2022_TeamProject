#include "MessageDispatcher.h"
#include "Camera.h"
#include "..\Scene\Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Player.h"
#include "..\Object\Monster.h"
#include "..\Object\MonsterState.h"
#include "..\Object\BillBoardObject.h"
#include "..\Sound\SoundManager.h"
#include "..\Scene\MainScene.h"

void CMessageDispatcher::RegisterListener(MessageType messageType, IMessageListener* listener, void* filterObject)
{
	ListenerInfo info = { listener, filterObject };
	m_listeners[static_cast<int>(messageType)].push_back(info);
}
void PlayerAttackListener::HandleMessage(const Message& message, const PlayerParams& params)
{
    if (message.getType() == MessageType::PLAYER_ATTACK) {
		if (params.pPlayer->CheckCollision(m_pObject))
		{
			CollideParams collide_params;
			collide_params.xmf3CollidePosition = m_pObject->GetPosition();
			CMessageDispatcher::GetInst()->Dispatch_Message<CollideParams>(MessageType::COLLISION, &collide_params, nullptr);

			SoundPlayParams sound_play_params;
			sound_play_params.monster_type = ((CMonster*)m_pObject)->GetMonsterType();
			sound_play_params.sound_category = SOUND_CATEGORY::SOUND_VOICE;
			CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &sound_play_params, ((CPlayer*)(params.pPlayer))->m_pStateMachine->GetCurrentState());
		}
    }
}
void SoundPlayComponent::HandleMessage(const Message& message, const SoundPlayParams& params)
{
	if (!m_bEnable)
		return;

    if (message.getType() == MessageType::PLAY_SOUND && m_sc == params.sound_category && m_mt == params.monster_type) {
		std::vector<CSound>::iterator pSound;
		if (m_sc == SOUND_CATEGORY::SOUND_VOICE)
		{
			switch (m_mt)
			{
			case MONSTER_TYPE::GOBLIN:
				pSound = CSoundManager::GetInst()->FindSound(m_nSoundNumber, m_sc);
				break;
			case MONSTER_TYPE::ORC:
				pSound = CSoundManager::GetInst()->FindSound(GOBLIN_MOAN_SOUND_NUM + m_nSoundNumber, m_sc);
				break;
			case MONSTER_TYPE::SKELETON:
				pSound = CSoundManager::GetInst()->FindSound(GOBLIN_MOAN_SOUND_NUM + ORC_MOAN_SOUND_NUM + m_nSoundNumber, m_sc);
				break;
			default:
				break;
			}
		}
		else
			pSound = CSoundManager::GetInst()->FindSound(m_nSoundNumber, m_sc);

 		CSoundManager::GetInst()->PlaySound(pSound->GetPath(), m_fVolume, m_fDelay);
    }
}
void CameraShakeComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (pCamera->m_bCameraShaking && m_fDuration > m_ft) {
		m_ft += fElapsedTime;

		XMFLOAT3 CameraDir = pCamera->GetRightVector();

		float fShakeDistance = (m_fMagnitude - (m_fMagnitude / m_fDuration) * m_ft) * cos((2 * PI * m_ft) / m_fFrequency);
		float RotateConstant = urd(dre);
		RotateConstant *= XM_PI;

		CameraDir = Vector3::ScalarProduct(CameraDir, fShakeDistance, false);
		XMMATRIX RotateMatrix = XMMatrixRotationAxis(XMLoadFloat3(&pCamera->GetLookVector()), RotateConstant);

		XMFLOAT3 ShakeOffset; 
		XMStoreFloat3(&ShakeOffset, XMVector3TransformCoord(XMLoadFloat3(&CameraDir), RotateMatrix));
		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, ShakeOffset);
	}
	else {
		m_ft = 0.0f;
		pCamera->m_bCameraShaking = false;
	}
}
void CameraShakeComponent::Reset()
{
	m_ft = 0.0f;
}
void CameraShakeComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (!m_bEnable)
		return;

	Update(params.pCamera, params.fElapsedTime);
}
void CameraZoomerComponent::Update(CCamera* pCamera, float fElapsedTime, const CameraUpdateParams& params)
{
	if (!pCamera->m_bCameraZooming)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	float ZoomConstant = (m_bIsIN) ? 1.0f : -1.0f;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * fElapsedTime;
		m_fCurrDistance += length;

		XMFLOAT3 xmf3ZoomDirection = Vector3::Normalize(Vector3::Subtract(pCamera->GetPosition(), ((CPlayer*)(params.pPlayer))->GetTargetPosition()));
		offset = Vector3::Add(offset, Vector3::ScalarProduct(xmf3ZoomDirection, length * ZoomConstant, false));

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

		XMFLOAT3 xmf3ZoomDirection = Vector3::Normalize(Vector3::Subtract(pCamera->GetPosition(), ((CPlayer*)(params.pPlayer))->GetTargetPosition()));
		offset = Vector3::Add(offset, Vector3::ScalarProduct(xmf3ZoomDirection, -length * ZoomConstant, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}
void CameraZoomerComponent::Reset()
{
	m_fCurrDistance = 0.f;
	offset.x = 0.0f;
	offset.y = 0.0f;
	offset.z = 0.0f;
}
void CameraZoomerComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (!m_bEnable)
		return;
	Update(params.pCamera, params.fElapsedTime, params);
}
void CameraMoveComponent::Update(CCamera* pCamera, float fElapsedTime, const CameraUpdateParams& params)
{
	if (!pCamera->m_bCameraMoving)
		return;

	m_fSpeed = m_fMaxDistance / m_fMovingTime;
	m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	if (m_fCurrDistance < m_fMaxDistance) {
		float length = m_fSpeed * 1.0f * fElapsedTime;

		m_fCurrDistance += length;

		offset = Vector3::Add(offset, Vector3::ScalarProduct(((CPlayer*)(params.pPlayer))->GetATKDirection(), length, false));

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

		offset = Vector3::Add(offset, Vector3::ScalarProduct(((CPlayer*)(params.pPlayer))->GetATKDirection(), -length, false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}
void CameraMoveComponent::Reset()
{
	m_fCurrDistance = 0.f;
	offset.x = 0.0f;
	offset.y = 0.0f;
	offset.z = 0.0f;
}
void CameraMoveComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (!m_bEnable)
		return;

	Update(params.pCamera, params.fElapsedTime, params);
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
			XMFLOAT3 xmf3DamageVec = XMFLOAT3(0.0f, 0.0f, 0.0f);

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

			xmf3DamageVec = Vector3::ScalarProduct(XMFLOAT3{ pMonster->GetHitterVec().x, 0.0f, pMonster->GetHitterVec().z }, pMonster->m_fDamageDistance, false);
			pMonster->Move(xmf3DamageVec, true);
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
			XMFLOAT3 xmf3ShakeVec = XMFLOAT3(0.0f, 0.0f, 0.0f);

			pMonster->m_fShakeDistance = 0.0f;

			float fTimeElapsed = pMonster->m_pSkinnedAnimationController->m_fTime + pMonster->m_fStunTime;

			if (m_fDuration < fTimeElapsed)
				return;

			if (pMonster->m_pStateMachine->GetCurrentState() == Damaged_Monster::GetInst() || pMonster->m_pStateMachine->GetCurrentState() == Stun_Monster::GetInst())
			{
				float fShakeDistance = (m_fDistance - (m_fDistance / m_fDuration) * fTimeElapsed) * cos((2 * PI * fTimeElapsed) / m_fFrequency);
				pMonster->m_fShakeDistance = fShakeDistance;
			}
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
	if (!m_bEnable)
		return;

	CParticleObject* pParticle = dynamic_cast<CParticleObject*>(params.pObject);

	if (pParticle)
	{
		pParticle->SetEmit(true);
		pParticle->SetSize(m_fSize);
		pParticle->SetStartAlpha(m_fAlpha);
		pParticle->SetColor(m_xmf3Color);
		pParticle->SetSpeed(m_fSpeed);
		pParticle->SetLifeTime(m_fLifeTime);
		pParticle->SetMaxParticleN(m_nParticleNumber);
		pParticle->SetEmitParticleN(m_nEmitParticleNumber);
		pParticle->SetPosition(params.xmf3Position);
		pParticle->SetParticleType(m_iParticleType);
		pParticle->ChangeTexture(m_pTexture);
	}
}
void ImpactEffectComponent::HandleMessage(const Message& message, const ImpactCompParams& params)
{
	if (!m_bEnable)
		return;

	CMultiSpriteObject* pMultiSprite = dynamic_cast<CMultiSpriteObject*>(params.pObject);

	if (pMultiSprite)
	{
		pMultiSprite->SetEnable(true);
		pMultiSprite->SetSize(m_fSize);
		pMultiSprite->SetSpeed(m_fSpeed);
		pMultiSprite->SetStartAlpha(m_fAlpha);
		pMultiSprite->SetPosition(params.xmf3Position);
		pMultiSprite->ChangeTexture(m_pTexture);
	}
}
void SceneCollideListener::HandleMessage(const Message& message, const CollideParams& params)
{
	m_pScene->HandleCollision(params);
}
void TerrainSpriteComponent::SetTexture(LPCTSTR pszFileName)
{
}

void TerrainSpriteComponent::HandleMessage(const Message& message, const TerrainSpriteCompParams& params)
{
	CTerrainSpriteObject* pSpriteObject = dynamic_cast<CTerrainSpriteObject*>(params.pObject);

	if (pSpriteObject)
	{
		pSpriteObject->SetType(TerrainSpriteType::TERRAINSPRITE_CROSS_FADE);
		pSpriteObject->SetEnable(true);
		pSpriteObject->SetPosition(params.xmf3Position);
		pSpriteObject->SetLifeTime(m_fLifeTime);
		pSpriteObject->SetStart(true);
		pSpriteObject->SetStartAlpha(1.f);
	}
}

void SmokeParticleComponent::HandleMessage(const Message& message, const ParticleSmokeParams& params)
{
	CParticleObject* pParticleObject = dynamic_cast<CParticleObject*>(params.pObject);
	
	if (pParticleObject)
	{
		pParticleObject->SetEmit(true);
		pParticleObject->SetSize(m_fSize);
		pParticleObject->SetStartAlpha(m_fAlpha);
		pParticleObject->SetColor(m_xmf3Color);
		pParticleObject->SetSpeed(m_fSpeed);
		pParticleObject->SetLifeTime(m_fLifeTime);
		pParticleObject->SetMaxParticleN(m_nParticleNumber);
		pParticleObject->SetPosition(params.xmf3Position);
		pParticleObject->SetParticleType(m_iParticleType);
	}
}

void UpDownParticleComponent::HandleMessage(const Message& message, const ParticleUpDownParams& params)
{
	CParticleObject* pParticleObject = dynamic_cast<CParticleObject*>(params.pObject);

	pParticleObject->SetEmit(true);
	pParticleObject->SetPosition(params.xmf3Position);
	pParticleObject->SetParticleType(m_iParticleType);

	pParticleObject->SetSize(m_fSize);
	pParticleObject->SetStartAlpha(m_fAlpha);
	pParticleObject->SetColor(m_xmf3Color);
	pParticleObject->SetLifeTime(m_fLifeTime);
	pParticleObject->SetSpeed(m_fSpeed);
	pParticleObject->SetDirection(XMFLOAT3(0.f, 1.f, 0.f));
}


void TrailParticleComponent::HandleMessage(const Message& message, const ParticleTrailParams& params)
{
	CParticleObject* pParticleObject = dynamic_cast<CParticleObject*>(params.pObject);
	if (pParticleObject)
	{
		pParticleObject->SetEmit(true);
		pParticleObject->SetSize(m_fSize);
		pParticleObject->SetStartAlpha(m_fAlpha);
		pParticleObject->SetColor(m_xmf3Color);
		pParticleObject->SetSpeed(m_fSpeed);
		pParticleObject->SetLifeTime(m_fLifeTime);
		pParticleObject->SetMaxParticleN(m_nParticleNumber);
		pParticleObject->SetEmitParticleN(m_nEmitMinParticleNumber + rand() % (m_nEmitMaxParticleNumber - m_nEmitMinParticleNumber));
		pParticleObject->SetPosition(params.xmf3Position);
		pParticleObject->SetParticleType(m_iParticleType);
		//pParticleObject->ChangeTexture(m_pTexture);
	}
}

void RegisterArticulationListener::HandleMessage(const Message& message, const RegisterArticulationParams& params)
{
	m_pScene->RequestRegisterArticulation(params);
}

void PlayerLocationListener::HandleMessage(const Message& message, const PlayerParams& params)
{
	if (message.getType() == MessageType::CHECK_IS_PLAYER_IN_FRONT_OF_MONSTER) {
		CMonster* pMonster = dynamic_cast<CMonster*>(m_pObject);

		pMonster->CheckIsPlayerInFrontOfThis(params.pPlayer->GetPosition());
	}
}

void HitLagComponent::HandleMessage(const Message& message, const PlayerParams& params)
{
	if (!m_bEnable)
		return;

	if (message.getType() == MessageType::UPDATE_HITLAG) {
		CPlayer* pPlayer = (CPlayer*)params.pPlayer;
		if (pPlayer->m_fCurLagTime < m_fMaxLagTime)
		{
			pPlayer->m_fAnimationPlayWeight *= m_fLagScale;
			if (pPlayer->m_fAnimationPlayWeight < pPlayer->m_fAnimationPlayerWeightMin)
				pPlayer->m_fAnimationPlayWeight = pPlayer->m_fAnimationPlayerWeightMin;
		}
		else
			pPlayer->m_fAnimationPlayWeight = 1.0f;
	}
}
