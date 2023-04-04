#pragma once
#include "..\Global\stdafx.h"
#include "Shader.h"
#include "..\Object\Light.h"

#define MAX_FLARES 6
struct VS_CB_LENSFLARE_INFO
{
	XMFLOAT4 m_xmf2LensFlarePositions[MAX_FLARES];
	XMFLOAT4 m_xmf4UpandRight;
	float m_fFlareAlpha;
};


class CLensFlareShader : public CShader
{
private:
	std::vector<float> m_vfLensFlareWeights;
	std::vector<XMFLOAT2> m_vLensFlarePositions;
	XMFLOAT4 m_xmf4UpandRightVector;

	ComPtr<ID3D12Resource> m_pd3dcbLensFlareInfo = NULL;
	VS_CB_LENSFLARE_INFO* m_pcbMappedLensFlareInfo = NULL;

	ComPtr<ID3D12Resource> m_pd3dPositionBuffer = NULL;
	ComPtr<ID3D12Resource> m_pd3dPositionUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW m_pd3dPositionBufferView;

	UINT                            m_nStride = 0;
	UINT							m_nVertices = 0;

	std::unique_ptr<CTexture> m_pTexture = NULL;

	bool m_bEnable = true;
	float m_fFlareAlpha = 0.0f;

	float m_fFlareAppearTime = 3.0f;
	float m_fCurrentTime = 0.0f;

public:
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, int nPipelineState);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);

	void UpdateFlareAlpha(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState);

	void CalculateFlaresPlace(CCamera* pCamera, LIGHT* pLight);

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
};

