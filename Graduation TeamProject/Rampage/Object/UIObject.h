#pragma once
#include "../Object/Object.h"

class CUIObject : public CGameObject
{
public:
	CUIObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize); 
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