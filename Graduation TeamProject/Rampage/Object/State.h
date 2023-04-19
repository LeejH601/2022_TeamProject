#pragma once
#include "..\Global\stdafx.h"
#include "..\Global\MessageDispatcher.h"
#include "Object.h"
class CPlayer;


// Define Stun Animation component
class HitLagComponent{
    bool m_bEnable = false;
    float m_fMaxLagTime = 0.5f;
    float m_fCurLagTime = 0.0f;
public:
    bool& GetEnable() { return m_bEnable; }
    float& GetMaxLagTime() { return m_fMaxLagTime; }
    float GetCurLagTime() { return m_fCurLagTime; }

    void SetEnable(bool bEnable) { m_bEnable = bEnable; }
    void SetMaxLagTime(float maxlagtime) { m_fMaxLagTime = maxlagtime; }
    void SetCurLagTime(float lagtime) { m_fCurLagTime =  lagtime; }
};

template <class entity_type>
class CState
{
protected:
    std::vector<std::unique_ptr<IMessageListener>> m_pListeners;
    HitLagComponent m_HitlagComponent;
public:
	virtual ~CState() {}
    HitLagComponent* GetHitLagComponent() { return &m_HitlagComponent; }
    virtual IMessageListener* GetShockSoundComponent();
    virtual IMessageListener* GetShootSoundComponent();
    virtual IMessageListener* GetEffectSoundComponent();
    virtual IMessageListener* GetGoblinMoanComponent();
    virtual IMessageListener* GetOrcMoanComponent();
    virtual IMessageListener* GetSkeletonMoanComponent();
    virtual IMessageListener* GetCameraShakeComponent();
    virtual IMessageListener* GetCameraMoveComponent();
    virtual IMessageListener* GetCameraZoomerComponent();
    virtual IMessageListener* GetDamageAnimationComponent();
    virtual IMessageListener* GetShakeAnimationComponent();
    virtual IMessageListener* GetStunAnimationComponent();
    virtual IMessageListener* GetParticleComponent();
    virtual IMessageListener* GetImpactComponent();

	virtual void Enter(entity_type*) = 0;
	virtual void Execute(entity_type*, float) = 0;
    virtual void Animate(entity_type*, float) = 0;
	virtual void Exit(entity_type*) = 0;
};

class Idle_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Idle_Player);
    Idle_Player();
    ~Idle_Player();
    
    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk1_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Atk1_Player);
    Atk1_Player();
    ~Atk1_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk2_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Atk2_Player);
    Atk2_Player();
    ~Atk2_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk3_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Atk3_Player);
    Atk3_Player();
    ~Atk3_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Run_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Run_Player);
    Run_Player();
    ~Run_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetShockSoundComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        SoundPlayComponent* pSoundPlayComponent = dynamic_cast<SoundPlayComponent*>(listener.get());
        if (pSoundPlayComponent && pSoundPlayComponent->GetSoundCategory() == SOUND_CATEGORY::SOUND_SHOCK){
                return true;
        }
        return false;
    });
    
    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetShootSoundComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        SoundPlayComponent* pSoundPlayComponent = dynamic_cast<SoundPlayComponent*>(listener.get());
        if (pSoundPlayComponent && pSoundPlayComponent->GetSoundCategory() == SOUND_CATEGORY::SOUND_SHOOT) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetEffectSoundComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        SoundPlayComponent* pSoundPlayComponent = dynamic_cast<SoundPlayComponent*>(listener.get());
        if (pSoundPlayComponent && pSoundPlayComponent->GetSoundCategory() == SOUND_CATEGORY::SOUND_EFFECT) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetGoblinMoanComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        SoundPlayComponent* pSoundPlayComponent = dynamic_cast<SoundPlayComponent*>(listener.get());
        if (pSoundPlayComponent && pSoundPlayComponent->GetSoundCategory() == SOUND_CATEGORY::SOUND_VOICE && pSoundPlayComponent->GetMonsterCategory() == MONSTER_TYPE::GOBLIN) {
            return true;
        }
        return false;
        });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetOrcMoanComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        SoundPlayComponent* pSoundPlayComponent = dynamic_cast<SoundPlayComponent*>(listener.get());
        if (pSoundPlayComponent && pSoundPlayComponent->GetSoundCategory() == SOUND_CATEGORY::SOUND_VOICE && pSoundPlayComponent->GetMonsterCategory() == MONSTER_TYPE::ORC) {
            return true;
        }
        return false;
        });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetSkeletonMoanComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        SoundPlayComponent* pSoundPlayComponent = dynamic_cast<SoundPlayComponent*>(listener.get());
        if (pSoundPlayComponent && pSoundPlayComponent->GetSoundCategory() == SOUND_CATEGORY::SOUND_VOICE && pSoundPlayComponent->GetMonsterCategory() == MONSTER_TYPE::SKELETON) {
            return true;
        }
        return false;
        });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetCameraShakeComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        CameraShakeComponent* pCameraShakeComponent = dynamic_cast<CameraShakeComponent*>(listener.get());
        if (pCameraShakeComponent) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetCameraMoveComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        CameraMoveComponent* pCameraMoveComponent = dynamic_cast<CameraMoveComponent*>(listener.get());
        if (pCameraMoveComponent) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetCameraZoomerComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        CameraZoomerComponent* pCameraZoomerComponent = dynamic_cast<CameraZoomerComponent*>(listener.get());
        if (pCameraZoomerComponent) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetDamageAnimationComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        DamageAnimationComponent* pDamageAnimationComponent = dynamic_cast<DamageAnimationComponent*>(listener.get());
        if (pDamageAnimationComponent) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetShakeAnimationComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        ShakeAnimationComponent* pShakeAnimationComponent = dynamic_cast<ShakeAnimationComponent*>(listener.get());
        if (pShakeAnimationComponent) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetStunAnimationComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        StunAnimationComponent* pStunAnimationComponent = dynamic_cast<StunAnimationComponent*>(listener.get());
        if (pStunAnimationComponent) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetParticleComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        ParticleComponent* pParticleComponent = dynamic_cast<ParticleComponent*>(listener.get());
        if (pParticleComponent) {
            return true;
        }
        return false;
    });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

template<class entity_type>
inline IMessageListener* CState<entity_type>::GetImpactComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        ImpactEffectComponent* pImpactComponent = dynamic_cast<ImpactEffectComponent*>(listener.get());
        if (pImpactComponent) {
            return true;
        }
        return false;
        });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}
