#include "MessageDispatcher.h"
#include "Camera.h"
#include "Timer.h"
#include "InitialValue.h"
#include "..\Scene\Scene.h"
#include "..\Object\Object.h"
#include "..\Object\Player.h"
#include "..\Object\Monster.h"
#include "..\Object\MonsterState.h"
#include "..\Object\BillBoardObject.h"
#include "..\Sound\SoundManager.h"
#include "..\Scene\MainScene.h"
#include "..\Scene\SimulatorScene.h"
#include "..\Object\SwordTrailObject.h"
#include "..\Object\UIObject.h"
#include "Logger.h"

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
CameraShakeComponent::CameraShakeComponent()
{
	Reset(false);
}
void CameraShakeComponent::Update(CCamera* pCamera, float fElapsedTime)
{
	if (pCamera->m_bCameraShaking && m_fDuration > m_ft) {
		m_ft += fElapsedTime;

		XMFLOAT3 CameraDir = pCamera->GetRightVector();

		float fShakeDistance = (m_fMagnitude - (m_fMagnitude / m_fDuration) * m_ft) * cos((2 * PI * m_ft) / m_fFrequency);
		float RotateConstant = urd(dre);
		RotateConstant *= XM_PI;

		CameraDir = Vector3::ScalarProduct(CameraDir, MeterToUnit(fShakeDistance), false);
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
void CameraShakeComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;

	m_ft = 0.0f;
	m_fMagnitude = CAMERA_SHAKE_MAGNITUDE_DEFAULT;
	m_fDuration = CAMERA_SHAKE_DURATION_DEFAULT;
	m_fFrequency = CAMERA_SHAKE_FREQUENCY_DEFAULT;
}
void CameraShakeComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (!m_bEnable)
		return;

	Update(params.pCamera, params.fElapsedTime);
}
CameraZoomerComponent::CameraZoomerComponent()
{
	Reset(false);
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
		offset = Vector3::Add(offset, Vector3::ScalarProduct(xmf3ZoomDirection, MeterToUnit(length * ZoomConstant), false));

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
		offset = Vector3::Add(offset, Vector3::ScalarProduct(xmf3ZoomDirection, MeterToUnit(-length * ZoomConstant), false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}
void CameraZoomerComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;

	m_fCurrDistance = 0.f;
	offset.x = 0.0f;
	offset.y = 0.0f;
	offset.z = 0.0f;

	m_fMaxDistance = CAMERA_ZOOMOUT_DISTANCE_DEFAULT;
	m_fMovingTime = CAMERA_ZOOMOUT_TIME_DEFAULT;
	m_fRollBackTime = CAMERA_ZOOMOUT_ROLLBACKTIME_DEFAULT;
	m_bIsIN = CAMERA_ZOOMOUT_ISIN_DEFAULT;
}
void CameraZoomerComponent::HandleMessage(const Message& message, const CameraUpdateParams& params)
{
	if (!m_bEnable)
		return;
	Update(params.pCamera, params.fElapsedTime, params);
}
CameraMoveComponent::CameraMoveComponent()
{
	Reset(false);
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

		offset = Vector3::Add(offset, Vector3::ScalarProduct(((CPlayer*)(params.pPlayer))->GetATKDirection(), MeterToUnit(length), false));

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

		offset = Vector3::Add(offset, Vector3::ScalarProduct(((CPlayer*)(params.pPlayer))->GetATKDirection(), -MeterToUnit(length), false));

		pCamera->m_xmf3CalculatedPosition = Vector3::Add(pCamera->m_xmf3CalculatedPosition, offset);
	}
}
void CameraMoveComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_fMaxDistance = CAMERA_MOVE_DISTANCE_DEFAULT;
	m_fMovingTime = CAMERA_MOVE_TIME_DEFAULT;
	m_fRollBackTime = CAMERA_MOVE_ROLLBACKTIME_DEFAULT;

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
DamageAnimationComponent::DamageAnimationComponent()
{
	Reset(false);
}
void DamageAnimationComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_fMaxDistance = DAMAGE_ANIMATION_DISTANCE_DEFAULT;
	m_fSpeed = DAMAGE_ANIMATION_SPEED_DEFAULT;
}
void DamageAnimationComponent::HandleMessage(const Message& message, const AnimationCompParams& params)
{
}
ShakeAnimationComponent::ShakeAnimationComponent()
{
	Reset(false);
}
void ShakeAnimationComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_fDuration = SHAKE_ANIMATION_DURATION_DEFAULT;
	m_fDistance = SHAKE_ANIMATION_DISTANCE_DEFAULT;
	m_fFrequency = SHAKE_ANIMATION_FREQUENCY_DEFAULT;
}
void ShakeAnimationComponent::HandleMessage(const Message& message, const AnimationCompParams& params)
{
}
StunAnimationComponent::StunAnimationComponent()
{
	Reset(false);
}
void StunAnimationComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_fStunTime = STUN_ANIMATION_STUNTIME_DEFAULT;
}
void StunAnimationComponent::HandleMessage(const Message& message, const AnimationCompParams& params)
{
}
ParticleComponent::ParticleComponent()
{
	Reset(false);
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
		pParticle->SetTotalRowColumn(m_iTotalRow, m_iTotalColumn);
		pParticle->SetColor(m_xmf3Color);
		pParticle->SetSpeed(m_fSpeed);
		pParticle->SetLifeTime(m_fLifeTime);
		pParticle->SetMaxParticleN(m_nParticleNumber);
		pParticle->SetEmitParticleN(m_nEmitParticleNumber);
		pParticle->SetPosition(params.xmf3Position);
		pParticle->SetParticleType(m_iParticleType);
		pParticle->SetFieldSpeed(m_fFieldSpeed);
		pParticle->SetNoiseStrength(m_fNoiseStrength);
		pParticle->SetFieldMainDirection(m_xmf3FieldMainDirection);
		pParticle->SetProgressionRate(m_fProgressionRate);
		pParticle->SetLengthScale(m_fLengthScale);
		pParticle->SetTextureIndex(m_iTextureIndex + CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture));
		pParticle->SetEmissive(m_fEmissive);
		pParticle->SetRotateFactor(m_bSimulateRotate);
		pParticle->SetScaleFactor(m_bSimulateRotate);
		//pParticle->SetEmitAxis()
		pParticle->EmitParticle(10);
	}
}
void ParticleComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;

	m_nParticleNumber = MAX_PARTICLES;
	m_iParticleType = ParticleType::SPHERE_PARTICLE;
	m_fFieldSpeed = 1.0f;
	m_xmf3FieldMainDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fProgressionRate = 1.0f;
	m_fLengthScale = 1.0f;
	m_bScaleFlag = false;
	m_iTotalRow = 1;
	m_iTotalColumn = 1;

	if (CSimulatorScene::GetInst()->GetTextureManager()) m_iTextureOffset = CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture);

	m_iTextureIndex = PARTICLE_TEXTURE_INDEX_DEFAULT;
	m_fSize = XMFLOAT2(PARTICLE_SIZEX_DEFAULT, PARTICLE_SIZEY_DEFAULT);
	m_fAlpha = PARTICLE_ALPHA_DEFAULT;
	m_fLifeTime = PARTICLE_LIFETIME_DEFAULT;
	m_nEmitParticleNumber = PARTICLE_COUNT_DEFAULT;
	m_fSpeed = PARTICLE_SPEED_DEFAULT;
	m_xmf3Color = XMFLOAT3(PARTICLE_COLOR_R_DEFAULT, PARTICLE_COLOR_G_DEFAULT, PARTICLE_COLOR_B_DEFAULT);
	m_fEmissive = PARTICLE_EMISSIVE_DEFAULT;
	m_bSimulateRotate = PARTICLE_SIMULATEROTATE_DEFAULT;
	m_fNoiseStrength = PARTICLE_NOISESTRENGTH_DEFAULT;
}
ImpactEffectComponent::ImpactEffectComponent()
{
	Reset(false);
}
void ImpactEffectComponent::SetTotalRowColumn(int iTotalRow, int iTotalColumn)
{
	m_iTotalRow = iTotalRow;
	m_iTotalColumn = iTotalColumn;
}
void ImpactEffectComponent::HandleMessage(const Message& message, const ImpactCompParams& params)
{
	if (!m_bEnable)
		return;

	CParticleObject* pMultiSpriteParticle = dynamic_cast<CParticleObject*>(params.pObject);

	if (pMultiSpriteParticle)
	{
		pMultiSpriteParticle->SetParticleType(m_iParticleType);
		pMultiSpriteParticle->SetEmit(true);
		pMultiSpriteParticle->SetTotalRowColumn(m_iTotalRow, m_iTotalColumn);
		pMultiSpriteParticle->SetSize(m_fSize);
		pMultiSpriteParticle->SetDirection(XMFLOAT3(0.f, 0.f, 0.f));
		pMultiSpriteParticle->SetStartAlpha(m_fAlpha);
		pMultiSpriteParticle->SetColor(m_xmf3Color);
		pMultiSpriteParticle->SetLifeTime(m_fLifeTime);
		pMultiSpriteParticle->SetAnimation(true);
		pMultiSpriteParticle->SetEmitParticleN(m_nEmitParticleNumber);
		pMultiSpriteParticle->SetMaxParticleN(m_nParticleNumber);
		pMultiSpriteParticle->SetPosition(params.xmf3Position);
		pMultiSpriteParticle->SetTextureIndex(m_iTextureIndex + m_iTextureOffset);
		pMultiSpriteParticle->SetEmissive(m_fEmissive);
		pMultiSpriteParticle->EmitParticle(7);
	}
	CLogger::GetInst()->Log(std::string("MultiSprite HandleMessge Called"));
}
void ImpactEffectComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_nParticleNumber = MAX_PARTICLES;
	m_nEmitParticleNumber = 1;
	m_iParticleType = ParticleType::ATTACK_PARTICLE;
	m_fSpeed = 1.f;
	m_nParticleIndex = 0;
	m_xmfPosOffset = XMFLOAT3(0.f, 2.f, 0.f);

	m_iTextureOffset = CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::BillBoardTexture);

	m_iTextureIndex = IMPACT_TEXTURE_INDEX_DEFAULT;
	m_fLifeTime = IMPACT_LIFETIME_DEFAULT;
	m_fAlpha = IMPACT_ALPHA_DEFAULT;
	m_fSize = XMFLOAT2(IMPACT_SIZEX_DEFAULT, IMPACT_SIZEY_DEFAULT);
	m_xmf3Color = XMFLOAT3(IMPACT_COLOR_R_DEFAULT, IMPACT_COLOR_G_DEFAULT, IMPACT_COLOR_B_DEFAULT);
	m_fEmissive = IMPACT_EMISSIVE_DEFAULT;

	m_bSimulateRotate = false;

	std::shared_ptr<CTexture> pTexture = CSimulatorScene::GetInst()->GetTextureManager()->GetTexture(TextureType::BillBoardTexture);
	SetTotalRowColumn(pTexture->GetRow(IMPACT_TEXTURE_INDEX_DEFAULT), pTexture->GetColumn(IMPACT_TEXTURE_INDEX_DEFAULT));
}
void SceneCollideListener::HandleMessage(const Message& message, const CollideParams& params)
{
	m_pScene->HandleCollision(params);
}

