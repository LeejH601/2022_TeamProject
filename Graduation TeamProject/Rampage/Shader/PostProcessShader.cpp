#include "PostProcessShader.h"
#include "..\Object\State.h"
#include "..\Sound\SoundManager.h"
#include "..\Scene\SimulatorScene.h"

CPostProcessShader::~CPostProcessShader()
{
	if (m_pd3dRtvCPUDescriptorHandles) delete[] m_pd3dRtvCPUDescriptorHandles;
}

D3D12_INPUT_LAYOUT_DESC CPostProcessShader::CreateInputLayout(int nPipelineState)
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CPostProcessShader::CreateDepthStencilState(int nPipelineState)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_RASTERIZER_DESC CPostProcessShader::CreateRasterizerState(int nPipelineState)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
#ifdef _WITH_RASTERIZER_DEPTH_BIAS
	d3dRasterizerDesc.DepthBias = 8500;
#endif
	d3dRasterizerDesc.DepthBiasClamp = 0;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.f;
	d3dRasterizerDesc.DepthClipEnable = FALSE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_SHADER_BYTECODE CPostProcessShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"PostProcessingVertexShader.cso", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPostProcessShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"PostProcessingPixelShader.cso", ppd3dShaderBlob));
}

void CPostProcessShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat, int nPipelineState)
{
	if (m_ppd3dPipelineStates.size() != 1)
	{
		m_nPipelineStates = 1;
		m_ppd3dPipelineStates.resize(m_nPipelineStates);
	}

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, dxgiDsvFormat, nPipelineState);
}
#define _WITH_SCENE_ROOT_SIGNATURE
void CPostProcessShader::CreateResourcesAndViews(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nResources, DXGI_FORMAT* pdxgiFormats, UINT nWidth, UINT nHeight, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle, UINT nShaderResources)
{
	m_pHDRTexture = std::make_unique<CTexture>(1, RESOURCE_TEXTURE2D, 0, 1);
	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.0f, 0.0f, 1.0f } };

	m_pTexture = std::make_shared<CTexture>(nResources, RESOURCE_TEXTURE2D, 0, 1, nResources, 0, 1, 0, 1);
	m_pHDRTexture->CreateTexture(pd3dDevice, pd3dCommandList, 0, RESOURCE_TEXTURE2D, nWidth, nHeight, 1, 0, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue);

	for (UINT i = 0; i < nResources; i++)
	{
		d3dClearValue.Format = pdxgiFormats[i];
		m_pTexture->CreateTexture(pd3dDevice, pd3dCommandList, i, RESOURCE_TEXTURE2D, nWidth, nHeight, 1, 0, pdxgiFormats[i], D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue);
	}

	CreateCbvSrvUavDescriptorHeaps(pd3dDevice, 0, nShaderResources);
	CreateShaderVariables(pd3dDevice, NULL);
#ifdef _WITH_SCENE_ROOT_SIGNATURE
	CreateShaderResourceViews(pd3dDevice, m_pTexture.get(), 0, 13);
	CreateComputeShaderResourceView(pd3dDevice, &m_pTexture.get()[0], 0, 0, 0, 1);
#else
	CreateShaderResourceViews(pd3dDevice, m_pTexture, 0, 0);
#endif

	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	m_pd3dRtvCPUDescriptorHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nResources];

	for (UINT i = 0; i < nResources; i++)
	{
		d3dRenderTargetViewDesc.Format = pdxgiFormats[i];
		ID3D12Resource* pd3dTextureResource = m_pTexture->GetResource(i);
		pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
		m_pd3dRtvCPUDescriptorHandles[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}
}

void CPostProcessShader::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle)
{
	int nResources = m_pTexture->GetTextures();
	D3D12_CPU_DESCRIPTOR_HANDLE* pd3dAllRtvCPUHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nRenderTargets + nResources];

	for (int i = 0; i < nRenderTargets; i++)
	{
		pd3dAllRtvCPUHandles[i] = pd3dRtvCPUHandles[i];
		pd3dCommandList->ClearRenderTargetView(pd3dRtvCPUHandles[i], Colors::White, 0, NULL);
	}

	for (int i = 0; i < nResources; i++)
	{
		::SynchronizeResourceTransition(pd3dCommandList, GetTextureResource(i), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

		D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = GetRtvCPUDescriptorHandle(i);
		FLOAT pfClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		FLOAT pfClearColor_4[4] = { 1, 1, 1, 1.0f };
		if (i != 5)
			pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, NULL);
		else
			pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor_4, 0, NULL);
		pd3dAllRtvCPUHandles[nRenderTargets + i] = d3dRtvCPUDescriptorHandle;
	}
	pd3dCommandList->OMSetRenderTargets(nRenderTargets + nResources, pd3dAllRtvCPUHandles, FALSE, &d3dDepthStencilBufferDSVCPUHandle);

	if (pd3dAllRtvCPUHandles) delete[] pd3dAllRtvCPUHandles;
}

void CPostProcessShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	int nResources = m_pTexture->GetTextures();

	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);

	for (int i = 0; i < nResources; i++)
	{
		::SynchronizeResourceTransition(pd3dCommandList, GetTextureResource(i), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
	}
}

CBreakScreenEffectShader::CBreakScreenEffectShader()
{
	m_fMaxSlashWeight = 15.0f;
	m_fMaxSlideWeight = 120.0f;

	m_fSlashChangeSpeed = m_fMaxSlashWeight / 0.07f;
	m_fSlideMoveSpeed = m_fMaxSlideWeight / 0.3f;


	for (int i = 0; i < m_nMaxCrackLevel + 1; ++i) {
		m_fCrackWeights.push_back((i * 7.0f));
	}
	m_xmf3Colors.push_back(XMFLOAT3(1, 1, 1));
	m_xmf3Colors.push_back(XMFLOAT3(0.7607, 0.2980, 0.9647));
	m_xmf3Colors.push_back(XMFLOAT3(1, 0.0784, 0.5764));
	m_xmf3Colors.push_back(XMFLOAT3(0.9882, 0.4313, 0.1333));
	m_xmf3Colors.push_back(XMFLOAT3(1, 1, 0.4));



	m_xmf2UVOffset = XMFLOAT2(0, 0);
	m_xmf2UVScale = XMFLOAT2(1, 1);
}

CBreakScreenEffectShader::~CBreakScreenEffectShader()
{
}

D3D12_SHADER_BYTECODE CBreakScreenEffectShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState)
{
	return(CShader::ReadCompiledShaderFile(L"BreakScreenEffectPixelShader.cso", ppd3dShaderBlob));
}

void CBreakScreenEffectShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_fSlashWeight, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_fSlideWeight, 1);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_fCrackWeights[m_nCrackLevel], 2);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 3, &m_xmf3Colors[m_nCrackLevel], 4);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_xmf2UVOffset.x, 7);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_xmf2UVOffset.y, 8);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_xmf2UVScale.x, 9);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_xmf2UVScale.y, 10);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_fCrackEmissive, 11);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_fGenericAlpha, 12);
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 1, &m_nCrackIndex, 13);
}

