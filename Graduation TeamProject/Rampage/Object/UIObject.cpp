#include <math.h>
#include "../Object/Texture.h"
#include "UIObject.h"
#include "../Object/Mesh.h"
#include "../Global/Camera.h"

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
		// ȭ�� ũ�⸦ �������� Size ���� �ִ� ũ�� (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = m_xmf2Size.x / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);
		m_xmf4x4World._33 = m_iTextureIndex; // TextureIndex
		//1, 023x((���� ��ǥ) + 1.0)x0.5

		m_xmf4x4World._12 = 1.f; // U
		m_xmf4x4World._13 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN
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
		// UI Size ���� Update
		// 
		// CGameObject�� ������ �ѱ� ���۰� �ְ�, �ش� ���ۿ� ���� CPU �����Ͱ� ������ UpdateShaderVariables �Լ��� ȣ���Ѵ�.
		UpdateShaderVariables(pd3dCommandList);
		// ���⼭ �޽��� ������ �Ѵ�.
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
		// ȭ�� ũ�⸦ �������� Size ���� �ִ� ũ�� (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = m_iTextureIndex; // �ؽ��� �ε���

		m_xmf4x4World._12 = m_fCurrentValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN
		//1, 023x((���� ��ǥ) + 1.0)x0.5

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
		m_fPreValue = m_fCurrentValue; // ���� Value
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
		// UI Size ���� Update
		// 
		// CGameObject�� ������ �ѱ� ���۰� �ְ�, �ش� ���ۿ� ���� CPU �����Ͱ� ������ UpdateShaderVariables �Լ��� ȣ���Ѵ�.
		PreBarUpdate(0.f);
		UpdateShaderVariables(pd3dCommandList);
		// ���⼭ �޽��� ������ �Ѵ�.
		m_pMesh->OnPreRender(pd3dCommandList);
		m_pMesh->Render(pd3dCommandList, 0);

		CurBarUpdate(0.f);
		UpdateShaderVariables(pd3dCommandList);
		// ���⼭ �޽��� ������ �Ѵ�.
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
		// ȭ�� ũ�⸦ �������� Size ���� �ִ� ũ�� (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 12; // �ؽ��� �ε���

		m_xmf4x4World._12 = m_fPreValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN

		//1, 023x((���� ��ǥ) + 1.0)x0.5

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
		// ȭ�� ũ�⸦ �������� Size ���� �ִ� ũ�� (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 8; // �ؽ��� �ε���

		m_xmf4x4World._12 = m_fCurrentValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN

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
		// ȭ�� ũ�⸦ �������� Size ���� �ִ� ũ�� (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 11; // �ؽ��� �ε���

		m_xmf4x4World._12 = m_fPreValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN

		//1, 023x((���� ��ǥ) + 1.0)x0.5
	
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
		// ȭ�� ũ�⸦ �������� Size ���� �ִ� ũ�� (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = (/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH);
		m_xmf4x4World._22 = m_xmf2Size.y / (FRAME_BUFFER_HEIGHT);

		m_xmf4x4World._33 = 9; // �ؽ��� �ε���

		m_xmf4x4World._12 = m_fCurrentValue / m_fTotalValue; // U
		m_xmf4x4World._13 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN

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
	if (m_iNumber != iNumber)
	{
		m_iNumber = iNumber;

		// ���� �� �ڸ����� ����
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

		if(m_fAlpha <= 1.5f)
			m_fAlpha += 0.03f;
		//m_xmf2ScreenPosition5 = XMFLOAT2()
		// ȭ�� ũ�⸦ �������� Size ���� �ִ� ũ�� (MAX WIDTH: FRAME_BUFFER_WIDTH, MAX_HEIGHT: FRAME_BUFFER_HEIGHT)
		m_xmf4x4World._11 = ((/*(m_fCurrentHp / m_fTotalHp) * */m_xmf2Size.x) / (FRAME_BUFFER_WIDTH)) * m_fAnimationTime;
		m_xmf4x4World._22 = (m_xmf2Size.y / (FRAME_BUFFER_HEIGHT)); // *m_fAnimationTime;

		m_xmf4x4World._33 = m_iTextureIndex + N; // �ؽ��� �ε���

		m_xmf4x4World._12 = 1.f; // U
		m_xmf4x4World._13 = 1.f; // V

		float color = 2.8f;
		m_xmf4x4World._21 = 1.f; // RGBN // 2.8
		m_xmf4x4World._31 = 1.f;// +(m_iNumber % 10) * 0.1f;
		m_xmf4x4World._44 = 1.f;// +(m_iNumber % 10) * 0.1f;
		if ((m_iNumber / 3) % 3 == 0) // 0. 3, 6
			m_xmf4x4World._21 = color;
		else if ((m_iNumber / 3) % 3 == 1) // 1, 4, 7 -> �����
		{
			m_xmf4x4World._44 = color;
		}
		else if ((m_iNumber / 3) % 3 == 2) // 2, 5, 8
		{
			m_xmf4x4World._21 = color;
			m_xmf4x4World._44 = color;
		}

		m_xmf4x4World._23 = m_fAlpha; // ALPHA
		//1, 023x((���� ��ǥ) + 1.0)x0.5
		
		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH) + ((m_xmf2Size.x) / (FRAME_BUFFER_WIDTH) * (0.725f - (1.f - m_fAnimationTime)) * ORDER);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT) + (1.f - m_fAnimationTime) * 0.01f;
		//(m_xmf2ScreenPosition.y * 2.f) / (FRAME_BUFFER_HEIGHT); // -1 ~ 1
		m_xmf4x4World._43 = 0.f;
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
		// UI Size ���� Update
		// 
		// CGameObject�� ������ �ѱ� ���۰� �ְ�, �ش� ���ۿ� ���� CPU �����Ͱ� ������ UpdateShaderVariables �Լ��� ȣ���Ѵ�.
		for (int i = 0; i < m_vecNumObject.size(); i++)
		{
			UpdateNumberTexture(m_vecNumObject[m_vecNumObject.size() - i - 1], i);
			UpdateShaderVariables(pd3dCommandList);
			// ���⼭ �޽��� ������ �Ѵ�.
			m_pMesh->OnPreRender(pd3dCommandList);
			m_pMesh->Render(pd3dCommandList, 0);
		}
	}

	if (m_pChild) m_pChild->Render(pd3dCommandList, b_UseTexture, pCamera);
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, b_UseTexture, pCamera);
}

CBloodEffectObject::CBloodEffectObject(int iTextureIndex, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fSize) : CUIObject(iTextureIndex, pd3dDevice, pd3dCommandList, fSize)
{
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
			m_bEnable = false;
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

		m_xmf4x4World._33 = m_iTextureIndex; // �ؽ��� �ε���

		m_xmf4x4World._12 = 1.f; // U
		m_xmf4x4World._13 = 1.f; // V

		m_xmf4x4World._21 = 1.f; // RGBN
		//1, 023x((���� ��ǥ) + 1.0)x0.5

		// -1 ~ 1
		m_xmf4x4World._41 = (m_xmf2ScreenPosition.x / FRAME_BUFFER_WIDTH);
		m_xmf4x4World._42 = (m_xmf2ScreenPosition.y / FRAME_BUFFER_HEIGHT);
		m_xmf4x4World._43 = 0.f;


		m_xmf4x4World._23 = m_fLifeTime; // ALPHA
	}

}