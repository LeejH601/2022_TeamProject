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
	m_iTextureIndex = iTextureIndex;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetMesh(std::make_shared<CUIRectMesh>(pd3dDevice, pd3dCommandList));
}

CUIObject::CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize)
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

		m_xmf4x4World._12 = 1.f; // U
		m_xmf4x4World._13 = 1.f; // V

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

CBarObject::CBarObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CUIObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{

}

CBarObject::~CBarObject()
{
}

void CBarObject::Update(float fTimeElapsed)
{
	if (m_bEnable) {
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = m_fCurrentValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		//1, 023x((정규 좌표) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

void CBarObject::Set_Value(float fCurrentValue, float fTotalValue)
{
	if (fCurrentValue < m_fCurrentValue)
		m_fPreValue = m_fCurrentValue; // 이전 Value
	m_fCurrentValue = fCurrentValue;
	m_fTotalValue = fTotalValue;
}

CHPObject::CHPObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CBarObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
}

CHPObject::~CHPObject()
{
}

void CHPObject::Update(float fTimeElapsed)
{
}

void CHPObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;
	if (m_pMesh)
	{
		// UI Size 정보 Update
		// 
		// CGameObject의 정보를 넘길 버퍼가 있고, 해당 버퍼에 대한 CPU 포인터가 있으면 UpdateShaderVariables 함수를 호출한다.
		PreBarUpdate(0.f);
		UpdateShaderVariables(pd3dCommandList);
		// 여기서 메쉬의 렌더를 한다.
		m_pMesh->OnPreRender(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList, 0);

		CurBarUpdate(0.f);
		UpdateShaderVariables(pd3dCommandList);
		// 여기서 메쉬의 렌더를 한다.
		m_pMesh->OnPreRender(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList, 0);
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CHPObject::PreBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		if (m_fPreValue && (m_fPreValue > m_fCurrentValue))
			m_fPreValue -= (m_fTotalValue - m_fCurrentValue) * 0.1f;
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 10; // 텍스쳐 인덱스

		m_xmf4x4World._12 = m_fPreValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		//1, 023x((정규 좌표) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

void CHPObject::CurBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 8; // 텍스쳐 인덱스

		m_xmf4x4World._12 = m_fCurrentValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

CSTAMINAObject::CSTAMINAObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CBarObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
}

CSTAMINAObject::~CSTAMINAObject()
{
}

void CSTAMINAObject::PreBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		if (m_fPreValue && (m_fPreValue > m_fCurrentValue))
			m_fPreValue -= (m_fTotalValue - m_fCurrentValue) * 0.1f;
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 11; // 텍스쳐 인덱스

		m_xmf4x4World._12 = m_fPreValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		//1, 023x((정규 좌표) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

void CSTAMINAObject::CurBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 9; // 텍스쳐 인덱스

		m_xmf4x4World._12 = m_fCurrentValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

CNumberObject::CNumberObject(int iOffsetTextureIndex, int Number, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CUIObject(iOffsetTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{

}

CNumberObject::~CNumberObject()
{
}

void CNumberObject::UpdateNumber(UINT iNumber)
{
	m_iNumber = iNumber;

	// 십의 몇 자리인지 구함
	int N = 0;
	int SaveNum = iNumber;
	while ((SaveNum / 10) >= 1)
	{
		N++;
		SaveNum /= 10;
	}
	m_vecNumObject.resize(N + 1);
	for (int i = m_vecNumObject.size() - 1; i >= 0; i--)
	{
		m_vecNumObject[i] = iNumber / ((int)pow(10, i));
		iNumber = iNumber % ((int)pow(10, i));
	}
}

void CNumberObject::UpdateNumberTexture(UINT N, UINT ORDER)
{
	if (m_bEnable) {
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex + N; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 1.f; // U
		m_xmf4x4World._13 = 1.f; // V

		//1, 023x((정규 좌표) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) + ((m_xmf2Size.x) / (FRAME_BUFFER_WIDTH) * 0.725f * ORDER);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}

}

void CNumberObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;
	if (m_pMesh)
	{
		// UI Size 정보 Update
		// 
		// CGameObject의 정보를 넘길 버퍼가 있고, 해당 버퍼에 대한 CPU 포인터가 있으면 UpdateShaderVariables 함수를 호출한다.
		for (int i = 0; i < m_vecNumObject.size(); i++)
		{
			UpdateNumberTexture(m_vecNumObject[m_vecNumObject.size() - i - 1], i);
			UpdateShaderVariables(pd3dCommandList);
			// 여기서 메쉬의 렌더를 한다.
			m_pMesh->OnPreRender(pd3dCommandList);
			m_pMesh->Render(pd3dCommandList, 0);
		}
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}