void TerrainSpriteComponent::HandleMessage(const Message& message, const TerrainSpriteCompParams& params)
{
	//CTerrainSpriteObject* pSpriteObject = dynamic_cast<CTerrainSpriteObject*>(params.pObject);
	//if (!m_bEnable)
	//	return;

	CParticleObject* pMultiSpriteParticle = dynamic_cast<CParticleObject*>(params.pObject);

	if (pMultiSpriteParticle)
	{
		pMultiSpriteParticle->SetParticleType(m_iParticleType);
		pMultiSpriteParticle->SetEmit(true);
		pMultiSpriteParticle->SetTotalRowColumn(1, 1);
		pMultiSpriteParticle->SetSize(m_fSize);
		pMultiSpriteParticle->SetDirection(XMFLOAT3(0.f, 0.f, 0.f));
		pMultiSpriteParticle->SetStartAlpha(m_fAlpha);
		pMultiSpriteParticle->SetColor(m_xmf3Color);
		pMultiSpriteParticle->SetLifeTime(m_fLifeTime);
		pMultiSpriteParticle->SetAnimation(true);
		pMultiSpriteParticle->SetEmitParticleN(m_nEmitParticleNumber);
		pMultiSpriteParticle->SetMaxParticleN(m_nParticleNumber);
		pMultiSpriteParticle->SetPosition(params.xmf3Position);
		//pMultiSpriteParticle->SetTextureIndex(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::UniformTexture) + 1);
		pMultiSpriteParticle->SetEmissive(10.0f);
		pMultiSpriteParticle->EmitParticle(8);
	}


	//CParticleObject* pMultiSprite = dynamic_cast<CParticleObject*>(params.pObject);

	//if (pMultiSprite)
	//{
	//	pMultiSprite->SetParticleType(ParticleType::ATTACK_PARTICLE);
	//	pMultiSprite->SetEmit(true);
	//	pMultiSprite->SetSize(m_fSize);
	//	pMultiSprite->SetTotalRowColumn(1, 1);
	//	pMultiSprite->SetDirection(XMFLOAT3(0.f, 0.f, 0.f));
	//	pMultiSprite->SetStartAlpha(m_fAlpha);
	//	pMultiSprite->SetColor(m_xmf3Color);
	//	pMultiSprite->SetSpeed(m_fSpeed);
	//	pMultiSprite->SetLifeTime(m_fLifeTime);
	//	//pParticle->SetMaxParticleN(m_nParticleNumber);
	//	pMultiSprite->SetEmitParticleN(m_nEmitParticleNumber);
	//	pMultiSprite->SetMaxParticleN(m_nParticleNumber);
	//	pMultiSprite->SetPosition(params.xmf3Position);
	//	pMultiSprite->EmitParticle(0);
	//}

	//if (pSpriteObject)
	//{
	//	pSpriteObject->SetType(TerrainSpriteType::TERRAINSPRITE_CROSS_FADE);
	//	pSpriteObject->SetEnable(true);
	//	pSpriteObject->SetPosition(params.xmf3Position);
	//	pSpriteObject->SetLifeTime(m_fLifeTime);
	//	pSpriteObject->SetStart(true);
	//	pSpriteObject->SetStartAlpha(1.f);
	//}
}

