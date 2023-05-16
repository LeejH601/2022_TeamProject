#pragma once
#include "stdafx.h"
#include "Global.h"

class CGameObject;
class CCamera;
class CScene;
class CMainTMPScene;
class CTexture;
class CShader;

// Define base message class
class Message {
private:
	MessageType m_type;
public:
	Message(MessageType type) : m_type(type) {}
	MessageType getType() const { return m_type; }
};

struct PlayerParams {
	CGameObject* pPlayer;
};

struct MonsterParams {
    CGameObject* pMonster;
};

struct SoundPlayParams {
	MONSTER_TYPE monster_type = MONSTER_TYPE::NONE;
	SOUND_CATEGORY sound_category = SOUND_CATEGORY::SOUND_SHOCK;
};

struct CameraUpdateParams {
	CCamera* pCamera = NULL;
	CGameObject* pPlayer = NULL;
	float fElapsedTime;
};

struct AnimationCompParams {
	std::vector<std::unique_ptr<CGameObject>>* pObjects;
	float fElapsedTime;
};

struct ParticleCompParams {
	CGameObject* pObject;
	XMFLOAT3 xmf3Position;
};

struct ImpactCompParams {
	CGameObject* pObject;
	XMFLOAT3 xmf3Position;
};

struct ParticleSmokeParams {
	CGameObject* pObject;
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmfDirection;
};

struct ParticleUpDownParams {
	//std::vector<std::unique_ptr<CGameObject>>* pObjects;
	CGameObject* pObject;
	XMFLOAT3 xmf3Position;
	int iIndex;
};

struct ParticleTrailParams {
	CGameObject* pObject;
	XMFLOAT3 xmf3Position;
	int iPlayerAttack; // 플레이어 공격 종류
	float m_fTime = 0.f;
};

struct TrailUpdateParams {
	CGameObject* pObject;
	CShader* pShader;
	std::shared_ptr<CTexture> pTexture;
};

struct CollideParams {
	XMFLOAT3 xmf3CollidePosition;
};

struct OnGroundParams {
    XMFLOAT3 xmf3OnGroundPosition;
};

struct AttackSpriteCompParams {
	CGameObject* pObject;
	XMFLOAT3 xmf3Position;
};

struct TerrainSpriteCompParams {
	CGameObject* pObject;
	XMFLOAT3 xmf3Position;
};

struct RegisterArticulationParams {
	CGameObject* pObject;
	XMFLOAT3 m_force;
};

// Define message listener interface
class IMessageListener {
protected:
	bool m_bEnable = false;
public:
	bool& GetEnable() { return m_bEnable; }
	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	virtual void Reset() { }

    virtual void HandleMessage(const Message& message, const CollideParams& params) {}
    virtual void HandleMessage(const Message& message, const OnGroundParams& params) {}
    virtual void HandleMessage(const Message& message, const PlayerParams& params) {}
    virtual void HandleMessage(const Message& message, const SoundPlayParams& params) {}
    virtual void HandleMessage(const Message& message, const CameraUpdateParams& params) {}
    virtual void HandleMessage(const Message& message, const AnimationCompParams& params) {}
    virtual void HandleMessage(const Message& message, const ParticleCompParams& params) {}
    virtual void HandleMessage(const Message& message, const ImpactCompParams& params) {}
    virtual void HandleMessage(const Message& message, const ParticleSmokeParams& params) {}
    virtual void HandleMessage(const Message& message, const TerrainSpriteCompParams& params) {}
    virtual void HandleMessage(const Message& message, const RegisterArticulationParams& params) {}

	virtual void HandleMessage(const Message& message, const ParticleUpDownParams& params) {}
	virtual void HandleMessage(const Message& message, const ParticleTrailParams& params) {}
	virtual void HandleMessage(const Message& message, const TrailUpdateParams& params){}
};

// Define RegisterAriticulationListner
class RegisterArticulationListener : public IMessageListener {
	CMainTMPScene* m_pScene;
public:
	void SetScene(CMainTMPScene* pScene) { m_pScene = pScene; };
	virtual void HandleMessage(const Message& message, const RegisterArticulationParams& params);
};

