#pragma once
#include "..\Global\stdafx.h"
#include "Shader.h"
#include "..\Object\Texture.h"

class CPostProcessShader : public CShader
{
public:
	CPostProcessShader() {};
	virtual ~CPostProcessShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat, int nPipelineState);
	virtual void CreateResourcesAndViews(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nResources, DXGI_FORMAT* pdxgiFormats,
		UINT nWidth, UINT nHeight, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle, UINT nShaderResources);

	virtual void OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles,
		D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

protected:
	std::shared_ptr<CTexture> m_pTexture = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE* m_pd3dRtvCPUDescriptorHandles = NULL;

public:
	std::shared_ptr<CTexture> m_pHDRTexture = nullptr;

public:
	CTexture* GetTexture() { return(m_pTexture.get()); }
	std::shared_ptr<CTexture> GetTextureShared() { return m_pTexture; };
	ID3D12Resource* GetTextureResource(UINT nIndex) { return(m_pTexture->GetResource(nIndex)); }
	void SetTexture(std::shared_ptr<CTexture> pTexture) { m_pTexture = pTexture; };

	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUDescriptorHandle(UINT nIndex) { return(m_pd3dRtvCPUDescriptorHandles[nIndex]); }
	D3D12_CPU_DESCRIPTOR_HANDLE* GetRtvCPUDescriptorHandle() { return(m_pd3dRtvCPUDescriptorHandles); }
	void SetRtvCPUDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* src, int nResource) {
		m_pd3dRtvCPUDescriptorHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nResource];
		memcpy(m_pd3dRtvCPUDescriptorHandles, src, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * nResource);
	}
};

enum class BREAKSCREENSEQUENCE {
	SLASHING,
	SLIDING,
	CRACKING,
	FlASHING,
	SEQUENCE_END,
};
class CPlayer;
class CBreakScreenEffectShader : public CPostProcessShader
{
	bool m_bEnable = false;
	float m_fSlashWeight = 0.f;
	float m_fMaxSlashWeight = 15.0f;
	float m_fSlideWeight = 0.f;
	float m_fMaxSlideWeight = 80.0f;
	int m_nCrackLevel = 0;
	int m_nMaxCrackLevel = 4;
	std::vector<float> m_fCrackWeights;
	std::vector<XMFLOAT3> m_xmf3Colors;
	BREAKSCREENSEQUENCE sequence = BREAKSCREENSEQUENCE::SLASHING;

	float m_fSlashChangeSpeed = 5.0f;
	float m_fSlideMoveSpeed = 20.0f;
	XMFLOAT2 m_xmf2UVOffset;
	XMFLOAT2 m_xmf2UVScale;
	float m_fCrackEmissive = 1.0f;
	float m_fGenericAlpha = 1.0f;
	UINT m_nCrackIndex = 0;

	CPlayer* m_pPlayer = nullptr;

public:
	CBreakScreenEffectShader();
	~CBreakScreenEffectShader();

	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	void Reset();
	bool GetEnable() { return m_bEnable; };
	void SetEnable(bool value) { m_bEnable = value; };
	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; };
	CPlayer* GetPlayer() { return m_pPlayer; };
	void SetCrackIndex(UINT index) { m_nCrackIndex = index; };
};