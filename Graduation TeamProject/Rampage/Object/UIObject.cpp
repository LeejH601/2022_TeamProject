#include <math.h>
#include "../Object/Texture.h"
#include "UIObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"
#include "../Object/TextureManager.h"
#include "../Sound/SoundManager.h"

CUIObject::CUIObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4World._23 = 1.f; // ALPHA

	m_xmf4x4World._21 = 1.f; // RGBN // 2.8
	m_xmf4x4World._31 = 1.f;
	m_xmf4x4World._44 = 1.f;
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4Texture = Matrix4x4::Identity();
	m_iTextureIndex = iTextureIndex;

	m_tRect.push_back(RECT(0.f, 0.f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetMesh(std::make_shared<CUIRectMesh>(pd3dDevice, pd3dCommandList));
}

CUIObject::CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_xmf4x4World._21 = 1.f; // RGBN
	m_xmf4x4World._23 = 1.f; // ALPHA

	m_xmf4x4World._21 = 1.f; // RGBN // 2.8
	m_xmf4x4World._31 = 1.f;
	m_xmf4x4World._44 = 1.f;
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

		m_xmf4x4World._12 = 0.f; // U1
		m_xmf4x4World._13 = 1.f; // U2
		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 1.f;
		m_xmf4x4World._23 = m_fAlpha; // RGBN // m_xmfColor
		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;

		// Color
		m_xmf4x4World._21 = m_xmf3Color.x;
		m_xmf4x4World._31 = m_xmf3Color.y;
		m_xmf4x4World._44 = m_xmf3Color.z;

		for (int i = 0; i < m_pChildUI.size(); i++)
			m_pChildUI[i]->Update(fTimeElapsed);
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

		//pd3dCommandList->RSSetScissorRects(1, &m_tRect[0]);
		m_pMesh->Render(pd3dCommandList, 0);
	}

	for (int i = 0; i < m_pChildUI.size(); i++)
		m_pChildUI[i]->Render(pd3dCommandList, b_UseTexture, pCamera);
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