// Define Player Attack component
class PlayerAttackListener : public IMessageListener {
private:
	CGameObject* m_pObject;
public:
	void SetObject(CGameObject* pObject) { m_pObject = pObject; }
	virtual void HandleMessage(const Message& message, const PlayerParams& params);
};

// Define Player Attack component
class PlayerLocationListener : public IMessageListener {
private:
	CGameObject* m_pObject;
public:
	void SetObject(CGameObject* pObject) { m_pObject = pObject; }
	virtual void HandleMessage(const Message& message, const PlayerParams& params);
};

// Define CollideListener
class SceneCollideListener : public IMessageListener {
	CScene* m_pScene;
public:
	void SetScene(CScene* pScene) { m_pScene = pScene; }
	virtual void HandleMessage(const Message& message, const CollideParams& params);
};

// Define OnGroundListener
class SceneOnGroundListener : public IMessageListener {
    CScene* m_pScene;
public:
    void SetScene(CScene* pScene) { m_pScene = pScene; }
    virtual void HandleMessage(const Message& message, const OnGroundParams& params);
};

// Define Sound Play component
class SoundPlayComponent : public IMessageListener {
	unsigned int m_nSoundNumber = 0;
	float m_fDelay = 0.0f;
	float m_fVolume = 0.0f;
	MONSTER_TYPE m_mt = MONSTER_TYPE::NONE;
	SOUND_CATEGORY m_sc = SOUND_CATEGORY::SOUND_SHOCK;
public:
	float& GetDelay() { return m_fDelay; }
	float& GetVolume() { return m_fVolume; }
	unsigned int& GetSoundNumber() { return m_nSoundNumber; }
	MONSTER_TYPE GetMonsterCategory() { return m_mt; }
	SOUND_CATEGORY GetSoundCategory() { return m_sc; }

	void SetSoundNumber(unsigned int SoundNumber) { m_nSoundNumber = SoundNumber; }
	void SetDelay(float Delay) { m_fDelay = Delay; }
	void SetVolume(float Volume) { m_fVolume = Volume; }
	void SetMT(MONSTER_TYPE mt) { m_mt = mt; }
	void SetSC(SOUND_CATEGORY sc) { m_sc = sc; }

	virtual void HandleMessage(const Message& message, const SoundPlayParams& params);
};

// Define Shake Movement component
class CameraShakeComponent : public IMessageListener {
	std::uniform_real_distribution<float> urd{ -1.0f, 1.0f };

	float m_ft = 0.0f;;
	float m_fDuration = 0.5f;
	float m_fMagnitude = 0.5f;
	float m_fFrequency = 0.015f;
public:
	float& GetDuration() { return m_fDuration; }
	float& GetMagnitude() { return m_fMagnitude; }
	float& GetFrequency() { return m_fFrequency; }

	void SetDuration(float duration) { m_fDuration = duration; }
	void SetMagnitude(float magnitude) { m_fMagnitude = magnitude; }
	void SetFrequency(float frequency) { m_fFrequency = frequency; }

	void Update(CCamera* pCamera, float fElapsedTime);
	virtual void Reset();
	virtual void HandleMessage(const Message& message, const CameraUpdateParams& params);
};

// Define CameraMove component
class CameraMoveComponent : public IMessageListener {
	float m_fMaxDistance = 2.0f;
	float m_fCurrDistance = 0.0f;
	float m_fMovingTime = 0.02f;
	float m_fSpeed = m_fMaxDistance / m_fMovingTime;;

	float m_fRollBackTime = 0.1f;
	float m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	XMFLOAT3 offset = XMFLOAT3(0.0f, 0.0f, 0.0f);;
public:
	float& GetMaxDistance() { return m_fMaxDistance; }
	float& GetMovingTime() { return m_fMovingTime; }
	float& GetRollBackTime() { return m_fRollBackTime; }

	void SetMaxDistance(float max_distance) { m_fMaxDistance = max_distance; }
	void SetMovingTime(float moving_time) { m_fMovingTime = moving_time; }
	void SetRollBackTime(float rollback_time) { m_fRollBackTime = rollback_time; }

	void Update(CCamera* pCamera, float fElapsedTime, const CameraUpdateParams& params);
	virtual void Reset();
	virtual void HandleMessage(const Message& message, const CameraUpdateParams& params);
};

// Define Zoom In/Out component
class CameraZoomerComponent : public IMessageListener {
	bool m_bIsIN = true;

