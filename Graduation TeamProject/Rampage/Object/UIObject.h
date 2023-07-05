#pragma once
#include "../Object/Object.h"

class CUIObject : public CGameObject
{
public:
	CUIObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize); 
	CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CUIObject();
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void SetEnable(bool bEnable);
	virtual bool& GetEnable();
public:
	void SetSize(XMFLOAT2 xmf2Size);
	void SetScreenPosition(XMFLOAT2 xmf2ScreenPosition);
	void SetTextureIndex(UINT iTextureIndex);
	void AddMessageListener(std::unique_ptr<IMessageListener> pListener);

protected:
	std::vector<D3D12_RECT> m_tRect;
	XMFLOAT2 m_xmf2Size = XMFLOAT2(1.f, 1.f);
	XMFLOAT2 m_xmf2ScreenPosition = XMFLOAT2(0.f, 0.f);
	UINT	m_iTextureIndex = 0;
	std::vector<std::unique_ptr<IMessageListener>> m_pListeners;
};	


class CBarObject : public CUIObject
{
public:
	CBarObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CBarObject();

	virtual void Update(float fTimeElapsed);

	void Set_Value(float fCurrentValue, float fTotalValue);
protected:
	XMFLOAT2 m_xmf2OffsetPosition = XMFLOAT2(0.f, 0.f);
	XMFLOAT2 m_xmf2OffsetSize = XMFLOAT2(0.f, 0.f);

	float m_fTotalValue = 100.f;
	float m_fCurrentValue = 100.f;

	float m_fPreValue = m_fTotalValue; // ���� Value
};

class CHPObject : public CBarObject
{
public:
	CHPObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CHPObject();
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void PreBarUpdate(float fTimeElapsed);
	virtual void CurBarUpdate(float fTimeElapsed);
protected:

};

class CSTAMINAObject : public CBarObject
{
public:
	CSTAMINAObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CSTAMINAObject();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void PreBarUpdate(float fTimeElapsed);
	virtual void CurBarUpdate(float fTimeElapsed);
protected:

};

class CMonsterHPObject : public CBarObject
{
public:
	CMonsterHPObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CMonsterHPObject();
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void PreBarUpdate(float fTimeElapsed);
	virtual void CurBarUpdate(float fTimeElapsed);
protected:

};

class CNumberObject : public CUIObject
{
public:
	CNumberObject(int iOffsetTextureIndex, int Number, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CNumberObject();
	virtual void UpdateNumber(UINT iNumber);
	virtual void UpdateNumberTexture(UINT N, UINT ORDER);
	virtual void UpdateLifeTime();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
protected:
	std::vector<UINT> m_vecNumObject;
	UINT m_iNumber = 0;
	float m_fAnimationTime = 1.f;
	bool m_bAnimation = false;
	float m_fAlpha = 0.f;
};

class CComboNumberObject : public CNumberObject
{
public:
	CComboNumberObject(int iOffsetTextureIndex, int Number, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CComboNumberObject();

	virtual void UpdateNumberTexture(UINT N, UINT ORDER);
};

class CBloodEffectObject : public CUIObject
{
public:
	CBloodEffectObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CBloodEffectObject();

	virtual void UpdateLifeTime(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

private:
	float m_fSize = 0.5f;
	float m_fAlpha = 1.f;
	float m_fLifeTime = 0.f;
	bool m_bAnimation = false;
};

class CButtonObject : public CUIObject
{
public:
	CButtonObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CButtonObject();

public:
	bool CheckCollisionMouse(POINT ptCursorPo);

private:
	bool m_bCollision = false;
};