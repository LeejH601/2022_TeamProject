#pragma once
#include "stdafx.h"
//----------------------------------------------------------------------------
// 22/09/20 CGameObject 클래스 추가 - Leejh
//----------------------------------------------------------------------------
#define MAX_FRAMENAME 64

class CMesh;
class CCamera;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();


public:
	char m_pstrFrameName[MAX_FRAMENAME];

	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4X4 m_xmf4x4World;

	CGameObject* m_pParent = nullptr;
	CGameObject* m_pChild = nullptr;
	CGameObject* m_pSibling = nullptr;

	std::shared_ptr<CMesh> m_pMesh;

	BoundingOrientedBox m_xmOOBB;

	//----------------------------------------------------------------------------

	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);

	virtual void OnInitialize() { }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	CGameObject* GetParent() { return (m_pParent); }

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(char* pstrFrameName);
};