	float m_fMaxDistance = 2.0f;
	float m_fCurrDistance = 0.0f;
	float m_fMovingTime = 0.01f;;
	float m_fSpeed = m_fMaxDistance / m_fMovingTime;

	float m_fRollBackTime = 0.1f;
	float m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

	XMFLOAT3 offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
public:
	float& GetMaxDistance() { return m_fMaxDistance; }
	float& GetMovingTime() { return m_fMovingTime; }
	float& GetRollBackTime() { return m_fRollBackTime; }
	bool& GetIsIn() { return m_bIsIN; }

	void SetMaxDistance(float max_distance) { m_fMaxDistance = max_distance; }
	void SetMovingTime(float moving_time) { m_fMovingTime = moving_time; }
	void SetRollBackTime(float roolback_time) { m_fRollBackTime = roolback_time; }
	void SetIsIn(bool is_in) { m_bIsIN = is_in; }

	void Update(CCamera* pCamera, float fElapsedTime, const CameraUpdateParams& params);
	virtual void Reset();
	virtual void HandleMessage(const Message& message, const CameraUpdateParams& params);
};

// Define Damage Animation component
class DamageAnimationComponent : public IMessageListener {
	float m_fMaxDistance = 5.0f;
	float m_fSpeed = 100.0f;
public:
	float& GetMaxDistance() { return m_fMaxDistance; }
	float& GetSpeed() { return m_fSpeed; }

	void SetMaxDistance(float max_distance) { m_fMaxDistance = max_distance; }
	void SetSpeed(float speed) { m_fSpeed = speed; }

	virtual void HandleMessage(const Message& message, const AnimationCompParams& params);
};

// Define Shake Animation component
class ShakeAnimationComponent : public IMessageListener {
	float m_fDuration = 1.0f;
	float m_fDistance = 0.15f;
	float m_fFrequency = 0.05f;
public:
	float& GetDuration() { return m_fDuration; }
	float& GetDistance() { return m_fDistance; }
	float& GetFrequency() { return m_fFrequency; }

	void SetDuration(float fDuration) { m_fDuration = fDuration; }
	void SetDistance(float distance) { m_fDistance = distance; }
	void SetFrequency(float frequency) { m_fFrequency = frequency; }

	virtual void HandleMessage(const Message& message, const AnimationCompParams& params);
};

// Define Stun Animation component
class StunAnimationComponent : public IMessageListener {
	float m_fStunTime = 0.5f;
public:
	float& GetStunTime() { return m_fStunTime; }

	void SetStunTime(float stun_time) { m_fStunTime = stun_time; }

	virtual void HandleMessage(const Message& message, const AnimationCompParams& params);
};

// Define HitLag Animation component
class HitLagComponent : public IMessageListener {
    float m_fLagScale = 0.5f;
    float m_fMaxLagTime = 0.5f;
public:
    float& GetMaxLagTime() { return m_fMaxLagTime; }
    float& GetLagScale() { return m_fLagScale; }

    void SetMaxLagTime(float maxlagtime) { m_fMaxLagTime = maxlagtime; }
    void SetLagScale(float lagScale) { m_fLagScale = lagScale; }

    virtual void HandleMessage(const Message& message, const PlayerParams& params);
};

// Define Particle Animation component
enum ParticleType {
    SPHERE_PARTICLE,
    RECOVERY_PARTICLE,
    SMOKE_PARTICLE,
    TRAIL_PARTICLE,
    ATTACK_PARTICLE
};

#define MAX_PARTICLES				10000
class ParticleComponent : public IMessageListener {

    int m_nParticleNumber = MAX_PARTICLES;
    int m_nEmitParticleNumber = 500;
    int m_iParticleType = ParticleType::SPHERE_PARTICLE;
    XMFLOAT2 m_fSize = XMFLOAT2(3.f, 3.f);
    float m_fAlpha = 1.f;
    float m_fLifeTime = 2.f;
    float m_fSpeed = 20.f;
    XMFLOAT3 m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
	int m_iTextureIndex = 0;
	int m_iTextureOffset = 0;
public:
    ParticleComponent();

public:
    //int& GetParticleNumber() { return m_nParticleNumber; }
    int& GetEmitParticleNumber() { return m_nEmitParticleNumber; }
    XMFLOAT2& GetSize() { return m_fSize; }
    float& GetXSize() { return m_fSize.x; }
    float& GetYSize() { return m_fSize.y; }
    float& GetAlpha() { return m_fAlpha; }
    float& GetLifeTime() { return m_fLifeTime; }
    float& GetSpeed() { return m_fSpeed; }
    XMFLOAT3& GetColor() { return m_xmf3Color; }
	int& GetTextureIndex() { return m_iTextureIndex; }
	int& GetTextureOffset() { return m_iTextureOffset; }

