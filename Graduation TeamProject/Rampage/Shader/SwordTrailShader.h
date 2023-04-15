#pragma once
#include "Shader.h"

#define MAX_TRAILCONTROLLPOINTS 50

struct VS_CB_SWTRAIL_INFO
{
	XMFLOAT4 m_xmf4TrailControllPoints[MAX_TRAILCONTROLLPOINTS];
	UINT m_nDrawedControllPoints;
};

class CSwordTrailShader : public CShader
{
	XMFLOAT4X4 m_xmf4x4SwordTrailControllPointers;
	std::vector<XMFLOAT4> m_xmf4TrailControllPoints;
	UINT m_nDrawedControllPoints;

	ComPtr<ID3D12Resource> m_pd3dcbTrail = NULL;
	VS_CB_SWTRAIL_INFO* m_pcbMappedTrail = NULL;

public:
	CSwordTrailShader();
	virtual ~CSwordTrailShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, int nPipelineState);

	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	void SetNextControllPoint(XMFLOAT4 point1, XMFLOAT4 point2);
	void SetNextControllPoint(XMFLOAT4 point1);
};