void CUIObject::AddMessageListener(std::unique_ptr<IMessageListener> pListener)
{
	m_pListeners.push_back(std::move(pListener));
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

		m_xmf4x4World._12 = 0.f; // U1
		m_xmf4x4World._13 = m_fCurrentValue / m_fTotalValue; // U2
		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 1.f; // RGBN
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
	if (fCurrentValue <0 || fTotalValue <0)
		return;

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
		////// 여기서 메쉬의 렌더를 한다.
		m_pMesh->OnPreRender(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList, 0);

		CurBarUpdate(0.f);
		UpdateShaderVariables(pd3dCommandList);
		// 여기서 메쉬의 렌더를 한다.
		m_pMesh->OnPreRender(pd3dCommandList);

		m_tRect[0] = { (LONG)(FRAME_BUFFER_WIDTH * 0.15f), (LONG)(FRAME_BUFFER_HEIGHT * 0.15f) , (LONG)(FRAME_BUFFER_WIDTH * 0.35f) , (LONG)(FRAME_BUFFER_HEIGHT * 0.2f) };
		//pd3dCommandList->RSSetScissorRects(1, &m_tRect[0]);
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
		m_xmf4x4World._11 = ((m_fPreValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U
		m_xmf4x4World._13 = m_fPreValue / m_fTotalValue; // U

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 0.5f; // RGBN
		m_xmf4x4World._21 = 0.6f; // ALPHA
		

		m_xmf4x4World._21 = 1.f; // R
		m_xmf4x4World._31 = 1.f; // G
		m_xmf4x4World._44 = 1.f; // B

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fPreValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
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
		m_xmf4x4World._11 = ((m_fCurrentValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U
		m_xmf4x4World._13 = m_fCurrentValue / m_fTotalValue; // U

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 1.f; // RGBN

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fCurrentValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
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
		{
			m_fPreValue -= (m_fTotalValue - m_fCurrentValue) * 0.1f;
			if (m_fPreValue < 0.f)
				m_fPreValue = 0.f;
		}
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((m_fPreValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U1
		m_xmf4x4World._13 = m_fPreValue / m_fTotalValue; // U2

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f; // V


		//1, 023x((정규 좌표) + 1.0)x0.5
	
		m_xmf4x4World._23 = 0.8f; // B

		m_xmf4x4World._21 = 1.f; // R
		m_xmf4x4World._31 = 1.f; // G
		m_xmf4x4World._44 = 1.f; // B


		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fPreValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
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
		m_xmf4x4World._11 = ((m_fCurrentValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U1
		m_xmf4x4World._13 = 1;  m_fCurrentValue / m_fTotalValue; // U2

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN

		m_xmf4x4World._21 = 1.f; // R
		m_xmf4x4World._31 = 1.f; // G
		m_xmf4x4World._44 = 1.f; // B

		m_xmf4x4World._23 = 1.f; // B
		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fCurrentValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;


	}
}

void CSTAMINAObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
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

		//m_tRect[0] = { (LONG)(FRAME_BUFFER_WIDTH * 0.15f), (LONG)(FRAME_BUFFER_HEIGHT * 0.2f) , (LONG)(FRAME_BUFFER_WIDTH * 0.35f) , (LONG)(FRAME_BUFFER_HEIGHT * 0.35f) };
		//pd3dCommandList->RSSetScissorRects(1, &m_tRect[0]);
		m_pMesh->Render(pd3dCommandList, 0);

	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}

CNumberObject::CNumberObject(int iOffsetTextureIndex, int Number, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CUIObject(iOffsetTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
	m_fAlpha = 0.f;
	//m_tRect.push_back(RECT(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT * 0.12f));
}

CNumberObject::~CNumberObject()
{
}

void CNumberObject::UpdateNumber(UINT iNumber)
{
	if (m_iNumber != iNumber)
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
		m_fAnimationTime = 0.f;
		m_fAlpha = 0.45f;
		m_bAnimation = true;
	}
}

void CNumberObject::UpdateNumberTexture(UINT N, UINT ORDER)
{
	if (m_bEnable) {

		if(m_fAlpha <= m_fMaxAlpha)
			m_fAlpha += m_fAlphaSpeed;
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH)) * m_fAnimationTime;
		m_xmf4x4World._22 = (m_xmf2Size.y / (FRAME_BUFFER_HEIGHT)); // *m_fAnimationTime;

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = N * 0.1f; // U1
		m_xmf4x4World._13 = N * 0.1f + 0.1f; // U2
		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f; // V

		m_xmf4x4World._23 = m_fAlpha; // ALPHA
		//1, 023x((정규 좌표) + 1.0)x0.5
		
		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) + ((m_xmf2Size.x) / (FRAME_BUFFER_WIDTH) * (0.725f - (1.f - m_fAnimationTime)) * ORDER);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT) + (1.f - m_fAnimationTime) * 0.01f;
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;

		m_xmf4x4World._21 = m_xmf3Color.x;
		m_xmf4x4World._31 = m_xmf3Color.y;
		m_xmf4x4World._44 = m_xmf3Color.z;
	}

}

void CNumberObject::UpdateLifeTime()
{
	if (m_fAnimationTime > 1.3f)
	{
		m_bAnimation = false;
	}
	if (m_fAnimationTime >= 1.f && (!m_bAnimation))
	{
		m_fAnimationTime -= 1.f;
		if (m_fAnimationTime < 1.f)
		{
			m_fAnimationTime = 1.f;
		}
	}
	else
		m_fAnimationTime += 0.8f;
}

void CNumberObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;
	
	UpdateLifeTime();

	if (m_pMesh)
	{
		m_tRect[0] = { (LONG)(FRAME_BUFFER_WIDTH * 0.8f), (LONG)(FRAME_BUFFER_HEIGHT * 0.45f) , (LONG)(FRAME_BUFFER_WIDTH * 0.95f) , (LONG)(FRAME_BUFFER_HEIGHT * 0.55f) };
		//pd3dCommandList->RSSetScissorRects(1, &m_tRect[0]);
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

CComboNumberObject::CComboNumberObject(int iOffsetTextureIndex, int Number, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CNumberObject(iOffsetTextureIndex, Number, pd3dDevice, pd3dCommandList, fSize)
{
}

CComboNumberObject::~CComboNumberObject()
{
}

void CComboNumberObject::UpdateNumberTexture(UINT N, UINT ORDER)
{
	if (m_bEnable) {

		if (m_fAlpha <= 1.5f)
			m_fAlpha += 0.03f;
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH)) * m_fAnimationTime;
		m_xmf4x4World._22 = (m_xmf2Size.y / (FRAME_BUFFER_HEIGHT)); // *m_fAnimationTime;

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = N * 0.1f; // U1
		m_xmf4x4World._13 = N * 0.1f + 0.1f; // U2
		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f; // V

		float color = 4.5f;
		m_xmf4x4World._21 = 1.f; // RGBN // 2.8
		m_xmf4x4World._31 = 1.f;// +(m_iNumber % 10) * 0.1f;
		m_xmf4x4World._44 = 1.f;// +(m_iNumber % 10) * 0.1f;
		if ((m_iNumber / 3) % 3 == 0) // 0. 3, 6
			m_xmf4x4World._21 = color;
		else if ((m_iNumber / 3) % 3 == 1) // 1, 4, 7 -> 보라색
		{
			m_xmf4x4World._44 = color;
		}
		else if ((m_iNumber / 3) % 3 == 2) // 2, 5, 8
		{
			m_xmf4x4World._21 = color;
			m_xmf4x4World._44 = color;
		}

		m_xmf4x4World._23 = m_fAlpha; // ALPHA
		//1, 023x((정규 좌표) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) + ((m_xmf2Size.x) / (FRAME_BUFFER_WIDTH) * (0.725f - (1.f - m_fAnimationTime)) * ORDER);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT) + (1.f - m_fAnimationTime) * 0.01f;
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}


CBloodEffectObject::CBloodEffectObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CUIObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
	m_tRect[0] = (RECT(0, -FRAME_BUFFER_HEIGHT * 0.12f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT * 0.12f));
	m_tRect.push_back(RECT(0, FRAME_BUFFER_HEIGHT * 0.88f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
	m_tRect.push_back(RECT(0, FRAME_BUFFER_HEIGHT * 0.12f, FRAME_BUFFER_WIDTH * 0.12f, FRAME_BUFFER_HEIGHT * 0.88f));
	m_tRect.push_back(RECT(FRAME_BUFFER_WIDTH * 0.9f, FRAME_BUFFER_HEIGHT * 0.12f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT * 0.88f));
}

CBloodEffectObject::~CBloodEffectObject()
{
}

void CBloodEffectObject::UpdateLifeTime(float fTimeElapsed)
{
	if (m_fLifeTime <= 0.7f && !m_bAnimation)
		m_fLifeTime += fTimeElapsed * 0.4f;
	else
	{
		m_bAnimation = true;
		if (m_fLifeTime >= 0.2f)
			m_fLifeTime -= fTimeElapsed * 0.8f;
		else
		{
			//m_bEnable = false;
			m_bAnimation = false;
			m_fLifeTime = 0.2f;
		}
	}
}

void CBloodEffectObject::Update(float fTimeElapsed)
{
	if (m_bEnable) {

		UpdateLifeTime(fTimeElapsed);
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U1
		m_xmf4x4World._13 = 1.f; // U2
		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN
		//1, 023x((정규 좌표) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		m_xmf4x4World._43 = 0.f;

		//gmtxGameObject._21_31_44
		m_xmf4x4World._21 = 0.5f;
		m_xmf4x4World._31 = 0.5f;
		m_xmf4x4World._44 = 0.5f;
		m_xmf4x4World._23 = m_fLifeTime; // ALPHA
	}

}

void CBloodEffectObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
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
		for (int i = 0; i < m_tRect.size(); i++)
		{
			//pd3dCommandList->RSSetScissorRects(1, &m_tRect[i]);
			m_pMesh->Render(pd3dCommandList, 0);
		}
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}

CButtonObject::CButtonObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CUIObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
	m_fAlpha = 20.f;
}


CButtonObject::~CButtonObject()
{
}


bool CButtonObject::CheckCollisionMouse(POINT ptCursorPo)
{
	if ((m_xmf2ScreenPosition.x + m_xmf2Size.x * 0.5f > ptCursorPo.x) && (m_xmf2ScreenPosition.x - m_xmf2Size.x * 0.5f < ptCursorPo.x)
		&&
		((FRAME_BUFFER_HEIGHT - m_xmf2ScreenPosition.y + m_xmf2Size.y * 0.5f) > ptCursorPo.y - 10.f) && ((FRAME_BUFFER_HEIGHT - m_xmf2ScreenPosition.y - m_xmf2Size.y * 0.5f) < ptCursorPo.y - 10.f)
		)
	{
		if(!m_bCollision)
			CSoundManager::GetInst()->PlaySound("Sound/UI/Logo Button.wav", 0.8f, 0.f);
		m_bCollision = true;
		return true;
	}
	m_bCollision = false;
	//m_xmfColor = 1.f;
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("현재 마우스 위치 = %d %d\n"), ptCursorPo.x, ptCursorPo.y);
	OutputDebugString(pstrDebug);
	return false;
}

CMonsterHPObject::CMonsterHPObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CBarObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
}

