#pragma once


//----------------------------------------------------------------------------
// 22/09/20 CGameObject Ŭ���� �߰� - Leejh
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

	std::shared_ptr<CGameObject> m_pParent = nullptr;
	std::shared_ptr<CGameObject> m_pChild = nullptr;
	std::shared_ptr<CGameObject> m_pSibling = nullptr;

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

	CGameObject* GetParent() { return(m_pParent.get()); }

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(char* pstrFrameName);
};