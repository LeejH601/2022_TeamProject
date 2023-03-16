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

struct CameraShakeParams {
    CCamera* pCamera = NULL;
    float fElapsedTime;
};
// Define message listener interface
class IMessageListener {
public:
    virtual void HandleMessage(const Message& message, const PlayerAttackParams& params) {}
    virtual void HandleMessage(const Message& message, const SoundPlayParams& params) {}
    virtual void HandleMessage(const Message& message, const CameraShakeParams& params) {}
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
    void Reset();
    virtual void HandleMessage(const Message& message, const CameraShakeParams& params);
};

struct ListenerInfo {
    IMessageListener* listener;
    void* filterObject;
};

// Define message dispatcher
class CMessageDispatcher {
private:
    std::vector<ListenerInfo> m_listeners[static_cast<int>(MessageType::UPDATE_CAMERA) + 1];
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