void SmokeParticleComponent::HandleMessage(const Message& message, const ParticleSmokeParams& params)
{
	CParticleObject* pParticleObject = dynamic_cast<CParticleObject*>(params.pObject);

	if (pParticleObject)
	{
		pParticleObject->SetParticleType(m_iParticleType);
		pParticleObject->SetEmit(true);
		pParticleObject->SetDirection(params.xmfDirection);
		pParticleObject->SetSize(m_fSize);
		pParticleObject->SetStartAlpha(m_fAlpha);
		pParticleObject->SetColor(m_xmf3Color);
		pParticleObject->SetSpeed(m_fSpeed);
		pParticleObject->SetLifeTime(m_fLifeTime);
		pParticleObject->SetMaxParticleN(m_nParticleNumber);
		pParticleObject->SetPosition(params.xmf3Position);
		pParticleObject->EmitParticle(2);
	}
}

void UpDownParticleComponent::HandleMessage(const Message& message, const ParticleUpDownParams& params)
{
	CParticleObject* pParticleObject = dynamic_cast<CParticleObject*>(params.pObject);

	if (pParticleObject)
	{
		pParticleObject->SetParticleType(m_iParticleType);
		pParticleObject->SetEmit(true);
		pParticleObject->SetPosition(params.xmf3Position);
		pParticleObject->SetSize(m_fSize);
		pParticleObject->SetStartAlpha(m_fAlpha);
		pParticleObject->SetColor(m_xmf3Color);
		pParticleObject->SetLifeTime(m_fLifeTime);
		pParticleObject->SetSpeed(m_fSpeed);
		pParticleObject->SetDirection(XMFLOAT3(0.f, 1.f, 0.f));
	}
}

