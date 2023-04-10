#pragma once
#include "stdafx.h"
#include "Global.h"

class CGameObject;
class CCamera;
class CScene;
class CTexture;

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
    XMFLOAT3 xmfShift;
};

struct CollideParams {
    XMFLOAT3 xmf3CollidePosition;
};

struct AttackSpriteCompParams {
    CGameObject* pObject;
    XMFLOAT3 xmf3Position;
};

struct TerrainSpriteCompParams {
    CGameObject* pObject;
    XMFLOAT3 xmf3Position;
};

// Define message listener interface
class IMessageListener {
protected:
    bool m_bEnable = false;
public:
    bool& GetEnable() { return m_bEnable; }
    void SetEnable(bool bEnable) { m_bEnable = bEnable; }

    virtual void HandleMessage(const Message& message, const CollideParams& params) {}
    virtual void HandleMessage(const Message& message, const PlayerParams& params) {}
    virtual void HandleMessage(const Message& message, const SoundPlayParams& params) {}
    virtual void HandleMessage(const Message& message, const CameraUpdateParams& params) {}
    virtual void HandleMessage(const Message& message, const AnimationCompParams& params) {}
    virtual void HandleMessage(const Message& message, const ParticleCompParams& params) {}
    virtual void HandleMessage(const Message& message, const ImpactCompParams& params) {}
    virtual void HandleMessage(const Message& message, const ParticleSmokeParams& params) {}
    virtual void HandleMessage(const Message& message, const TerrainSpriteCompParams& params) {}

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
    float m_fDuration;
    float m_fMagnitude;
    float m_fFrequency = 0.015f;
public:
    float& GetDuration() { return m_fDuration; }
    float& GetMagnitude() { return m_fMagnitude; }
    float& GetFrequency() { return m_fFrequency; }

    void SetDuration(float duration) { m_fDuration = duration; }
    void SetMagnitude(float magnitude) { m_fMagnitude = magnitude; }
    void SetFrequency(float frequency) { m_fFrequency = frequency; }

    void Update(CCamera* pCamera, float fElapsedTime);
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
    float m_fMaxTime = 1.0f;
    float m_fDistance = 0.15f;
    float m_fFrequency = 0.05f;
public:
    float& GetMaxTime() { return m_fMaxTime; }
    float& GetDistance() { return m_fDistance; }
    float& GetFrequency() { return m_fFrequency; }

    void SetMaxTime(float fMaxtime) { m_fMaxTime = fMaxtime; }
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

// Define Particle Animation component
#define SPHERE_PARITLCE 0
#define SMOKE_PARITLCE 1

#define MAX_PARTICLES				250
class ParticleComponent : public IMessageListener {
    int m_nParticleNumber = MAX_PARTICLES;
    int m_iParticleType = SPHERE_PARITLCE;
    int m_nParticleIndex = 0;
    float m_fSize = 1.f;
    float m_fAlpha = 1.f;
    float m_fLifeTime = 5.f;
    float m_fSpeed = 20.f;
    XMFLOAT3 m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
    std::shared_ptr<CTexture> m_pTexture;
public:
    int& GetParticleNumber() { return m_nParticleNumber; }
    int& GetParticleIndex() { return m_nParticleIndex; }
    float& GetSize() { return m_fSize; }
    float& GetAlpha() { return m_fAlpha; }
    float& GetLifeTime() { return m_fLifeTime; }
    float& GetSpeed() { return m_fSpeed; }
    XMFLOAT3& GetColor() { return m_xmf3Color; }
    std::shared_ptr<CTexture>& GetTexture() { return m_pTexture; }

