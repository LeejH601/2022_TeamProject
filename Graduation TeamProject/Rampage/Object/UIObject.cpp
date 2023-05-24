#include <math.h>
#include "../Object/Texture.h"
#include "UIObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"

CUIObject::CUIObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetMesh(std::make_shared<CUIRectMesh>(pd3dDevice, pd3dCommandList));
}

CUIObject::~CUIObject()
{
}

void CUIObject::Animate(float fTimeElapsed)
{
	if (m_bEnable) {
		CGameObject::Animate(fTimeElapsed);
	}
}

void CUIObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		// CGameObject의 정보를 넘길 버퍼가 있고, 해당 버퍼에 대한 CPU 포인터가 있으면 UpdateShaderVariables 함수를 호출한다.
		UpdateShaderVariables(pd3dCommandList);
		// 여기서 메쉬의 렌더를 한다.
		m_pMesh->OnPreRender(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList, 0);
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CUIObject::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

bool& CUIObject::GetEnable()
{
	return m_bEnable;
}