TrailParticleComponent::TrailParticleComponent()
{
}
void TrailParticleComponent::HandleMessage(const Message& message, const ParticleTrailParams& params)
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
		pParticle->SetTextureIndex(m_iTextureIndex + CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture));
		pParticle->SetEmissive(m_fEmissive);
		pParticle->SetRotateFactor(m_bSimulateRotate);
		pParticle->SetScaleFactor(m_bSimulateRotate);
		//pParticle->SetEmitAxis()
		pParticle->EmitParticle(3);
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
		pMonster->CheckIsPlayerInFrontOfThis(params.pPlayer);
	}
}
HitLagComponent::HitLagComponent()
{
	Reset(false);
}
void HitLagComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_fMinTimeScale = HIT_LAG_MAXTIME_DEFAULT;
	m_fLagScale = HIT_LAG_SCALEWEIGHT_DEFAULT;
	m_fDuration = HIT_LAG_DURATION_DEFAULT;
}
void HitLagComponent::HandleMessage(const Message& message, const PlayerParams& params)
{
	if (!m_bEnable)
		return;
}
TrailComponent::TrailComponent()
{
	Reset(false);
}

void TrailComponent::HandleMessage(const Message& message, const TrailUpdateParams& params)
{
	if (message.getType() == MessageType::UPDATE_SWORDTRAIL) {
		CSwordTrailObject* pTrail = dynamic_cast<CSwordTrailObject*>(params.pObject);

		pTrail->m_nCurves = this->m_nCurves;
		for (int i = 0; i < this->m_nCurves; ++i) {
			pTrail->m_fR_CurvePoints[i] = this->m_fR_CurvePoints[i];
			pTrail->m_fG_CurvePoints[i] = this->m_fG_CurvePoints[i];
			pTrail->m_fB_CurvePoints[i] = this->m_fB_CurvePoints[i];
			pTrail->m_fColorCurveTimes[i] = this->m_fColorCurveTimes_R[i];
		}
		pTrail->m_fEmissiveFactor = this->m_fEmissiveFactor;
		pTrail->m_eTrailUpdateMethod = GetEnable() ? TRAIL_UPDATE_METHOD::UPDATE_NEW_CONTROL_POINT : TRAIL_UPDATE_METHOD::NON_UPDATE_NEW_CONTROL_POINT;

		pTrail->SetTextureIndex(m_nMainTextureIndex + m_nMainTextureOffset);
		pTrail->SetNoiseTextureIndex(m_nNoiseTextureIndex + m_nNoiseTextureOffset);
	}
}
void TrailComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_nMainTextureIndex = TRAIL_TEXTURE_INDEX_DEFAULT;
	m_fEmissiveFactor = TRAIL_EMISSIVE_DEFAULT;
	this->m_fR_CurvePoints[0] = 0.0f; this->m_fR_CurvePoints[1] = 0.329105049f; this->m_fR_CurvePoints[2] = 0.345385194f;  this->m_fR_CurvePoints[3] = 1.89199996;
	this->m_fG_CurvePoints[0] = 0.0f; this->m_fG_CurvePoints[1] = 0.459346294f; this->m_fG_CurvePoints[2] = 0.198863804f;  this->m_fG_CurvePoints[3] = 0.595000029f;
	this->m_fB_CurvePoints[0] = 0.0f; this->m_fB_CurvePoints[1] = 0.256999999f; this->m_fB_CurvePoints[2] = 0.679128408f;  this->m_fB_CurvePoints[3] = 0.f;
	this->m_fColorCurveTimes_R[0] = 0.0f; this->m_fColorCurveTimes_R[1] = 0.3; this->m_fColorCurveTimes_R[2] = 0.37;  this->m_fColorCurveTimes_R[3] = 1.0;
	this->m_fColorCurveTimes_G[0] = 0.0f; this->m_fColorCurveTimes_G[1] = 0.3; this->m_fColorCurveTimes_G[2] = 0.37;  this->m_fColorCurveTimes_G[3] = 1.0;
	this->m_fColorCurveTimes_B[0] = 0.0f; this->m_fColorCurveTimes_B[1] = 0.3; this->m_fColorCurveTimes_B[2] = 0.37;  this->m_fColorCurveTimes_B[3] = 1.0;
	this->m_nCurves = TRAIL_CURVES_DEFAULT;
	m_nNoiseTextureIndex = TRAIL_NOISETEXTURE_INDEX_DEFAULT;

	this->SetMainTextureOffset(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::TrailBaseTexture));
	this->SetNoiseTextureOffset(CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::TrailNoiseTexture));
}
void SceneOnGroundListener::HandleMessage(const Message& message, const OnGroundParams& params)
{
	m_pScene->HandleOnGround(params);
}

