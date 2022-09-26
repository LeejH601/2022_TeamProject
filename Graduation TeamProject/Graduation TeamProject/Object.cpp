#include "stdafx.h"
#include "Object.h"


CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
	if (m_pChild) delete m_pChild;
	if (m_pSibling) delete m_pSibling;
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4Transform);
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pMesh)
	{
		// 여기서 메쉬의 렌더를 한다.
	}
}

XMFLOAT3 CGameObject::GetPosition()
{
	return XMFLOAT3(m_xmf4x4Transform._41, m_xmf4x4Transform._42, m_xmf4x4Transform._43);
}

XMFLOAT3 CGameObject::GetLook()
{
	// Normalize 해서 넘겨주는 것 필요할지도..?
	return XMFLOAT3(m_xmf4x4Transform._31, m_xmf4x4Transform._32, m_xmf4x4Transform._33);
}

XMFLOAT3 CGameObject::GetUp()
{
	// Normalize 해서 넘겨주는 것 필요할지도..?
	return XMFLOAT3(m_xmf4x4Transform._21, m_xmf4x4Transform._22, m_xmf4x4Transform._23);
}

XMFLOAT3 CGameObject::GetRight()
{
	// Normalize 해서 넘겨주는 것 필요할지도..?
	return XMFLOAT3(m_xmf4x4Transform._11, m_xmf4x4Transform._12, m_xmf4x4Transform._13);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	return nullptr;
}
