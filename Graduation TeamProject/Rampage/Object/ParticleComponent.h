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
    void SetTexture(int iIndex, LPCTSTR pszFileName);
    void Set_ParticleComponent(std::shared_ptr<CParticleObject> pParticleComponent);
    int& GetParticleNumber();
    void Update();

public:
    int m_nParticleNumber = MAX_PARTICLES;
    float m_fSize = 1.f;
    float m_fAlpha = 1.f;
    float m_fLifeTime = 5.f;
    XMFLOAT3 m_f3Color = XMFLOAT3(1.f, 1.f, 1.f);
};
