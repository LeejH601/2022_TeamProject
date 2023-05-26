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

void CUIObject::Update(float fTimeElapsed)
{
	if (m_bEnable) {
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = m_xmf2Size.x / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);
		m_xmf4x4World._33 = m_iTextureIndex; // TextureIndex
		//1, 023x((정규 좌표) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

void CUIObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;
	if (m_pMesh)
	{
		// UI Size 정보 Update
		// 
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

void CUIObject::SetSize(XMFLOAT2 xmf2Size)
{
	m_xmf2Size = xmf2Size;
}

void CUIObject::SetScreenPosition(XMFLOAT2 xmf2ScreenPosition)
{
	m_xmf2ScreenPosition = xmf2ScreenPosition;
}

void CUIObject::SetTextureIndex(UINT iTextureIndex)
{
	m_iTextureIndex = iTextureIndex;
}