void DamageListener::HandleMessage(const Message& message, const DamageParams& params)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pObject);
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pObject);

	if (pMonster)
	{
		if (!pMonster->m_bEnable)
			return;

		CPlayer* pAttacker = (CPlayer*)params.pAttacker;
		pMonster->HandleDamage(pAttacker, params.fDamage);
	}

	if (pPlayer)
	{
		if (!pPlayer->m_bEnable)
			return;

		CMonster* pAttacker = (CMonster*)params.pAttacker;
		pPlayer->HandleDamage(pAttacker, params.fDamage);
	}
}

void UpdateDynamicTimeScaleListener::HandleMessage(const Message& message, const TimerParams& params)
{
	m_pTimer->SetDynamicTimeScale(params.fDynamicTimeScale, params.fDuration, params.fMinTimeScale);
}

SlashHitComponent::SlashHitComponent()
{
	Reset(false);
}
void SlashHitComponent::Reset(bool bEnable)
{
	m_bEnable = bEnable;
	m_iTextureOffset = 5;
	m_bSimulateRotate = true;
	m_bScaleFlag = false;
	m_nEmitParticleNumber = 1;

	m_iTextureIndex = SLASH_TEXTURE_INDEX_DEFAULT;
	m_fSize.x = SLASH_SIZEX_DEFAULT; m_fSize.y = SLASH_SIZEY_DEFAULT;
	m_fAlpha = SLASH_ALPHA_DEFAULT;
	m_fLifeTime = SLASH_LIFETIME_DEFAULT;
	m_xmf3Color = XMFLOAT3(SLASH_COLOR_R_DEFAULT, SLASH_COLOR_G_DEFAULT, SLASH_COLOR_B_DEFAULT);
	m_fEmissive = SLASH_EMISSIVE_DEFAULT;
}
void SlashHitComponent::HandleMessage(const Message& message, const ParticleCompParams& params)
{
	if (!m_bEnable)
		return;

	CParticleObject* pParticle = dynamic_cast<CParticleObject*>(params.pObject);

	if (pParticle)
	{
		pParticle->SetEmit(true);
		pParticle->SetSize(m_fSize);
		pParticle->SetStartAlpha(m_fAlpha);
		pParticle->SetTotalRowColumn(m_iTotalRow, m_iTotalColumn);
		pParticle->SetColor(m_xmf3Color);
		pParticle->SetSpeed(m_fSpeed);
		pParticle->SetLifeTime(m_fLifeTime);
		pParticle->SetMaxParticleN(m_nParticleNumber);
		pParticle->SetEmitParticleN(m_nEmitParticleNumber);
		pParticle->SetPosition(params.xmf3Position);
		pParticle->SetParticleType(m_iParticleType);
		pParticle->SetTextureIndex(m_iTextureIndex + CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture));
		pParticle->SetEmissive(m_fEmissive);
		pParticle->SetRotateFactor(m_bSimulateRotate);
		pParticle->SetScaleFactor(m_bScaleFlag);
		pParticle->EmitParticle(6);
	}
}