CMonsterHPObject::~CMonsterHPObject()
{
}

void CMonsterHPObject::Update(float fTimeElapsed)
{
}
void CMonsterHPObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	if (!m_bEnable)
		return;

	if (m_pMesh)
	{
		CurBarUpdate(0.f);
		UpdateShaderVariables(pd3dCommandList);
		// 여기서 메쉬의 렌더를 한다.
		m_pMesh->OnPreRender(pd3dCommandList);

		m_tRect[0] = { (LONG)(FRAME_BUFFER_WIDTH * 0.0f), (LONG)(FRAME_BUFFER_HEIGHT * 0.f) , (LONG)(FRAME_BUFFER_WIDTH * 1.f) , (LONG)(FRAME_BUFFER_HEIGHT * 1.f) };
		//pd3dCommandList->RSSetScissorRects(1, &m_tRect[0]);
		m_pMesh->Render(pd3dCommandList, 0);
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}


void CMonsterHPObject::PreBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		if (m_fPreValue && (m_fPreValue > m_fCurrentValue))
			m_fPreValue -= (m_fTotalValue - m_fCurrentValue) * 0.001f;
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((m_fPreValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex + 2; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U
		m_xmf4x4World._13 = m_fPreValue / m_fTotalValue; // U

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 0.5f; // RGBN
		m_xmf4x4World._23 = 0.6f; // ALPHA


		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fPreValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

void CMonsterHPObject::CurBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((m_fCurrentValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U
		m_xmf4x4World._13 = m_fCurrentValue / m_fTotalValue; // U

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 1.f; // RGBN

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fCurrentValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

CColorButtonObject::CColorButtonObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CButtonObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
}

CColorButtonObject::~CColorButtonObject()
{
}

void CColorButtonObject::Update(float fTimeElapsed)
{

	if (m_bCollision)
	{
		if(m_fAlpha <= 0.6f)
			m_fAlpha += fTimeElapsed * 2.f;
	}
	else
	{
		if (m_fAlpha > 0.f)
			m_fAlpha -= fTimeElapsed * 2.f;
	}
	CUIObject::Update(fTimeElapsed);

}

CResultFrame::CResultFrame(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize, CTextureManager* pTextureManager) : CUIObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
	// -------------------------------------------------ResultFrame--------------------------------------------------------

	std::unique_ptr<CUIObject> pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.5f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/ResultBackGround.dds"));
	m_pChildUI.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(744.f * 1.5f, 459.f * 1.5f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.55f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/ResultFrame.dds"));
	m_pChildUI.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CUIObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(744.f * 1.5f, 459.f * 1.5f));
	pUIObject->SetAlpha(2.f);
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.55f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/ResultScoreMenu.dds"));
	m_pChildUI.push_back(std::move(pUIObject));

	// MaxCombo -16
	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(1.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.f, 1.f, 1.f));
	pUIObject->SetSize(XMFLOAT2(64.f * 0.85f, 60.f * 0.85f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.46f, FRAME_BUFFER_HEIGHT * 0.5f - 30.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(5);
	m_pChildUI.push_back(std::move(pUIObject));

	// MaxCombo - Score -20
	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(1.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.5f, 1.f, 1.f));
	pUIObject->SetSize(XMFLOAT2(64.f * 0.85f, 60.f * 0.85f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.6f, FRAME_BUFFER_HEIGHT * 0.5f - 30.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(5);
	m_pChildUI.push_back(std::move(pUIObject));

	// PlayTime 
	// 분-17
	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(1.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.f, 1.f, 1.f));
	pUIObject->SetSize(XMFLOAT2(64.f * 0.85f, 60.f * 0.85f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.46f, FRAME_BUFFER_HEIGHT * 0.5f - 115.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(50);
	m_pChildUI.push_back(std::move(pUIObject));

	// 초 - 18
	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(1.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.f, 1.f, 1.f));
	pUIObject->SetSize(XMFLOAT2(64.f * 0.85f, 60.f * 0.85f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.525f, FRAME_BUFFER_HEIGHT * 0.5f - 115.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(5);
	m_pChildUI.push_back(std::move(pUIObject));

	// PlayTime  - Score -21

	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(1.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.f, 1.5f, 1.f));
	pUIObject->SetSize(XMFLOAT2(64.f * 0.85f, 60.f * 0.85f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.6f, FRAME_BUFFER_HEIGHT * 0.5f - 115.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(50);
	m_pChildUI.push_back(std::move(pUIObject));

	// PotionCount -19
	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(1.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.f, 1.f, 1.f));
	pUIObject->SetSize(XMFLOAT2(64.f * 0.85f, 60.f * 0.85f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.46f, FRAME_BUFFER_HEIGHT * 0.5f - 200.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(5);
	m_pChildUI.push_back(std::move(pUIObject));


	// PotionCount  - Score -22
	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(1.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.f, 1.f, 1.5f));
	pUIObject->SetSize(XMFLOAT2(64.f * 0.85f, 60.f * 0.85f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.6f, FRAME_BUFFER_HEIGHT * 0.5f - 200.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(5);
	m_pChildUI.push_back(std::move(pUIObject));


	// -------------------------------------------------ResultFrame--------------------------------------------------------


	// Total
	pUIObject = std::make_unique<CNumberObject>(14, 35, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetEnable(false);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetMaxAlpha(3.f);
	dynamic_cast<CNumberObject*>(pUIObject.get())->SetSpeedAlpha(0.15f);
	pUIObject->SetColor(XMFLOAT3(1.5f, 1.5f, 1.0f));
	pUIObject->SetSize(XMFLOAT2(64.f * 1.3f, 60.f * 1.3f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.63f, FRAME_BUFFER_HEIGHT * 0.65f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/Number/Numbers2.dds"));
	dynamic_cast<CNumberObject*>(pUIObject.get())->UpdateNumber(100);
	m_pChildUI.push_back(std::move(pUIObject));
}

void CResultFrame::SetEnable(bool bEnable)
{
	if (bEnable)
	{
		//CSoundManager::GetInst()->RegisterSound("Sound/Background/Logo Bgm.wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
		//CSoundManager::GetInst()->RegisterSound("Sound/Background/GameClear.wav", false, SOUND_CATEGORY::SOUND_BACKGROUND);
		CSoundManager::GetInst()->PlaySound("Sound/Background/GameClear.wav", 1.f, 0.5f);


		//pSound = CSoundManager::GetInst()->FindSound(GOBLIN_MOAN_SOUND_NUM + ORC_MOAN_SOUND_NUM + m_nSoundNumber, m_sc);
		//break;
		//	default:
		//		break;

	}
	m_bEnable = bEnable;

}

void CResultFrame::Update(float fTimeElapsed)
{
	CUIObject::Update(fTimeElapsed);
	if (m_bEnable)
	{
		m_fEnableTime -= fTimeElapsed;
		if (m_fEnableTime < 0.f)
		{
			m_fEnableTime = m_fTotalEnableTime;

			for (int i = 0; i < m_pChildUI.size(); i++)
			{
				if (!m_pChildUI[i]->m_bEnable)
				{
					if (i == 5)
						m_pChildUI[i + 1]->m_bEnable = true;
					m_pChildUI[i]->m_bEnable = true;
					if(i == 4 || i == 7 || i == 9)
					CSoundManager::GetInst()->PlaySound("Sound/UI/Up.wav", 1.f, 0.5f);
					break;
				}
			}
		}
	}
}

void CResultFrame::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	CUIObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}

void CResultFrame::SetResultData(UINT iTotalComboN, float fPlayTime, UINT iPotionN)
{
	dynamic_cast<CNumberObject*>(m_pChildUI[3].get())->UpdateNumber(iTotalComboN);
	dynamic_cast<CNumberObject*>(m_pChildUI[4].get())->UpdateNumber(100 * iTotalComboN); // 20 // Combo

	dynamic_cast<CNumberObject*>(m_pChildUI[5].get())->UpdateNumber(((UINT)fPlayTime) / 60);
	dynamic_cast<CNumberObject*>(m_pChildUI[6].get())->UpdateNumber(((UINT)fPlayTime) % 60);
	dynamic_cast<CNumberObject*>(m_pChildUI[7].get())->UpdateNumber(CalculatePlaytimeScore(fPlayTime)); // 시간 점수

	dynamic_cast<CNumberObject*>(m_pChildUI[8].get())->UpdateNumber(iPotionN); // PotionN
	dynamic_cast<CNumberObject*>(m_pChildUI[9].get())->UpdateNumber(100 * iPotionN); // 물약 점수

	dynamic_cast<CNumberObject*>(m_pChildUI[10].get())->UpdateNumber(100 * iTotalComboN + CalculatePlaytimeScore(fPlayTime) + 100 * 5); // Total
	//dynamic_cast<CUIObject*>(m_pChildUI[13].get())->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.85f + 160.f + (dynamic_cast<CNumberObject*>(m_pChildUI[3].get()))->GetNumSize() * 25.f, FRAME_BUFFER_HEIGHT * 0.5f - 9.f));
}

UINT CResultFrame::CalculatePlaytimeScore(float fPlayTime)
{
	UINT IMinute = ((UINT)fPlayTime) / 60;
	if (IMinute < 5)
		return 500;
	else if (IMinute < 10)
		return 300;
	else if (IMinute < 15)
		return 100;
	else
		return 50;
	return 0;
}

CSquareBar::CSquareBar(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize, CTextureManager* pTextureManager) : CBarObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
	std::unique_ptr<CUIObject> pUIObject = std::make_unique<CHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.18f, 64.f * 0.8f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f, FRAME_BUFFER_HEIGHT * 0.25f - 80.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/SkillGauge.dds"));
	dynamic_cast<CHPObject*>(pUIObject.get())->Set_Value(0.f, 0.f);
	m_pChildUI.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(64.f * 0.8f, 1024.f * 0.14f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f + 80.f, FRAME_BUFFER_HEIGHT * 0.25f + 0.5f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/SkillGauge2.dds"));
	dynamic_cast<CHPObject*>(pUIObject.get())->Set_Value(0.f, 0.f);
	m_pChildUI.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(1024.f * 0.18f, 64.f * 0.8f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f, FRAME_BUFFER_HEIGHT * 0.25f + 80.f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/SkillGauge.dds"));
	dynamic_cast<CHPObject*>(pUIObject.get())->Set_Value(0.f, 0.f);
	m_pChildUI.push_back(std::move(pUIObject));

	pUIObject = std::make_unique<CHPObject>(2, pd3dDevice, pd3dCommandList, 10.f);
	pUIObject->SetSize(XMFLOAT2(64.f * 0.8f, 1024.f * 0.14f));
	pUIObject->SetScreenPosition(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f - 80.f, FRAME_BUFFER_HEIGHT * 0.25f + 0.5f));
	pUIObject->SetTextureIndex(pTextureManager->LoadTotalTextureIndex(TextureType::UITexture, L"Image/UiImages/SkillGauge2.dds"));
	dynamic_cast<CHPObject*>(pUIObject.get())->Set_Value(0.f, 0.f);
	m_pChildUI.push_back(std::move(pUIObject));
}

CSquareBar::~CSquareBar()
{
}


void CSquareBar::Update(float fTimeElapsed)
{

		if (m_fCurrentValue >= (m_fTotalValue / 4) * 4)
		{
			for (int i = 0; i < 4; i++)
			{
				dynamic_cast<CBarObject*>(m_pChildUI[i].get())->Set_Value(100.f, 100.f); // 250
			}
		}

		else if (m_fCurrentValue >= (m_fTotalValue / 4) * 3)
		{
			for (int i = 0; i < 3; i++)
			{
				dynamic_cast<CBarObject*>(m_pChildUI[i].get())->Set_Value(100.f, 100.f); // 250
			}

			dynamic_cast<CBarObject*>(m_pChildUI[3].get())->Set_Value(m_fCurrentValue - (m_fTotalValue / 4) * 3, m_fTotalValue / 4); // 250
		}
		else if (m_fCurrentValue >= (m_fTotalValue / 4) * 2)
		{
			for (int i = 0; i < 2; i++)
			{
				dynamic_cast<CBarObject*>(m_pChildUI[i].get())->Set_Value(100.f, 100.f); // 250
			}

			dynamic_cast<CBarObject*>(m_pChildUI[2].get())->Set_Value(m_fCurrentValue - (m_fTotalValue / 4) * 2, m_fTotalValue / 4); // 250

			for (int i = 3; i < 4; i++)
			{
				dynamic_cast<CBarObject*>(m_pChildUI[i].get())->Set_Value(0.f, 100.f); // 250
			}
		}
		else if (m_fCurrentValue >= (m_fTotalValue / 4) * 1)
		{
			for (int i = 0; i < 1; i++)
			{
				dynamic_cast<CBarObject*>(m_pChildUI[i].get())->Set_Value(100.f, 100.f); // 250
			}

			dynamic_cast<CBarObject*>(m_pChildUI[1].get())->Set_Value(m_fCurrentValue - (m_fTotalValue / 4) * 1, m_fTotalValue / 4); // 250

			for (int i = 2; i < 4; i++)
			{
				dynamic_cast<CBarObject*>(m_pChildUI[i].get())->Set_Value(100.f, 100.f); // 250
			}
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				dynamic_cast<CBarObject*>(m_pChildUI[i].get())->Set_Value(0.f, 1.f); // 250
			}
		}

}

void CSquareBar::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool b_UseTexture, CCamera* pCamera)
{
	CUIObject::Render(pd3dCommandList, b_UseTexture, pCamera);
}


void CSquareBar::PreBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		if (m_fPreValue && (m_fPreValue > m_fCurrentValue))
			m_fPreValue -= (m_fTotalValue - m_fCurrentValue) * 0.1f;
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((m_fPreValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U
		m_xmf4x4World._13 = m_fPreValue / m_fTotalValue; // U

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 0.5f; // RGBN
		m_xmf4x4World._21 = 0.6f; // ALPHA


		m_xmf4x4World._21 = 1.f; // R
		m_xmf4x4World._31 = 1.f; // G
		m_xmf4x4World._44 = 1.f; // B

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fPreValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}

void CSquareBar::CurBarUpdate(float fTimeElapsed)
{
	if (m_bEnable) {
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// 화면 크기를 기준으로 Size 설정 최대 크기 (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((m_fCurrentValue / m_fTotalValue) * m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // 텍스쳐 인덱스

		m_xmf4x4World._12 = 0.f; // U
		m_xmf4x4World._13 = m_fCurrentValue / m_fTotalValue; // U

		m_xmf4x4World._14 = 0.f; // V
		m_xmf4x4World._24 = 1.f;

		m_xmf4x4World._21 = 1.f; // RGBN

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) - (((m_fTotalValue - m_fCurrentValue) / m_fTotalValue) * m_xmf2Size.x * 0.5f) / (FRAME_BUFFER_WIDTH);;
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
	}
}