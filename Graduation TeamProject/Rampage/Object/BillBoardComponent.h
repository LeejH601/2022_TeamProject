#pragma once
#include "..\Global\Component.h"
#include "..\Object\BillBoardObject.h"
#include "..\Object\Monster.h"
class CAttackSpriteComponent : public CComponent
{
public:
    static std::vector<std::pair<std::shared_ptr<CMonster>, std::shared_ptr<CMultiSpriteObject>>> m_vSprite;

public:
    DECLARE_SINGLE(CAttackSpriteComponent);

    void Add_AttackComponent(std::pair<std::shared_ptr<CMonster>, std::shared_ptr<CMultiSpriteObject>> pAttackComponent);
    void Collision_Check();

    virtual void SetSpriteEnable(int iIndex);

    void SetTexture(int iIndex, LPCTSTR pszFileName);
    void SetSpeed(float fSpeed);
    void SetAlpha(float fAlpha);

    void UpdateData();

public:
    unsigned int& GetAttackNumber();
    float& GetSpeed();
    float& GetAlpha();

private:
    unsigned int m_nMonsterAttackNumber = 0;
    float   m_fSpeed = 5.f;
    float   m_fAlpha = 1.f;
    float   m_fSize = 0.5f;
    bool    m_bAnimation = false;
};