    void SetEmitParticleNumber(int iEmitParticleNumber) { m_nEmitParticleNumber = iEmitParticleNumber; }
    void SetParticleNumber(int nParticleNumber) { m_nParticleNumber = nParticleNumber; }
    void SetParticleType(int iParticleType) { m_iParticleType = iParticleType; }
    void SetSize(XMFLOAT2 fSize) { m_fSize = fSize; }
    void SetSizeX(float fSize) { m_fSize.x = fSize; }
    void SetSizeY(float fSize) { m_fSize.y = fSize; }
    void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
    void SetLifeTime(float fLifeTime) { m_fLifeTime = fLifeTime; }
    void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
    void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }
    void SetColorR(float r) { m_xmf3Color.x = r; }
    void SetColorG(float g) { m_xmf3Color.y = g; }
    void SetColorB(float b) { m_xmf3Color.z = b; }
	void SetTextureIndex(int iIndex) { m_iTextureIndex = iIndex; };
	void SetTextureOffset(int iOffset) { m_iTextureOffset = iOffset; }
	virtual void HandleMessage(const Message& message, const ParticleCompParams& params);
};

#define MAX_COLORCURVES 8
class TrailComponent : public IMessageListener
{
	static std::vector<std::string> s_MainTextureNames;
	static std::vector<std::string> s_NoiseTextureNames;

	

	std::shared_ptr<CTexture> m_pTexture;
	int m_nMainTextureIndex = 0;
	int m_nNoiseTextureIndex = 0;

public:
	double m_fR_CurvePoints[MAX_COLORCURVES];
	double m_fG_CurvePoints[MAX_COLORCURVES];
	double m_fB_CurvePoints[MAX_COLORCURVES];
	double m_fColorCurveTimes_R[MAX_COLORCURVES];
	double m_fColorCurveTimes_G[MAX_COLORCURVES];
	double m_fColorCurveTimes_B[MAX_COLORCURVES];
	int m_nCurves;

public:
	TrailComponent();
	static std::vector<std::string>& GetMainTextureNames() { return s_MainTextureNames; };
	static std::vector<std::string>& GetNoiseTexturNames() { return s_NoiseTextureNames; };

	int& GetMainTextureIndex() { return m_nMainTextureIndex; };
	int& GetNoiseTextureIndex() { return m_nNoiseTextureIndex; };
	virtual void HandleMessage(const Message& message, const TrailUpdateParams& params);
};

inline std::vector<std::string> TrailComponent::s_MainTextureNames = { "Image/T_Sword_Slash_11", "Image/T_Sword_Slash_21" };
inline std::vector<std::string> TrailComponent::s_NoiseTextureNames = { "Image/VAP1_Noise_4", "Image/VAP1_Noise_14" };

class SmokeParticleComponent : public IMessageListener {
    int m_nParticleNumber = 20;
    int m_iParticleType = ParticleType::SMOKE_PARTICLE;
    int m_nParticleIndex = 0;
    XMFLOAT2 m_fSize = XMFLOAT2(3.f, 3.f);
    float m_fAlpha = 1.5f;
    float m_fLifeTime = 1.f;
    float m_fSpeed = 10.f;
    int m_iIndex = 0;
    XMFLOAT3 m_xmf3Color = XMFLOAT3(0.191f * 2.f, 0.167f * 2.f, 0.096f * 2.f);
    //XMFLOAT3 m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
public:
	int& GetParticleNumber() { return m_nParticleNumber; }
	int& GetParticleIndex() { return m_nParticleIndex; }
	XMFLOAT2& GetSize() { return m_fSize; }
	float& GetXSize() { return m_fSize.x; }
	float& GetYSize() { return m_fSize.y; }
	float& GetAlpha() { return m_fAlpha; }
	float& GetLifeTime() { return m_fLifeTime; }
	float& GetSpeed() { return m_fSpeed; }
	XMFLOAT3& GetColor() { return m_xmf3Color; }

