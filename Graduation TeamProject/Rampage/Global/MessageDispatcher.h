#pragma once
#include "stdafx.h"
#include "Global.h"

class CGameObject;
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
    SOUND_PLAY_TYPE spt = SOUND_PLAY_TYPE::SOUND_PT_NONE;
};

// Define message listener interface
class IMessageListener {
public:
    virtual void HandleMessage(const Message& message, const PlayerAttackParams& params) {}
    virtual void HandleMessage(const Message& message, const SoundPlayParams& params) {}
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
    SOUND_PLAY_TYPE m_spt = SOUND_PLAY_TYPE::SOUND_PT_NONE;
public:
    void SetSoundNumber(unsigned int SoundNumber) { m_nSoundNumber = SoundNumber; }
    void SetDelay(float Delay) { m_fDelay = Delay; }
    void SetVolume(float Volume) { m_fVolume = Volume; }
    void SetSC(SOUND_CATEGORY sc) { m_sc = sc; }
    void SetSPT(SOUND_PLAY_TYPE spt) { m_spt = spt; }

    virtual void HandleMessage(const Message& message, const SoundPlayParams& params);
};

struct ListenerInfo {
    IMessageListener* listener;
    void* filterObject;
};

// Define message dispatcher
class CMessageDispatcher {
private:
    std::vector<ListenerInfo> m_listeners[static_cast<int>(MessageType::PLAY_SOUND) + 1];
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