    void SetParticleNumber(int nParticleNumber) { m_nParticleNumber = nParticleNumber; }
    void SetParticleIndex(int nParticleIndex) { m_nParticleIndex = nParticleIndex; }
    void SetParticleType(int iParticleType) { m_iParticleType = iParticleType; }
    void SetSize(float fSize) { m_fSize = fSize; }
    void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
    void SetLifeTime(float fLifeTime) { m_fLifeTime = fLifeTime; }
    void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
    void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }
    void SetColorR(float r) { m_xmf3Color.x = r; }
    void SetColorG(float g) { m_xmf3Color.y = g; }
    void SetColorB(float b) { m_xmf3Color.z = b; }
    void SetParticleTexture(std::shared_ptr<CTexture> pTexture) { m_pTexture = pTexture; }

    virtual void HandleMessage(const Message& message, const ParticleCompParams& params);
};

class SmokeParticleComponent : public IMessageListener {
    int m_nParticleNumber = 4;
    int m_iParticleType = SMOKE_PARITLCE;
    int m_nParticleIndex = 0;
    float m_fSize = 5.f;
    float m_fAlpha = 0.9f;
    float m_fLifeTime = 0.05f;
    float m_fSpeed = 20.f;

    XMFLOAT3 m_xmf3Color = XMFLOAT3(0.55f, 0.5f, 0.51f);
public:
    int& GetParticleNumber() { return m_nParticleNumber; }
    int& GetParticleIndex() { return m_nParticleIndex; }
    float& GetSize() { return m_fSize; }
    float& GetAlpha() { return m_fSize; }
    float& GetLifeTime() { return m_fSize; }
    float& GetSpeed() { return m_fSize; }
    XMFLOAT3& GetColor() { return m_xmf3Color; }

    void SetParticleNumber(int nParticleNumber) { m_nParticleNumber = nParticleNumber; }
    void SetParticleIndex(int nParticleIndex) { m_nParticleIndex = nParticleIndex; }
    void SetParticleType(int iParticleType) { m_iParticleType = iParticleType; }
    void SetSize(float fSize) { m_fSize = fSize; }
    void SetAlpha(float fAlpha) { m_fSize = fAlpha; }
    void SetLifeTime(float fLifeTime) { m_fLifeTime = fLifeTime; }
    void SetSpeed(float fSpeed) { m_fSize = fSpeed; }
    void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }

    virtual void HandleMessage(const Message& message, const ParticleSmokeParams& params);
};

class TerrainSpriteComponent : public IMessageListener
{
private:
    float   m_fSpeed = 5.f;
    float   m_fAlpha = 1.f;
    float   m_fSize = 0.5f;
    bool    m_bAnimation = false;

public:
    void SetTexture(LPCTSTR pszFileName);
    //void SetSpeed(float fSpeed);
    //void SetAlpha(float fAlpha);
    void UpdateData();

public:
    //float& GetSpeed();
    //float& GetAlpha();

    virtual void HandleMessage(const Message& message, const TerrainSpriteCompParams& params);
};

// Define Impact Effect component
class ImpactEffectComponent : public IMessageListener {
    int m_nTextureIndex = 0;
    float m_fSpeed = 5.f;
    float m_fAlpha = 1.f;
    float m_fSize = 0.5f;
    std::shared_ptr<CTexture> m_pTexture;
public:
    int& GetTextureIndex() { return m_nTextureIndex; }
    float& GetSpeed() { return m_fSpeed; }
    float& GetAlpha() { return m_fAlpha; }
    float& GetSize() { return m_fSize; }
    std::shared_ptr<CTexture>& GetTexture() { return m_pTexture; }

    void SetTextureIndex(int nTextureIndex) { m_nTextureIndex = nTextureIndex; }
    void SetSize(float fSize) { m_fSize = fSize; }
    void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
    void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
    void SetImpactTexture(std::shared_ptr<CTexture> pTexture) { m_pTexture = pTexture; }

    virtual void HandleMessage(const Message& message, const ImpactCompParams& params);
};

struct ListenerInfo {
    IMessageListener* listener;
    void* filterObject;
};

// Define message dispatcher
class CMessageDispatcher {
private:
    std::vector<ListenerInfo> m_listeners[static_cast<int>(MessageType::UPDATE_SPRITE) + 1];
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