	void SetParticleNumber(int nParticleNumber) { m_nParticleNumber = nParticleNumber; }
	void SetParticleIndex(int nParticleIndex) { m_nParticleIndex = nParticleIndex; }
	void SetParticleType(int iParticleType) { m_iParticleType = iParticleType; }
	void SetSize(XMFLOAT2 fSize) { m_fSize = fSize; }
	void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
	void SetLifeTime(float fLifeTime) { m_fLifeTime = fLifeTime; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }

	virtual void HandleMessage(const Message& message, const ParticleSmokeParams& params);
};

class UpDownParticleComponent : public IMessageListener {
	int m_nParticleNumber = MAX_PARTICLES;
	int m_iParticleType = ParticleType::RECOVERY_PARTICLE;
	int m_nParticleIndex = 0;
	XMFLOAT2 m_fSize = XMFLOAT2(2.f, 2.f);
	float m_fAlpha = 1.f;
	float m_fLifeTime = 5.f;
	float m_fSpeed = 30.f;
	XMFLOAT3 m_xmf3Color = XMFLOAT3(1.f, 1.f, 0.f);
public:
	int& GetParticleNumber() { return m_nParticleNumber; }
	int& GetParticleIndex() { return m_nParticleIndex; }
	XMFLOAT2& GetSize() { return m_fSize; }
	float& GetXSize() { return m_fSize.x; }
	float& GetYSize() { return m_fSize.y; }
	float& GetAlpha() { return m_fAlpha; }
	float& GetLifeTime() { return m_fLifeTime; }
	float& GetSpeed() { return m_fSpeed; }
	XMFLOAT3& GetColor() { return m_xmf3Color; }

	void SetParticleNumber(int nParticleNumber) { m_nParticleNumber = nParticleNumber; }
	void SetParticleIndex(int nParticleIndex) { m_nParticleIndex = nParticleIndex; }
	void SetParticleType(int iParticleType) { m_iParticleType = iParticleType; }
	void SetSize(XMFLOAT2 fSize) { m_fSize = fSize; }
	void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
	void SetLifeTime(float fLifeTime) { m_fLifeTime = fLifeTime; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }

	virtual void HandleMessage(const Message& message, const ParticleUpDownParams& params);
};

class TrailParticleComponent : public IMessageListener {
	int m_nParticleNumber = MAX_PARTICLES;
	int m_nEmitMinParticleNumber = 2;
	int m_nEmitMaxParticleNumber = 3;
	int m_iParticleType = ParticleType::SPHERE_PARTICLE;
	XMFLOAT2   m_fSize = XMFLOAT2(1.f, 1.f);
	float m_fAlpha = 1.f;
	float m_fLifeTime = 1.f;
	float m_fSpeed = 2.f;
	XMFLOAT3 m_xmf3Color = XMFLOAT3(10.f, 10.f, 10.f);
	int m_iPlayerAttack = 0;
	XMFLOAT3 m_xm3Position = XMFLOAT3(0.f, 0.f, 0.f);

public:
	int& GetParticleNumber() { return m_nParticleNumber; }
	XMFLOAT2& GetSize() { return m_fSize; }
	float& GetAlpha() { return m_fAlpha; }
	float& GetLifeTime() { return m_fLifeTime; }
	float& GetSpeed() { return m_fSpeed; }
	XMFLOAT3& GetColor() { return m_xmf3Color; }

	float& GetXSize() { return m_fSize.x; }
	float& GetYSize() { return m_fSize.y; }

	void SetParticleNumber(int nParticleNumber) { m_nParticleNumber = nParticleNumber; }
	void SetParticleType(int iParticleType) { m_iParticleType = iParticleType; }
	void SetSize(XMFLOAT2 fSize) { m_fSize = fSize; }
	void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
	void SetLifeTime(float fLifeTime) { m_fLifeTime = fLifeTime; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }

	virtual void HandleMessage(const Message& message, const ParticleTrailParams& params);
};

