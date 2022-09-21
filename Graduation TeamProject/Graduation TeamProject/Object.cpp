#include "stdafx.h"
#include "Object.h"


CGameObject::CGameObject()
{
	
}

CGameObject::~CGameObject()
{
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

XMFLOAT3 CGameObject::GetPosition()
{
	return XMFLOAT3();
}

XMFLOAT3 CGameObject::GetLook()
{
	return XMFLOAT3();
}

XMFLOAT3 CGameObject::GetUp()
{
	return XMFLOAT3();
}

XMFLOAT3 CGameObject::GetRight()
{
	return XMFLOAT3();
}

void CGameObject::SetPosition(float x, float y, float z)
{
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
}

void CGameObject::SetScale(float x, float y, float z)
{
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	return nullptr;
}
