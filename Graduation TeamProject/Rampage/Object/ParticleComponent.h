#pragma once
#include "..\Global\Component.h"
#include "..\Object\ParticleObject.h"
#include "..\Object\Monster.h"
class CParticleComponent : public CComponent
{
public:
    static std::shared_ptr<CParticleObject> m_pParticleObject;

public:
    DECLARE_SINGLE(CParticleComponent);
    void Set_ParticleComponent(std::shared_ptr<CParticleObject> pParticleComponent);
    void Update();

public:
    float m_fSize = 1.f;
};
