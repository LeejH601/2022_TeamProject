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
	virtual bool Reset() { return true; };
public:
	void SetSize(XMFLOAT2 xmf2Size);
	void SetScreenPosition(XMFLOAT2 xmf2ScreenPosition);
	void SetTextureIndex(UINT iTextureIndex);
	void SetColor(float fColor) { m_xmfColor = fColor; }
	void SetAlpha(float fAlpha) { m_fAlpha = fAlpha; }
	void SetColor(XMFLOAT3 xmf3Color) { m_xmf3Color = xmf3Color; }
	void AddMessageListener(std::unique_ptr<IMessageListener> pListener);

protected:
	std::vector<D3D12_RECT> m_tRect;
	XMFLOAT2 m_xmf2Size = XMFLOAT2(1.f, 1.f);
	XMFLOAT2 m_xmf2ScreenPosition = XMFLOAT2(0.f, 0.f);
	float m_xmfColor = 1.f;
	UINT  m_iTextureIndex = 0;
	float m_fAlpha = 1.f;
	XMFLOAT3 m_xmf3Color = XMFLOAT3(1.f, 1.f, 1.f);
	std::vector<std::unique_ptr<CUIObject>> m_pChildUI;
	std::vector<std::unique_ptr<IMessageListener>> m_pListeners;
};	

class CGradationObject : public CUIObject
{
public:
	CGradationObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CGradationObject();
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void PreBarUpdate(float fTimeElapsed);
	virtual void CurBarUpdate(float fTimeElapsed);

	void SetGradationValue(float fGradationValue) { m_fGradationValue = fGradationValue; } // 0.1 ~ 0.8f
protected:
	XMFLOAT2 m_xmf2OffsetPosition = XMFLOAT2(0.f, 0.f);
	XMFLOAT2 m_xmf2OffsetSize = XMFLOAT2(0.f, 0.f);

	float m_fTotalValue = 100.f;
	float m_fCurrentValue = 100.f;

	float m_fPreValue = m_fTotalValue; // 이전 Value

	float m_fGradationValue = 0.f;
};


class CBarObject : public CUIObject
{
public:
	CBarObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CBarObject();

	virtual void Update(float fTimeElapsed);

	virtual void Set_Value(float fCurrentValue, float fTotalValue);
protected:
	XMFLOAT2 m_xmf2OffsetPosition = XMFLOAT2(0.f, 0.f);
	XMFLOAT2 m_xmf2OffsetSize = XMFLOAT2(0.f, 0.f);

	float m_fTotalValue = 100.f;
	float m_fCurrentValue = 100.f;

	float m_fPreValue = m_fTotalValue; // 이전 Value
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

public:
	UINT GetNumSize() { return m_vecNumObject.size(); };
	void SetMaxAlpha(float fMaxAlpha) { m_fMaxAlpha = fMaxAlpha; }
	void SetSpeedAlpha(float fSpeedAlpha) { m_fAlphaSpeed = fSpeedAlpha; }
protected:
	std::vector<UINT> m_vecNumObject;
	UINT m_iNumber = 0;
	float m_fAnimationTime = 1.f;
	bool m_bAnimation = false;
	float m_fMaxAlpha = 1.5f;
	float m_fAlphaSpeed = 0.05f;
	bool m_bRotation = false;


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
	float m_fLifeTime = 0.f;
	bool m_bAnimation = false;
};

class CAppearObject : public CUIObject
{
public:
	CAppearObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CAppearObject();

	virtual void UpdateLifeTime(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);

	virtual void SetEnable(bool bEnable);
private:
};

class CButtonObject : public CUIObject
{
public:
	CButtonObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CButtonObject();

public:
	bool CheckCollisionMouse(POINT ptCursorPo);

protected:
	bool m_bCollision = false;
};

class CColorButtonObject : public CButtonObject
{
public:
	CColorButtonObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CColorButtonObject();

public:
	virtual void Update(float fTimeElapsed);

};

class CResultFrame : public CUIObject
{
public:
	CResultFrame(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize, class CTextureManager* pTextureManager);
	virtual ~CResultFrame() {};

public:
	virtual void SetEnable(bool bEnable);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	void SetResultData(UINT iTotalComboN, float fPlayTime, UINT iPotionN);
	virtual bool Reset();
private:
	UINT CalculatePlaytimeScore(float fPlayTime);

private:
	float m_fTotalEnableTime = 1.f;
	float m_fEnableTime = 0.f;
};

class CSquareBar : public CBarObject
{
public:
	CSquareBar(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize, class CTextureManager* pTextureManager);
	virtual ~CSquareBar();

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
	virtual void PreBarUpdate(float fTimeElapsed);
	virtual void CurBarUpdate(float fTimeElapsed);

	virtual void Set_Value(float fCurrentValue, float fTotalValue);
	virtual bool Reset();
	bool IsFullGauge();
	bool IsProgressFull() { return m_fSkillGauge >= m_fMaxSkillGague; };
private:
	bool m_bOnGlow = false;
	float m_fEmissive = 1.0f;
	float m_fGlowValue = 5.0f;
	float m_fGlowDurationTime = 1.0f;
	float m_fGlowDurationAcculate = 0.0f;

	float m_fSkillGauge = 0.f;
	float m_fMaxSkillGague = 100.f;
};

class CFlickerObject : public CUIObject
{

public:
	enum FlickerType
	{
		INVISIBLE,
		VISIBLE,
		FlickerType_END
	};
	CFlickerObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CFlickerObject();
	virtual void Flicker(float fTimeElapsed);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
protected:
	FlickerType m_iFlickerType = FlickerType::INVISIBLE;
	float m_fFlickerTime = 0.6f;
	float m_fDrawTime = 1.4f;
	float m_fTime = 0.f;
};