void RegisterArticulationSleepListener::HandleMessage(const Message& message, const RegisterArticulationSleepParams& params)
{
	m_pScene->RequestSleepArticulation(params);
}

void MonsterAttackListener::HandleMessage(const Message& message, const MonsterParams& params)
{
	if (message.getType() == MessageType::MONSTER_ATTACK)
	{
		params.pMonster->CheckCollision(m_pPlayer);
	}
}

void CinematicAllUpdatedListener::HandleMessage(const Message& message, const PlayerParams& params)
{
	if (message.getType() == MessageType::CINEMATIC_ALL_UPDATED)
		m_pScene->AdvanceStage();
}

void MonsterDeadListener::HandleMessage(const Message& message, const MonsterParams& params)
{
	if (message.getType() == MessageType::MONSTER_DEAD)
		m_pScene->HandleMonsterDeadMessage();
}

void AllyDamagedListener::HandleMessage(const Message& message, const PlayerParams& params)
{
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pObject);

	if (pMonster->m_bEnable)
		pMonster->HandleAllyDamagedMessage(params.pPlayer);
}

void PlayerDeadListener::HandleMessage(const Message& message, const PlayerParams& params)
{
	if (message.getType() == MessageType::PLAYER_DEAD)
		m_pScene->HandlePlayerDeadMessage();
}