void CBreakScreenEffectShader::AnimateObjects(float fTimeElapsed)
{
	static std::random_device rd;
	static std::default_random_engine dre{ rd() };
	static std::uniform_real_distribution<float> urd(0.0f, 1.0f);

	if (!m_bEnable)
		return;

	switch (sequence)
	{
	case BREAKSCREENSEQUENCE::SLASHING:
	{
		static float m_SlashingDelay = 0.0f;
		static float m_SlashingAcculate = 0.0f;
		m_SlashingAcculate += fTimeElapsed;
		if (m_SlashingAcculate < m_SlashingDelay)
			break;

		m_fSlashWeight += fTimeElapsed * m_fSlashChangeSpeed;
		m_fSlashWeight = min(m_fMaxSlashWeight, m_fSlashWeight);
		if (m_fMaxSlashWeight - m_fSlashWeight < 0.0001f) {
			sequence = BREAKSCREENSEQUENCE::SLIDING;
			m_SlashingAcculate = 0.0f;
			ChargeAttack_Player::GetInst()->SetPlayingMoveRunning(true);
		}
	}
	break;
	case BREAKSCREENSEQUENCE::SLIDING:
	{
		static float m_SlidingDelay = 1.0f;
		static float m_SlidingAcculate = 0.0f;
		static bool bIsSlideSoundPlay = false;
		m_SlidingAcculate += fTimeElapsed;
		if (m_SlidingAcculate < m_SlidingDelay)
			break;

		if (!bIsSlideSoundPlay){
			CSoundManager::GetInst()->PlaySound("Sound/shoot/ethanchase7744__sword-slash.wav", 1.0, 0.0);
			bIsSlideSoundPlay = true;
		}

		m_fSlashWeight += fTimeElapsed * m_fSlashChangeSpeed * 0.5f;
		m_fSlideWeight += fTimeElapsed * m_fSlideMoveSpeed;
		m_fSlideWeight = min(m_fMaxSlideWeight, m_fSlideWeight);

		if (m_fMaxSlideWeight - m_fSlideWeight < 0.0001f) {
			sequence = BREAKSCREENSEQUENCE::CRACKING;
			m_SlidingAcculate = 0.0f;
			bIsSlideSoundPlay = false;
		}
	}
	break;
	case BREAKSCREENSEQUENCE::CRACKING:
	{
		static float m_CrackingDelay = 1.3f;
		static float m_CrackingAcculate = 0.0f;
		m_CrackingAcculate += fTimeElapsed;
		m_fCrackEmissive -= fTimeElapsed * 50.f;
		m_fCrackEmissive = max(1.0f, m_fCrackEmissive);

		if (m_CrackingAcculate >= m_CrackingDelay - (m_nCrackLevel / 5.0)) {
			m_CrackingAcculate = 0.0f;
			m_nCrackLevel++;
			m_nCrackLevel = min(m_nCrackLevel, m_nMaxCrackLevel);
			m_xmf2UVOffset = XMFLOAT2(urd(dre) * 0.5f, urd(dre) * 0.5f);
			float scale = 1.f / ((m_nMaxCrackLevel + 1) - m_nCrackLevel);
			m_xmf2UVScale = XMFLOAT2(scale, scale);
			m_fCrackEmissive = 50.0f;
			m_fSlideWeight += fTimeElapsed * m_fSlideMoveSpeed * 5.0f;

			CSoundManager::GetInst()->PlaySound("Sound/effect/kneeling__cleaver.mp3", 1.0, 0.0);

			if (m_nCrackLevel >= m_nMaxCrackLevel)
				sequence = BREAKSCREENSEQUENCE::FlASHING;
		}
	}
	break;
	case BREAKSCREENSEQUENCE::FlASHING:
	{
		static float m_FlashingDelay = 0.5f;
		static float m_FlashingAcculate = 0.0f;
		static bool bIsPlaySound = false;
		m_FlashingAcculate += fTimeElapsed;
		if (m_FlashingAcculate < m_FlashingDelay) {
			break;
		}

		if (!bIsPlaySound) {
			CSoundManager::GetInst()->PlaySound("Sound/effect/Swoosh Explosion by Electroviolence Id-234555 1.wav", 1.0, 0.0);
			bIsPlaySound = true;
		}

		m_fSlashWeight += fTimeElapsed * m_fSlashChangeSpeed * 10.0f;
		m_fSlideWeight += fTimeElapsed * m_fSlideMoveSpeed * 2.0f;

		static float m_FlashDuring = 1.0f;
		static float m_FlashAcculate = 0.0f;
		m_FlashAcculate += fTimeElapsed;

		if (m_FlashAcculate >= m_FlashDuring) {
			m_FlashingAcculate = 0.0f;
			m_FlashAcculate = 0.0f;
			bIsPlaySound = false;
			m_fSlideWeight = 0.0f;
			m_fSlashWeight = 0.0f;
			sequence = BREAKSCREENSEQUENCE::SEQUENCE_END;
		}
	}
	break;
	case BREAKSCREENSEQUENCE::SEQUENCE_END:
	{
		static float m_FadeOutDelay = 1.0f;
		static float m_FadeOutAcculate = 0.0f;

		m_fGenericAlpha -= fTimeElapsed;
		m_fGenericAlpha = max(0.0f, m_fGenericAlpha);
		m_FadeOutAcculate += fTimeElapsed;

		if (m_FadeOutAcculate >= m_FadeOutDelay) {
			m_FadeOutAcculate = 0.0f;
			Reset();
			ChargeAttack_Player::GetInst()->SetPlayingMoveRunning(false);
			if (m_pPlayer) {
				PlayerParams param;
				param.pPlayer = (CGameObject*)(m_pPlayer);
				CMessageDispatcher::GetInst()->Dispatch_Message(MessageType::SPECIALMOVE_DAMAGED, &param, nullptr);
			}
		}
	}
		break;
	default:
		break;
	}
}

void CBreakScreenEffectShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (!m_bEnable)
		return;

	//CShader::Render(pd3dCommandList, pCamera, nPipelineState);
	if (m_ppd3dPipelineStates.data() && m_ppd3dPipelineStates[nPipelineState]) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState].Get());
	UpdateShaderVariables(pd3dCommandList);

	//if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}

void CBreakScreenEffectShader::Reset()
{
	m_bEnable = false;
	m_fSlashWeight = 0.f;
	m_fSlideWeight = 0.f;
	m_nCrackLevel = 0;
	m_xmf2UVOffset = XMFLOAT2(0, 0);
	m_xmf2UVScale = XMFLOAT2(1, 1);
	m_fCrackEmissive = 1.0f;
	sequence = BREAKSCREENSEQUENCE::SLASHING;
	m_fGenericAlpha = 1.0f;
}
