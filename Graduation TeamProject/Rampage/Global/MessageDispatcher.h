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
    // 어떤 사운드 ???
    // 얼마나 딜레이 ???
    // 얼마나의 크기 ???

    unsigned int m_nSoundNumber = 0;
    float m_fDelay = 0.0f;
    float m_fCurrDelayed = 0.0f;
    float m_fVolume = 0.0f;

    CGameObject* m_pObject;
public:
    virtual void HandleMessage(const Message& message, const SoundPlayParams& params);
};

// Define message dispatcher
class CMessageDispatcher {
private:
    std::vector<IMessageListener*> m_listeners[static_cast<int>(MessageType::PLAYER_ATTACK) + 1];
public:
    DECLARE_SINGLE(CMessageDispatcher);
    CMessageDispatcher() { }
    ~CMessageDispatcher() { }
    template<typename Params>
    void Dispatch_Message(MessageType messageType, Params* params);
    void RegisterListener(MessageType messageType, IMessageListener* listener);
};

template<typename Params>
inline void CMessageDispatcher::Dispatch_Message(MessageType messageType, Params* params)
{
    auto message = std::make_unique<Message>(messageType);
    auto messageParams = std::make_unique<Params>(*params);

    // Dispatch message to all listeners
    for (auto listener : m_listeners[static_cast<int>(messageType)]) {
        listener->HandleMessage(*message, *messageParams);
    }
}
