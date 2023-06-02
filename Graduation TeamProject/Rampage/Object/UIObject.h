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

protected:
	XMFLOAT2 m_xmf2Size = XMFLOAT2(1.f, 1.f);
	XMFLOAT2 m_xmf2ScreenPosition = XMFLOAT2(0.f, 0.f);
	UINT	m_iTextureIndex = 0;
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

	float m_fPreValue = m_fTotalValue; // ¿Ã¿¸ Value
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
	virtual void PreBarUpdate(float fTimeElapsed);
	virtual void CurBarUpdate(float fTimeElapsed);
protected:

};

class CNumberObject : public CUIObject
{
public:
	CNumberObject(int iOffsetTextureIndex, int Number, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize);
	virtual ~CNumberObject();
	void UpdateNumber(UINT iNumber);
	void UpdateNumberTexture(UINT N, UINT ORDER);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera = NULL);
protected:
	std::vector<UINT> m_vecNumObject;
	UINT m_iNumber = 0;
	
};