ShieldHitComponent::ShieldHitComponent()
{
	m_fFieldSpeed = 1.0f;
	m_fNoiseStrength = 1.0f;;
	m_xmf3FieldMainDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fProgressionRate = 1.0f;
	m_fLengthScale = 1.0f;

	m_bSimulateRotate = false;

}

void ShieldHitComponent::HandleMessage(const Message& message, const ParticleCompParams& params)
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
		/*pParticle->SetFieldSpeed(m_fFieldSpeed);
		pParticle->SetNoiseStrength(m_fNoiseStrength);
		pParticle->SetFieldMainDirection(m_xmf3FieldMainDirection);
		pParticle->SetProgressionRate(m_fProgressionRate);
		pParticle->SetLengthScale(m_fLengthScale);*/
		pParticle->SetTextureIndex(m_iTextureIndex + CSimulatorScene::GetInst()->GetTextureManager()->GetTextureOffset(TextureType::ParticleTexture));
		pParticle->SetEmissive(m_fEmissive);
		pParticle->SetRotateFactor(m_bSimulateRotate);
		pParticle->SetScaleFactor(m_bSimulateRotate);
		pParticle->EmitParticle(m_iParticleType);
	}
}

PotionRemainUpdateComponent::PotionRemainUpdateComponent()
{
}

void PotionRemainUpdateComponent::HandleMessage(const Message& message, const UpdateNumParams& params)
{
	if (!m_bEnable)
		return;

	if (m_pNumUIObject) {
		dynamic_cast<CNumberObject*>(m_pNumUIObject)->UpdateNumber(params.num);
	}
}