enum TerrainSpriteType {
	TERRAINSPRITE_CROSS_FADE,
	TERRAINSPRITE_FADE_IN,
	TERRAINSPRITE_FADE_OUT,
	TERRAINSPRITETYPE_END
};

class TerrainSpriteComponent : public IMessageListener
{
private:
    int m_nParticleNumber = MAX_PARTICLES;
    int m_nEmitParticleNumber = 500;
    int m_iParticleType = ParticleType::ATTACK_PARTICLE;
    int m_nParticleIndex = 0;
    XMFLOAT3 m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
	float   m_fSpeed = 5.f;
	float   m_fAlpha = 1.f;
	XMFLOAT2   m_fSize = XMFLOAT2(0.5f, 0.5f);
	float   m_fLifeTime = 6.f;
	int m_iTerrainSpriteType = TerrainSpriteType::TERRAINSPRITE_CROSS_FADE;

public:
	virtual void HandleMessage(const Message& message, const TerrainSpriteCompParams& params);
};

// Define Impact Effect component
class ImpactEffectComponent : public IMessageListener {
private:
    int m_nParticleNumber = MAX_PARTICLES;
    int m_nEmitParticleNumber = 1;
    int m_iParticleType = ParticleType::ATTACK_PARTICLE;
    int m_nParticleIndex = 0;
    float m_fLifeTime = 3.f;
	float m_fSpeed = 1.f;
	float m_fAlpha = 1.f;
	XMFLOAT2 m_fSize = XMFLOAT2(3.f, 3.f);
	XMFLOAT3 m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
	XMFLOAT3 m_xmfPosOffset = XMFLOAT3(0.f, 2.f, 0.f);
	int m_iTextureIndex = 0;
	int m_iTextureOffset = 0;
	int m_iTotalRow, m_iTotalColumn;

public:
	int& GetTextureIndex() { return m_iTextureIndex; }
	int GetTextureOffset() { return m_iTextureOffset; }
	float& GetSpeed() { return m_fSpeed; }
	float& GetAlpha() { return m_fAlpha; }
	float& GetLifetime() { return m_fLifeTime; }
	XMFLOAT2& GetSize() { return m_fSize; }
	XMFLOAT3& GetColor() { return m_xmf3Color; }

	float& GetXSize() { return m_fSize.x; }
	float& GetYSize() { return m_fSize.y; }

	void SetTextureIndex(int nTextureIndex) { m_iTextureIndex = nTextureIndex; }
	void SetSize(XMFLOAT2 fSize) { m_fSize = fSize; }
	void SetSizeX(float fSize) { m_fSize.x = fSize; }
	void SetSizeY(float fSize) { m_fSize.y = fSize; }
	void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetColorR(float r) { m_xmf3Color.x = r; }
	void SetColorG(float g) { m_xmf3Color.y = g; }
	void SetColorB(float b) { m_xmf3Color.z = b; }
	void SetTextureOffset(int iOffset) { m_iTextureOffset = iOffset; }
	void SetTotalRowColumn(int iTotalRow, int iTotalColumn);
	virtual void HandleMessage(const Message& message, const ImpactCompParams& params);
};

struct ListenerInfo {
	IMessageListener* listener;
	void* filterObject;
};

// Define message dispatcher
class CMessageDispatcher {
private:
	std::vector<ListenerInfo> m_listeners[static_cast<int>(MessageType::MESSAGE_END)];
public:
	DECLARE_SINGLE(CMessageDispatcher);
	CMessageDispatcher() { }
	~CMessageDispatcher() { }
	template<typename Params>
	void Dispatch_Message(MessageType messageType, Params* params, void* sourceObject);
	void RegisterListener(MessageType messageType, IMessageListener* listener, void* filterObject = nullptr);
};


template<typename Params>
inline void CMessageDispatcher::Dispatch_Message(MessageType messageType, Params* params, void* sourceObject)
{
	auto message = std::make_unique<Message>(messageType);
	auto messageParams = std::make_unique<Params>(*params);

	// Dispatch message to all listeners that match the source object
	for (auto& listenerInfo : m_listeners[static_cast<int>(messageType)]) {
		if (listenerInfo.filterObject == nullptr || listenerInfo.filterObject == sourceObject) {
			listenerInfo.listener->HandleMessage(*message, *messageParams);
		}
	}
}