#pragma once
#include "..\Global\stdafx.h"
#include "..\Global\MessageDispatcher.h"
#include "Object.h"

class CPlayer;

template <class entity_type>
class CState
{
protected:
    std::vector<std::unique_ptr<IMessageListener>> m_pListeners;
public:
	virtual ~CState() {}
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
    virtual IMessageListener* GetHitLagComponent();
    virtual IMessageListener* GetParticleComponent();
    virtual IMessageListener* GetTrailComponent();
    virtual IMessageListener* GetUpDownParticleComponent();
    virtual IMessageListener* GetImpactComponent();
    virtual IMessageListener* GetSlashHitComponent();

	virtual void Enter(entity_type*) = 0;
	virtual void Execute(entity_type*, float) = 0;
    virtual void Animate(entity_type*, float) = 0;
    virtual void OnRootMotion(entity_type*, float) = 0;
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
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
    virtual void Exit(CPlayer* player);
};

class Atk_Player : public CState<CPlayer>
{
public:
    virtual void InitAtkPlayer();
    virtual void SetPlayerRootVel(CPlayer* player);

    virtual void CheckComboAttack(CPlayer* player) = 0;
    virtual void SendCollisionMessage(CPlayer* player) = 0;
    virtual void SpawnTrailParticle(CPlayer* player) = 0;
    virtual void CheckEvasion(CPlayer* player, float holdTime);

    virtual void Enter(CPlayer* player) = 0;
    virtual void Execute(CPlayer* player, float fElapsedTime) = 0;
    virtual void Animate(CPlayer* player, float fElapsedTime) = 0;
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
    virtual void Exit(CPlayer* player);
};

class Atk1_Player : public Atk_Player
{
public:
    DECLARE_SINGLE(Atk1_Player);
    Atk1_Player();
    ~Atk1_Player();

    virtual void CheckComboAttack(CPlayer* player);
    virtual void SendCollisionMessage(CPlayer* player);
    virtual void SpawnTrailParticle(CPlayer* player);

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk2_Player : public Atk_Player
{
public:
    DECLARE_SINGLE(Atk2_Player);
    Atk2_Player();
    ~Atk2_Player();

    virtual void CheckComboAttack(CPlayer* player);
    virtual void SendCollisionMessage(CPlayer* player);
    virtual void SpawnTrailParticle(CPlayer* player);

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk3_Player : public Atk_Player
{
public:
    DECLARE_SINGLE(Atk3_Player);
    Atk3_Player();
    ~Atk3_Player();

    virtual void CheckComboAttack(CPlayer* player);
    virtual void SendCollisionMessage(CPlayer* player);
    virtual void SpawnTrailParticle(CPlayer* player);

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk4_Player : public Atk_Player
{
public:
    DECLARE_SINGLE(Atk4_Player);
    Atk4_Player();
    ~Atk4_Player();

    virtual void CheckComboAttack(CPlayer* player);
    virtual void SendCollisionMessage(CPlayer* player);
    virtual void SpawnTrailParticle(CPlayer* player);

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class Atk5_Player : public Atk_Player
{
public:
    DECLARE_SINGLE(Atk5_Player);
    Atk5_Player();
    ~Atk5_Player();

    virtual void CheckComboAttack(CPlayer* player);
    virtual void SendCollisionMessage(CPlayer* player);
    virtual void SpawnTrailParticle(CPlayer* player);

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void Exit(CPlayer* player);
};

class ChargeStart_Player : public CState<CPlayer>
{
    virtual void SetPlayerRootVel(CPlayer* player);
public:
    DECLARE_SINGLE(ChargeStart_Player);
    ChargeStart_Player();
    ~ChargeStart_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
    virtual void Exit(CPlayer* player);
};

class Charge_Player : public CState<CPlayer>
{
    float m_fChargedTime = 0.0f;
    float m_fChargedEndTime = 2.0f;

    virtual void SetPlayerRootVel(CPlayer* player);
public:
    DECLARE_SINGLE(Charge_Player);
    Charge_Player();
    ~Charge_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
    virtual void Exit(CPlayer* player);
};

class ChargeAttack_Player : public Atk_Player
{
    virtual void SetPlayerRootVel(CPlayer* player);
public:
    DECLARE_SINGLE(ChargeAttack_Player);
    ChargeAttack_Player();
    ~ChargeAttack_Player();

    virtual void CheckComboAttack(CPlayer* player) {};
    virtual void SendCollisionMessage(CPlayer* player) {};
    virtual void SpawnTrailParticle(CPlayer* player) {};

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
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
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
    virtual void Exit(CPlayer* player);

};

class Evasion_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Evasion_Player);
    Evasion_Player();
    ~Evasion_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
    virtual void Exit(CPlayer* player);

    virtual void SetPlayerRootVel(CPlayer* player);
protected:
    XMFLOAT3 m_xmf3VelociyuCache;
    float m_fSpeedCache;
};

class Damaged_Player : public CState<CPlayer>
{
public:
    DECLARE_SINGLE(Damaged_Player);
    Damaged_Player();
    ~Damaged_Player();

    virtual void Enter(CPlayer* player);
    virtual void Execute(CPlayer* player, float fElapsedTime);
    virtual void Animate(CPlayer* player, float fElapsedTime);
    virtual void OnRootMotion(CPlayer* player, float fTimeElapsed);
    virtual void Exit(CPlayer* player);

    virtual void SetPlayerRootVel(CPlayer* player);
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
inline IMessageListener* CState<entity_type>::GetHitLagComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        HitLagComponent* pHitLagComponent = dynamic_cast<HitLagComponent*>(listener.get());
        if (pHitLagComponent) {
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
inline IMessageListener* CState<entity_type>::GetTrailComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        TrailComponent* pTrailComponent = dynamic_cast<TrailComponent*>(listener.get());
        if (pTrailComponent) {
            return true;
        }
        return false;
        });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}


template<class entity_type>
inline IMessageListener* CState<entity_type>::GetUpDownParticleComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        UpDownParticleComponent* pUpDownParticleComponent = dynamic_cast<UpDownParticleComponent*>(listener.get());
        if (pUpDownParticleComponent) {
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


template<class entity_type>
inline IMessageListener* CState<entity_type>::GetSlashHitComponent()
{
    std::vector<std::unique_ptr<IMessageListener>>::iterator p = std::find_if(m_pListeners.begin(), m_pListeners.end(), [](const std::unique_ptr<IMessageListener>& listener) {
        SlashHitComponent* pImpactComponent = dynamic_cast<SlashHitComponent*>(listener.get());
        if (pImpactComponent) {
            return true;
        }
        return false;
        });

    if (p != m_pListeners.end())
        return (*p).get();

    return nullptr;
}

