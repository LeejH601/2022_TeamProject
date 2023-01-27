#pragma once
#include "Shader.h"
class CTexture;
class CGameObject;
class CShadowMapShader : public CShader
{
public:
	DECLARE_SINGLE(CShadowMapShader);

	CShadowMapShader() { }
	virtual ~CShadowMapShader() { }

	void SetObjects(std::vector<std::unique_ptr<CGameObject>>* pObjects) { m_pObjects = pObjects; }
	void SetTerrain(CGameObject* pTerrain) { m_pTerrain = pTerrain; }

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat, int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
public:
	std::vector<std::unique_ptr<CGameObject>>* m_pObjects;
	CGameObject* m_pTerrain;
	CTexture* m_pDepthTexture = NULL;
};