void SpecialMoveListener::HandleMessage(const Message& message, const SpecialMoveUpdateParams& params)
{
	if (!m_bEnable)
		return;

	dynamic_cast<CBreakScreenEffectShader*>(m_pBreakScreenShader)->SetEnable(params.bEnable);
}

void SpecialMoveDamageListener::HandleMessage(const Message& message, const PlayerParams& params)
{
	static std::random_device rd;
	static std::default_random_engine dre(rd());
	static std::uniform_real_distribution<float> urd(-1, 1);

	CKnightPlayer* pPlayer = dynamic_cast<CKnightPlayer*>(params.pPlayer);

	for (std::unique_ptr<CGameObject>& obj : *m_ppMonsters) {
		CMonster* pMonster = dynamic_cast<CMonster*>(obj.get());

		if (pMonster->m_bEnable) {
			pMonster->m_xmf3HitterVec = Vector3::Normalize(Vector3::Subtract(pMonster->GetPosition(), pPlayer->GetPosition()));
			pMonster->HandleDamage(pPlayer, 1000.0f);
			pPlayer->UpdateCombo();

			//pMonster->UpdateTransform(NULL);
			BoundingOrientedBox* TargetBoundingBox = pMonster->GetBoundingBox();
			//pPlayer->SetTargetPosition(*TargetBoundingBox);
			pPlayer->m_xmf3AtkDirection = Vector3::Normalize(XMFLOAT3(urd(dre), urd(dre), urd(dre)));

			CollideParams colParam;
			colParam.xmf3CollidePosition = TargetBoundingBox->Center;
			CMessageDispatcher::GetInst()->Dispatch_Message(MessageType::COLLISION, &colParam, nullptr);

			/*ParticleCompParams particle_comp_params;
			particle_comp_params.pObject = dynamic_cast<CParticleObject*>(m_pParticleObj);
			particle_comp_params.xmf3Position = colParam.xmf3CollidePosition;
			dynamic_cast<CParticleObject*>(particle_comp_params.pObject)->SetEmitAxis(((CPlayer*)pPlayer)->m_xmf3AtkDirection);
			CMessageDispatcher::GetInst()->Dispatch_Message<ParticleCompParams>(MessageType::UPDATE_PARTICLE, &particle_comp_params, ((CPlayer*)pPlayer)->m_pStateMachine->GetCurrentState());
			CMessageDispatcher::GetInst()->Dispatch_Message<ParticleCompParams>(MessageType::UPDATE_SLASHHITPARTICLE, &particle_comp_params, ((CPlayer*)pPlayer)->m_pStateMachine->GetCurrentState());
			
			ImpactCompParams impact_comp_params;
			impact_comp_params.pObject = m_pImpactObj;
			impact_comp_params.xmf3Position = colParam.xmf3CollidePosition;
			CMessageDispatcher::GetInst()->Dispatch_Message<ImpactCompParams>(MessageType::UPDATE_BILLBOARD, &impact_comp_params, ((CPlayer*)pPlayer)->m_pStateMachine->GetCurrentState());
*/

			SoundPlayParams SoundPlayParam;
			SoundPlayParam.sound_category = SOUND_CATEGORY::SOUND_SHOCK;
			CMessageDispatcher::GetInst()->Dispatch_Message<SoundPlayParams>(MessageType::PLAY_SOUND, &SoundPlayParam, pPlayer->m_pStateMachine->GetCurrentState());


		}


	}
	pPlayer->SetMonsterAttack(true);
	dynamic_cast<CMainTMPScene*>(m_pScene)->m_bPlayCutScene = false;
	dynamic_cast<CMainTMPScene*>(m_pScene)->returnMainCamera();
}
void AutoResetListener::HandleMessage(const Message& message, const MonsterParams& params)
{
	if (message.getType() == MessageType::AUTO_RESET) {
		if (((CSimulatorScene*)m_pSimulatorScene)->GetAutoReset())
			((CSimulatorScene*)m_pSimulatorScene)->SpawnAndSetMonster();
	}
}