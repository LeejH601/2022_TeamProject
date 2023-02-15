#pragma once
#include "Shader.h"

class CBillBoardObjectShader : public CShader
{
public:
	DECLARE_SINGLE(CBillBoardObjectShader);

public:
	std::shared_ptr<CTexture> Load_Texture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR pszFileName);

public:
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, int nPipelineState);

	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);

public:
	std::vector<std::shared_ptr<CTexture>> m_vTextures;
};
