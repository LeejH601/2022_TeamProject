#pragma once
#include "stdafx.h"
#include "Global.h"

class CGameObject;
class CCamera;

// Define base message class
class Message {
private:
    MessageType m_type;
public:
    Message(MessageType type) : m_type(type) {}
    MessageType getType() const { return m_type; }
};

struct PlayerAttackParams {
    CGameObject* pPlayer;
};

struct SoundPlayParams {
    SOUND_CATEGORY sound_category = SOUND_CATEGORY::SOUND_SHOCK;
};

struct CameraUpdateParams {
    CCamera* pCamera = NULL;
    float fElapsedTime;
};

struct AnimationCompParams {
    std::vector<std::unique_ptr<CGameObject>>* pObjects;
    float fElapsedTime;
};

// Define message listener interface
class IMessageListener {
public:
    virtual void HandleMessage(const Message& message, const PlayerAttackParams& params) {}
    virtual void HandleMessage(const Message& message, const SoundPlayParams& params) {}
    virtual void HandleMessage(const Message& message, const CameraUpdateParams& params) {}
    virtual void HandleMessage(const Message& message, const AnimationCompParams& params) {}
};

// Define Player Attack component
class PlayerAttackComponent : public IMessageListener {
private:
    CGameObject* m_pObject;
public:
    void SetObject(CGameObject* pObject) { m_pObject = pObject; }
    virtual void HandleMessage(const Message& message, const PlayerAttackParams& params);
};

// Define Sound Play component
class SoundPlayComponent : public IMessageListener {
    unsigned int m_nSoundNumber = 0;
    float m_fDelay = 0.0f;
    float m_fVolume = 0.0f;
    SOUND_CATEGORY m_sc = SOUND_CATEGORY::SOUND_SHOCK;
public:
    void SetSoundNumber(unsigned int SoundNumber) { m_nSoundNumber = SoundNumber; }
    void SetDelay(float Delay) { m_fDelay = Delay; }
    void SetVolume(float Volume) { m_fVolume = Volume; }
    void SetSC(SOUND_CATEGORY sc) { m_sc = sc; }

    virtual void HandleMessage(const Message& message, const SoundPlayParams& params);
};

// Define Shake Movement component
class CameraShakeComponent : public IMessageListener {
    bool m_bEnable = true;

    std::uniform_real_distribution<float> urd{ -1.0f, 1.0f };

    float m_ft = 0.0f;;
    float m_fDuration;
    float m_fMagnitude;
public:
    void SetDuration(float duration) { m_fDuration = duration; }
    void SetMagnitude(float magnitude) { m_fMagnitude = magnitude; }

    void Update(CCamera* pCamera, float fElapsedTime);
    virtual void HandleMessage(const Message& message, const CameraUpdateParams& params);
};

// Define CameraMove component
class CameraMoveComponent : public IMessageListener {
    bool m_bEnable = true;

    XMFLOAT3 m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

    float m_fMaxDistance = 2.0f;
    float m_fCurrDistance = 0.0f;
    float m_fMovingTime = 0.02f;
    float m_fSpeed = m_fMaxDistance / m_fMovingTime;;

    float m_fRollBackTime = 0.1f;
    float m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

    XMFLOAT3 offset = XMFLOAT3(0.0f, 0.0f, 0.0f);;
public:
    void SetDirection(XMFLOAT3 xmf3direction) { m_xmf3Direction = xmf3direction; }
    void SetMaxDistance(float max_distance) { m_fMaxDistance = max_distance; }
    void SetMovingTime(float moving_time) { m_fMovingTime = moving_time; }
    void SetRollBackTime(float rollback_time) { m_fRollBackTime = rollback_time; }

    void Update(CCamera* pCamera, float fElapsedTime);
    virtual void HandleMessage(const Message& message, const CameraUpdateParams& params);
};

// Define Zoom In/Out component
class CameraZoomerComponent : public IMessageListener {
    bool m_bEnable = true;

    XMFLOAT3 m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

    bool m_bIsIN = true;

    float m_fMaxDistance = 2.0f;
    float m_fCurrDistance = 0.0f;
    float m_fMovingTime = 0.01f;;
    float m_fSpeed = m_fMaxDistance / m_fMovingTime;

    float m_fRollBackTime = 0.1f;
    float m_fBackSpeed = m_fMaxDistance / m_fRollBackTime;

    XMFLOAT3 offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
public:
    void SetDirection(XMFLOAT3 direction) { m_xmf3Direction = direction; }
    void SetMaxDistance(float max_distance) { m_fMaxDistance = max_distance; }
    void SetMovingTime(float moving_time) { m_fMovingTime = moving_time; }
    void SetRollBackTime(float roolback_time) { m_fRollBackTime = roolback_time; }
    void SetIsIn(bool is_in) { m_bIsIN = is_in; }

    void Update(CCamera* pCamera, float fElapsedTime);
    virtual void HandleMessage(const Message& message, const CameraUpdateParams& params);
};

// Define Player Attack component
class DamageAnimationComponent : public IMessageListener {
    bool m_bEnable = true;

    float m_fMaxDistance = 5.0f;
    float m_fSpeed = 100.0f;
public:
    void SetMaxDistance(float max_distance) { m_fMaxDistance = max_distance; }
    void SetSpeed(float speed) { m_fSpeed = speed; }

    virtual void HandleMessage(const Message& message, const AnimationCompParams& params);
};

struct ListenerInfo {
    IMessageListener* listener;
    void* filterObject;
};

// Define message dispatcher
class CMessageDispatcher {
private:
    std::vector<ListenerInfo> m_listeners[static_cast<int>(MessageType::UPDATE_OBJECT) + 1];
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